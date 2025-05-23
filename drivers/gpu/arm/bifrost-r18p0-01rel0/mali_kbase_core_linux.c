/*
 *
 * (C) COPYRIGHT 2010-2019 ARM Limited. All rights reserved.
 *
 * This program is free software and is provided to you under the terms of the
 * GNU General Public License version 2 as published by the Free Software
 * Foundation, and any use by you of this program is subject to the terms
 * of such GNU licence.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you can access it online at
 * http://www.gnu.org/licenses/gpl-2.0.html.
 *
 * SPDX-License-Identifier: GPL-2.0
 *
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas"
#pragma GCC diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic ignored "-Wunknown-warning-option"
#pragma GCC diagnostic ignored "-Wformat="
#pragma GCC diagnostic ignored "-Wdeclaration-after-statement"

#include <mali_kbase.h>
#include <mali_kbase_config_defaults.h>
#include <mali_midg_regmap.h>
#include <mali_kbase_gator.h>
#include <mali_kbase_mem_linux.h>
#if defined(CONFIG_MALI_DEVFREQ) || defined(CONFIG_PM_DEVFREQ)
#include <linux/devfreq.h>
#include <backend/gpu/mali_kbase_devfreq.h>
#ifdef CONFIG_DEVFREQ_THERMAL
#include <ipa/mali_kbase_ipa_debugfs.h>
#endif /* CONFIG_DEVFREQ_THERMAL */
#endif /* CONFIG_MALI_DEVFREQ */
#ifdef CONFIG_MALI_NO_MALI
#include "mali_kbase_model_linux.h"
#include <backend/gpu/mali_kbase_model_dummy.h>
#endif /* CONFIG_MALI_NO_MALI */
#include "mali_kbase_mem_profile_debugfs_buf_size.h"
#include "mali_kbase_debug_mem_view.h"
#include "mali_kbase_mem.h"
#include "mali_kbase_mem_pool_debugfs.h"
#include "mali_kbase_debugfs_helper.h"
#if !MALI_CUSTOMER_RELEASE
#include "mali_kbase_regs_dump_debugfs.h"
#endif /* !MALI_CUSTOMER_RELEASE */
#include "mali_kbase_regs_history_debugfs.h"
#include <mali_kbase_hwaccess_backend.h>
#include <mali_kbase_hwaccess_jm.h>
#include <mali_kbase_ctx_sched.h>
#include <backend/gpu/mali_kbase_device_internal.h>
#include <backend/gpu/mali_kbase_pm_internal.h>
#include "mali_kbase_ioctl.h"
#include "mali_kbase_hwcnt_context.h"
#include "mali_kbase_hwcnt_virtualizer.h"
#include "mali_kbase_hwcnt_legacy.h"
#include "mali_kbase_vinstr.h"

#ifdef CONFIG_MALI_CINSTR_GWT
#include "mali_kbase_gwt.h"
#endif

#include <linux/module.h>
#include <linux/init.h>
#include <linux/poll.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/of.h>
#include <linux/platform_device.h>
#include <linux/of_platform.h>
#include <linux/miscdevice.h>
#include <linux/list.h>
#include <linux/semaphore.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/mm.h>
#include <linux/compat.h>	/* is_compat_task/in_compat_syscall */
#include <linux/mman.h>
#include <linux/version.h>
#include <mali_kbase_hw.h>
#if defined(CONFIG_SYNC) || defined(CONFIG_SYNC_FILE)
#include <mali_kbase_sync.h>
#endif /* CONFIG_SYNC || CONFIG_SYNC_FILE */
#ifdef CONFIG_PM_DEVFREQ
#include <linux/devfreq.h>
#endif /* CONFIG_PM_DEVFREQ */
#include <linux/clk.h>
#include <linux/delay.h>
#include <linux/sched.h>
#if (KERNEL_VERSION(4, 14, 0) <= LINUX_VERSION_CODE)
#include <uapi/linux/sched/types.h>
#endif
#include <linux/log2.h>

#include <mali_kbase_config.h>


#if (KERNEL_VERSION(3, 13, 0) <= LINUX_VERSION_CODE)
#include <linux/pm_opp.h>
#else
#include <linux/opp.h>
#endif

#include <linux/pm_runtime.h>

#include <mali_kbase_timeline.h>

#include <mali_kbase_as_fault_debugfs.h>
#include <linux/hisi/hisi_drmdriver.h>
#ifdef CONFIG_HUAWEI_DSM
/* for runmode_is_factory */
#include <linux/hisi/hw_cmdline_parse.h>
#endif

/* GPU IRQ Tags */
#define	JOB_IRQ_TAG	0
#define MMU_IRQ_TAG	1
#define GPU_IRQ_TAG	2

static int kbase_dev_nr;

static DEFINE_MUTEX(kbase_dev_list_lock);
static LIST_HEAD(kbase_dev_list);

#define KERNEL_SIDE_DDK_VERSION_STRING "K:" MALI_RELEASE_NAME "(GPL)"

/**
 * kbase_file_new - Create an object representing a device file
 *
 * @kbdev:  An instance of the GPU platform device, allocated from the probe
 *          method of the driver.
 * @filp:   Pointer to the struct file corresponding to device file
 *          /dev/malixx instance, passed to the file's open method.
 *
 * In its initial state, the device file has no context (i.e. no GPU
 * address space) and no API version number. Both must be assigned before
 * kbase_file_get_kctx_if_setup_complete() can be used successfully.
 *
 * @return Address of an object representing a simulated device file, or NULL
 *         on failure.
 */
static struct kbase_file *kbase_file_new(struct kbase_device *const kbdev,
	struct file *const filp)
{
	struct kbase_file *const kfile = kmalloc(sizeof(*kfile), GFP_KERNEL);

	if (kfile) {
		kfile->kbdev = kbdev;
		kfile->filp = filp;
		kfile->kctx = NULL;
		kfile->api_version = 0;
		atomic_set(&kfile->setup_state, KBASE_FILE_NEED_VSN);
	}
	return kfile;
}

/**
 * kbase_file_get_api_version - Set the application programmer interface version
 *
 * @kfile:  A device file created by kbase_file_new()
 * @major:  Major version number (must not exceed 12 bits)
 * @minor:  Major version number (must not exceed 12 bits)
 *
 * An application programmer interface (API) version must be specified
 * before calling kbase_file_create_kctx(), otherwise an error is returned.
 *
 * If a version number was already set for the given @kfile (or is in the
 * process of being set by another thread) then an error is returned.
 *
 * Return: 0 if successful, otherwise a negative error code.
 */
static int kbase_file_set_api_version(struct kbase_file *const kfile,
	u16 const major, u16 const minor)
{
	if (WARN_ON(!kfile))
		return -EINVAL;

	/* setup pending, try to signal that we'll do the setup,
	 * if setup was already in progress, err this call
	 */
	if (atomic_cmpxchg(&kfile->setup_state, KBASE_FILE_NEED_VSN,
		KBASE_FILE_VSN_IN_PROGRESS) != KBASE_FILE_NEED_VSN)
		return -EPERM;

	/* save the proposed version number for later use */
	kfile->api_version = KBASE_API_VERSION(major, minor);

	atomic_set(&kfile->setup_state, KBASE_FILE_NEED_CTX);
	return 0;
}

/**
 * kbase_file_get_api_version - Get the application programmer interface version
 *
 * @kfile:  A device file created by kbase_file_new()
 *
 * Return: The version number (encoded with KBASE_API_VERSION) or 0 if none has
 *         been set.
 */
static unsigned long kbase_file_get_api_version(struct kbase_file *const kfile)
{
	if (WARN_ON(!kfile))
		return 0;

	if (atomic_read(&kfile->setup_state) < KBASE_FILE_NEED_CTX)
		return 0;

	return kfile->api_version;
}

/**
 * kbase_file_create_kctx - Create a kernel base context
 *
 * @kfile:  A device file created by kbase_file_new()
 * @flags:  Flags to set, which can be any combination of
 *          BASEP_CONTEXT_CREATE_KERNEL_FLAGS.
 *
 * This creates a new context for the GPU platform device instance that was
 * specified when kbase_file_new() was called. Each context has its own GPU
 * address space. If a context was already created for the given @kfile (or is
 * in the process of being created for it by another thread) then an error is
 * returned.
 *
 * An API version number must have been set by kbase_file_set_api_version()
 * before calling this function, otherwise an error is returned.
 *
 * Return: 0 if a new context was created, otherwise a negative error code.
 */
static int kbase_file_create_kctx(struct kbase_file *kfile,
	base_context_create_flags flags);

/**
 * kbase_file_get_kctx_if_setup_complete - Get a kernel base context
 *                                         pointer from a device file
 *
 * @kfile: A device file created by kbase_file_new()
 *
 * This function returns an error code (encoded with ERR_PTR) if no context
 * has been created for the given @kfile. This makes it safe to use in
 * circumstances where the order of initialization cannot be enforced, but
 * only if the caller checks the return value.
 *
 * Return: Address of the kernel base context associated with the @kfile, or
 *         NULL if no context exists.
 */
static struct kbase_context *kbase_file_get_kctx_if_setup_complete(
	struct kbase_file *const kfile)
{
	if (WARN_ON(!kfile) ||
		atomic_read(&kfile->setup_state) != KBASE_FILE_COMPLETE ||
		WARN_ON(!kfile->kctx))
		return NULL;

	return kfile->kctx;
}

/**
 * kbase_file_delete - Destroy an object representing a device file
 *
 * @kfile: A device file created by kbase_file_new()
 *
 * If any context was created for the @kfile then it is destroyed.
 */
static void kbase_file_delete(struct kbase_file *const kfile)
{
	struct kbase_device *kbdev = NULL;

	if (WARN_ON(!kfile))
		return;

	kfile->filp->private_data = NULL;
	kbdev = kfile->kbdev;

	if (atomic_read(&kfile->setup_state) == KBASE_FILE_COMPLETE) {
		struct kbase_context *kctx = kfile->kctx;

#ifdef CONFIG_HISI_DEBUG_FS
		kbasep_mem_profile_debugfs_remove(kctx);
#endif

		mutex_lock(&kctx->legacy_hwcnt_lock);
		/* If this client was performing hardware counter dumping and
		 * did not explicitly detach itself, destroy it now
		 */
		kbase_hwcnt_legacy_client_destroy(kctx->legacy_hwcnt_cli);
		kctx->legacy_hwcnt_cli = NULL;
		mutex_unlock(&kctx->legacy_hwcnt_lock);

		kbase_destroy_context(kctx);

		dev_dbg(kbdev->dev, "deleted base context\n");
	}

	kbase_release_device(kbdev);

	kfree(kfile);
}
/*lint -e647, -e666, -e574, -e454, -e455, -e648, -e429, -e1058, -e58, -e613, -e668*/
static int kbase_api_handshake(struct kbase_file *kfile,
			       struct kbase_ioctl_version_check *version)
{
	int err = 0;

	switch (version->major) {
	case BASE_UK_VERSION_MAJOR:
		/* set minor to be the lowest common */
		version->minor = min_t(int, BASE_UK_VERSION_MINOR,
				       (int)version->minor);
		break;
	default:
		/* We return our actual version regardless if it
		 * matches the version returned by userspace -
		 * userspace can bail if it can't handle this
		 * version
		 */
		version->major = BASE_UK_VERSION_MAJOR;
		version->minor = BASE_UK_VERSION_MINOR;
		break;
	}

	/* save the proposed version number for later use */
	err = kbase_file_set_api_version(kfile, version->major, version->minor);
	if (unlikely(err))
		return err;

	/* For backward compatibility, we may need to create the context before
	 * the flags have been set. Originally it was created on file open
	 * (with job submission disabled) but we don't support that usage.
	 */
	if (kbase_file_get_api_version(kfile) < KBASE_API_VERSION(11, 15))
		err = kbase_file_create_kctx(kfile,
			BASE_CONTEXT_SYSTEM_MONITOR_SUBMIT_DISABLED);

	return err;
}

/**
 * enum mali_error - Mali error codes shared with userspace
 *
 * This is subset of those common Mali errors that can be returned to userspace.
 * Values of matching user and kernel space enumerators MUST be the same.
 * MALI_ERROR_NONE is guaranteed to be 0.
 *
 * @MALI_ERROR_NONE: Success
 * @MALI_ERROR_OUT_OF_GPU_MEMORY: Not used in the kernel driver
 * @MALI_ERROR_OUT_OF_MEMORY: Memory allocation failure
 * @MALI_ERROR_FUNCTION_FAILED: Generic error code
 */
enum mali_error {
	MALI_ERROR_NONE = 0,
	MALI_ERROR_OUT_OF_GPU_MEMORY,
	MALI_ERROR_OUT_OF_MEMORY,
	MALI_ERROR_FUNCTION_FAILED,
};

enum {
	inited_mem = (1u << 0),
	inited_js = (1u << 1),
	/* Bit number 2 was earlier assigned to the runtime-pm initialization
	 * stage (which has been merged with the backend_early stage).
	 */
#ifdef CONFIG_MALI_DEVFREQ
	inited_devfreq = (1u << 3),
#endif /* CONFIG_MALI_DEVFREQ */
	inited_tlstream = (1u << 4),
	inited_backend_early = (1u << 5),
	inited_hwcnt_gpu_iface = (1u << 6),
	inited_hwcnt_gpu_ctx = (1u << 7),
	inited_hwcnt_gpu_virt = (1u << 8),
	inited_vinstr = (1u << 9),
	inited_backend_late = (1u << 10),
	inited_device = (1u << 11),
	inited_job_fault = (1u << 13),
	inited_sysfs_group = (1u << 14),
	inited_misc_register = (1u << 15),
	inited_get_device = (1u << 16),
	inited_dev_list = (1u << 17),
	inited_debugfs = (1u << 18),
	inited_gpu_device = (1u << 19),
	inited_registers_map = (1u << 20),
	inited_io_history = (1u << 21),
	inited_power_control = (1u << 22),
	inited_buslogger = (1u << 23),
	inited_protected = (1u << 24),
	inited_ctx_sched = (1u << 25)
};

static struct kbase_device *to_kbase_device(struct device *dev)
{
	return dev_get_drvdata(dev);
}

static int assign_irqs(struct platform_device *pdev)
{
	struct kbase_device *kbdev = to_kbase_device(&pdev->dev);
	int i;

	if (!kbdev)
		return -ENODEV;

	/* 3 IRQ resources */
	for (i = 0; i < 3; i++) {
		struct resource *irq_res;
		int irqtag;

		irq_res = platform_get_resource(pdev, IORESOURCE_IRQ, i);
		if (!irq_res) {
			dev_err(kbdev->dev, "No IRQ resource at index %d\n", i);
			return -ENOENT;
		}

#ifdef CONFIG_OF
		if (!strncmp(irq_res->name, "JOB", 4)) {
			irqtag = JOB_IRQ_TAG;
		} else if (!strncmp(irq_res->name, "MMU", 4)) {
			irqtag = MMU_IRQ_TAG;
		} else if (!strncmp(irq_res->name, "GPU", 4)) {
			irqtag = GPU_IRQ_TAG;
		} else {
			dev_err(&pdev->dev, "Invalid irq res name: '%s'\n",
				irq_res->name);
			return -EINVAL;
		}
#else
		irqtag = i;
#endif /* CONFIG_OF */
		kbdev->irqs[irqtag].irq = irq_res->start;
		kbdev->irqs[irqtag].flags = irq_res->flags & IRQF_TRIGGER_MASK;
	}

	return 0;
}

/*
 * API to acquire device list mutex and
 * return pointer to the device list head
 */
const struct list_head *kbase_dev_list_get(void)
{
	mutex_lock(&kbase_dev_list_lock);
	return &kbase_dev_list;
}
KBASE_EXPORT_TEST_API(kbase_dev_list_get);

/* API to release the device list mutex */
void kbase_dev_list_put(const struct list_head *dev_list)
{
	mutex_unlock(&kbase_dev_list_lock);
}
KBASE_EXPORT_TEST_API(kbase_dev_list_put);

/* Find a particular kbase device (as specified by minor number), or find the "first" device if -1 is specified */
struct kbase_device *kbase_find_device(int minor)
{
	struct kbase_device *kbdev = NULL;
	struct list_head *entry;
	const struct list_head *dev_list = kbase_dev_list_get();

	list_for_each(entry, dev_list) {
		struct kbase_device *tmp;

		tmp = list_entry(entry, struct kbase_device, entry);
		if (tmp->mdev.minor == minor || minor == -1) {
			kbdev = tmp;
			get_device(kbdev->dev);
			break;
		}
	}
	kbase_dev_list_put(dev_list);

	return kbdev;
}
EXPORT_SYMBOL(kbase_find_device);

void kbase_release_device(struct kbase_device *kbdev)
{
	put_device(kbdev->dev);
}
EXPORT_SYMBOL(kbase_release_device);

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 6, 0) && \
		!(LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 28) && \
		LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0))
/*
 * Older versions, before v4.6, of the kernel doesn't have
 * kstrtobool_from_user(), except longterm 4.4.y which had it added in 4.4.28
 */
static int kstrtobool_from_user(const char __user *s, size_t count, bool *res)
{
	char buf[4];

	count = min(count, sizeof(buf) - 1);

	if (copy_from_user(buf, s, count))
		return -EFAULT;
	buf[count] = '\0';

	return strtobool(buf, res);
}
#endif

static ssize_t write_ctx_infinite_cache(struct file *f, const char __user *ubuf, size_t size, loff_t *off)
{
	struct kbase_context *kctx = f->private_data;
	int err;
	bool value;

	err = kstrtobool_from_user(ubuf, size, &value);
	if (err)
		return err;

	if (value)
		kbase_ctx_flag_set(kctx, KCTX_INFINITE_CACHE);
	else
		kbase_ctx_flag_clear(kctx, KCTX_INFINITE_CACHE);

	return size;
}

static ssize_t read_ctx_infinite_cache(struct file *f, char __user *ubuf, size_t size, loff_t *off)
{
	struct kbase_context *kctx = f->private_data;
	char buf[32];
	int count;
	bool value;

	value = kbase_ctx_flag(kctx, KCTX_INFINITE_CACHE);

	count = scnprintf(buf, sizeof(buf), "%s\n", value ? "Y" : "N");

	return simple_read_from_buffer(ubuf, size, off, buf, count);
}

static const struct file_operations kbase_infinite_cache_fops = {
	.open = simple_open,
	.write = write_ctx_infinite_cache,
	.read = read_ctx_infinite_cache,
};

static ssize_t write_ctx_force_same_va(struct file *f, const char __user *ubuf,
		size_t size, loff_t *off)
{
	struct kbase_context *kctx = f->private_data;
	int err;
	bool value;

	err = kstrtobool_from_user(ubuf, size, &value);
	if (err)
		return err;

	if (value) {
#if defined(CONFIG_64BIT)
		/* 32-bit clients cannot force SAME_VA */
		if (kbase_ctx_flag(kctx, KCTX_COMPAT))
			return -EINVAL;
		kbase_ctx_flag_set(kctx, KCTX_FORCE_SAME_VA);
#else /* defined(CONFIG_64BIT) */
		/* 32-bit clients cannot force SAME_VA */
		return -EINVAL;
#endif /* defined(CONFIG_64BIT) */
	} else {
		kbase_ctx_flag_clear(kctx, KCTX_FORCE_SAME_VA);
	}

	return size;
}

static ssize_t read_ctx_force_same_va(struct file *f, char __user *ubuf,
		size_t size, loff_t *off)
{
	struct kbase_context *kctx = f->private_data;
	char buf[32];
	int count;
	bool value;

	value = kbase_ctx_flag(kctx, KCTX_FORCE_SAME_VA);

	count = scnprintf(buf, sizeof(buf), "%s\n", value ? "Y" : "N");

	return simple_read_from_buffer(ubuf, size, off, buf, count);
}

static const struct file_operations kbase_force_same_va_fops = {
	.open = simple_open,
	.write = write_ctx_force_same_va,
	.read = read_ctx_force_same_va,
};

static int kbase_file_create_kctx(struct kbase_file *const kfile,
	base_context_create_flags const flags)
{
	struct kbase_device *kbdev = NULL;
	struct kbase_context *kctx = NULL;
#ifdef CONFIG_HISI_DEBUG_FS
	char kctx_name[64];
#endif

	if (WARN_ON(!kfile))
		return -EINVAL;

	/* setup pending, try to signal that we'll do the setup,
	 * if setup was already in progress, err this call
	 */
	if (atomic_cmpxchg(&kfile->setup_state, KBASE_FILE_NEED_CTX,
		KBASE_FILE_CTX_IN_PROGRESS) != KBASE_FILE_NEED_CTX)
		return -EPERM;

	kbdev = kfile->kbdev;

#if (KERNEL_VERSION(4, 6, 0) <= LINUX_VERSION_CODE)
	kctx = kbase_create_context(kbdev, in_compat_syscall(),
		flags, kfile->api_version, kfile->filp);
#else
	kctx = kbase_create_context(kbdev, is_compat_task(),
		flags, kfile->api_version, kfile->filp);
#endif /* (KERNEL_VERSION(4, 6, 0) <= LINUX_VERSION_CODE) */

	/* if bad flags, will stay stuck in setup mode */
	if (!kctx)
		return -ENOMEM;

	if (kbdev->infinite_cache_active_default)
		kbase_ctx_flag_set(kctx, KCTX_INFINITE_CACHE);

#ifdef CONFIG_HISI_DEBUG_FS
	snprintf(kctx_name, 64, "%d_%d", kctx->tgid, kctx->id); /* unsafe_function_ignore: snprintf */

	kctx->kctx_dentry = debugfs_create_dir(kctx_name,
			kbdev->debugfs_ctx_directory);

	if (IS_ERR_OR_NULL(kctx->kctx_dentry)) {
		/* we don't treat this as a fail - just warn about it */
		dev_warn(kbdev->dev, "couldn't create debugfs dir for kctx\n");
	} else {
		debugfs_create_file("infinite_cache", 0644, kctx->kctx_dentry,
				kctx, &kbase_infinite_cache_fops);
		debugfs_create_file("force_same_va", 0600,
				kctx->kctx_dentry, kctx,
				&kbase_force_same_va_fops);

		mutex_init(&kctx->mem_profile_lock);

		kbasep_jd_debugfs_ctx_init(kctx);
		kbase_debug_mem_view_init(kctx);

		kbase_debug_job_fault_context_init(kctx);

		kbase_mem_pool_debugfs_init(kctx->kctx_dentry, kctx);

		kbase_jit_debugfs_init(kctx);
	}
#endif /* CONFIG_HISI_DEBUG_FS */

	dev_dbg(kbdev->dev, "created base context\n");

	kfile->kctx = kctx;
	atomic_set(&kfile->setup_state, KBASE_FILE_COMPLETE);

	return 0;
}

static int kbase_open(struct inode *inode, struct file *filp)
{
	struct kbase_device *kbdev = NULL;
	struct kbase_file *kfile;
	int ret = 0;

	kbdev = kbase_find_device(iminor(inode));

	if (!kbdev)
		return -ENODEV;

	kfile = kbase_file_new(kbdev, filp);
	if (!kfile) {
		ret = -ENOMEM;
		goto out;
	}

	filp->private_data = kfile;
	filp->f_mode |= FMODE_UNSIGNED_OFFSET;

	return 0;

 out:
	kbase_release_device(kbdev);
	return ret;
}

static int kbase_release(struct inode *inode, struct file *filp)
{
	struct kbase_file *const kfile = filp->private_data;

	kbase_file_delete(kfile);
	return 0;
}

static int kbase_api_set_flags(struct kbase_file *kfile,
		struct kbase_ioctl_set_flags *flags)
{
	int err = 0;
	unsigned long const api_version = kbase_file_get_api_version(kfile);
	struct kbase_context *kctx = NULL;

	/* Validate flags */
	if (flags->create_flags !=
		(flags->create_flags & BASEP_CONTEXT_CREATE_KERNEL_FLAGS))
		return -EINVAL;

	/* For backward compatibility, the context may have been created before
	 * the flags were set.
	 */
	if (api_version >= KBASE_API_VERSION(11, 15)) {
		err = kbase_file_create_kctx(kfile, flags->create_flags);
	} else {
		struct kbasep_js_kctx_info *js_kctx_info = NULL;
		unsigned long irq_flags = 0;

		/* If setup is incomplete (e.g. because the API version
		 * wasn't set) then we have to give up.
		 */
		kctx = kbase_file_get_kctx_if_setup_complete(kfile);
		if (unlikely(!kctx))
			return -EPERM;

		js_kctx_info = &kctx->jctx.sched_info;
		mutex_lock(&js_kctx_info->ctx.jsctx_mutex);
		spin_lock_irqsave(&kctx->kbdev->hwaccess_lock, irq_flags);

		/* Translate the flags */
		if ((flags->create_flags &
			BASE_CONTEXT_SYSTEM_MONITOR_SUBMIT_DISABLED) == 0)
			kbase_ctx_flag_clear(kctx, KCTX_SUBMIT_DISABLED);

		spin_unlock_irqrestore(&kctx->kbdev->hwaccess_lock, irq_flags);
		mutex_unlock(&js_kctx_info->ctx.jsctx_mutex);
	}

	return err;
}

static int kbase_api_job_submit(struct kbase_context *kctx,
		struct kbase_ioctl_job_submit *submit)
{
	return kbase_jd_submit(kctx, u64_to_user_ptr(submit->addr),
			submit->nr_atoms,
			submit->stride, false);
}

static int kbase_api_get_gpuprops(struct kbase_context *kctx,
		struct kbase_ioctl_get_gpuprops *get_props)
{
	struct kbase_gpu_props *kprops = &kctx->kbdev->gpu_props;
	int err;

	if (get_props->flags != 0) {
		dev_err(kctx->kbdev->dev, "Unsupported flags to get_gpuprops");
		return -EINVAL;
	}

	if (get_props->size == 0)
		return kprops->prop_buffer_size;
	if (get_props->size < kprops->prop_buffer_size)
		return -EINVAL;

	err = copy_to_user(u64_to_user_ptr(get_props->buffer),
			kprops->prop_buffer,
			kprops->prop_buffer_size);
	if (err)
		return -EFAULT;
	return kprops->prop_buffer_size;
}

static int kbase_api_post_term(struct kbase_context *kctx)
{
	kbase_event_close(kctx);
	return 0;
}

static int kbase_api_mem_alloc(struct kbase_context *kctx,
		union kbase_ioctl_mem_alloc *alloc)
{
	struct kbase_va_region *reg;
	u64 flags = alloc->in.flags;
	u64 gpu_va;

	rcu_read_lock();
	/* Don't allow memory allocation until user space has set up the
	 * tracking page (which sets kctx->process_mm). Also catches when we've
	 * forked.
	 */
	if (rcu_dereference(kctx->process_mm) != current->mm) {
		rcu_read_unlock();
		return -EINVAL;
	}
	rcu_read_unlock();

	if (flags & BASEP_MEM_FLAGS_KERNEL_ONLY)
		return -ENOMEM;

	/* Force SAME_VA if a 64-bit client.
	 * The only exception is GPU-executable memory if an EXEC_VA zone
	 * has been initialized. In that case, GPU-executable memory may
	 * or may not be SAME_VA.
	 */
	if ((!kbase_ctx_flag(kctx, KCTX_COMPAT)) &&
			kbase_ctx_flag(kctx, KCTX_FORCE_SAME_VA)) {
		if (!(flags & BASE_MEM_PROT_GPU_EX) || !kbase_has_exec_va_zone(kctx))
			flags |= BASE_MEM_SAME_VA;
	}


	reg = kbase_mem_alloc(kctx, alloc->in.va_pages,
			alloc->in.commit_pages,
			alloc->in.extent,
			&flags, &gpu_va);

	if (!reg)
		return -ENOMEM;

	alloc->out.flags = flags;
	alloc->out.gpu_va = gpu_va;

	return 0;
}

static int kbase_api_mem_query(struct kbase_context *kctx,
		union kbase_ioctl_mem_query *query)
{
	return kbase_mem_query(kctx, query->in.gpu_addr,
			query->in.query, &query->out.value);
}

static int kbase_api_mem_free(struct kbase_context *kctx,
		struct kbase_ioctl_mem_free *free)
{
	return kbase_mem_free(kctx, free->gpu_addr);
}

static int kbase_api_hwcnt_reader_setup(struct kbase_context *kctx,
		struct kbase_ioctl_hwcnt_reader_setup *setup)
{
	return kbase_vinstr_hwcnt_reader_setup(kctx->kbdev->vinstr_ctx, setup);
}

static int kbase_api_hwcnt_enable(struct kbase_context *kctx,
		struct kbase_ioctl_hwcnt_enable *enable)
{
	int ret;

	mutex_lock(&kctx->legacy_hwcnt_lock);
	if (enable->dump_buffer != 0) {
		/* Non-zero dump buffer, so user wants to create the client */
		if (kctx->legacy_hwcnt_cli == NULL) {
			ret = kbase_hwcnt_legacy_client_create(
				kctx->kbdev->hwcnt_gpu_virt,
				enable,
				&kctx->legacy_hwcnt_cli);
		} else {
			/* This context already has a client */
			ret = -EBUSY;
		}
	} else {
		/* Zero dump buffer, so user wants to destroy the client */
		if (kctx->legacy_hwcnt_cli != NULL) {
			kbase_hwcnt_legacy_client_destroy(
				kctx->legacy_hwcnt_cli);
			kctx->legacy_hwcnt_cli = NULL;
			ret = 0;
		} else {
			/* This context has no client to destroy */
			ret = -EINVAL;
		}
	}
	mutex_unlock(&kctx->legacy_hwcnt_lock);

	return ret;
}

static int kbase_api_hwcnt_dump(struct kbase_context *kctx)
{
	int ret;

	mutex_lock(&kctx->legacy_hwcnt_lock);
	ret = kbase_hwcnt_legacy_client_dump(kctx->legacy_hwcnt_cli);
	mutex_unlock(&kctx->legacy_hwcnt_lock);

	return ret;
}

static int kbase_api_hwcnt_clear(struct kbase_context *kctx)
{
	int ret;

	mutex_lock(&kctx->legacy_hwcnt_lock);
	ret = kbase_hwcnt_legacy_client_clear(kctx->legacy_hwcnt_cli);
	mutex_unlock(&kctx->legacy_hwcnt_lock);

	return ret;
}

#ifdef CONFIG_MALI_NO_MALI
static int kbase_api_hwcnt_set(struct kbase_context *kctx,
		struct kbase_ioctl_hwcnt_values *values)
{
	gpu_model_set_dummy_prfcnt_sample(
			(u32 __user *)(uintptr_t)values->data,
			values->size);

	return 0;
}
#endif

static int kbase_api_disjoint_query(struct kbase_context *kctx,
		struct kbase_ioctl_disjoint_query *query)
{
	query->counter = kbase_disjoint_event_get(kctx->kbdev);

	return 0;
}

static int kbase_api_get_ddk_version(struct kbase_context *kctx,
		struct kbase_ioctl_get_ddk_version *version)
{
	int ret;
	int len = sizeof(KERNEL_SIDE_DDK_VERSION_STRING);

	if (version->version_buffer == 0)
		return len;

	if (version->size < len)
		return -EOVERFLOW;

	ret = copy_to_user(u64_to_user_ptr(version->version_buffer),
			KERNEL_SIDE_DDK_VERSION_STRING,
			sizeof(KERNEL_SIDE_DDK_VERSION_STRING));

	if (ret)
		return -EFAULT;

	return len;
}

/* Defaults for legacy JIT init ioctl */
#define DEFAULT_MAX_JIT_ALLOCATIONS 255
#define JIT_LEGACY_TRIM_LEVEL (0) /* No trimming */

static int kbase_api_mem_jit_init_old(struct kbase_context *kctx,
		struct kbase_ioctl_mem_jit_init_old *jit_init)
{
	kctx->jit_version = 1;

	return kbase_region_tracker_init_jit(kctx, jit_init->va_pages,
			DEFAULT_MAX_JIT_ALLOCATIONS,
			JIT_LEGACY_TRIM_LEVEL, BASE_MEM_GROUP_DEFAULT);
}

static int kbase_api_mem_jit_init(struct kbase_context *kctx,
		struct kbase_ioctl_mem_jit_init *jit_init)
{
	int i;

	kctx->jit_version = 2;

	for (i = 0; i < sizeof(jit_init->padding); i++) {
		/* Ensure all padding bytes are 0 for potential future
		 * extension
		 */
		if (jit_init->padding[i])
			return -EINVAL;
	}

	return kbase_region_tracker_init_jit(kctx, jit_init->va_pages,
			jit_init->max_allocations, jit_init->trim_level,
			jit_init->group_id);
}

static int kbase_api_mem_exec_init(struct kbase_context *kctx,
		struct kbase_ioctl_mem_exec_init *exec_init)
{
	return kbase_region_tracker_init_exec(kctx, exec_init->va_pages);
}

static int kbase_api_mem_sync(struct kbase_context *kctx,
		struct kbase_ioctl_mem_sync *sync)
{
	struct basep_syncset sset = {
		.mem_handle.basep.handle = sync->handle,
		.user_addr = sync->user_addr,
		.size = sync->size,
		.type = sync->type
	};

	return kbase_sync_now(kctx, &sset);
}

static int kbase_api_mem_find_cpu_offset(struct kbase_context *kctx,
		union kbase_ioctl_mem_find_cpu_offset *find)
{
	return kbasep_find_enclosing_cpu_mapping_offset(
			kctx,
			find->in.cpu_addr,
			find->in.size,
			&find->out.offset);
}

static int kbase_api_mem_find_gpu_start_and_offset(struct kbase_context *kctx,
		union kbase_ioctl_mem_find_gpu_start_and_offset *find)
{
	return kbasep_find_enclosing_gpu_mapping_start_and_offset(
			kctx,
			find->in.gpu_addr,
			find->in.size,
			&find->out.start,
			&find->out.offset);
}

static int kbase_api_get_context_id(struct kbase_context *kctx,
		struct kbase_ioctl_get_context_id *info)
{
	info->id = kctx->id;

	return 0;
}

static int kbase_api_tlstream_acquire(struct kbase_context *kctx,
		struct kbase_ioctl_tlstream_acquire *acquire)
{
	return kbase_timeline_io_acquire(kctx->kbdev, acquire->flags);
}

static int kbase_api_tlstream_flush(struct kbase_context *kctx)
{
	kbase_timeline_streams_flush(kctx->kbdev->timeline);

	return 0;
}

static int kbase_api_mem_commit(struct kbase_context *kctx,
		struct kbase_ioctl_mem_commit *commit)
{
	return kbase_mem_commit(kctx, commit->gpu_addr, commit->pages);
}

static int kbase_api_mem_alias(struct kbase_context *kctx,
		union kbase_ioctl_mem_alias *alias)
{
	struct base_mem_aliasing_info *ai;
	u64 flags;
	int err;

	if (alias->in.nents == 0 || alias->in.nents > 2048)
		return -EINVAL;

	if (alias->in.stride > (U64_MAX / 2048))
		return -EINVAL;

	ai = vmalloc(sizeof(*ai) * alias->in.nents);
	if (!ai)
		return -ENOMEM;

	err = copy_from_user(ai,
			u64_to_user_ptr(alias->in.aliasing_info),
			sizeof(*ai) * alias->in.nents);
	if (err) {
		vfree(ai);
		return -EFAULT;
	}

	flags = alias->in.flags;
	if (flags & BASEP_MEM_FLAGS_KERNEL_ONLY) {
		vfree(ai);
		return -EINVAL;
	}

	alias->out.gpu_va = kbase_mem_alias(kctx, &flags,
			alias->in.stride, alias->in.nents,
			ai, &alias->out.va_pages);

	alias->out.flags = flags;

	vfree(ai);

	if (alias->out.gpu_va == 0)
		return -ENOMEM;

	return 0;
}

static int kbase_api_mem_import(struct kbase_context *kctx,
		union kbase_ioctl_mem_import *import)
{
	int ret;
	u64 flags = import->in.flags;

	if (flags & BASEP_MEM_FLAGS_KERNEL_ONLY)
		return -ENOMEM;

	ret = kbase_mem_import(kctx,
			import->in.type,
			u64_to_user_ptr(import->in.phandle),
			import->in.padding,
			&import->out.gpu_va,
			&import->out.va_pages,
			&flags,
			import->in.header_page_number);

	import->out.flags = flags;

	return ret;
}

static int kbase_api_mem_flags_change(struct kbase_context *kctx,
		struct kbase_ioctl_mem_flags_change *change)
{
	if (change->flags & BASEP_MEM_FLAGS_KERNEL_ONLY)
		return -ENOMEM;

	return kbase_mem_flags_change(kctx, change->gpu_va,
			change->flags, change->mask);
}

static int kbase_api_stream_create(struct kbase_context *kctx,
		struct kbase_ioctl_stream_create *stream)
{
#if defined(CONFIG_SYNC) || defined(CONFIG_SYNC_FILE)
	int fd, ret;

	/* Name must be NULL-terminated and padded with NULLs, so check last
	 * character is NULL
	 */
	if (stream->name[sizeof(stream->name)-1] != 0)
		return -EINVAL;

	ret = kbase_sync_fence_stream_create(stream->name, &fd);

	if (ret)
		return ret;
	return fd;
#else
	return -ENOENT;
#endif
}

static int kbase_api_fence_validate(struct kbase_context *kctx,
		struct kbase_ioctl_fence_validate *validate)
{
#if defined(CONFIG_SYNC) || defined(CONFIG_SYNC_FILE)
	return kbase_sync_fence_validate(validate->fd);
#else
	return -ENOENT;
#endif
}

static int kbase_api_mem_profile_add(struct kbase_context *kctx,
		struct kbase_ioctl_mem_profile_add *data)
{
	char *buf;
	int err;

	if (data->len > KBASE_MEM_PROFILE_MAX_BUF_SIZE) {
		dev_err(kctx->kbdev->dev, "mem_profile_add: buffer too big\n");
		return -EINVAL;
	}

	buf = kmalloc(data->len, GFP_KERNEL);
	if (ZERO_OR_NULL_PTR(buf))
		return -ENOMEM;

	err = copy_from_user(buf, u64_to_user_ptr(data->buffer),
			data->len);
	if (err) {
		kfree(buf);
		return -EFAULT;
	}

	return kbasep_mem_profile_debugfs_insert(kctx, buf, data->len);
}

static int kbase_api_soft_event_update(struct kbase_context *kctx,
		struct kbase_ioctl_soft_event_update *update)
{
	if (update->flags != 0)
		return -EINVAL;

	return kbase_soft_event_update(kctx, update->event, update->new_status);
}

static int kbase_api_sticky_resource_map(struct kbase_context *kctx,
		struct kbase_ioctl_sticky_resource_map *map)
{
	int ret;
	u64 i;
	u64 gpu_addr[BASE_EXT_RES_COUNT_MAX];

	if (!map->count || map->count > BASE_EXT_RES_COUNT_MAX)
		return -EOVERFLOW;

	ret = copy_from_user(gpu_addr, u64_to_user_ptr(map->address),
			sizeof(u64) * map->count);

	if (ret != 0)
		return -EFAULT;

	kbase_gpu_vm_lock(kctx);

	for (i = 0; i < map->count; i++) {
		if (!kbase_sticky_resource_acquire(kctx, gpu_addr[i])) {
			/* Invalid resource */
			ret = -EINVAL;
			break;
		}
	}

	if (ret != 0) {
		while (i > 0) {
			i--;
			kbase_sticky_resource_release(kctx, NULL, gpu_addr[i]);
		}
	}

	kbase_gpu_vm_unlock(kctx);

	return ret;
}

static int kbase_api_sticky_resource_unmap(struct kbase_context *kctx,
		struct kbase_ioctl_sticky_resource_unmap *unmap)
{
	int ret;
	u64 i;
	u64 gpu_addr[BASE_EXT_RES_COUNT_MAX];

	if (!unmap->count || unmap->count > BASE_EXT_RES_COUNT_MAX)
		return -EOVERFLOW;

	ret = copy_from_user(gpu_addr, u64_to_user_ptr(unmap->address),
			sizeof(u64) * unmap->count);

	if (ret != 0)
		return -EFAULT;

	kbase_gpu_vm_lock(kctx);

	for (i = 0; i < unmap->count; i++) {
		if (!kbase_sticky_resource_release(kctx, NULL, gpu_addr[i])) {
			/* Invalid resource, but we keep going anyway */
			ret = -EINVAL;
		}
	}

	kbase_gpu_vm_unlock(kctx);

	return ret;
}

static int kbase_api_set_render_thread_prio(struct kbase_context *kctx,
		struct kbase_ioctl_render_prio_values *data)
{
	struct kbase_ioctl_render_prio_values *info = data;
	struct task_struct *p_task;
	struct sched_param param_task;
	struct cpumask cpu_mask;

	int cpu_no;
	int retval;
	u32  size =  info->size;
	u32 i ;

	if ( size > MAX_TASK_INFO) {
		return -EINVAL;
	}

	for (i = 0; i < size; i++) {
		param_task.sched_priority = info->task[i].sched_priority;

		if ( (param_task.sched_priority >=  0) &&  (param_task.sched_priority <= 90) ) {
			rcu_read_lock();
			p_task = find_task_by_vpid(info->task[i].pid);

			if (p_task != NULL)
			{
				if (info->task[i].sched_policy == SCHED_RR) {
					retval = sched_setscheduler_nocheck(p_task, info->task[i].sched_policy, &param_task);
					pr_err("Mali sched_setscheduler policy %d, pid : %d\n", retval, info->task[i].pid);
				}
			} else {
				pr_err("Mali sched_setscheduler not find task  pid : %d\n", info->task[i].pid);
			}
			rcu_read_unlock();
		}
	}

	if (!info->bound_flag)
		return 0;

	cpumask_clear(&cpu_mask);

	for (cpu_no = 4; cpu_no < 8; cpu_no++)
	{
		cpumask_set_cpu(cpu_no, &cpu_mask);
	}

	for (i = 0; i < size; i++) {
		if(sched_setaffinity(info->task[i].pid, &cpu_mask) < 0)
		{
			pr_err("Mali sched_setaffinity  err\n");
		}
	}

	return 0;
}

#if MALI_UNIT_TEST
static int kbase_api_tlstream_test(struct kbase_context *kctx,
		struct kbase_ioctl_tlstream_test *test)
{
	kbase_timeline_test(
			kctx->kbdev,
			test->tpw_count,
			test->msg_delay,
			test->msg_count,
			test->aux_msg);

	return 0;
}

static int kbase_api_tlstream_stats(struct kbase_context *kctx,
		struct kbase_ioctl_tlstream_stats *stats)
{
	kbase_timeline_stats(kctx->kbdev->timeline,
			&stats->bytes_collected,
			&stats->bytes_generated);

	return 0;
}
#endif /* MALI_UNIT_TEST */


#define KBASE_HANDLE_IOCTL(cmd, function, arg)    \
	do {                                          \
		BUILD_BUG_ON(_IOC_DIR(cmd) != _IOC_NONE); \
		return function(arg);                     \
	} while (0)

#define KBASE_HANDLE_IOCTL_IN(cmd, function, type, arg)    \
	do {                                                   \
		type param;                                        \
		int err;                                           \
		BUILD_BUG_ON(_IOC_DIR(cmd) != _IOC_WRITE);         \
		BUILD_BUG_ON(sizeof(param) != _IOC_SIZE(cmd));     \
		err = copy_from_user(&param, uarg, sizeof(param)); \
		if (err)                                           \
			return -EFAULT;                                \
		return function(arg, &param);                      \
	} while (0)

#define KBASE_HANDLE_IOCTL_OUT(cmd, function, type, arg)   \
	do {                                                   \
		type param;                                        \
		int ret, err;                                      \
		BUILD_BUG_ON(_IOC_DIR(cmd) != _IOC_READ);          \
		BUILD_BUG_ON(sizeof(param) != _IOC_SIZE(cmd));     \
		ret = function(arg, &param);                       \
		err = copy_to_user(uarg, &param, sizeof(param));   \
		if (err)                                           \
			return -EFAULT;                                \
		return ret;                                        \
	} while (0)

#define KBASE_HANDLE_IOCTL_INOUT(cmd, function, type, arg)     \
	do {                                                       \
		type param;                                            \
		int ret, err;                                          \
		BUILD_BUG_ON(_IOC_DIR(cmd) != (_IOC_WRITE|_IOC_READ)); \
		BUILD_BUG_ON(sizeof(param) != _IOC_SIZE(cmd));         \
		err = copy_from_user(&param, uarg, sizeof(param));     \
		if (err)                                               \
			return -EFAULT;                                    \
		ret = function(arg, &param);                           \
		err = copy_to_user(uarg, &param, sizeof(param));       \
		if (err)                                               \
			return -EFAULT;                                    \
		return ret;                                            \
	} while (0)
/*lint -e527*/
static long kbase_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct kbase_file *const kfile = filp->private_data;
	struct kbase_context *kctx = NULL;
	struct kbase_device *kbdev = kfile->kbdev;
	void __user *uarg = (void __user *)arg;

	/* Only these ioctls are available until setup is complete */
	switch (cmd) {
	case KBASE_IOCTL_VERSION_CHECK:
		KBASE_HANDLE_IOCTL_INOUT(KBASE_IOCTL_VERSION_CHECK,
				kbase_api_handshake,
				struct kbase_ioctl_version_check,
				kfile);
		break;

	case KBASE_IOCTL_SET_FLAGS:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_SET_FLAGS,
				kbase_api_set_flags,
				struct kbase_ioctl_set_flags,
				kfile);
		break;
	}

	kctx = kbase_file_get_kctx_if_setup_complete(kfile);
	if (unlikely(!kctx))
		return -EPERM;

	/* Normal ioctls */
	switch (cmd) {
	case KBASE_IOCTL_JOB_SUBMIT:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_JOB_SUBMIT,
				kbase_api_job_submit,
				struct kbase_ioctl_job_submit,
				kctx);
		break;
	case KBASE_IOCTL_GET_GPUPROPS:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_GET_GPUPROPS,
				kbase_api_get_gpuprops,
				struct kbase_ioctl_get_gpuprops,
				kctx);
		break;
	case KBASE_IOCTL_POST_TERM:
		KBASE_HANDLE_IOCTL(KBASE_IOCTL_POST_TERM,
				kbase_api_post_term,
				kctx);
		break;
	case KBASE_IOCTL_MEM_ALLOC:
		KBASE_HANDLE_IOCTL_INOUT(KBASE_IOCTL_MEM_ALLOC,
				kbase_api_mem_alloc,
				union kbase_ioctl_mem_alloc,
				kctx);
		break;
	case KBASE_IOCTL_MEM_QUERY:
		KBASE_HANDLE_IOCTL_INOUT(KBASE_IOCTL_MEM_QUERY,
				kbase_api_mem_query,
				union kbase_ioctl_mem_query,
				kctx);
		break;
	case KBASE_IOCTL_MEM_FREE:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_MEM_FREE,
				kbase_api_mem_free,
				struct kbase_ioctl_mem_free,
				kctx);
		break;
	case KBASE_IOCTL_DISJOINT_QUERY:
		KBASE_HANDLE_IOCTL_OUT(KBASE_IOCTL_DISJOINT_QUERY,
				kbase_api_disjoint_query,
				struct kbase_ioctl_disjoint_query,
				kctx);
		break;
	case KBASE_IOCTL_GET_DDK_VERSION:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_GET_DDK_VERSION,
				kbase_api_get_ddk_version,
				struct kbase_ioctl_get_ddk_version,
				kctx);
		break;
	case KBASE_IOCTL_MEM_JIT_INIT_OLD:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_MEM_JIT_INIT_OLD,
				kbase_api_mem_jit_init_old,
				struct kbase_ioctl_mem_jit_init_old,
				kctx);
		break;
	case KBASE_IOCTL_MEM_JIT_INIT:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_MEM_JIT_INIT,
				kbase_api_mem_jit_init,
				struct kbase_ioctl_mem_jit_init,
				kctx);
		break;
	case KBASE_IOCTL_MEM_EXEC_INIT:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_MEM_EXEC_INIT,
				kbase_api_mem_exec_init,
				struct kbase_ioctl_mem_exec_init,
				kctx);
		break;
	case KBASE_IOCTL_MEM_SYNC:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_MEM_SYNC,
				kbase_api_mem_sync,
				struct kbase_ioctl_mem_sync,
				kctx);
		break;
	case KBASE_IOCTL_MEM_FIND_CPU_OFFSET:
		KBASE_HANDLE_IOCTL_INOUT(KBASE_IOCTL_MEM_FIND_CPU_OFFSET,
				kbase_api_mem_find_cpu_offset,
				union kbase_ioctl_mem_find_cpu_offset,
				kctx);
		break;
	case KBASE_IOCTL_MEM_FIND_GPU_START_AND_OFFSET:
		KBASE_HANDLE_IOCTL_INOUT(KBASE_IOCTL_MEM_FIND_GPU_START_AND_OFFSET,
				kbase_api_mem_find_gpu_start_and_offset,
				union kbase_ioctl_mem_find_gpu_start_and_offset,
				kctx);
		break;
	case KBASE_IOCTL_GET_CONTEXT_ID:
		KBASE_HANDLE_IOCTL_OUT(KBASE_IOCTL_GET_CONTEXT_ID,
				kbase_api_get_context_id,
				struct kbase_ioctl_get_context_id,
				kctx);
		break;
	case KBASE_IOCTL_TLSTREAM_ACQUIRE:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_TLSTREAM_ACQUIRE,
				kbase_api_tlstream_acquire,
				struct kbase_ioctl_tlstream_acquire,
				kctx);
		break;
	case KBASE_IOCTL_TLSTREAM_FLUSH:
		KBASE_HANDLE_IOCTL(KBASE_IOCTL_TLSTREAM_FLUSH,
				kbase_api_tlstream_flush,
				kctx);
		break;
	case KBASE_IOCTL_MEM_COMMIT:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_MEM_COMMIT,
				kbase_api_mem_commit,
				struct kbase_ioctl_mem_commit,
				kctx);
		break;
	case KBASE_IOCTL_MEM_ALIAS:
		KBASE_HANDLE_IOCTL_INOUT(KBASE_IOCTL_MEM_ALIAS,
				kbase_api_mem_alias,
				union kbase_ioctl_mem_alias,
				kctx);
		break;
	case KBASE_IOCTL_MEM_IMPORT:
		KBASE_HANDLE_IOCTL_INOUT(KBASE_IOCTL_MEM_IMPORT,
				kbase_api_mem_import,
				union kbase_ioctl_mem_import,
				kctx);
		break;
	case KBASE_IOCTL_MEM_FLAGS_CHANGE:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_MEM_FLAGS_CHANGE,
				kbase_api_mem_flags_change,
				struct kbase_ioctl_mem_flags_change,
				kctx);
		break;
	case KBASE_IOCTL_STREAM_CREATE:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_STREAM_CREATE,
				kbase_api_stream_create,
				struct kbase_ioctl_stream_create,
				kctx);
		break;
	case KBASE_IOCTL_FENCE_VALIDATE:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_FENCE_VALIDATE,
				kbase_api_fence_validate,
				struct kbase_ioctl_fence_validate,
				kctx);
		break;
	case KBASE_IOCTL_MEM_PROFILE_ADD:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_MEM_PROFILE_ADD,
				kbase_api_mem_profile_add,
				struct kbase_ioctl_mem_profile_add,
				kctx);
		break;
	case KBASE_IOCTL_SOFT_EVENT_UPDATE:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_SOFT_EVENT_UPDATE,
				kbase_api_soft_event_update,
				struct kbase_ioctl_soft_event_update,
				kctx);
		break;
	case KBASE_IOCTL_STICKY_RESOURCE_MAP:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_STICKY_RESOURCE_MAP,
				kbase_api_sticky_resource_map,
				struct kbase_ioctl_sticky_resource_map,
				kctx);
		break;
	case KBASE_IOCTL_STICKY_RESOURCE_UNMAP:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_STICKY_RESOURCE_UNMAP,
				kbase_api_sticky_resource_unmap,
				struct kbase_ioctl_sticky_resource_unmap,
				kctx);
		break;

	/* Instrumentation. */
	case KBASE_IOCTL_HWCNT_READER_SETUP:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_HWCNT_READER_SETUP,
				kbase_api_hwcnt_reader_setup,
				struct kbase_ioctl_hwcnt_reader_setup,
				kctx);
		break;
	case KBASE_IOCTL_HWCNT_ENABLE:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_HWCNT_ENABLE,
				kbase_api_hwcnt_enable,
				struct kbase_ioctl_hwcnt_enable,
				kctx);
		break;
	case KBASE_IOCTL_HWCNT_DUMP:
		KBASE_HANDLE_IOCTL(KBASE_IOCTL_HWCNT_DUMP,
				kbase_api_hwcnt_dump,
				kctx);
		break;
	case KBASE_IOCTL_HWCNT_CLEAR:
		KBASE_HANDLE_IOCTL(KBASE_IOCTL_HWCNT_CLEAR,
				kbase_api_hwcnt_clear,
				kctx);
		break;
#ifdef CONFIG_MALI_NO_MALI
	case KBASE_IOCTL_HWCNT_SET:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_HWCNT_SET,
				kbase_api_hwcnt_set,
				struct kbase_ioctl_hwcnt_values,
				kctx);
		break;
#endif
#ifdef CONFIG_MALI_CINSTR_GWT
	case KBASE_IOCTL_CINSTR_GWT_START:
		KBASE_HANDLE_IOCTL(KBASE_IOCTL_CINSTR_GWT_START,
				kbase_gpu_gwt_start,
				kctx);
		break;
	case KBASE_IOCTL_CINSTR_GWT_STOP:
		KBASE_HANDLE_IOCTL(KBASE_IOCTL_CINSTR_GWT_STOP,
				kbase_gpu_gwt_stop,
				kctx);
		break;
	case KBASE_IOCTL_CINSTR_GWT_DUMP:
		KBASE_HANDLE_IOCTL_INOUT(KBASE_IOCTL_CINSTR_GWT_DUMP,
				kbase_gpu_gwt_dump,
				union kbase_ioctl_cinstr_gwt_dump,
				kctx);
		break;
#endif
	case KBASE_IOCTL_SET_RENDER_THREAD_PRIO:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_SET_RENDER_THREAD_PRIO,
				kbase_api_set_render_thread_prio,
				struct kbase_ioctl_render_prio_values,
				kctx);
		break;
#if MALI_UNIT_TEST
	case KBASE_IOCTL_TLSTREAM_TEST:
		KBASE_HANDLE_IOCTL_IN(KBASE_IOCTL_TLSTREAM_TEST,
				kbase_api_tlstream_test,
				struct kbase_ioctl_tlstream_test,
				kctx);
		break;
	case KBASE_IOCTL_TLSTREAM_STATS:
		KBASE_HANDLE_IOCTL_OUT(KBASE_IOCTL_TLSTREAM_STATS,
				kbase_api_tlstream_stats,
				struct kbase_ioctl_tlstream_stats,
				kctx);
		break;
#endif
	}

	dev_warn(kbdev->dev, "Unknown ioctl 0x%x nr:%d", cmd, _IOC_NR(cmd));

	return -ENOIOCTLCMD;
}
/*lint +e527*/
static ssize_t kbase_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos)
{
	struct kbase_file *const kfile = filp->private_data;
	struct kbase_context *const kctx =
		kbase_file_get_kctx_if_setup_complete(kfile);
	struct base_jd_event_v2 uevent;
	int out_count = 0;

	if (unlikely(!kctx))
		return -EPERM;

	if (count < sizeof(uevent))
		return -ENOBUFS;

	do {
		while (kbase_event_dequeue(kctx, &uevent)) {
			if (out_count > 0)
				goto out;

			if (filp->f_flags & O_NONBLOCK)
				return -EAGAIN;

			if (wait_event_interruptible(kctx->event_queue,
					kbase_event_pending(kctx)) != 0)
				return -ERESTARTSYS;
		}
		if (uevent.event_code == BASE_JD_EVENT_DRV_TERMINATED) {
			if (out_count == 0)
				return -EPIPE;
			goto out;
		}

		if (copy_to_user(buf, &uevent, sizeof(uevent)) != 0)
			return -EFAULT;

		buf += sizeof(uevent);
		out_count++;
		count -= sizeof(uevent);
	} while (count >= sizeof(uevent));

 out:
	return out_count * sizeof(uevent);
}

static unsigned int kbase_poll(struct file *filp, poll_table *wait)
{
	struct kbase_file *const kfile = filp->private_data;
	struct kbase_context *const kctx =
		kbase_file_get_kctx_if_setup_complete(kfile);

	if (unlikely(!kctx))
		return POLLERR;

	poll_wait(filp, &kctx->event_queue, wait);
	if (kbase_event_pending(kctx))
		return POLLIN | POLLRDNORM;

	return 0;
}

void kbase_event_wakeup(struct kbase_context *kctx)
{
	KBASE_DEBUG_ASSERT(kctx);

	wake_up_interruptible(&kctx->event_queue);
}

KBASE_EXPORT_TEST_API(kbase_event_wakeup);

static int kbase_mmap(struct file *const filp, struct vm_area_struct *const vma)
{
	struct kbase_file *const kfile = filp->private_data;
	struct kbase_context *const kctx =
		kbase_file_get_kctx_if_setup_complete(kfile);

	if (unlikely(!kctx))
		return -EPERM;

	return kbase_context_mmap(kctx, vma);
}

static int kbase_check_flags(int flags)
{
	/* Enforce that the driver keeps the O_CLOEXEC flag so that execve() always
	 * closes the file descriptor in a child process.
	 */
	if (0 == (flags & O_CLOEXEC))
		return -EINVAL;

	return 0;
}

static unsigned long kbase_get_unmapped_area(struct file *const filp,
		const unsigned long addr, const unsigned long len,
		const unsigned long pgoff, const unsigned long flags)
{
	struct kbase_file *const kfile = filp->private_data;
	struct kbase_context *const kctx =
		kbase_file_get_kctx_if_setup_complete(kfile);

	if (unlikely(!kctx))
		return -EPERM;

	return kbase_context_get_unmapped_area(kctx, addr, len, pgoff, flags);
}

static const struct file_operations kbase_fops = {
	.owner = THIS_MODULE,
	.open = kbase_open,
	.release = kbase_release,
	.read = kbase_read,
	.poll = kbase_poll,
	.unlocked_ioctl = kbase_ioctl,
	.compat_ioctl = kbase_ioctl,
	.mmap = kbase_mmap,
	.check_flags = kbase_check_flags,
	.get_unmapped_area = kbase_get_unmapped_area,
};

/**
 * show_policy - Show callback for the power_policy sysfs file.
 *
 * This function is called to get the contents of the power_policy sysfs
 * file. This is a list of the available policies with the currently active one
 * surrounded by square brackets.
 *
 * @dev:	The device this sysfs file is for
 * @attr:	The attributes of the sysfs file
 * @buf:	The output buffer for the sysfs file contents
 *
 * Return: The number of bytes output to @buf.
 */
static ssize_t show_policy(struct device *dev, struct device_attribute *attr, char *const buf)
{
	struct kbase_device *kbdev;
	const struct kbase_pm_policy *current_policy;
	const struct kbase_pm_policy *const *policy_list;
	int policy_count;
	int i;
	ssize_t ret = 0;

	kbdev = to_kbase_device(dev);

	if (!kbdev)
		return -ENODEV;

	current_policy = kbase_pm_get_policy(kbdev);

	policy_count = kbase_pm_list_policies(kbdev, &policy_list);

	for (i = 0; i < policy_count && ret < PAGE_SIZE; i++) {
		if (policy_list[i] == current_policy)
			ret += scnprintf(buf + ret, PAGE_SIZE - ret, "[%s] ", policy_list[i]->name);
		else
			ret += scnprintf(buf + ret, PAGE_SIZE - ret, "%s ", policy_list[i]->name);
	}

	if (ret < PAGE_SIZE - 1) {
		ret += scnprintf(buf + ret, PAGE_SIZE - ret, "\n");
	} else {
		buf[PAGE_SIZE - 2] = '\n';
		buf[PAGE_SIZE - 1] = '\0';
		ret = PAGE_SIZE - 1;
	}

	return ret;
}

/**
 * set_policy - Store callback for the power_policy sysfs file.
 *
 * This function is called when the power_policy sysfs file is written to.
 * It matches the requested policy against the available policies and if a
 * matching policy is found calls kbase_pm_set_policy() to change the
 * policy.
 *
 * @dev:	The device with sysfs file is for
 * @attr:	The attributes of the sysfs file
 * @buf:	The value written to the sysfs file
 * @count:	The number of bytes written to the sysfs file
 *
 * Return: @count if the function succeeded. An error code on failure.
 */
static ssize_t set_policy(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct kbase_device *kbdev;
	const struct kbase_pm_policy *new_policy = NULL;
	const struct kbase_pm_policy *const *policy_list;
	int policy_count;
	int i;

	kbdev = to_kbase_device(dev);

	if (!kbdev)
		return -ENODEV;

	policy_count = kbase_pm_list_policies(kbdev, &policy_list);

	for (i = 0; i < policy_count; i++) {
		if (sysfs_streq(policy_list[i]->name, buf)) {
			new_policy = policy_list[i];
			break;
		}
	}

	if (!new_policy) {
		dev_err(dev, "power_policy: policy not found\n");
		return -EINVAL;
	}

	kbase_pm_set_policy(kbdev, new_policy);

	return count;
}

/*
 * The sysfs file power_policy.
 *
 * This is used for obtaining information about the available policies,
 * determining which policy is currently active, and changing the active
 * policy.
 */
static DEVICE_ATTR(power_policy, S_IRUGO | S_IWUSR, show_policy, set_policy);

/*
 * show_core_mask - Show callback for the core_mask sysfs file.
 *
 * This function is called to get the contents of the core_mask sysfs file.
 *
 * @dev:	The device this sysfs file is for
 * @attr:	The attributes of the sysfs file
 * @buf:	The output buffer for the sysfs file contents
 *
 * Return: The number of bytes output to @buf.
 */
static ssize_t show_core_mask(struct device *dev, struct device_attribute *attr, char * const buf)
{
	struct kbase_device *kbdev;
	ssize_t ret = 0;

	kbdev = to_kbase_device(dev);

	if (!kbdev)
		return -ENODEV;

	ret += scnprintf(buf + ret, PAGE_SIZE - ret,
			"Current core mask (JS0) : 0x%llX\n",
			kbdev->pm.debug_core_mask[0]);
	ret += scnprintf(buf + ret, PAGE_SIZE - ret,
			"Current core mask (JS1) : 0x%llX\n",
			kbdev->pm.debug_core_mask[1]);
	ret += scnprintf(buf + ret, PAGE_SIZE - ret,
			"Current core mask (JS2) : 0x%llX\n",
			kbdev->pm.debug_core_mask[2]);
	ret += scnprintf(buf + ret, PAGE_SIZE - ret,
			"Available core mask : 0x%llX\n",
			kbdev->gpu_props.props.raw_props.shader_present);

	return ret;
}

/**
 * set_core_mask - Store callback for the core_mask sysfs file.
 *
 * This function is called when the core_mask sysfs file is written to.
 *
 * @dev:	The device with sysfs file is for
 * @attr:	The attributes of the sysfs file
 * @buf:	The value written to the sysfs file
 * @count:	The number of bytes written to the sysfs file
 *
 * Return: @count if the function succeeded. An error code on failure.
 */
static ssize_t set_core_mask(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct kbase_device *kbdev;
	u64 new_core_mask[3];
	int items, i;
	ssize_t err = count;
	unsigned long flags;
	u64 shader_present, group0_core_mask;

	kbdev = to_kbase_device(dev);

	if (!kbdev)
		return -ENODEV;

	items = sscanf(buf, "%llx %llx %llx", /* unsafe_function_ignore: sscanf */
			&new_core_mask[0], &new_core_mask[1],
			&new_core_mask[2]);

	if (items != 1 && items != 3) {
		dev_err(kbdev->dev, "Couldn't process core mask write operation.\n"
			"Use format <core_mask>\n"
			"or <core_mask_js0> <core_mask_js1> <core_mask_js2>\n");
		err = -EINVAL;
		goto end;
	}

	if (items == 1)
		new_core_mask[1] = new_core_mask[2] = new_core_mask[0];

	mutex_lock(&kbdev->pm.lock);
	spin_lock_irqsave(&kbdev->hwaccess_lock, flags);

	shader_present = kbdev->gpu_props.props.raw_props.shader_present;
	group0_core_mask = kbdev->gpu_props.props.coherency_info.group[0].core_mask;

	for (i = 0; i < 3; ++i) {
		if ((new_core_mask[i] & shader_present) != new_core_mask[i]) {
			dev_err(dev, "Invalid core mask 0x%llX for JS %d: Includes non-existent cores (present = 0x%llX)",
					new_core_mask[i], i, shader_present);
			err = -EINVAL;
			goto unlock;

		} else if (!(new_core_mask[i] & shader_present & kbdev->pm.backend.ca_cores_enabled)) {
			dev_err(dev, "Invalid core mask 0x%llX for JS %d: No intersection with currently available cores (present = 0x%llX, CA enabled = 0x%llX\n",
					new_core_mask[i], i,
					kbdev->gpu_props.props.raw_props.shader_present,
					kbdev->pm.backend.ca_cores_enabled);
			err = -EINVAL;
			goto unlock;

		} else if (!(new_core_mask[i] & group0_core_mask)) {
			dev_err(dev, "Invalid core mask 0x%llX for JS %d: No intersection with group 0 core mask 0x%llX\n",
					new_core_mask[i], i, group0_core_mask);
			err = -EINVAL;
			goto unlock;
		}
	}

#ifdef CONFIG_HISI_GPU_CORE_HOTPLUG
	/* don't debug core_mask if the thermal is controlling */
	if (kbdev->pm.thermal_controlling)
		goto unlock;
#endif

	if (kbdev->pm.debug_core_mask[0] != new_core_mask[0] ||
			kbdev->pm.debug_core_mask[1] !=
					new_core_mask[1] ||
			kbdev->pm.debug_core_mask[2] !=
					new_core_mask[2]) {

		kbase_pm_set_debug_core_mask(kbdev, new_core_mask[0],
				new_core_mask[1], new_core_mask[2]);
	}

unlock:
	spin_unlock_irqrestore(&kbdev->hwaccess_lock, flags);
	mutex_unlock(&kbdev->pm.lock);
end:
	return err;
}

/*
 * The sysfs file core_mask.
 *
 * This is used to restrict shader core availability for debugging purposes.
 * Reading it will show the current core mask and the mask of cores available.
 * Writing to it will set the current core mask.
 */
static DEVICE_ATTR(core_mask, S_IRUGO | S_IWUSR, show_core_mask, set_core_mask);

/**
 * set_soft_job_timeout - Store callback for the soft_job_timeout sysfs
 * file.
 *
 * @dev: The device this sysfs file is for.
 * @attr: The attributes of the sysfs file.
 * @buf: The value written to the sysfs file.
 * @count: The number of bytes written to the sysfs file.
 *
 * This allows setting the timeout for software jobs. Waiting soft event wait
 * jobs will be cancelled after this period expires, while soft fence wait jobs
 * will print debug information if the fence debug feature is enabled.
 *
 * This is expressed in milliseconds.
 *
 * Return: count if the function succeeded. An error code on failure.
 */
static ssize_t set_soft_job_timeout(struct device *dev,
				      struct device_attribute *attr,
				      const char *buf, size_t count)
{
	struct kbase_device *kbdev;
	int soft_job_timeout_ms;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	if ((kstrtoint(buf, 0, &soft_job_timeout_ms) != 0) ||
	    (soft_job_timeout_ms <= 0))
		return -EINVAL;

	atomic_set(&kbdev->js_data.soft_job_timeout_ms,
		   soft_job_timeout_ms);

	return count;
}

/**
 * show_soft_job_timeout - Show callback for the soft_job_timeout sysfs
 * file.
 *
 * This will return the timeout for the software jobs.
 *
 * @dev: The device this sysfs file is for.
 * @attr: The attributes of the sysfs file.
 * @buf: The output buffer for the sysfs file contents.
 *
 * Return: The number of bytes output to buf.
 */
static ssize_t show_soft_job_timeout(struct device *dev,
				       struct device_attribute *attr,
				       char * const buf)
{
	struct kbase_device *kbdev;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	return scnprintf(buf, PAGE_SIZE, "%i\n",
			 atomic_read(&kbdev->js_data.soft_job_timeout_ms));
}

static DEVICE_ATTR(soft_job_timeout, S_IRUGO | S_IWUSR,
		   show_soft_job_timeout, set_soft_job_timeout);

static u32 timeout_ms_to_ticks(struct kbase_device *kbdev, long timeout_ms,
				int default_ticks, u32 old_ticks)
{
	if (timeout_ms > 0) {
		u64 ticks = timeout_ms * 1000000ULL; /* [false alarm]: thirdparty code */
		do_div(ticks, kbdev->js_data.scheduling_period_ns);
		if (!ticks)
			return 1;
		return ticks;
	} else if (timeout_ms < 0) {
		return default_ticks; /* [false alarm]: no problem - fortify check */
	} else {
		return old_ticks;
	}
}

/**
 * set_js_timeouts - Store callback for the js_timeouts sysfs file.
 *
 * This function is called to get the contents of the js_timeouts sysfs
 * file. This file contains five values separated by whitespace. The values
 * are basically the same as %JS_SOFT_STOP_TICKS, %JS_HARD_STOP_TICKS_SS,
 * %JS_HARD_STOP_TICKS_DUMPING, %JS_RESET_TICKS_SS, %JS_RESET_TICKS_DUMPING
 * configuration values (in that order), with the difference that the js_timeout
 * values are expressed in MILLISECONDS.
 *
 * The js_timeouts sysfile file allows the current values in
 * use by the job scheduler to get override. Note that a value needs to
 * be other than 0 for it to override the current job scheduler value.
 *
 * @dev:	The device with sysfs file is for
 * @attr:	The attributes of the sysfs file
 * @buf:	The value written to the sysfs file
 * @count:	The number of bytes written to the sysfs file
 *
 * Return: @count if the function succeeded. An error code on failure.
 */
static ssize_t set_js_timeouts(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct kbase_device *kbdev;
	int items;
	long js_soft_stop_ms;
	long js_soft_stop_ms_cl;
	long js_hard_stop_ms_ss;
	long js_hard_stop_ms_cl;
	long js_hard_stop_ms_dumping;
	long js_reset_ms_ss;
	long js_reset_ms_cl;
	long js_reset_ms_dumping;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	items = sscanf(buf, "%ld %ld %ld %ld %ld %ld %ld %ld", /* unsafe_function_ignore: sscanf */
			&js_soft_stop_ms, &js_soft_stop_ms_cl,
			&js_hard_stop_ms_ss, &js_hard_stop_ms_cl,
			&js_hard_stop_ms_dumping, &js_reset_ms_ss,
			&js_reset_ms_cl, &js_reset_ms_dumping);

	if (items == 8) {
		struct kbasep_js_device_data *js_data = &kbdev->js_data;
		unsigned long flags;

		spin_lock_irqsave(&kbdev->hwaccess_lock, flags);

#define UPDATE_TIMEOUT(ticks_name, ms_name, default) do {\
	js_data->ticks_name = timeout_ms_to_ticks(kbdev, ms_name, \
			default, js_data->ticks_name); \
	dev_dbg(kbdev->dev, "Overriding " #ticks_name \
			" with %lu ticks (%lu ms)\n", \
			(unsigned long)js_data->ticks_name, \
			ms_name); \
	} while (0)

		UPDATE_TIMEOUT(soft_stop_ticks, js_soft_stop_ms,
				DEFAULT_JS_SOFT_STOP_TICKS);
		UPDATE_TIMEOUT(soft_stop_ticks_cl, js_soft_stop_ms_cl,
				DEFAULT_JS_SOFT_STOP_TICKS_CL);
		UPDATE_TIMEOUT(hard_stop_ticks_ss, js_hard_stop_ms_ss,
				kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_8408) ?
				DEFAULT_JS_HARD_STOP_TICKS_SS_8408 :
				DEFAULT_JS_HARD_STOP_TICKS_SS);
		UPDATE_TIMEOUT(hard_stop_ticks_cl, js_hard_stop_ms_cl,
				DEFAULT_JS_HARD_STOP_TICKS_CL);
		UPDATE_TIMEOUT(hard_stop_ticks_dumping,
				js_hard_stop_ms_dumping,
				DEFAULT_JS_HARD_STOP_TICKS_DUMPING);
		UPDATE_TIMEOUT(gpu_reset_ticks_ss, js_reset_ms_ss,
				kbase_hw_has_issue(kbdev, BASE_HW_ISSUE_8408) ?
				DEFAULT_JS_RESET_TICKS_SS_8408 :
				DEFAULT_JS_RESET_TICKS_SS);
		UPDATE_TIMEOUT(gpu_reset_ticks_cl, js_reset_ms_cl,
				DEFAULT_JS_RESET_TICKS_CL);
		UPDATE_TIMEOUT(gpu_reset_ticks_dumping, js_reset_ms_dumping,
				DEFAULT_JS_RESET_TICKS_DUMPING);

		kbase_js_set_timeouts(kbdev);

		spin_unlock_irqrestore(&kbdev->hwaccess_lock, flags);

		return count;
	}

	dev_err(kbdev->dev, "Couldn't process js_timeouts write operation.\n"
			"Use format <soft_stop_ms> <soft_stop_ms_cl> <hard_stop_ms_ss> <hard_stop_ms_cl> <hard_stop_ms_dumping> <reset_ms_ss> <reset_ms_cl> <reset_ms_dumping>\n"
			"Write 0 for no change, -1 to restore default timeout\n");
	return -EINVAL;
}

static unsigned long get_js_timeout_in_ms(
		u32 scheduling_period_ns,
		u32 ticks)
{
	u64 ms = (u64)ticks * scheduling_period_ns;

	do_div(ms, 1000000UL);
	return ms;
}

/**
 * show_js_timeouts - Show callback for the js_timeouts sysfs file.
 *
 * This function is called to get the contents of the js_timeouts sysfs
 * file. It returns the last set values written to the js_timeouts sysfs file.
 * If the file didn't get written yet, the values will be current setting in
 * use.
 * @dev:	The device this sysfs file is for
 * @attr:	The attributes of the sysfs file
 * @buf:	The output buffer for the sysfs file contents
 *
 * Return: The number of bytes output to @buf.
 */
static ssize_t show_js_timeouts(struct device *dev, struct device_attribute *attr, char * const buf)
{
	struct kbase_device *kbdev;
	ssize_t ret;
	unsigned long js_soft_stop_ms;
	unsigned long js_soft_stop_ms_cl;
	unsigned long js_hard_stop_ms_ss;
	unsigned long js_hard_stop_ms_cl;
	unsigned long js_hard_stop_ms_dumping;
	unsigned long js_reset_ms_ss;
	unsigned long js_reset_ms_cl;
	unsigned long js_reset_ms_dumping;
	u32 scheduling_period_ns;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	scheduling_period_ns = kbdev->js_data.scheduling_period_ns;

#define GET_TIMEOUT(name) get_js_timeout_in_ms(\
		scheduling_period_ns, \
		kbdev->js_data.name)

	js_soft_stop_ms = GET_TIMEOUT(soft_stop_ticks);
	js_soft_stop_ms_cl = GET_TIMEOUT(soft_stop_ticks_cl);
	js_hard_stop_ms_ss = GET_TIMEOUT(hard_stop_ticks_ss);
	js_hard_stop_ms_cl = GET_TIMEOUT(hard_stop_ticks_cl);
	js_hard_stop_ms_dumping = GET_TIMEOUT(hard_stop_ticks_dumping);
	js_reset_ms_ss = GET_TIMEOUT(gpu_reset_ticks_ss);
	js_reset_ms_cl = GET_TIMEOUT(gpu_reset_ticks_cl);
	js_reset_ms_dumping = GET_TIMEOUT(gpu_reset_ticks_dumping);

#undef GET_TIMEOUT

	ret = scnprintf(buf, PAGE_SIZE, "%lu %lu %lu %lu %lu %lu %lu %lu\n",
			js_soft_stop_ms, js_soft_stop_ms_cl,
			js_hard_stop_ms_ss, js_hard_stop_ms_cl,
			js_hard_stop_ms_dumping, js_reset_ms_ss,
			js_reset_ms_cl, js_reset_ms_dumping);

	if (ret >= PAGE_SIZE) {
		buf[PAGE_SIZE - 2] = '\n';
		buf[PAGE_SIZE - 1] = '\0';
		ret = PAGE_SIZE - 1;
	}

	return ret;
}
/*lint +e647, +e666, +e574, +e454, +e455, +e648, +e429, +e1058, +e58, +e613, +e668*/

/*
 * The sysfs file js_timeouts.
 *
 * This is used to override the current job scheduler values for
 * JS_STOP_STOP_TICKS_SS
 * JS_STOP_STOP_TICKS_CL
 * JS_HARD_STOP_TICKS_SS
 * JS_HARD_STOP_TICKS_CL
 * JS_HARD_STOP_TICKS_DUMPING
 * JS_RESET_TICKS_SS
 * JS_RESET_TICKS_CL
 * JS_RESET_TICKS_DUMPING.
 */
static DEVICE_ATTR(js_timeouts, S_IRUGO | S_IWUSR, show_js_timeouts, set_js_timeouts);

static u32 get_new_js_timeout(
		u32 old_period,
		u32 old_ticks,
		u32 new_scheduling_period_ns)
{
	u64 ticks = (u64)old_period * (u64)old_ticks;
	do_div(ticks, new_scheduling_period_ns);
	return ticks?ticks:1;
}

/**
 * set_js_scheduling_period - Store callback for the js_scheduling_period sysfs
 *                            file
 * @dev:   The device the sysfs file is for
 * @attr:  The attributes of the sysfs file
 * @buf:   The value written to the sysfs file
 * @count: The number of bytes written to the sysfs file
 *
 * This function is called when the js_scheduling_period sysfs file is written
 * to. It checks the data written, and if valid updates the js_scheduling_period
 * value
 *
 * Return: @count if the function succeeded. An error code on failure.
 */
static ssize_t set_js_scheduling_period(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct kbase_device *kbdev;
	int ret;
	unsigned int js_scheduling_period;
	u32 new_scheduling_period_ns;
	u32 old_period;
	struct kbasep_js_device_data *js_data;
	unsigned long flags;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	js_data = &kbdev->js_data;

	ret = kstrtouint(buf, 0, &js_scheduling_period);
	if (ret || !js_scheduling_period) {
		dev_err(kbdev->dev, "Couldn't process js_scheduling_period write operation.\n"
				"Use format <js_scheduling_period_ms>\n");
		return -EINVAL;
	}

	new_scheduling_period_ns = js_scheduling_period * 1000000;

	/* Update scheduling timeouts */
	mutex_lock(&js_data->runpool_mutex);
	spin_lock_irqsave(&kbdev->hwaccess_lock, flags);

	/* If no contexts have been scheduled since js_timeouts was last written
	 * to, the new timeouts might not have been latched yet. So check if an
	 * update is pending and use the new values if necessary. */

	/* Use previous 'new' scheduling period as a base if present. */
	old_period = js_data->scheduling_period_ns;

#define SET_TIMEOUT(name) \
		(js_data->name = get_new_js_timeout(\
				old_period, \
				kbdev->js_data.name, \
				new_scheduling_period_ns))

	SET_TIMEOUT(soft_stop_ticks);
	SET_TIMEOUT(soft_stop_ticks_cl);
	SET_TIMEOUT(hard_stop_ticks_ss);
	SET_TIMEOUT(hard_stop_ticks_cl);
	SET_TIMEOUT(hard_stop_ticks_dumping);
	SET_TIMEOUT(gpu_reset_ticks_ss);
	SET_TIMEOUT(gpu_reset_ticks_cl);
	SET_TIMEOUT(gpu_reset_ticks_dumping);

#undef SET_TIMEOUT

	js_data->scheduling_period_ns = new_scheduling_period_ns;

	kbase_js_set_timeouts(kbdev);

	spin_unlock_irqrestore(&kbdev->hwaccess_lock, flags);
	mutex_unlock(&js_data->runpool_mutex);

	dev_dbg(kbdev->dev, "JS scheduling period: %dms\n",
			js_scheduling_period);

	return count;
}

/**
 * show_js_scheduling_period - Show callback for the js_scheduling_period sysfs
 *                             entry.
 * @dev:  The device this sysfs file is for.
 * @attr: The attributes of the sysfs file.
 * @buf:  The output buffer to receive the GPU information.
 *
 * This function is called to get the current period used for the JS scheduling
 * period.
 *
 * Return: The number of bytes output to @buf.
 */
static ssize_t show_js_scheduling_period(struct device *dev,
		struct device_attribute *attr, char * const buf)
{
	struct kbase_device *kbdev;
	u32 period;
	ssize_t ret;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	period = kbdev->js_data.scheduling_period_ns;

	ret = scnprintf(buf, PAGE_SIZE, "%d\n",
			period / 1000000);

	return ret;
}

static DEVICE_ATTR(js_scheduling_period, S_IRUGO | S_IWUSR,
		show_js_scheduling_period, set_js_scheduling_period);

#if !MALI_CUSTOMER_RELEASE
/**
 * set_force_replay - Store callback for the force_replay sysfs file.
 *
 * @dev:	The device with sysfs file is for
 * @attr:	The attributes of the sysfs file
 * @buf:	The value written to the sysfs file
 * @count:	The number of bytes written to the sysfs file
 *
 * Return: @count if the function succeeded. An error code on failure.
 */
static ssize_t set_force_replay(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct kbase_device *kbdev;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	if (!strncmp("limit=", buf, MIN(6, count))) {
		int force_replay_limit;
		int items = sscanf(buf, "limit=%u", &force_replay_limit); /* unsafe_function_ignore: sscanf */

		if (items == 1) {
			kbdev->force_replay_random = false;
			kbdev->force_replay_limit = force_replay_limit;
			kbdev->force_replay_count = 0;

			return count;
		}
	} else if (!strncmp("random_limit", buf, MIN(12, count))) {
		kbdev->force_replay_random = true;
		kbdev->force_replay_count = 0;

		return count;
	} else if (!strncmp("norandom_limit", buf, MIN(14, count))) {
		kbdev->force_replay_random = false;
		kbdev->force_replay_limit = KBASEP_FORCE_REPLAY_DISABLED;
		kbdev->force_replay_count = 0;

		return count;
	} else if (!strncmp("core_req=", buf, MIN(9, count))) {
		unsigned int core_req;
		int items = sscanf(buf, "core_req=%x", &core_req); /* unsafe_function_ignore: sscanf */

		if (items == 1) {
			kbdev->force_replay_core_req = (base_jd_core_req)core_req;

			return count;
		}
	}
	dev_err(kbdev->dev, "Couldn't process force_replay write operation.\nPossible settings: limit=<limit>, random_limit, norandom_limit, core_req=<core_req>\n");
	return -EINVAL;
}

/**
 * show_force_replay - Show callback for the force_replay sysfs file.
 *
 * This function is called to get the contents of the force_replay sysfs
 * file. It returns the last set value written to the force_replay sysfs file.
 * If the file didn't get written yet, the values will be 0.
 *
 * @dev:	The device this sysfs file is for
 * @attr:	The attributes of the sysfs file
 * @buf:	The output buffer for the sysfs file contents
 *
 * Return: The number of bytes output to @buf.
 */
static ssize_t show_force_replay(struct device *dev,
		struct device_attribute *attr, char * const buf)
{
	struct kbase_device *kbdev;
	ssize_t ret;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	if (kbdev->force_replay_random)
		ret = scnprintf(buf, PAGE_SIZE,
				"limit=0\nrandom_limit\ncore_req=%x\n",
				kbdev->force_replay_core_req);
	else
		ret = scnprintf(buf, PAGE_SIZE,
				"limit=%u\nnorandom_limit\ncore_req=%x\n",
				kbdev->force_replay_limit,
				kbdev->force_replay_core_req);

	if (ret >= PAGE_SIZE) {
		buf[PAGE_SIZE - 2] = '\n';
		buf[PAGE_SIZE - 1] = '\0';
		ret = PAGE_SIZE - 1;
	}

	return ret;
}

/*
 * The sysfs file force_replay.
 */
static DEVICE_ATTR(force_replay, S_IRUGO | S_IWUSR, show_force_replay,
		set_force_replay);
#endif /* !MALI_CUSTOMER_RELEASE */

#ifdef CONFIG_MALI_DEBUG
static ssize_t set_js_softstop_always(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct kbase_device *kbdev;
	int ret;
	int softstop_always;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	ret = kstrtoint(buf, 0, &softstop_always);
	if (ret || ((softstop_always != 0) && (softstop_always != 1))) {
		dev_err(kbdev->dev, "Couldn't process js_softstop_always write operation.\n"
				"Use format <soft_stop_always>\n");
		return -EINVAL;
	}

	kbdev->js_data.softstop_always = (bool) softstop_always;
	dev_dbg(kbdev->dev, "Support for softstop on a single context: %s\n",
			(kbdev->js_data.softstop_always) ?
			"Enabled" : "Disabled");
	return count;
}

static ssize_t show_js_softstop_always(struct device *dev,
		struct device_attribute *attr, char * const buf)
{
	struct kbase_device *kbdev;
	ssize_t ret;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	ret = scnprintf(buf, PAGE_SIZE, "%d\n", kbdev->js_data.softstop_always);

	if (ret >= PAGE_SIZE) {
		buf[PAGE_SIZE - 2] = '\n';
		buf[PAGE_SIZE - 1] = '\0';
		ret = PAGE_SIZE - 1;
	}

	return ret;
}

/*
 * By default, soft-stops are disabled when only a single context is present.
 * The ability to enable soft-stop when only a single context is present can be
 * used for debug and unit-testing purposes.
 * (see CL t6xx_stress_1 unit-test as an example whereby this feature is used.)
 */
static DEVICE_ATTR(js_softstop_always, S_IRUGO | S_IWUSR, show_js_softstop_always, set_js_softstop_always);
#endif /* CONFIG_MALI_DEBUG */

#ifdef CONFIG_MALI_DEBUG
typedef void (kbasep_debug_command_func) (struct kbase_device *);

enum kbasep_debug_command_code {
	KBASEP_DEBUG_COMMAND_DUMPTRACE,

	/* This must be the last enum */
	KBASEP_DEBUG_COMMAND_COUNT
};

struct kbasep_debug_command {
	char *str;
	kbasep_debug_command_func *func;
};

/* Debug commands supported by the driver */
static const struct kbasep_debug_command debug_commands[] = {
	{
	 .str = "dumptrace",
	 .func = &kbasep_trace_dump,
	 }
};

/**
 * show_debug - Show callback for the debug_command sysfs file.
 *
 * This function is called to get the contents of the debug_command sysfs
 * file. This is a list of the available debug commands, separated by newlines.
 *
 * @dev:	The device this sysfs file is for
 * @attr:	The attributes of the sysfs file
 * @buf:	The output buffer for the sysfs file contents
 *
 * Return: The number of bytes output to @buf.
 */
static ssize_t show_debug(struct device *dev, struct device_attribute *attr, char * const buf)
{
	struct kbase_device *kbdev;
	int i;
	ssize_t ret = 0;

	kbdev = to_kbase_device(dev);

	if (!kbdev)
		return -ENODEV;

	for (i = 0; i < KBASEP_DEBUG_COMMAND_COUNT && ret < PAGE_SIZE; i++)
		ret += scnprintf(buf + ret, PAGE_SIZE - ret, "%s\n", debug_commands[i].str);

	if (ret >= PAGE_SIZE) {
		buf[PAGE_SIZE - 2] = '\n';
		buf[PAGE_SIZE - 1] = '\0';
		ret = PAGE_SIZE - 1;
	}

	return ret;
}

/**
 * issue_debug - Store callback for the debug_command sysfs file.
 *
 * This function is called when the debug_command sysfs file is written to.
 * It matches the requested command against the available commands, and if
 * a matching command is found calls the associated function from
 * @debug_commands to issue the command.
 *
 * @dev:	The device with sysfs file is for
 * @attr:	The attributes of the sysfs file
 * @buf:	The value written to the sysfs file
 * @count:	The number of bytes written to the sysfs file
 *
 * Return: @count if the function succeeded. An error code on failure.
 */
static ssize_t issue_debug(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct kbase_device *kbdev;
	int i;

	kbdev = to_kbase_device(dev);

	if (!kbdev)
		return -ENODEV;

	for (i = 0; i < KBASEP_DEBUG_COMMAND_COUNT; i++) {
		if (sysfs_streq(debug_commands[i].str, buf)) {
			debug_commands[i].func(kbdev);
			return count;
		}
	}

	/* Debug Command not found */
	dev_err(dev, "debug_command: command not known\n");
	return -EINVAL;
}

/* The sysfs file debug_command.
 *
 * This is used to issue general debug commands to the device driver.
 * Reading it will produce a list of debug commands, separated by newlines.
 * Writing to it with one of those commands will issue said command.
 */
static DEVICE_ATTR(debug_command, S_IRUGO | S_IWUSR, show_debug, issue_debug);
#endif /* CONFIG_MALI_DEBUG */

/**
 * kbase_show_gpuinfo - Show callback for the gpuinfo sysfs entry.
 * @dev: The device this sysfs file is for.
 * @attr: The attributes of the sysfs file.
 * @buf: The output buffer to receive the GPU information.
 *
 * This function is called to get a description of the present Mali
 * GPU via the gpuinfo sysfs entry.  This includes the GPU family, the
 * number of cores, the hardware version and the raw product id.  For
 * example
 *
 *    Mali-T60x MP4 r0p0 0x6956
 *
 * Return: The number of bytes output to @buf.
 */
static ssize_t kbase_show_gpuinfo(struct device *dev,
				  struct device_attribute *attr, char *buf)
{
	static const struct gpu_product_id_name {
		unsigned id;
		char *name;
	} gpu_product_id_names[] = {
		{ .id = GPU_ID_PI_T60X, .name = "Mali-T60x" },
		{ .id = GPU_ID_PI_T62X, .name = "Mali-T62x" },
		{ .id = GPU_ID_PI_T72X, .name = "Mali-T72x" },
		{ .id = GPU_ID_PI_T76X, .name = "Mali-T76x" },
		{ .id = GPU_ID_PI_T82X, .name = "Mali-T82x" },
		{ .id = GPU_ID_PI_T83X, .name = "Mali-T83x" },
		{ .id = GPU_ID_PI_T86X, .name = "Mali-T86x" },
		{ .id = GPU_ID_PI_TFRX, .name = "Mali-T88x" },
		{ .id = GPU_ID2_PRODUCT_TMIX >> GPU_ID_VERSION_PRODUCT_ID_SHIFT,
		  .name = "Mali-G71" },
		{ .id = GPU_ID2_PRODUCT_THEX >> GPU_ID_VERSION_PRODUCT_ID_SHIFT,
		  .name = "Mali-G72" },
		{ .id = GPU_ID2_PRODUCT_TSIX >> GPU_ID_VERSION_PRODUCT_ID_SHIFT,
		  .name = "Mali-G51" },
		{ .id = GPU_ID2_PRODUCT_TNOX >> GPU_ID_VERSION_PRODUCT_ID_SHIFT,
		  .name = "Mali-G76" },
		{ .id = GPU_ID2_PRODUCT_TDVX >> GPU_ID_VERSION_PRODUCT_ID_SHIFT,
		  .name = "Mali-G31" },
		{ .id = GPU_ID2_PRODUCT_TGOX >> GPU_ID_VERSION_PRODUCT_ID_SHIFT,
		  .name = "Mali-G52" },
	};
	const char *product_name = "(Unknown Mali GPU)";
	struct kbase_device *kbdev;
	u32 gpu_id;
	unsigned product_id, product_id_mask;
	unsigned i;
	bool is_new_format;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	gpu_id = kbdev->gpu_props.props.raw_props.gpu_id;
	product_id = gpu_id >> GPU_ID_VERSION_PRODUCT_ID_SHIFT;
	is_new_format = GPU_ID_IS_NEW_FORMAT(product_id);
	product_id_mask =
		(is_new_format ?
			GPU_ID2_PRODUCT_MODEL :
			GPU_ID_VERSION_PRODUCT_ID) >>
		GPU_ID_VERSION_PRODUCT_ID_SHIFT;

	for (i = 0; i < ARRAY_SIZE(gpu_product_id_names); ++i) {
		const struct gpu_product_id_name *p = &gpu_product_id_names[i];

		if ((GPU_ID_IS_NEW_FORMAT(p->id) == is_new_format) &&
		    (p->id & product_id_mask) ==
		    (product_id & product_id_mask)) {
			product_name = p->name;
			break;
		}
	}

	return scnprintf(buf, PAGE_SIZE, "%s %d cores r%dp%d 0x%04X\n",
		product_name, kbdev->gpu_props.num_cores,
		(gpu_id & GPU_ID_VERSION_MAJOR) >> GPU_ID_VERSION_MAJOR_SHIFT,
		(gpu_id & GPU_ID_VERSION_MINOR) >> GPU_ID_VERSION_MINOR_SHIFT,
		product_id);
}
static DEVICE_ATTR(gpuinfo, S_IRUGO, kbase_show_gpuinfo, NULL);

/**
 * set_dvfs_period - Store callback for the dvfs_period sysfs file.
 * @dev:   The device with sysfs file is for
 * @attr:  The attributes of the sysfs file
 * @buf:   The value written to the sysfs file
 * @count: The number of bytes written to the sysfs file
 *
 * This function is called when the dvfs_period sysfs file is written to. It
 * checks the data written, and if valid updates the DVFS period variable,
 *
 * Return: @count if the function succeeded. An error code on failure.
 */
static ssize_t set_dvfs_period(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct kbase_device *kbdev;
	int ret;
	int dvfs_period;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	ret = kstrtoint(buf, 0, &dvfs_period);
	if (ret || dvfs_period <= 0) {
		dev_err(kbdev->dev, "Couldn't process dvfs_period write operation.\n"
				"Use format <dvfs_period_ms>\n");
		return -EINVAL;
	}

	kbdev->pm.dvfs_period = dvfs_period;
	dev_dbg(kbdev->dev, "DVFS period: %dms\n", dvfs_period);

	return count;
}

/**
 * show_dvfs_period - Show callback for the dvfs_period sysfs entry.
 * @dev:  The device this sysfs file is for.
 * @attr: The attributes of the sysfs file.
 * @buf:  The output buffer to receive the GPU information.
 *
 * This function is called to get the current period used for the DVFS sample
 * timer.
 *
 * Return: The number of bytes output to @buf.
 */
static ssize_t show_dvfs_period(struct device *dev,
		struct device_attribute *attr, char * const buf)
{
	struct kbase_device *kbdev;
	ssize_t ret;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	ret = scnprintf(buf, PAGE_SIZE, "%d\n", kbdev->pm.dvfs_period);

	return ret;
}

static DEVICE_ATTR(dvfs_period, S_IRUGO | S_IWUSR, show_dvfs_period,
		set_dvfs_period);

/**
 * set_pm_poweroff - Store callback for the pm_poweroff sysfs file.
 * @dev:   The device with sysfs file is for
 * @attr:  The attributes of the sysfs file
 * @buf:   The value written to the sysfs file
 * @count: The number of bytes written to the sysfs file
 *
 * This function is called when the pm_poweroff sysfs file is written to.
 *
 * This file contains three values separated by whitespace. The values
 * are gpu_poweroff_time (the period of the poweroff timer, in ns),
 * poweroff_shader_ticks (the number of poweroff timer ticks before an idle
 * shader is powered off), and poweroff_gpu_ticks (the number of poweroff timer
 * ticks before the GPU is powered off), in that order.
 *
 * Return: @count if the function succeeded. An error code on failure.
 */
static ssize_t set_pm_poweroff(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct kbase_device *kbdev;
	struct kbasep_pm_tick_timer_state *stt;
	int items;
	u64 gpu_poweroff_time;
	unsigned int poweroff_shader_ticks, poweroff_gpu_ticks;
	unsigned long flags;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	items = sscanf(buf, "%llu %u %u", &gpu_poweroff_time, /* unsafe_function_ignore: sscanf */
			&poweroff_shader_ticks,
			&poweroff_gpu_ticks);
	if (items != 3) {
		dev_err(kbdev->dev, "Couldn't process pm_poweroff write operation.\n"
				"Use format <gpu_poweroff_time_ns> <poweroff_shader_ticks> <poweroff_gpu_ticks>\n");
		return -EINVAL;
	}

	spin_lock_irqsave(&kbdev->hwaccess_lock, flags);

	stt = &kbdev->pm.backend.shader_tick_timer;
	stt->configured_interval = HR_TIMER_DELAY_NSEC(gpu_poweroff_time);
	stt->configured_ticks = poweroff_shader_ticks;

	spin_unlock_irqrestore(&kbdev->hwaccess_lock, flags);

	if (poweroff_gpu_ticks != 0)
		dev_warn(kbdev->dev, "Separate GPU poweroff delay no longer supported.\n");

	return count;
}

/**
 * show_pm_poweroff - Show callback for the pm_poweroff sysfs entry.
 * @dev:  The device this sysfs file is for.
 * @attr: The attributes of the sysfs file.
 * @buf:  The output buffer to receive the GPU information.
 *
 * This function is called to get the current period used for the DVFS sample
 * timer.
 *
 * Return: The number of bytes output to @buf.
 */
static ssize_t show_pm_poweroff(struct device *dev,
		struct device_attribute *attr, char * const buf)
{
	struct kbase_device *kbdev;
	struct kbasep_pm_tick_timer_state *stt;
	ssize_t ret;
	unsigned long flags;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	spin_lock_irqsave(&kbdev->hwaccess_lock, flags);

	stt = &kbdev->pm.backend.shader_tick_timer;
	ret = scnprintf(buf, PAGE_SIZE, "%llu %u 0\n",
			ktime_to_ns(stt->configured_interval),
			stt->configured_ticks);

	spin_unlock_irqrestore(&kbdev->hwaccess_lock, flags);

	return ret;
}

static DEVICE_ATTR(pm_poweroff, S_IRUGO | S_IWUSR, show_pm_poweroff,
		set_pm_poweroff);

/**
 * set_reset_timeout - Store callback for the reset_timeout sysfs file.
 * @dev:   The device with sysfs file is for
 * @attr:  The attributes of the sysfs file
 * @buf:   The value written to the sysfs file
 * @count: The number of bytes written to the sysfs file
 *
 * This function is called when the reset_timeout sysfs file is written to. It
 * checks the data written, and if valid updates the reset timeout.
 *
 * Return: @count if the function succeeded. An error code on failure.
 */
static ssize_t set_reset_timeout(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct kbase_device *kbdev;
	int ret;
	int reset_timeout;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	ret = kstrtoint(buf, 0, &reset_timeout);
	if (ret || reset_timeout <= 0) {
		dev_err(kbdev->dev, "Couldn't process reset_timeout write operation.\n"
				"Use format <reset_timeout_ms>\n");
		return -EINVAL;
	}

	kbdev->reset_timeout_ms = reset_timeout;
	dev_dbg(kbdev->dev, "Reset timeout: %dms\n", reset_timeout);

	return count;
}

/**
 * show_reset_timeout - Show callback for the reset_timeout sysfs entry.
 * @dev:  The device this sysfs file is for.
 * @attr: The attributes of the sysfs file.
 * @buf:  The output buffer to receive the GPU information.
 *
 * This function is called to get the current reset timeout.
 *
 * Return: The number of bytes output to @buf.
 */
static ssize_t show_reset_timeout(struct device *dev,
		struct device_attribute *attr, char * const buf)
{
	struct kbase_device *kbdev;
	ssize_t ret;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	ret = scnprintf(buf, PAGE_SIZE, "%d\n", kbdev->reset_timeout_ms);

	return ret;
}

static DEVICE_ATTR(reset_timeout, S_IRUGO | S_IWUSR, show_reset_timeout,
		set_reset_timeout);

#ifdef CONFIG_HISI_DEBUG_FS
static ssize_t show_mem_pool_size(struct device *dev,
		struct device_attribute *attr, char * const buf)
{
	struct kbase_device *const kbdev = to_kbase_device(dev);

	if (!kbdev)
		return -ENODEV;

	return kbase_debugfs_helper_get_attr_to_string(buf, PAGE_SIZE,
		kbdev->mem_pools.small, MEMORY_GROUP_MANAGER_NR_GROUPS,
		kbase_mem_pool_debugfs_size);
}

static ssize_t set_mem_pool_size(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct kbase_device *const kbdev = to_kbase_device(dev);
	int err;

	if (!kbdev)
		return -ENODEV;

	err = kbase_debugfs_helper_set_attr_from_string(buf,
		kbdev->mem_pools.small, MEMORY_GROUP_MANAGER_NR_GROUPS,
		kbase_mem_pool_debugfs_trim);

	return err ? err : count;
}

static DEVICE_ATTR(mem_pool_size, S_IRUGO | S_IWUSR, show_mem_pool_size,
		set_mem_pool_size);

static ssize_t show_mem_pool_max_size(struct device *dev,
		struct device_attribute *attr, char * const buf)
{
	struct kbase_device *const kbdev = to_kbase_device(dev);

	if (!kbdev)
		return -ENODEV;

	return kbase_debugfs_helper_get_attr_to_string(buf, PAGE_SIZE,
		kbdev->mem_pools.small, MEMORY_GROUP_MANAGER_NR_GROUPS,
		kbase_mem_pool_debugfs_max_size);
}

static ssize_t set_mem_pool_max_size(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct kbase_device *const kbdev = to_kbase_device(dev);
	int err;

	if (!kbdev)
		return -ENODEV;

	err = kbase_debugfs_helper_set_attr_from_string(buf,
		kbdev->mem_pools.small, MEMORY_GROUP_MANAGER_NR_GROUPS,
		kbase_mem_pool_debugfs_set_max_size);

	return err ? err : count;
}

static DEVICE_ATTR(mem_pool_max_size, S_IRUGO | S_IWUSR, show_mem_pool_max_size,
		set_mem_pool_max_size);

/**
 * show_lp_mem_pool_size - Show size of the large memory pages pool.
 * @dev:  The device this sysfs file is for.
 * @attr: The attributes of the sysfs file.
 * @buf:  The output buffer to receive the pool size.
 *
 * This function is called to get the number of large memory pages which currently populate the kbdev pool.
 *
 * Return: The number of bytes output to @buf.
 */
static ssize_t show_lp_mem_pool_size(struct device *dev,
		struct device_attribute *attr, char * const buf)
{
	struct kbase_device *const kbdev = to_kbase_device(dev);

	if (!kbdev)
		return -ENODEV;

	return kbase_debugfs_helper_get_attr_to_string(buf, PAGE_SIZE,
		kbdev->mem_pools.large, MEMORY_GROUP_MANAGER_NR_GROUPS,
		kbase_mem_pool_debugfs_size);
}

/**
 * set_lp_mem_pool_size - Set size of the large memory pages pool.
 * @dev:   The device this sysfs file is for.
 * @attr:  The attributes of the sysfs file.
 * @buf:   The value written to the sysfs file.
 * @count: The number of bytes written to the sysfs file.
 *
 * This function is called to set the number of large memory pages which should populate the kbdev pool.
 * This may cause existing pages to be removed from the pool, or new pages to be created and then added to the pool.
 *
 * Return: @count if the function succeeded. An error code on failure.
 */
static ssize_t set_lp_mem_pool_size(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct kbase_device *const kbdev = to_kbase_device(dev);
	int err;

	if (!kbdev)
		return -ENODEV;

	err = kbase_debugfs_helper_set_attr_from_string(buf,
		kbdev->mem_pools.large, MEMORY_GROUP_MANAGER_NR_GROUPS,
		kbase_mem_pool_debugfs_trim);

	return err ? err : count;
}

static DEVICE_ATTR(lp_mem_pool_size, S_IRUGO | S_IWUSR, show_lp_mem_pool_size,
		set_lp_mem_pool_size);

/**
 * show_lp_mem_pool_max_size - Show maximum size of the large memory pages pool.
 * @dev:  The device this sysfs file is for.
 * @attr: The attributes of the sysfs file.
 * @buf:  The output buffer to receive the pool size.
 *
 * This function is called to get the maximum number of large memory pages that the kbdev pool can possibly contain.
 *
 * Return: The number of bytes output to @buf.
 */
static ssize_t show_lp_mem_pool_max_size(struct device *dev,
		struct device_attribute *attr, char * const buf)
{
	struct kbase_device *const kbdev = to_kbase_device(dev);

	if (!kbdev)
		return -ENODEV;

	return kbase_debugfs_helper_get_attr_to_string(buf, PAGE_SIZE,
		kbdev->mem_pools.large, MEMORY_GROUP_MANAGER_NR_GROUPS,
		kbase_mem_pool_debugfs_max_size);
}

/**
 * set_lp_mem_pool_max_size - Set maximum size of the large memory pages pool.
 * @dev:   The device this sysfs file is for.
 * @attr:  The attributes of the sysfs file.
 * @buf:   The value written to the sysfs file.
 * @count: The number of bytes written to the sysfs file.
 *
 * This function is called to set the maximum number of large memory pages that the kbdev pool can possibly contain.
 *
 * Return: @count if the function succeeded. An error code on failure.
 */
static ssize_t set_lp_mem_pool_max_size(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct kbase_device *const kbdev = to_kbase_device(dev);
	int err;

	if (!kbdev)
		return -ENODEV;

	err = kbase_debugfs_helper_set_attr_from_string(buf,
		kbdev->mem_pools.large, MEMORY_GROUP_MANAGER_NR_GROUPS,
		kbase_mem_pool_debugfs_set_max_size);

	return err ? err : count;
}

static DEVICE_ATTR(lp_mem_pool_max_size, S_IRUGO | S_IWUSR, show_lp_mem_pool_max_size,
		set_lp_mem_pool_max_size);
#endif /* CONFIG_HISI_DEBUG_FS */

static ssize_t show_gpu_error_info(struct device *dev, struct device_attribute *attr, char * const buf)
{
	struct kbase_device *kbdev;
	ssize_t ret = 0;
	u32 total_error_num = 0;
	kbdev = to_kbase_device(dev);

	if (!kbdev)
		return -ENODEV;

	total_error_num = kbdev->error_num.page_fault +
					kbdev->error_num.gpu_fault +
					kbdev->error_num.soft_reset +
					kbdev->error_num.hard_reset;

	ret += scnprintf(buf + ret, PAGE_SIZE - ret,
			"page_fault num:%d\n",
			kbdev->error_num.page_fault);
	ret += scnprintf(buf + ret, PAGE_SIZE - ret,
			"gpu_fault num:%d\n",
			kbdev->error_num.gpu_fault);
	ret += scnprintf(buf + ret, PAGE_SIZE - ret,
			"soft_reset num:%d\n",
			kbdev->error_num.soft_reset);
	ret += scnprintf(buf + ret, PAGE_SIZE - ret,
			"hard_reset num:%d\n",
			kbdev->error_num.hard_reset);
	ret += scnprintf(buf + ret, PAGE_SIZE - ret,
			"total_error_num:%d\n",
			total_error_num);
	ret += scnprintf(buf + ret, PAGE_SIZE - ret,
			"last_time:%d\n",
			kbdev->error_num.ts/1000000000 + 1);

	return ret;
}

static ssize_t set_gpu_error_info(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	struct kbase_device *kbdev;

	kbdev = to_kbase_device(dev);

	if (!kbdev)
		return -ENODEV;

	kbdev->error_num.gpu_fault = 0;
	kbdev->error_num.soft_reset = 0;
	kbdev->error_num.page_fault = 0;
	kbdev->error_num.hard_reset = 0;
	kbdev->error_num.ts = 0;
	return count;
}

static DEVICE_ATTR(gpu_error_info, S_IRUGO | S_IWUSR, show_gpu_error_info, set_gpu_error_info);

/**
 * show_js_ctx_scheduling_mode - Show callback for js_ctx_scheduling_mode sysfs
 *                               entry.
 * @dev:  The device this sysfs file is for.
 * @attr: The attributes of the sysfs file.
 * @buf:  The output buffer to receive the context scheduling mode information.
 *
 * This function is called to get the context scheduling mode being used by JS.
 *
 * Return: The number of bytes output to @buf.
 */
static ssize_t show_js_ctx_scheduling_mode(struct device *dev,
		struct device_attribute *attr, char * const buf)
{
	struct kbase_device *kbdev;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	return scnprintf(buf, PAGE_SIZE, "%u\n", kbdev->js_ctx_scheduling_mode);
}

/**
 * set_js_ctx_scheduling_mode - Set callback for js_ctx_scheduling_mode sysfs
 *                              entry.
 * @dev:   The device this sysfs file is for.
 * @attr:  The attributes of the sysfs file.
 * @buf:   The value written to the sysfs file.
 * @count: The number of bytes written to the sysfs file.
 *
 * This function is called when the js_ctx_scheduling_mode sysfs file is written
 * to. It checks the data written, and if valid updates the ctx scheduling mode
 * being by JS.
 *
 * Return: @count if the function succeeded. An error code on failure.
 */
static ssize_t set_js_ctx_scheduling_mode(struct device *dev,
		struct device_attribute *attr, const char *buf, size_t count)
{
	struct kbase_context *kctx;
	u32 new_js_ctx_scheduling_mode;
	struct kbase_device *kbdev;
	unsigned long flags;
	int ret;

	kbdev = to_kbase_device(dev);
	if (!kbdev)
		return -ENODEV;

	ret = kstrtouint(buf, 0, &new_js_ctx_scheduling_mode);
	if (ret || new_js_ctx_scheduling_mode >= KBASE_JS_PRIORITY_MODE_COUNT) {
		dev_err(kbdev->dev, "Couldn't process js_ctx_scheduling_mode"
				" write operation.\n"
				"Use format <js_ctx_scheduling_mode>\n");
		return -EINVAL;
	}

	if (new_js_ctx_scheduling_mode == kbdev->js_ctx_scheduling_mode)
		return count;

	mutex_lock(&kbdev->kctx_list_lock);
	spin_lock_irqsave(&kbdev->hwaccess_lock, flags);

	/* Update the context priority mode */
	kbdev->js_ctx_scheduling_mode = new_js_ctx_scheduling_mode;

	/* Adjust priority of all the contexts as per the new mode */
	list_for_each_entry(kctx, &kbdev->kctx_list, kctx_list_link)
		kbase_js_update_ctx_priority(kctx);

	spin_unlock_irqrestore(&kbdev->hwaccess_lock, flags);
	mutex_unlock(&kbdev->kctx_list_lock);

	dev_dbg(kbdev->dev, "JS ctx scheduling mode: %u\n", new_js_ctx_scheduling_mode);

	return count;
}

static DEVICE_ATTR(js_ctx_scheduling_mode, S_IRUGO | S_IWUSR,
		show_js_ctx_scheduling_mode,
		set_js_ctx_scheduling_mode);
#ifdef CONFIG_HISI_DEBUG_FS

/* Number of entries in serialize_jobs_settings[] */
#define NR_SERIALIZE_JOBS_SETTINGS 5
/* Maximum string length in serialize_jobs_settings[].name */
#define MAX_SERIALIZE_JOBS_NAME_LEN 16

static struct
{
	char *name;
	u8 setting;
} serialize_jobs_settings[NR_SERIALIZE_JOBS_SETTINGS] = {
	{"none", 0},
	{"intra-slot", KBASE_SERIALIZE_INTRA_SLOT},
	{"inter-slot", KBASE_SERIALIZE_INTER_SLOT},
	{"full", KBASE_SERIALIZE_INTRA_SLOT | KBASE_SERIALIZE_INTER_SLOT},
	{"full-reset", KBASE_SERIALIZE_INTRA_SLOT | KBASE_SERIALIZE_INTER_SLOT |
			KBASE_SERIALIZE_RESET}
};

/**
 * kbasep_serialize_jobs_seq_show - Show callback for the serialize_jobs debugfs
 *                                  file
 * @sfile: seq_file pointer
 * @data:  Private callback data
 *
 * This function is called to get the contents of the serialize_jobs debugfs
 * file. This is a list of the available settings with the currently active one
 * surrounded by square brackets.
 *
 * Return: 0 on success, or an error code on error
 */
static int kbasep_serialize_jobs_seq_show(struct seq_file *sfile, void *data)
{
	struct kbase_device *kbdev = sfile->private;
	int i;

	CSTD_UNUSED(data);

	for (i = 0; i < NR_SERIALIZE_JOBS_SETTINGS; i++) {
		if (kbdev->serialize_jobs == serialize_jobs_settings[i].setting)
			seq_printf(sfile, "[%s] ",
					serialize_jobs_settings[i].name);
		else
			seq_printf(sfile, "%s ",
					serialize_jobs_settings[i].name);
	}

	seq_puts(sfile, "\n");

	return 0;
}

/**
 * kbasep_serialize_jobs_debugfs_write - Store callback for the serialize_jobs
 *                                       debugfs file.
 * @file:  File pointer
 * @ubuf:  User buffer containing data to store
 * @count: Number of bytes in user buffer
 * @ppos:  File position
 *
 * This function is called when the serialize_jobs debugfs file is written to.
 * It matches the requested setting against the available settings and if a
 * matching setting is found updates kbdev->serialize_jobs.
 *
 * Return: @count if the function succeeded. An error code on failure.
 */
static ssize_t kbasep_serialize_jobs_debugfs_write(struct file *file,
		const char __user *ubuf, size_t count, loff_t *ppos)
{
	struct seq_file *s = file->private_data;
	struct kbase_device *kbdev = s->private;
	char buf[MAX_SERIALIZE_JOBS_NAME_LEN];
	int i;
	bool valid = false;

	CSTD_UNUSED(ppos);

	count = min_t(size_t, sizeof(buf) - 1, count);
	if (copy_from_user(buf, ubuf, count))
		return -EFAULT;

	buf[count] = 0;

	for (i = 0; i < NR_SERIALIZE_JOBS_SETTINGS; i++) {
		if (sysfs_streq(serialize_jobs_settings[i].name, buf)) {
			kbdev->serialize_jobs =
					serialize_jobs_settings[i].setting;
			valid = true;
			break;
		}
	}

	if (!valid) {
		dev_err(kbdev->dev, "serialize_jobs: invalid setting\n");
		return -EINVAL;
	}

	return count;
}

/**
 * kbasep_serialize_jobs_debugfs_open - Open callback for the serialize_jobs
 *                                     debugfs file
 * @in:   inode pointer
 * @file: file pointer
 *
 * Return: Zero on success, error code on failure
 */
static int kbasep_serialize_jobs_debugfs_open(struct inode *in,
		struct file *file)
{
	return single_open(file, kbasep_serialize_jobs_seq_show, in->i_private);
}

static const struct file_operations kbasep_serialize_jobs_debugfs_fops = {
	.open = kbasep_serialize_jobs_debugfs_open,
	.read = seq_read,
	.write = kbasep_serialize_jobs_debugfs_write,
	.llseek = seq_lseek,
	.release = single_release,
};

#endif /* CONFIG_HISI_DEBUG_FS */

static void kbasep_protected_mode_hwcnt_disable_worker(struct work_struct *data)
{
	struct kbase_device *kbdev = container_of(data, struct kbase_device,
		protected_mode_hwcnt_disable_work);
	unsigned long flags;

	bool do_disable;

	spin_lock_irqsave(&kbdev->hwaccess_lock, flags);
	do_disable = !kbdev->protected_mode_hwcnt_desired &&
		!kbdev->protected_mode_hwcnt_disabled;
	spin_unlock_irqrestore(&kbdev->hwaccess_lock, flags);

	if (!do_disable)
		return;

	kbase_hwcnt_context_disable(kbdev->hwcnt_gpu_ctx);

	spin_lock_irqsave(&kbdev->hwaccess_lock, flags);
	do_disable = !kbdev->protected_mode_hwcnt_desired &&
		!kbdev->protected_mode_hwcnt_disabled;

	if (do_disable) {
		/* Protected mode state did not change while we were doing the
		 * disable, so commit the work we just performed and continue
		 * the state machine.
		 */
		kbdev->protected_mode_hwcnt_disabled = true;
		kbase_backend_slot_update(kbdev);
	} else {
		/* Protected mode state was updated while we were doing the
		 * disable, so we need to undo the disable we just performed.
		 */
		kbase_hwcnt_context_enable(kbdev->hwcnt_gpu_ctx);
	}

	spin_unlock_irqrestore(&kbdev->hwaccess_lock, flags);
}

static int kbasep_protected_mode_init(struct kbase_device *kbdev)
{
#ifdef CONFIG_OF
	struct device_node *protected_node;
	struct platform_device *pdev;
	struct protected_mode_device *protected_dev;
#endif

	if (kbase_hw_has_feature(kbdev, BASE_HW_FEATURE_PROTECTED_MODE)) {
		/* Use native protected ops */
		kbdev->protected_dev = kzalloc(sizeof(*kbdev->protected_dev),
				GFP_KERNEL);
		if (!kbdev->protected_dev)
			return -ENOMEM;
		kbdev->protected_dev->data = kbdev;
		kbdev->protected_ops = &kbase_native_protected_ops;
		kbdev->protected_mode_support = true;
		INIT_WORK(&kbdev->protected_mode_hwcnt_disable_work,
			kbasep_protected_mode_hwcnt_disable_worker);
		kbdev->protected_mode_hwcnt_desired = true;
		kbdev->protected_mode_hwcnt_disabled = false;
		return 0;
	}

	kbdev->protected_mode_support = false;

#ifdef CONFIG_OF
	protected_node = of_parse_phandle(kbdev->dev->of_node,
			"protected-mode-switcher", 0);

	if (!protected_node)
		protected_node = of_parse_phandle(kbdev->dev->of_node,
				"secure-mode-switcher", 0);

	if (!protected_node) {
		/* If protected_node cannot be looked up then we assume
		 * protected mode is not supported on this platform. */
		dev_info(kbdev->dev, "Protected mode not available\n");
		return 0;
	}

	pdev = of_find_device_by_node(protected_node);
	if (!pdev)
		return -EINVAL;

	protected_dev = platform_get_drvdata(pdev);
	if (!protected_dev)
		return -EPROBE_DEFER;

	kbdev->protected_ops = &protected_dev->ops;
	kbdev->protected_dev = protected_dev;

	if (kbdev->protected_ops) {
		int err;

		/* Make sure protected mode is disabled on startup */
		mutex_lock(&kbdev->pm.lock);
		err = kbdev->protected_ops->protected_mode_disable(
				kbdev->protected_dev);
		mutex_unlock(&kbdev->pm.lock);

		/* protected_mode_disable() returns -EINVAL if not supported */
		kbdev->protected_mode_support = (err != -EINVAL);
	}
#endif
	return 0;
}

static void kbasep_protected_mode_term(struct kbase_device *kbdev)
{
	if (kbase_hw_has_feature(kbdev, BASE_HW_FEATURE_PROTECTED_MODE)) {
		cancel_work_sync(&kbdev->protected_mode_hwcnt_disable_work);
		kfree(kbdev->protected_dev);
	}
}

#ifdef CONFIG_MALI_NO_MALI
static int kbase_common_reg_map(struct kbase_device *kbdev)
{
	return 0;
}
static void kbase_common_reg_unmap(struct kbase_device * const kbdev)
{
}
#else /* CONFIG_MALI_NO_MALI */
static int kbase_common_reg_map(struct kbase_device *kbdev)
{
	int err = 0;

	if (!request_mem_region(kbdev->reg_start, kbdev->reg_size, "mali")) {
		dev_err(kbdev->dev, "Register window unavailable\n");
		err = -EIO;
		goto out_region;
	}

	kbdev->reg = ioremap(kbdev->reg_start, kbdev->reg_size);
	if (!kbdev->reg) {
		dev_err(kbdev->dev, "Can't remap register window\n");
		err = -EINVAL;
		goto out_ioremap;
	}

	kbdev->crgreg = ioremap(SYS_REG_CRG_BASE_ADDR, SYS_REG_CRG_SIZE);
	if (!kbdev->crgreg) {
		dev_err(kbdev->dev, "Can't remap sys crg register window on platform\n");
		err = -EINVAL;
		goto out_crg_ioremap;
	}

	kbdev->pmctrlreg = ioremap(SYS_REG_PMCTRL_BASE_ADDR, SYS_REG_PMCTRL_SIZE);
	if (!kbdev->pmctrlreg) {
		dev_err(kbdev->dev, "Can't remap sys pmctrl register window on platform\n");
		err = -EINVAL;
		goto out_pmctrl_ioremap;
	}

	if (kbdev->gpu_chip_type == 2)
		kbdev->pctrlreg = ioremap(SYS_REG_PCTRL_BASE_ADDR_CS2, SYS_REG_PCTRL_SIZE);
	else
		kbdev->pctrlreg = ioremap(SYS_REG_PCTRL_BASE_ADDR, SYS_REG_PCTRL_SIZE);

	if (!kbdev->pctrlreg) {
		dev_err(kbdev->dev, "Can't remap sys pctrl register window on platform\n");
		err = -EINVAL;
		goto out_pctrl_ioremap;
	}

	return err;

out_pctrl_ioremap:
	iounmap(kbdev->pmctrlreg);

out_pmctrl_ioremap:
	iounmap(kbdev->crgreg);

out_crg_ioremap:
	iounmap(kbdev->reg);

 out_ioremap:
	release_mem_region(kbdev->reg_start, kbdev->reg_size);
 out_region:
	return err;
}

static void kbase_common_reg_unmap(struct kbase_device * const kbdev)
{
	iounmap(kbdev->crgreg);

	iounmap(kbdev->pmctrlreg);
	iounmap(kbdev->pctrlreg);

	if (kbdev->reg) {
		iounmap(kbdev->reg);
		release_mem_region(kbdev->reg_start, kbdev->reg_size);
		kbdev->reg = NULL;
		kbdev->reg_start = 0;
		kbdev->reg_size = 0;
	}
}
#endif /* CONFIG_MALI_NO_MALI */

static int registers_map(struct kbase_device * const kbdev)
{

		/* the first memory resource is the physical address of the GPU
		 * registers */
		struct platform_device *pdev = to_platform_device(kbdev->dev);
		struct resource *reg_res;
		int err;

		reg_res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
		if (!reg_res) {
			dev_err(kbdev->dev, "Invalid register resource\n");
			return -ENOENT;
		}

		kbdev->reg_start = reg_res->start;
		kbdev->reg_size = resource_size(reg_res);


		err = kbase_common_reg_map(kbdev);
		if (err) {
			dev_err(kbdev->dev, "Failed to map registers\n");
			return err;
		}

	return 0;
}

static void registers_unmap(struct kbase_device *kbdev)
{
	kbase_common_reg_unmap(kbdev);
}

static int power_control_init(struct platform_device *pdev)
{
	struct kbase_device *kbdev = to_kbase_device(&pdev->dev);
	int err = 0;

	if (!kbdev)
		return -ENODEV;

	kbdev->clock = clk_get(kbdev->dev, "clk_g3d");
	if (IS_ERR_OR_NULL(kbdev->clock)) {
		err = PTR_ERR(kbdev->clock);
		kbdev->clock = NULL;
		if (err == -EPROBE_DEFER) {
			dev_err(&pdev->dev, "Failed to get clock\n");
			goto fail;
		}
		dev_info(kbdev->dev, "Continuing without Mali clock control\n");
		/* Allow probe to continue without clock. */
	} else {
		err = clk_prepare_enable(kbdev->clock);
		if (err) {
			dev_err(kbdev->dev,
				"Failed to prepare and enable clock (%d)\n",
				err);
			goto fail;
		}
	}

#if defined(CONFIG_OF) && defined(CONFIG_PM_OPP)
	/* Register the OPPs if they are available in device tree */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0)) \
	|| defined(LSK_OPPV2_BACKPORT)
	err = dev_pm_opp_of_add_table(kbdev->dev);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 7, 0))
	err = of_init_opp_table(kbdev->dev);
#else
	err = 0;
#endif /* LINUX_VERSION_CODE */
	if (err)
		dev_dbg(kbdev->dev, "OPP table not found\n");
#endif /* CONFIG_OF && CONFIG_PM_OPP */

	return 0;

fail:

if (kbdev->clock != NULL) {
	clk_put(kbdev->clock);
	kbdev->clock = NULL;
}

	return err;
}

static void power_control_term(struct kbase_device *kbdev)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 0)) || \
		defined(LSK_OPPV2_BACKPORT)
	dev_pm_opp_of_remove_table(kbdev->dev);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0))
	of_free_opp_table(kbdev->dev);
#endif

	if (kbdev->clock) {
		clk_disable_unprepare(kbdev->clock);
		clk_put(kbdev->clock);
		kbdev->clock = NULL;
	}
}

#ifdef MALI_KBASE_BUILD
#ifdef CONFIG_HISI_DEBUG_FS

#include <mali_kbase_hwaccess_jm.h>

static void trigger_quirks_reload(struct kbase_device *kbdev)
{
	kbase_pm_context_active(kbdev);
	if (kbase_prepare_to_reset_gpu(kbdev))
		kbase_reset_gpu(kbdev);
	kbase_pm_context_idle(kbdev);
}

#define MAKE_QUIRK_ACCESSORS(type) \
static int type##_quirks_set(void *data, u64 val) \
{ \
	struct kbase_device *kbdev; \
	kbdev = (struct kbase_device *)data; \
	kbdev->hw_quirks_##type = (u32)val; \
	trigger_quirks_reload(kbdev); \
	return 0;\
} \
\
static int type##_quirks_get(void *data, u64 *val) \
{ \
	struct kbase_device *kbdev;\
	kbdev = (struct kbase_device *)data;\
	*val = kbdev->hw_quirks_##type;\
	return 0;\
} \
DEFINE_SIMPLE_ATTRIBUTE(fops_##type##_quirks, type##_quirks_get,\
		type##_quirks_set, "%llu\n")

MAKE_QUIRK_ACCESSORS(sc);
MAKE_QUIRK_ACCESSORS(tiler);
MAKE_QUIRK_ACCESSORS(mmu);
MAKE_QUIRK_ACCESSORS(jm);


/**
 * debugfs_protected_debug_mode_read - "protected_debug_mode" debugfs read
 * @file: File object to read is for
 * @buf:  User buffer to populate with data
 * @len:  Length of user buffer
 * @ppos: Offset within file object
 *
 * Retrieves the current status of protected debug mode
 * (0 = disabled, 1 = enabled)
 *
 * Return: Number of bytes added to user buffer
 */
static ssize_t debugfs_protected_debug_mode_read(struct file *file,
				char __user *buf, size_t len, loff_t *ppos)
{
	struct kbase_device *kbdev = (struct kbase_device *)file->private_data;
	u32 gpu_status;
	ssize_t ret_val;

	kbase_pm_context_active(kbdev);
	gpu_status = kbase_reg_read(kbdev, GPU_CONTROL_REG(GPU_STATUS));
	kbase_pm_context_idle(kbdev);

	if (gpu_status & GPU_DBGEN)
		ret_val = simple_read_from_buffer(buf, len, ppos, "1\n", 2);
	else
		ret_val = simple_read_from_buffer(buf, len, ppos, "0\n", 2);

	return ret_val;
}

/*
 * struct fops_protected_debug_mode - "protected_debug_mode" debugfs fops
 *
 * Contains the file operations for the "protected_debug_mode" debugfs file
 */
static const struct file_operations fops_protected_debug_mode = {
	.open = simple_open,
	.read = debugfs_protected_debug_mode_read,
	.llseek = default_llseek,
};

static int kbase_device_debugfs_mem_pool_max_size_show(struct seq_file *sfile,
	void *data)
{
	CSTD_UNUSED(data);
	return kbase_debugfs_helper_seq_read(sfile,
		MEMORY_GROUP_MANAGER_NR_GROUPS,
		kbase_mem_pool_config_debugfs_max_size);
}

static ssize_t kbase_device_debugfs_mem_pool_max_size_write(struct file *file,
		const char __user *ubuf, size_t count, loff_t *ppos)
{
	int err = 0;

	CSTD_UNUSED(ppos);
	err = kbase_debugfs_helper_seq_write(file, ubuf, count,
		MEMORY_GROUP_MANAGER_NR_GROUPS,
		kbase_mem_pool_config_debugfs_set_max_size);

	return err ? err : count;
}

static int kbase_device_debugfs_mem_pool_max_size_open(struct inode *in,
	struct file *file)
{
	return single_open(file, kbase_device_debugfs_mem_pool_max_size_show,
		in->i_private);
}

static const struct file_operations
	kbase_device_debugfs_mem_pool_max_size_fops = {
	.owner = THIS_MODULE,
	.open = kbase_device_debugfs_mem_pool_max_size_open,
	.read = seq_read,
	.write = kbase_device_debugfs_mem_pool_max_size_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static int kbase_device_debugfs_init(struct kbase_device *kbdev)
{
	struct dentry *debugfs_ctx_defaults_directory;
	int err;

	kbdev->mali_debugfs_directory = debugfs_create_dir(kbdev->devname,
			NULL);
	if (!kbdev->mali_debugfs_directory) {
		dev_err(kbdev->dev, "Couldn't create mali debugfs directory\n");
		err = -ENOMEM;
		goto out;
	}

	kbdev->debugfs_ctx_directory = debugfs_create_dir("ctx",
			kbdev->mali_debugfs_directory);
	if (!kbdev->debugfs_ctx_directory) {
		dev_err(kbdev->dev, "Couldn't create mali debugfs ctx directory\n");
		err = -ENOMEM;
		goto out;
	}

	debugfs_ctx_defaults_directory = debugfs_create_dir("defaults",
			kbdev->debugfs_ctx_directory);
	if (!debugfs_ctx_defaults_directory) {
		dev_err(kbdev->dev, "Couldn't create mali debugfs ctx defaults directory\n");
		err = -ENOMEM;
		goto out;
	}

#if !MALI_CUSTOMER_RELEASE
	kbasep_regs_dump_debugfs_init(kbdev);
#endif /* !MALI_CUSTOMER_RELEASE */
	kbasep_regs_history_debugfs_init(kbdev);

	kbase_debug_job_fault_debugfs_init(kbdev);
	kbasep_gpu_memory_debugfs_init(kbdev);
	kbase_as_fault_debugfs_init(kbdev);
	/* fops_* variables created by invocations of macro
	 * MAKE_QUIRK_ACCESSORS() above. */
	debugfs_create_file("quirks_sc", 0644,
			kbdev->mali_debugfs_directory, kbdev,
			&fops_sc_quirks);
	debugfs_create_file("quirks_tiler", 0644,
			kbdev->mali_debugfs_directory, kbdev,
			&fops_tiler_quirks);
	debugfs_create_file("quirks_mmu", 0644,
			kbdev->mali_debugfs_directory, kbdev,
			&fops_mmu_quirks);
	debugfs_create_file("quirks_jm", 0644,
			kbdev->mali_debugfs_directory, kbdev,
			&fops_jm_quirks);

	debugfs_create_bool("infinite_cache", 0644,
			debugfs_ctx_defaults_directory,
			&kbdev->infinite_cache_active_default);

	debugfs_create_file("mem_pool_max_size", 0644,
			debugfs_ctx_defaults_directory,
			&kbdev->mem_pool_defaults.small,
			&kbase_device_debugfs_mem_pool_max_size_fops);

	debugfs_create_file("lp_mem_pool_max_size", 0644,
			debugfs_ctx_defaults_directory,
			&kbdev->mem_pool_defaults.large,
			&kbase_device_debugfs_mem_pool_max_size_fops);

	if (kbase_hw_has_feature(kbdev, BASE_HW_FEATURE_PROTECTED_DEBUG_MODE)) {
		debugfs_create_file("protected_debug_mode", S_IRUGO,
				kbdev->mali_debugfs_directory, kbdev,
				&fops_protected_debug_mode);
	}

#if KBASE_TRACE_ENABLE
	kbasep_trace_debugfs_init(kbdev);
#endif /* KBASE_TRACE_ENABLE */

#ifdef CONFIG_MALI_DEVFREQ
#ifdef CONFIG_DEVFREQ_THERMAL
	if (kbdev->inited_subsys & inited_devfreq)
		kbase_ipa_debugfs_init(kbdev);
#endif /* CONFIG_DEVFREQ_THERMAL */
#endif /* CONFIG_MALI_DEVFREQ */

#ifdef CONFIG_HISI_DEBUG_FS
	debugfs_create_file("serialize_jobs", S_IRUGO | S_IWUSR,
			kbdev->mali_debugfs_directory, kbdev,
			&kbasep_serialize_jobs_debugfs_fops);
#endif /* CONFIG_HISI_DEBUG_FS */

	return 0;

out:
	debugfs_remove_recursive(kbdev->mali_debugfs_directory);
	return err;
}

static void kbase_device_debugfs_term(struct kbase_device *kbdev)
{
	debugfs_remove_recursive(kbdev->mali_debugfs_directory);
}

#else /* CONFIG_HISI_DEBUG_FS */
static inline int kbase_device_debugfs_init(struct kbase_device *kbdev)
{
	kbasep_gpu_memory_debugfs_init(kbdev);
	return 0;
}

static inline void kbase_device_debugfs_term(struct kbase_device *kbdev) { }
#endif /* CONFIG_HISI_DEBUG_FS */
#endif /* MALI_KBASE_BUILD */

static void kbase_device_coherency_init(struct kbase_device *kbdev,
		unsigned prod_id)
{
#ifdef CONFIG_OF
	u32 supported_coherency_bitmap =
		kbdev->gpu_props.props.raw_props.coherency_mode;
	const void *coherency_override_dts;
	u32 override_coherency;

	/* Only for tMIx :
	 * (COHERENCY_ACE_LITE | COHERENCY_ACE) was incorrectly
	 * documented for tMIx so force correct value here.
	 */
	if (GPU_ID_IS_NEW_FORMAT(prod_id) &&
		   (GPU_ID2_MODEL_MATCH_VALUE(prod_id) ==
				   GPU_ID2_PRODUCT_TMIX))
		if (supported_coherency_bitmap ==
				COHERENCY_FEATURE_BIT(COHERENCY_ACE))
			supported_coherency_bitmap |=
				COHERENCY_FEATURE_BIT(COHERENCY_ACE_LITE);

#endif /* CONFIG_OF */

	kbdev->system_coherency = COHERENCY_NONE;

	/* device tree may override the coherency */
#ifdef CONFIG_OF
	coherency_override_dts = of_get_property(kbdev->dev->of_node,
						"system-coherency",
						NULL);
	if (coherency_override_dts) {

		override_coherency = be32_to_cpup(coherency_override_dts);

		if ((override_coherency <= COHERENCY_NONE) &&
			(supported_coherency_bitmap &
			 COHERENCY_FEATURE_BIT(override_coherency))) {

			kbdev->system_coherency = override_coherency;

			dev_info(kbdev->dev,
				"Using coherency mode %u set from dtb",
				override_coherency);
		} else
			dev_warn(kbdev->dev,
				"Ignoring unsupported coherency mode %u set from dtb",
				override_coherency);
	}

#endif /* CONFIG_OF */

	kbdev->gpu_props.props.raw_props.coherency_mode =
		kbdev->system_coherency;
}

#ifdef CONFIG_MALI_BUSLOG

/* Callback used by the kbase bus logger client, to initiate a GPU reset
 * when the bus log is restarted.  GPU reset is used as reference point
 * in HW bus log analyses.
 */
static void kbase_logging_started_cb(void *data)
{
	struct kbase_device *kbdev = (struct kbase_device *)data;

	if (kbase_prepare_to_reset_gpu(kbdev))
		kbase_reset_gpu(kbdev);
	dev_info(kbdev->dev, "KBASE - Bus logger restarted\n");
}
#endif

static struct attribute *kbase_attrs[] = {
#ifdef CONFIG_MALI_DEBUG
	&dev_attr_debug_command.attr,
	&dev_attr_js_softstop_always.attr,
#endif
#if !MALI_CUSTOMER_RELEASE
	&dev_attr_force_replay.attr,
#endif
	&dev_attr_js_timeouts.attr,
	&dev_attr_soft_job_timeout.attr,
	&dev_attr_gpuinfo.attr,
	&dev_attr_dvfs_period.attr,
	&dev_attr_pm_poweroff.attr,
	&dev_attr_reset_timeout.attr,
	&dev_attr_js_scheduling_period.attr,
	&dev_attr_power_policy.attr,
	&dev_attr_core_mask.attr,
#ifdef CONFIG_HISI_DEBUG_FS
	&dev_attr_mem_pool_size.attr,
	&dev_attr_mem_pool_max_size.attr,
	&dev_attr_lp_mem_pool_size.attr,
	&dev_attr_lp_mem_pool_max_size.attr,
#endif
	&dev_attr_js_ctx_scheduling_mode.attr,
	&dev_attr_gpu_error_info.attr,
	NULL
};

static const struct attribute_group kbase_attr_group = {
	.attrs = kbase_attrs,
};

static int kbase_platform_device_remove(struct platform_device *pdev)
{
	struct kbase_device *kbdev = to_kbase_device(&pdev->dev);
	const struct list_head *dev_list;

	if (!kbdev)
		return -ENODEV;

	kfree(kbdev->gpu_props.prop_buffer);

#ifdef CONFIG_MALI_BUSLOG
	if (kbdev->inited_subsys & inited_buslogger) {
		bl_core_client_unregister(kbdev->buslogger);
		kbdev->inited_subsys &= ~inited_buslogger;
	}
#endif

	if (kbdev->inited_subsys & inited_dev_list) {
		dev_list = kbase_dev_list_get();
		list_del(&kbdev->entry);
		kbase_dev_list_put(dev_list);
		kbdev->inited_subsys &= ~inited_dev_list;
	}

	if (kbdev->inited_subsys & inited_misc_register) {
		misc_deregister(&kbdev->mdev);
		kbdev->inited_subsys &= ~inited_misc_register;
	}

	if (kbdev->inited_subsys & inited_sysfs_group) {
		sysfs_remove_group(&kbdev->dev->kobj, &kbase_attr_group);
		kbdev->inited_subsys &= ~inited_sysfs_group;
	}

	if (kbdev->inited_subsys & inited_get_device) {
		put_device(kbdev->dev);
		kbdev->inited_subsys &= ~inited_get_device;
	}

#ifdef MALI_KBASE_BUILD
	if (kbdev->inited_subsys & inited_debugfs) {
		kbase_device_debugfs_term(kbdev);
		kbdev->inited_subsys &= ~inited_debugfs;
	}
#endif

	if (kbdev->inited_subsys & inited_job_fault) {
		kbase_debug_job_fault_dev_term(kbdev);
		kbdev->inited_subsys &= ~inited_job_fault;
	}


	if (kbdev->inited_subsys & inited_backend_late) {
		kbase_backend_late_term(kbdev);
		kbdev->inited_subsys &= ~inited_backend_late;
	}

	if (kbdev->inited_subsys & inited_vinstr) {
		kbase_vinstr_term(kbdev->vinstr_ctx);
		kbdev->inited_subsys &= ~inited_vinstr;
	}

	if (kbdev->inited_subsys & inited_hwcnt_gpu_virt) {
		kbase_hwcnt_virtualizer_term(kbdev->hwcnt_gpu_virt);
		kbdev->inited_subsys &= ~inited_hwcnt_gpu_virt;
	}

	if (kbdev->inited_subsys & inited_hwcnt_gpu_ctx) {
		kbase_hwcnt_context_term(kbdev->hwcnt_gpu_ctx);
		kbdev->inited_subsys &= ~inited_hwcnt_gpu_ctx;
	}

	if (kbdev->inited_subsys & inited_hwcnt_gpu_iface) {
		kbase_hwcnt_backend_gpu_destroy(&kbdev->hwcnt_gpu_iface);
		kbdev->inited_subsys &= ~inited_hwcnt_gpu_iface;
	}

	if (kbdev->inited_subsys & inited_tlstream) {
		kbase_timeline_term(kbdev->timeline);
		kbdev->inited_subsys &= ~inited_tlstream;
	}

	/* Bring job and mem sys to a halt before we continue termination */

	if (kbdev->inited_subsys & inited_js)
		kbasep_js_devdata_halt(kbdev);

	if (kbdev->inited_subsys & inited_mem)
		kbase_mem_halt(kbdev);

	if (kbdev->inited_subsys & inited_protected) {
		kbasep_protected_mode_term(kbdev);
		kbdev->inited_subsys &= ~inited_protected;
	}

	if (kbdev->inited_subsys & inited_js) {
		kbasep_js_devdata_term(kbdev);
		kbdev->inited_subsys &= ~inited_js;
	}

	if (kbdev->inited_subsys & inited_mem) {
		kbase_mem_term(kbdev);
		kbdev->inited_subsys &= ~inited_mem;
	}

	if (kbdev->inited_subsys & inited_ctx_sched) {
		kbase_ctx_sched_term(kbdev);
		kbdev->inited_subsys &= ~inited_ctx_sched;
	}

	if (kbdev->inited_subsys & inited_device) {
		kbase_device_term(kbdev);
		kbdev->inited_subsys &= ~inited_device;
	}

	if (kbdev->inited_subsys & inited_backend_early) {
		kbase_backend_early_term(kbdev);
		kbdev->inited_subsys &= ~inited_backend_early;
	}

	if (kbdev->inited_subsys & inited_io_history) {
		kbase_io_history_term(&kbdev->io_history);
		kbdev->inited_subsys &= ~inited_io_history;
	}

	if (kbdev->inited_subsys & inited_power_control) {
		power_control_term(kbdev);
		kbdev->inited_subsys &= ~inited_power_control;
	}

	if (kbdev->inited_subsys & inited_registers_map) {
		registers_unmap(kbdev);
		kbdev->inited_subsys &= ~inited_registers_map;
	}

#ifdef CONFIG_MALI_NO_MALI
	if (kbdev->inited_subsys & inited_gpu_device) {
		gpu_device_destroy(kbdev);
		kbdev->inited_subsys &= ~inited_gpu_device;
	}
#endif /* CONFIG_MALI_NO_MALI */

	if (kbdev->inited_subsys != 0)
		dev_err(kbdev->dev, "Missing sub system termination\n");

#ifdef CONFIG_MALI_BOUND_REPORT
		kfifo_free(&kbdev->bound_report_info.bound_fifo);
#endif

	kbase_device_free(kbdev);

	return 0;
}

void kbase_backend_devfreq_term(struct kbase_device *kbdev)
{
#ifdef CONFIG_MALI_DEVFREQ
	if (kbdev->inited_subsys & inited_devfreq) {
		kbase_devfreq_term(kbdev);
		kbdev->inited_subsys &= ~inited_devfreq;
	}
#endif
}

int kbase_backend_devfreq_init(struct kbase_device *kbdev)
{
#ifdef CONFIG_MALI_DEVFREQ
	/* Devfreq uses hardware counters, so must be initialized after it. */
	int err = kbase_devfreq_init(kbdev);

	if (!err)
		kbdev->inited_subsys |= inited_devfreq;
	else
		dev_err(kbdev->dev, "Continuing without devfreq\n");
#endif /* CONFIG_MALI_DEVFREQ */
	return 0;
}

/* Number of register accesses for the buffer that we allocate during
 * initialization time. The buffer size can be changed later via debugfs. */
#define KBASEP_DEFAULT_REGISTER_HISTORY_SIZE ((u16)512)

#ifdef CONFIG_HUAWEI_DSM
static struct dsm_dev dsm_gpu =
{
	.name = "dsm_gpu",
	.device_name = "gpu",
	.ic_name = NULL,
	.module_name = NULL,
	.fops = NULL,
	.buff_size = 1024,
};
#endif

static int initlize_device_tree(struct kbase_device * const kbdev)
{
	struct device_node *np = NULL;
	int ret;

	KBASE_DEBUG_ASSERT(kbdev != NULL);

	/* init soc special flag */
	kbdev->gpu_outstanding = 0;
	kbdev->gpu_fpga_exist = 0;
	kbdev->gpu_chip_type = 0;
	kbdev->lb_enabled = 0;
	kbdev->shader_present_lo_cfg = 0;
#ifdef CONFIG_OF
	/* read outstanding value from dts*/
	np = of_find_compatible_node(NULL, NULL, "arm,mali-midgard");
	if (np) {
		ret = of_property_read_u32(np, "gpu_outstanding", &kbdev->gpu_outstanding);
		if (ret)
			dev_warn(kbdev->dev,
			"Failed to get outstanding limitation setting, by using default value\n");

		ret = of_property_read_u32(np, "fpga-gpu-exist", &kbdev->gpu_fpga_exist);
		if (ret)
			dev_warn(kbdev->dev,
			"Failed to get fpga gpu exist setting, assume that always exist\n");

		ret = of_property_read_u32(np, "last-buffer", &kbdev->lb_enabled);
		if (ret)
			dev_warn(kbdev->dev, "Failed to get last buffer setting.\n");

		ret = of_property_read_u32(np, "gpu-chip-type", &kbdev->gpu_chip_type);
		if (ret)
			dev_warn(kbdev->dev,
			"Failed to get gpu chip type setting, default means cs\n");
		ret = of_property_read_u32(np, "shader-present-lo-cfg",
			&kbdev->shader_present_lo_cfg);
		if (ret)
			dev_warn(kbdev->dev, "Failed to get shader present configuration!\n");
	} else {
		dev_err(kbdev->dev,
		"not find device node arm,mali-midgard!\n");
	}
#endif

	return 0;
}

static void hisi_gpu_special_init(struct kbase_device *kbdev)
{
	unsigned int value;

	if (kbase_has_hi_feature(kbdev, KBASE_FEATURE_HI0006)) {
		/* GPU and PMCTRL shader core power on/off decrease freq handshake start
		 * read G3DHPMBYPASS and set it's [3:0] and [19:16]bit to 0,
		 * enable G3D HPM hardware status contrl
		 */
		value = readl(kbdev->pmctrlreg + G3DHPMBYPASS) & MASK_G3DHPMBYPASS;
		writel(value, kbdev->pmctrlreg + G3DHPMBYPASS);

		/* read G3DAUTOCLKDIVBYPASS and set it's [0]bit to 0,
		 * enable G3D auto clkdiv
		 */
		value = readl(kbdev->pmctrlreg + G3DAUTOCLKDIVBYPASS) & MASK_G3DAUTOCLKDIVBYPASS;
		writel(value, kbdev->pmctrlreg + G3DAUTOCLKDIVBYPASS);
		/* GPU and PMCTRL shader core power on/off decrease freq handshake end */

		/* GPU IDLE VDM decrease freq start */
		/* read VS_CTRL_2 and set it's [26]bit to 1,
		 * enable L2 reduce freq when it IDLE
		 */
		value = readl(kbdev->pmctrlreg + VS_CTRL_2) | (1<<26);
		writel(value, kbdev->pmctrlreg + VS_CTRL_2);
		/* GPU IDLE VDM decrease freq end */
	}

	if (kbase_has_hi_feature(kbdev, KBASE_FEATURE_HI0010)) {
		/* GPU and PMCTRL shader core power on/off decrease freq handshake start */
		/* read G3DAUTOCLKDIVBYPASS and set it's [0]bit to 0,
		 * enable G3D auto clkdiv
		 */
		value = readl(kbdev->pmctrlreg + G3DAUTOCLKDIVBYPASS) & MASK_G3DAUTOCLKDIVBYPASS;
		writel(value, kbdev->pmctrlreg + G3DAUTOCLKDIVBYPASS);
		/* GPU and PMCTRL shader core power on/off decrease freq handshake end */

		/* GPU IDLE VDM decrease freq start */
		/* read VS_CTRL_2 and set it's [26]bit to 1,
		 * enable L2 reduce freq when it IDLE
		 */
		value = readl(kbdev->pmctrlreg + VS_CTRL_2) | (1<<26);
		writel(value, kbdev->pmctrlreg + VS_CTRL_2);
		/* GPU IDLE VDM decrease freq end */
	}

	if (kbase_has_hi_feature(kbdev, KBASE_FEATURE_HI0011)) {
		/* GPU and PMCTRL shader core power on/off decrease freq handshake start */
		/* read G3DAUTOCLKDIVBYPASS and set it's [0]bit to 0
		 * enable G3D auto clkdiv
		 */
		value = readl(kbdev->pmctrlreg + G3DAUTOCLKDIVBYPASS_2) & MASK_G3DAUTOCLKDIVBYPASS;
		writel(value, kbdev->pmctrlreg + G3DAUTOCLKDIVBYPASS_2);
		/* GPU and PMCTRL shader core power on/off decrease freq handshake end */

		/* GPU IDLE VDM decrease freq start */
		/* read VS_CTRL_2 and set it's [26]bit to 1
		 * enable L2 reduce freq when it IDLE
		 */
		value = readl(kbdev->pmctrlreg + VS_CTRL_GPU) | (1<<26);
		writel(value, kbdev->pmctrlreg + VS_CTRL_GPU);
		/* GPU IDLE VDM decrease freq end */
	}

	if (kbase_has_hi_feature(kbdev, KBASE_FEATURE_HI0012)) {
		/* for sigurd, set PCTRL93 bit16/17/18 to 1
		 * enable mem auto shutdown
		 */
		value = readl(kbdev->pctrlreg + PERI_CTRL93) | MASK_PERI_CTRL93;
		writel(value, kbdev->pctrlreg + PERI_CTRL93);
	}

	/* norr cs, trym es, gondul */
	if (kbase_has_hi_feature(kbdev, KBASE_FEATURE_HI0015)) {
		/* enable auto shutdown by hardware  */
		/* norr cs read PERI_CTRL93 set [31]bit to 1
		 * trym read PERI_CTRL93 and gondul read PERI_CTRL91 set [1]bit to 1
		 */
		value = readl(kbdev->pctrlreg + HW_AUTO_SHUTDOWN) | MASK_AUTOSDBYHW;
		writel(value, kbdev->pctrlreg + HW_AUTO_SHUTDOWN);
	}

	/* trym es */
	if (kbase_has_hi_feature(kbdev, KBASE_FEATURE_HI0017)) {
		/* read PERI_CTRL19 and set it's [11:9]bit to 1
		 * set striping granule to hash function with 256 byte
		 */
		value = readl(kbdev->pctrlreg + PERI_CTRL19) | MASK_HASH_STRIPING_GRANULE;
		writel(value, kbdev->pctrlreg + PERI_CTRL19);
	}

	return;
}

static int kbase_platform_device_probe(struct platform_device *pdev)
{
	struct kbase_device *kbdev;
	struct mali_base_gpu_core_props *core_props;
	u32 gpu_id;
	unsigned prod_id;
	const struct list_head *dev_list;
	int err;

	kbdev = kbase_device_alloc();
	if (!kbdev) {
		dev_err(&pdev->dev, "Allocate device failed\n");
		kbase_platform_device_remove(pdev);
		return -ENOMEM;
	}

	spin_lock_init(&kbdev->ctx_list_head_lock);
	INIT_LIST_HEAD(&kbdev->ctx_list_head);

	kbdev->dev = &pdev->dev;
	dev_set_drvdata(kbdev->dev, kbdev);

#ifdef CONFIG_MALI_NO_MALI
	err = gpu_device_create(kbdev);
	if (err) {
		dev_err(&pdev->dev, "Dummy model initialization failed\n");
		kbase_platform_device_remove(pdev);
		return err;
	}
	kbdev->inited_subsys |= inited_gpu_device;
#endif /* CONFIG_MALI_NO_MALI */

	err = assign_irqs(pdev);
	if (err) {
		dev_err(&pdev->dev, "IRQ search failed\n");
		kbase_platform_device_remove(pdev);
		return err;
	}

	err = initlize_device_tree(kbdev);
	if (err) {
		dev_err(&pdev->dev, "Init special devce tree setting failed\n");
		kbase_platform_device_remove(pdev);
		return err;
	}

	err = registers_map(kbdev);
	if (err) {
		dev_err(&pdev->dev, "Register map failed\n");
		kbase_platform_device_remove(pdev);
		return err;
	}
	kbdev->inited_subsys |= inited_registers_map;

	err = power_control_init(pdev);
	if (err) {
		dev_err(&pdev->dev, "Power control initialization failed\n");
		kbase_platform_device_remove(pdev);
		return err;
	}
	kbdev->inited_subsys |= inited_power_control;

	err = kbase_io_history_init(&kbdev->io_history,
			KBASEP_DEFAULT_REGISTER_HISTORY_SIZE);
	if (err) {
		dev_err(&pdev->dev, "Register access history initialization failed\n");
		kbase_platform_device_remove(pdev);
		return -ENOMEM;
	}
	kbdev->inited_subsys |= inited_io_history;

	err = kbase_backend_early_init(kbdev);
	if (err) {
		dev_err(kbdev->dev, "Early backend initialization failed\n");
		kbase_platform_device_remove(pdev);
		return err;
	}
	kbdev->inited_subsys |= inited_backend_early;

	scnprintf(kbdev->devname, DEVNAME_SIZE, "%s%d", kbase_drv_name,
			kbase_dev_nr);
	kbdev->id = kbase_dev_nr;

	kbase_disjoint_init(kbdev);

	/* obtain max configured gpu frequency, if devfreq is enabled then
	 * this will be overridden by the highest operating point found
	 */
	core_props = &(kbdev->gpu_props.props.core_props);
#ifdef GPU_FREQ_KHZ_MAX
	core_props->gpu_freq_khz_max = GPU_FREQ_KHZ_MAX;
#else
	core_props->gpu_freq_khz_max = DEFAULT_GPU_FREQ_KHZ_MAX;
#endif

	err = kbase_device_init(kbdev);
	if (err) {
		dev_err(kbdev->dev, "Device initialization failed (%d)\n", err);
		kbase_platform_device_remove(pdev);
		return err;
	}
	kbdev->inited_subsys |= inited_device;

	err = kbase_ctx_sched_init(kbdev);
	if (err) {
		dev_err(kbdev->dev, "Context scheduler initialization failed (%d)\n",
				err);
		kbase_platform_device_remove(pdev);
		return err;
	}
	kbdev->inited_subsys |= inited_ctx_sched;

	err = kbase_mem_init(kbdev);
	if (err) {
		dev_err(kbdev->dev, "Memory subsystem initialization failed\n");
		kbase_platform_device_remove(pdev);
		return err;
	}
	kbdev->inited_subsys |= inited_mem;

	gpu_id = kbdev->gpu_props.props.raw_props.gpu_id;
	gpu_id &= GPU_ID_VERSION_PRODUCT_ID;
	prod_id = gpu_id >> GPU_ID_VERSION_PRODUCT_ID_SHIFT;

	kbase_device_coherency_init(kbdev, prod_id);

	err = kbasep_protected_mode_init(kbdev);
	if (err) {
		dev_err(kbdev->dev, "Protected mode subsystem initialization failed\n");
		kbase_platform_device_remove(pdev);
		return err;
	}
	kbdev->inited_subsys |= inited_protected;

	dev_list = kbase_dev_list_get();
	list_add(&kbdev->entry, &kbase_dev_list);
	kbase_dev_list_put(dev_list);
	kbdev->inited_subsys |= inited_dev_list;

	err = kbasep_js_devdata_init(kbdev);
	if (err) {
		dev_err(kbdev->dev, "Job JS devdata initialization failed\n");
		kbase_platform_device_remove(pdev);
		return err;
	}
	kbdev->inited_subsys |= inited_js;

	atomic_set(&kbdev->timeline_is_enabled, 0);
	err = kbase_timeline_init(&kbdev->timeline, &kbdev->timeline_is_enabled);
	if (err) {
		dev_err(kbdev->dev, "Timeline stream initialization failed\n");
		kbase_platform_device_remove(pdev);
		return err;
	}
	kbdev->inited_subsys |= inited_tlstream;

	err = kbase_hwcnt_backend_gpu_create(kbdev, &kbdev->hwcnt_gpu_iface);
	if (err) {
		dev_err(kbdev->dev, "GPU hwcnt backend creation failed\n");
		kbase_platform_device_remove(pdev);
		return err;
	}
	kbdev->inited_subsys |= inited_hwcnt_gpu_iface;

	err = kbase_hwcnt_context_init(&kbdev->hwcnt_gpu_iface,
		&kbdev->hwcnt_gpu_ctx);
	if (err) {
		dev_err(kbdev->dev,
			"GPU hwcnt context initialization failed\n");
		kbase_platform_device_remove(pdev);
		return err;
	}
	kbdev->inited_subsys |= inited_hwcnt_gpu_ctx;

	err = kbase_hwcnt_virtualizer_init(
		kbdev->hwcnt_gpu_ctx,
		KBASE_HWCNT_GPU_VIRTUALIZER_DUMP_THRESHOLD_NS,
		&kbdev->hwcnt_gpu_virt);
	if (err) {
		dev_err(kbdev->dev,
			"GPU hwcnt virtualizer initialization failed\n");
		kbase_platform_device_remove(pdev);
		return err;
	}
	kbdev->inited_subsys |= inited_hwcnt_gpu_virt;

	err = kbase_vinstr_init(kbdev->hwcnt_gpu_virt, &kbdev->vinstr_ctx);
	if (err) {
		dev_err(kbdev->dev,
			"Virtual instrumentation initialization failed\n");
		kbase_platform_device_remove(pdev);
		return -EINVAL;
	}
	kbdev->inited_subsys |= inited_vinstr;

	/* The initialization of the devfreq is now embedded inside the
	 * kbase_backend_late_init(), calling the kbase_backend_devfreq_init()
	 * before the first trigger of pm_context_idle(). */
	err = kbase_backend_late_init(kbdev);
	if (err) {
		dev_err(kbdev->dev, "Late backend initialization failed\n");
		kbase_platform_device_remove(pdev);
		return err;
	}
	kbdev->inited_subsys |= inited_backend_late;


#ifdef MALI_KBASE_BUILD
	err = kbase_debug_job_fault_dev_init(kbdev);
	if (err) {
		dev_err(kbdev->dev, "Job fault debug initialization failed\n");
		kbase_platform_device_remove(pdev);
		return err;
	}
	kbdev->inited_subsys |= inited_job_fault;

	err = kbase_device_debugfs_init(kbdev);
	if (err) {
		dev_err(kbdev->dev, "DebugFS initialization failed");
		kbase_platform_device_remove(pdev);
		return err;
	}
	kbdev->inited_subsys |= inited_debugfs;

	kbdev->mdev.minor = MISC_DYNAMIC_MINOR;
	kbdev->mdev.name = kbdev->devname;
	kbdev->mdev.fops = &kbase_fops;
	kbdev->mdev.parent = get_device(kbdev->dev);
	kbdev->mdev.mode = 0666;
	kbdev->inited_subsys |= inited_get_device;

	/* This needs to happen before registering the device with misc_register(),
	 * otherwise it causes a race condition between registering the device and a
	 * uevent event being generated for userspace, causing udev rules to run
	 * which might expect certain sysfs attributes present. As a result of the
	 * race condition we avoid, some Mali sysfs entries may have appeared to
	 * udev to not exist.

	 * For more information, see
	 * https://www.kernel.org/doc/Documentation/driver-model/device.txt, the
	 * paragraph that starts with "Word of warning", currently the second-last
	 * paragraph.
	 */
	err = sysfs_create_group(&kbdev->dev->kobj, &kbase_attr_group);
	if (err) {
		dev_err(&pdev->dev, "SysFS group creation failed\n");
		kbase_platform_device_remove(pdev);
		return err;
	}
	kbdev->inited_subsys |= inited_sysfs_group;

	err = misc_register(&kbdev->mdev);
	if (err) {
		dev_err(kbdev->dev, "Misc device registration failed for %s\n",
			kbdev->devname);
		kbase_platform_device_remove(pdev);
		return err;
	}
	kbdev->inited_subsys |= inited_misc_register;

	/* init hisilicon special lp config */
	hisi_gpu_special_init(kbdev);

#ifdef CONFIG_MALI_BUSLOG
	err = bl_core_client_register(kbdev->devname,
						kbase_logging_started_cb,
						kbdev, &kbdev->buslogger,
						THIS_MODULE, NULL);
	if (err == 0) {
		kbdev->inited_subsys |= inited_buslogger;
		bl_core_set_threshold(kbdev->buslogger, 1024*1024*1024);
	} else {
		dev_warn(kbdev->dev, "Bus log client registration failed\n");
		err = 0;
	}
#endif

	err = kbase_gpuprops_populate_user_buffer(kbdev);
	if (err) {
		dev_err(&pdev->dev, "GPU property population failed");
		kbase_platform_device_remove(pdev);
		return err;
	}

	dev_info(kbdev->dev,
			"Probed as %s\n", dev_name(kbdev->mdev.this_device));

	kbase_dev_nr++;
#endif /* MALI_KBASE_BUILD */

#ifdef CONFIG_HUAWEI_DSM
	kbdev->gpu_dsm_client = dsm_register_client(&dsm_gpu);
	kbdev->runmode_normal = !runmode_is_factory();
#endif

#ifdef CONFIG_MALI_BOUND_REPORT
	{
		kbdev->bound_report_info.bound_times_in_fifo = 0;
		kbdev->bound_report_info.duration_times = BOUND_DURATION_TIMES;		//BOUND_DURATION_TIMES  x 20ms
		kbdev->bound_report_info.report_bound_flag = false;
		kfifo_alloc(&kbdev->bound_report_info.bound_fifo, BOUND_FIFO_SIZE, GFP_KERNEL);
	}
#endif
	return err;
}

#undef KBASEP_DEFAULT_REGISTER_HISTORY_SIZE

/**
 * kbase_device_suspend - Suspend callback from the OS.
 *
 * This is called by Linux when the device should suspend.
 *
 * @dev:  The device to suspend
 *
 * Return: A standard Linux error code
 */
static int kbase_device_suspend(struct device *dev)
{
	struct kbase_device *kbdev = to_kbase_device(dev);

	if (!kbdev)
		return -ENODEV;

	int refcount = atomic_read(&kbdev->regulator_refcount);
	dev_info(kbdev->dev, "%s: suspend +, refcount:[%d]\n", __func__, refcount);

	kbase_pm_suspend(kbdev);

#if defined(CONFIG_MALI_DEVFREQ) && \
		(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0))
	dev_dbg(dev, "Callback %s\n", __func__);
	if (kbdev->inited_subsys & inited_devfreq) {
		kbase_devfreq_enqueue_work(kbdev, DEVFREQ_WORK_SUSPEND);
		flush_workqueue(kbdev->devfreq_queue.workq);
	}
#endif
	refcount = atomic_read(&kbdev->regulator_refcount);
	dev_info(kbdev->dev, "%s: suspend -, refcount:[%d]\n", __func__, refcount);
	return 0;
}

/**
 * kbase_device_resume - Resume callback from the OS.
 *
 * This is called by Linux when the device should resume from suspension.
 *
 * @dev:  The device to resume
 *
 * Return: A standard Linux error code
 */
static int kbase_device_resume(struct device *dev)
{
	struct kbase_device *kbdev = to_kbase_device(dev);

	if (!kbdev)
		return -ENODEV;

	int refcount = atomic_read(&kbdev->regulator_refcount);
	dev_info(kbdev->dev, "%s: resume +, refcount:[%d]\n", __func__, refcount);

	kbase_pm_resume(kbdev);

#if defined(CONFIG_MALI_DEVFREQ) && \
		(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0))
	dev_dbg(dev, "Callback %s\n", __func__);
	if (kbdev->inited_subsys & inited_devfreq) {
		mutex_lock(&kbdev->pm.lock);
		if (kbdev->pm.active_count > 0)
			kbase_devfreq_enqueue_work(kbdev, DEVFREQ_WORK_RESUME);
		mutex_unlock(&kbdev->pm.lock);
		flush_workqueue(kbdev->devfreq_queue.workq);
	}
#endif
	refcount = atomic_read(&kbdev->regulator_refcount);
	dev_info(kbdev->dev, "%s: resume -, refcount:[%d]\n", __func__, refcount);
	return 0;
}

#ifdef CONFIG_HISI_GPU_CORE_HOTPLUG
/**
 * mali_dev_pause - pause by IPA or other external control requirment
 *
 * This is called by thermal component to make gpu suspend evenif it is busy.
 * Could decrease gpu effectively.
 */
void mali_dev_pause(struct device *dev)
{
	kbase_device_suspend(dev);
}
EXPORT_SYMBOL(mali_dev_pause);

/**
 * mali_dev_resume - resume by IPA or othe external control requirment
 *
 * This is called by thermal component resume gpu from pause
 * Restart gpu run to accept job task from UMD.
 */
void mali_dev_resume(struct device *dev)
{
	kbase_device_resume(dev);
}
EXPORT_SYMBOL(mali_dev_resume);
#endif

/**
 * kbase_device_runtime_suspend - Runtime suspend callback from the OS.
 *
 * This is called by Linux when the device should prepare for a condition in
 * which it will not be able to communicate with the CPU(s) and RAM due to
 * power management.
 *
 * @dev:  The device to suspend
 *
 * Return: A standard Linux error code
 */
#ifdef KBASE_PM_RUNTIME
static int kbase_device_runtime_suspend(struct device *dev)
{
	struct kbase_device *kbdev = to_kbase_device(dev);

	if (!kbdev)
		return -ENODEV;

	dev_dbg(dev, "Callback %s\n", __func__);
#if defined(CONFIG_MALI_DEVFREQ) && \
		(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0))
	if (kbdev->inited_subsys & inited_devfreq)
		kbase_devfreq_enqueue_work(kbdev, DEVFREQ_WORK_SUSPEND);
#endif

	mutex_lock(&kbdev->pm.suspend_lock);
	if (kbdev->pm.backend.callback_power_runtime_off) {
		kbdev->pm.backend.callback_power_runtime_off(kbdev);
		dev_dbg(dev, "runtime suspend\n");
	}
	mutex_unlock(&kbdev->pm.suspend_lock);

	return 0;
}
#endif /* KBASE_PM_RUNTIME */

/**
 * kbase_device_runtime_resume - Runtime resume callback from the OS.
 *
 * This is called by Linux when the device should go into a fully active state.
 *
 * @dev:  The device to suspend
 *
 * Return: A standard Linux error code
 */

#ifdef KBASE_PM_RUNTIME
static int kbase_device_runtime_resume(struct device *dev)
{
	int ret = 0;
	struct kbase_device *kbdev = to_kbase_device(dev);

	if (!kbdev)
		return -ENODEV;

	dev_dbg(dev, "Callback %s\n", __func__);
	if (kbdev->pm.backend.callback_power_runtime_on) {
		ret = kbdev->pm.backend.callback_power_runtime_on(kbdev);
		dev_dbg(dev, "runtime resume\n");
	}

#if defined(CONFIG_MALI_DEVFREQ) && \
		(LINUX_VERSION_CODE >= KERNEL_VERSION(3, 8, 0))
	if (kbdev->inited_subsys & inited_devfreq)
		kbase_devfreq_enqueue_work(kbdev, DEVFREQ_WORK_RESUME);
#endif

	return ret;
}
#endif /* KBASE_PM_RUNTIME */


#ifdef KBASE_PM_RUNTIME
/**
 * kbase_device_runtime_idle - Runtime idle callback from the OS.
 * @dev: The device to suspend
 *
 * This is called by Linux when the device appears to be inactive and it might
 * be placed into a low power state.
 *
 * Return: 0 if device can be suspended, non-zero to avoid runtime autosuspend,
 * otherwise a standard Linux error code
 */
static int kbase_device_runtime_idle(struct device *dev)
{
	struct kbase_device *kbdev = to_kbase_device(dev);

	if (!kbdev)
		return -ENODEV;

	dev_dbg(dev, "Callback %s\n", __func__);
	/* Use platform specific implementation if it exists. */
	if (kbdev->pm.backend.callback_power_runtime_idle)
		return kbdev->pm.backend.callback_power_runtime_idle(kbdev);

	/* Just need to update the device's last busy mark. Kernel will respect
	 * the autosuspend delay and so won't suspend the device immediately.
	 */
	pm_runtime_mark_last_busy(kbdev->dev);
	return 0;
}
#endif /* KBASE_PM_RUNTIME */

/* The power management operations for the platform driver.
 */
static const struct dev_pm_ops kbase_pm_ops = {
	.suspend = kbase_device_suspend,
	.resume = kbase_device_resume,
	.freeze = kbase_device_suspend,
	.thaw = kbase_device_resume,
	.poweroff = kbase_device_suspend,
	.restore = kbase_device_resume,
#ifdef KBASE_PM_RUNTIME
	.runtime_suspend = kbase_device_runtime_suspend,
	.runtime_resume = kbase_device_runtime_resume,
	.runtime_idle = kbase_device_runtime_idle,
#endif /* KBASE_PM_RUNTIME */
};

#ifdef CONFIG_OF
static const struct of_device_id kbase_dt_ids[] = {
	{ .compatible = "arm,malit6xx" },
	{ .compatible = "arm,mali-midgard" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, kbase_dt_ids);
#endif

static struct platform_driver kbase_platform_driver = {
	.probe = kbase_platform_device_probe,
	.remove = kbase_platform_device_remove,
	.driver = {
		   .name = kbase_drv_name,
		   .owner = THIS_MODULE,
		   .pm = &kbase_pm_ops,
		   .of_match_table = of_match_ptr(kbase_dt_ids),
	},
};

/*
 * The driver will not provide a shortcut to create the Mali platform device
 * anymore when using Device Tree.
 */
#ifdef CONFIG_OF
module_platform_driver(kbase_platform_driver);
#else

static int __init kbase_driver_init(void)
{
	int ret;

	ret = kbase_platform_register();
	if (ret)
		return ret;

	ret = platform_driver_register(&kbase_platform_driver);

	if (ret)
		kbase_platform_unregister();

	return ret;
}

static void __exit kbase_driver_exit(void)
{
	platform_driver_unregister(&kbase_platform_driver);
	kbase_platform_unregister();
}

module_init(kbase_driver_init);
module_exit(kbase_driver_exit);

#endif /* CONFIG_OF */

MODULE_LICENSE("GPL");
MODULE_VERSION(MALI_RELEASE_NAME " (UK version " \
		__stringify(BASE_UK_VERSION_MAJOR) "." \
		__stringify(BASE_UK_VERSION_MINOR) ")");

#if defined(CONFIG_MALI_GATOR_SUPPORT) || defined(CONFIG_MALI_SYSTEM_TRACE)
#define CREATE_TRACE_POINTS
#endif

#ifdef CONFIG_MALI_GATOR_SUPPORT
/* Create the trace points (otherwise we just get code to call a tracepoint) */
#include "mali_linux_trace.h"

EXPORT_TRACEPOINT_SYMBOL_GPL(mali_job_slots_event);
EXPORT_TRACEPOINT_SYMBOL_GPL(mali_pm_status);
EXPORT_TRACEPOINT_SYMBOL_GPL(mali_page_fault_insert_pages);
EXPORT_TRACEPOINT_SYMBOL_GPL(mali_total_alloc_pages_change);

void kbase_trace_mali_pm_status(u32 dev_id, u32 event, u64 value)
{
	trace_mali_pm_status(dev_id, event, value);
}

void kbase_trace_mali_job_slots_event(u32 dev_id, u32 event, const struct kbase_context *kctx, u8 atom_id)
{
	trace_mali_job_slots_event(dev_id, event,
		(kctx != NULL ? kctx->tgid : 0),
		(kctx != NULL ? kctx->pid : 0),
		atom_id);
}

void kbase_trace_mali_page_fault_insert_pages(u32 dev_id, int event, u32 value)
{
	trace_mali_page_fault_insert_pages(dev_id, event, value);
}

void kbase_trace_mali_total_alloc_pages_change(u32 dev_id, long long int event)
{
	trace_mali_total_alloc_pages_change(dev_id, event);
}
#endif /* CONFIG_MALI_GATOR_SUPPORT */
#ifdef CONFIG_MALI_SYSTEM_TRACE
#include "mali_linux_kbase_trace.h"
#endif
#pragma GCC diagnostic pop
