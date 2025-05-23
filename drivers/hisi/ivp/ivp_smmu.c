/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2016-2019. All rights reserved.
 * Description: ipc smmu function
 * Create: 2016-2-9
 */
#define pr_fmt(fmt) "[ivp-smmu]  " fmt

#include "ivp_smmu.h"
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/dma-mapping.h>
#include <asm/cacheflush.h>
#include <linux/io.h>
#include <linux/of.h>
#include <linux/scatterlist.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/hisi-iommu.h>
#include <linux/hisi/rdr_hisi_platform.h>
#include "ivp_log.h"
//lint -save -e750 -e753 -e750 -e528 -e144 -e82 -e64 -e785 -e715 -e712 -e40
//lint -save -e63 -e732 -e42 -e550 -e438 -e834 -e648 -e747 -e778 -e50 -e838
//lint -save -e571
/* Address Space 0: Non-security, page_size=0x1000, pagenum=2 */
#define     SMMU_NS_CR0             0x0000

#define     SMMU_NS_IDR0            0x0020

#define     SMMU_NS_GFSR            0x0048

#define     SMMU_NS_GFSYNR0         0x0050
#define     SMMU_NS_GFSYNR1         0x0054
#define     SMMU_NS_GFSYNR2         0x0058
#define     SMMU_NS_TLBIVMID        0x0064 // invalid tlb by VMID
#define     SMMU_NS_TLBGSYNC        0x0070
#define     SMMU_NS_TLBGSTATUS      0x0074
#define     SMMU_NS_CBA2R0          0x1800
#define     SMMU_NS_SMR0            0x0800
#define     SMMU_NS_S2CR0           0x0C00
#define     SMMU_NS_CBAR0           0x1000
#define     SMMU_NS_CB0_SCTLR       0x8000
#define     SMMU_NS_CB0_RESUME      0x8008
#define     SMMU_NS_CB0_TTBCR2      0x8010 // SMMU_CB0_TCR2
#define     SMMU_NS_CB0_TTBR0_LOW   0x8020
#define     SMMU_NS_CB0_TTBR0_HIGH  0x8024
#define     SMMU_NS_CB0_TTBCR       0x8030 // SMMU_CB0_TCR
#define     SMMU_NS_CB0_FSR         0x8058
#define     SMMU_NS_CB0_FAR_LOW     0x8060
#define     SMMU_NS_CB0_FAR_HIGH    0x8064
#define     SMMU_NS_CB0_FSYNR0      0x8068

#define     SMMU_IDR0_CTTW          BIT(14) // Coherent Translation Table Walk

#define     SMMU_FAULT_READ         0x0         // smmu read  fault
#define     SMMU_FAULT_WRITE        0x1         // smmu write fault

#define     SMMU_CB_IDX_IVP         0           // cbidx for ivp
#define     SMMU_CB_VMID_IVP        0           // vmid  for ivp

#define     SMMU_CB_FSYNR0_WNR          (1 << 4)
#define     SMMU_CB_FSR_SS              BIT(30)
#define     SMMU_CB_RESUME_TERMINATE    BIT(0)

#define     SMMU_TLBGSTATUS_GSACTIVE    BIT(0)
#define     SMMU_TLB_LOOP_TIMEOUT       1000000     /* 1s! */

#define     SMMU_CBAR_TYPE_SHIFT        16
#define     SMMU_CBAR_TYPE_S1_TRANS_S2_BYPASS   BIT(SMMU_CBAR_TYPE_SHIFT)

#define     SMMU_NS_CB0_FSR_SETVAL       0xFFFFFFFF
#define     SMMU_NS_GFSR_SETVAL          0xFFFFFFFF
#define     SMMU_NS_CR0_SETVAL           0x300036
#define     SMMU_NS_SMR0_SETVAL          0xFFFE0001
#define     SMMU_NS_CBA2R0_SETVAL        0x0
#define     SMMU_NS_CB0_SCTLR_SETVAL     0x65
#define     SMMU_NS_CR0_CLRVAL           0x200013

#ifdef CONFIG_HISI_IOMMU_LPAE
#define     SMMU_NS_S2CR0_SETVAL         0x3000000
#define     SMMU_NS_CB0_TTBCR_SETVAL     0x80000000
#define     SMMU_NS_CB0_TTBCR2_SETVAL    0x10002
#else
#define     SMMU_NS_S2CR0_SETVAL         0x0
#define     SMMU_NS_CB0_TTBCR_SETVAL     0x0
#define     SMMU_NS_CB0_TTBCR2_SETVAL    0x0
#endif

#define     SMMU_ISR_W_CB0_SCTLR         0x25
#define     SMMU_ISR_W_CR0               0x200032
#define     SMMU_REMOVE_W_CR0            0x200037
#define     DEV_IVP_BUFF_SIZE            256
struct ivp_smmu_dev *g_smmu_dev;
struct dsm_client *client_ivp;

struct dsm_client_ops ops5 = {
	.poll_state = NULL,
	.dump_func = NULL,
};

struct dsm_dev dev_ivp = {
	.name = "dsm_ivp",
	.device_name = NULL,
	.ic_name = NULL,
	.module_name = NULL,
	.fops = &ops5,
	.buff_size = DEV_IVP_BUFF_SIZE,
};

void ivp_dsm_error_notify(int error_no)
{
	if (!dsm_client_ocuppy(client_ivp)) {
		dsm_client_record(client_ivp, "ivp\n");
		dsm_client_notify(client_ivp, error_no);
		ivp_info("[I/DSM] %s ivp dsm error notify, error no %d",
			client_ivp->client_name, error_no);
	}
}

/* Wait for any pending TLB invalidations to complete */
static void ivp_smmu_tlb_sync(struct ivp_smmu_dev *smmu_dev)
{
	int count = 0;

	writel(0, smmu_dev->reg_base + SMMU_NS_TLBGSYNC);
	while (readl(smmu_dev->reg_base + SMMU_NS_TLBGSTATUS)
			& SMMU_TLBGSTATUS_GSACTIVE) {
		if (++count == SMMU_TLB_LOOP_TIMEOUT) {
			ivp_err("TLB sync timed out -- SMMU may be deadlocked\n");
			return;
		}
		udelay(1);
	}
}

/* Invalid TLB by vmid */
int ivp_smmu_invalid_tlb(struct ivp_smmu_dev *smmu_dev, unsigned int vmid)
{
	unsigned long flags = 0;

	if (!smmu_dev) {
		ivp_err("smmu_dev is null!");
		return -EINVAL;
	}

	spin_lock_irqsave(&smmu_dev->spinlock, flags);

	/* vmid = smmu_dev->vmid */
	writel(vmid, (smmu_dev->reg_base + SMMU_NS_TLBIVMID));
	ivp_smmu_tlb_sync(smmu_dev);

	spin_unlock_irqrestore(&smmu_dev->spinlock, flags);

	return EOK;
}

/* Flush cache for the page tables after pgt updated */
void ivp_smmu_flush_pgtable(struct ivp_smmu_dev *smmu_dev,
	void *addr, size_t size)
{
	unsigned long offset = (unsigned long)(uintptr_t)addr & ~PAGE_MASK;
	unsigned int idr0 = 0;

	if (!smmu_dev || !addr) {
		ivp_err("smmu_dev or addr is null!");
		return;
	}

	/* IDR0 */
	idr0 = readl(smmu_dev->reg_base + SMMU_NS_IDR0); //lint -e838

	/* Coherent translation table walks are supported */
	if (idr0 & SMMU_IDR0_CTTW) {
		dsb(ishst);
	} else {
		/**
		 * If the SMMU can't walk tables in the CPU caches, treat them
		 * like non-coherent DMA since we need to flush the new entries
		 * all the way out to memory. There's no possibility of
		 * recursion here as the SMMU table walker will not be wired
		 * through another SMMU.
		 */
		dma_map_page(smmu_dev->dev, virt_to_page((uintptr_t)addr),
			offset, size, DMA_TO_DEVICE); //lint -e834 -e648
	}
}

/* Enable the translation mode of SMMU */
int ivp_smmu_trans_enable(struct ivp_smmu_dev *smmu_dev)
{
	unsigned long flags = 0;
	unsigned int  cbar0 = 0;

	if (!smmu_dev) {
		ivp_err("smmu_dev is null!");
		return -EINVAL;
	}

	if (request_irq(smmu_dev->irq, smmu_dev->isr, 0,
					"ivp-smmu-isr", (void *)smmu_dev)) {
		ivp_err("%s: failed to request IRQ[%d]\n",
				__func__, smmu_dev->irq);
	}

	spin_lock_irqsave(&smmu_dev->spinlock, flags);

	/* Check the smmu state */
	if (smmu_dev->state == SMMU_STATE_ENABLE) {
		spin_unlock_irqrestore(&smmu_dev->spinlock, flags);
		ivp_err("%s: SMMU re-enable\n", __func__);
		return -EBUSY;
	}

	/* Check pgd base address */
	if (!smmu_dev->pgd_base) {
		spin_unlock_irqrestore(&smmu_dev->spinlock, flags);
		ivp_err("%s: invalid pgd_base\n", __func__);
		return -EINVAL;
	}

	/*
	 * clear smmu interrupt
	 */
	writel(SMMU_NS_CB0_FSR_SETVAL, (smmu_dev->reg_base + SMMU_NS_CB0_FSR));
	writel(SMMU_NS_GFSR_SETVAL, (smmu_dev->reg_base + SMMU_NS_GFSR));

	/*
	 * smcfcfg-0b1(bit21):raise stream match conflict fault;
	 * gcfgfie-0b1(bit5):global configuration fault interrupt enable
	 * gcfgfie-0b1(bit4): global configuration fault report enable
	 * gfie-0b1(bit2): global fault interrupt enable
	 * gfre--0b1(bit1): global fault report enable
	 * clientpd-0b0(bit0):transcation enable
	 */
	writel(SMMU_NS_CR0_SETVAL, (smmu_dev->reg_base + SMMU_NS_CR0));

	/**
	 * valid-0b1(bit31):entry is included in the stream mapping table search
	 * mask-0b10(bit16~30):mask of ID[1]
	 * id-0b1(bit0~14):stream identifier to match
	 */
	writel(SMMU_NS_SMR0_SETVAL, (smmu_dev->reg_base + SMMU_NS_SMR0));

	/**
	 * nscfg-0b11(bit18~19):no secure
	 * type-0b00(bit16~17):translation context bank index
	 * cbndx-0b00(bit0~7):context bank index is 0
	 */

	writel(SMMU_NS_S2CR0_SETVAL, (smmu_dev->reg_base + SMMU_NS_S2CR0));

	/**
	 * type-0b01(bit16~17):stage 1 context with stage 2 bypass
	 * bpshcfg-0b00(bit8~9):reserved
	 * vmid-0b00000000(bit0~7): Virtual Machine Identifier
	 */
	cbar0 |= smmu_dev->cbar | (smmu_dev->vmid & 0xFF);
	writel(cbar0, (smmu_dev->reg_base + SMMU_NS_CBAR0));

	/**
	 * va64-0x1(bit0):descriptor format(0-32bit 1-64bit)
	 */
	writel(SMMU_NS_CBA2R0_SETVAL, (smmu_dev->reg_base + SMMU_NS_CBA2R0));

	/**
	 * pd1-0b1(bit5):no trasaction table walk using ttbr1 on tlb miss
	 * pd0-0b0(bit4):perform translation table walks using ttbr0
	 * t0sz-0b000(bit0~2):size offset(2^(32-t0sz))
	 */
	writel(SMMU_NS_CB0_TTBCR_SETVAL, (smmu_dev->reg_base + SMMU_NS_CB0_TTBCR));

	/**
	 * type-0b000(bit15~17):Sign Extension Position(bit[31]-4G)
	 * pasize-0b000(bit0~2):physical address size(4G)
	 */
	writel(SMMU_NS_CB0_TTBCR2_SETVAL, (smmu_dev->reg_base + SMMU_NS_CB0_TTBCR2));

	/**
	 * set pgd base address and asid
	 */
	writel((u32)smmu_dev->pgd_base,
			(smmu_dev->reg_base + SMMU_NS_CB0_TTBR0_LOW));

	writel((u32)((smmu_dev->pgd_base) >> 32),
			(smmu_dev->reg_base + SMMU_NS_CB0_TTBR0_HIGH));

	/**
	 * cfie-0b1(bit6):Context Fault Interrupt Enable
	 * cfre-0b1(bit5):Context Fault Report Enable
	 * afe-0b1(bit2):AP[0] is an access flag
	 * m-0b1(bit0):smmu enable
	 */
	writel(SMMU_NS_CB0_SCTLR_SETVAL, (smmu_dev->reg_base + SMMU_NS_CB0_SCTLR));
	smmu_dev->state = SMMU_STATE_ENABLE;
	spin_unlock_irqrestore(&smmu_dev->spinlock, flags);

	return EOK;
}

/* Disable the translation mode of SMMU (switch to bypass mode) */
int ivp_smmu_trans_disable(struct ivp_smmu_dev *smmu_dev)
{
	unsigned long flags = 0;

	spin_lock_irqsave(&smmu_dev->spinlock, flags);
	smmu_dev->state = SMMU_STATE_DISABLE;

	/**
	 * clientpd-0b1(bit0):transcation disable
	 */
	writel(SMMU_NS_CR0_CLRVAL, (smmu_dev->reg_base + SMMU_NS_CR0));
	spin_unlock_irqrestore(&smmu_dev->spinlock, flags);

	/* free the irq */
	disable_irq(smmu_dev->irq);
	free_irq(smmu_dev->irq, (void *)smmu_dev);

	return EOK;
}

/* Return the smmu device handler to upper layer */
struct ivp_smmu_dev *ivp_smmu_get_device(unsigned long select __attribute__((unused)))
{
	return g_smmu_dev;
}

/* Update the physical base address of page table directory */
int ivp_smmu_set_pgd_base(struct ivp_smmu_dev *smmu_dev, unsigned long pgd_base)
{
	unsigned long flags = 0;
	if (!smmu_dev) {
		ivp_err("smmu_dev is NULL.\n");
		return -EINVAL;
	}

	spin_lock_irqsave(&smmu_dev->spinlock, flags);
	/* It's should be disabled then re-enabled */
	if (smmu_dev->state == SMMU_STATE_ENABLE) {
		spin_unlock_irqrestore(&smmu_dev->spinlock, flags);
		pr_warn("%s: error state for set pgd\n", __func__);
		return -EBUSY;
	}
	smmu_dev->pgd_base = pgd_base;
	spin_unlock_irqrestore(&smmu_dev->spinlock, flags);

	return EOK;
}

/* SMMU ISR, handler that SMMU reports fault to */
static irqreturn_t ivp_smmu_isr(int irq, void *dev_id)
{
	struct ivp_smmu_dev *smmu_dev = (struct ivp_smmu_dev *)dev_id;
	u32 gfsr;
	u32 gfsynr0;
	u32 gfsynr1;
	u32 gfsynr2;
	u32 fsr;
	u32 far;
	u32 fsynr;
	unsigned long iova;
	int flags;

	ivp_dsm_error_notify(DSM_IVP_SMMU_ERROR_NO);

	/* global fault, It seems not supported yet */
	gfsr = readl(smmu_dev->reg_base + SMMU_NS_GFSR);
	if (gfsr) {
		gfsynr0 = readl(smmu_dev->reg_base + SMMU_NS_GFSYNR0);
		gfsynr1 = readl(smmu_dev->reg_base + SMMU_NS_GFSYNR1);
		gfsynr2 = readl(smmu_dev->reg_base + SMMU_NS_GFSYNR2);
		ivp_err("Unexpected global fault, this could be serious\n");
		ivp_err("\tGFSR 0x%08x, GFSYNR0 0x%08x, GFSYNR1 0x%08x, GFSYNR2 0x%08x\n",
			gfsr, gfsynr0, gfsynr1, gfsynr2);
		writel(gfsr, smmu_dev->reg_base + SMMU_NS_GFSR);
	}

	/* context bank fault */
	fsr = readl(smmu_dev->reg_base + SMMU_NS_CB0_FSR);

	fsynr = readl(smmu_dev->reg_base + SMMU_NS_CB0_FSYNR0); //lint -e63
	flags = fsynr & SMMU_CB_FSYNR0_WNR ? SMMU_FAULT_WRITE : SMMU_FAULT_READ;

	far = readl(smmu_dev->reg_base + SMMU_NS_CB0_FAR_LOW);
	iova = far; //lint -e732
	far = readl(smmu_dev->reg_base + SMMU_NS_CB0_FAR_HIGH);
	iova |= (((unsigned long)far) << 32); //lint -e571

	ivp_err("Unexpected context fault (fsr 0x%x)\n", fsr);
	ivp_err("Unhandled  context fault: iova=0x%08lx, fsynr=0x%x\n",
			iova, fsynr);

	/* Report about an MMU fault to high-level users */
	if (smmu_dev->domain) {
		report_iommu_fault(smmu_dev->domain, smmu_dev->dev,
							iova, flags);
	}

	/* Clear the faulting FSR */
	writel(fsr, smmu_dev->reg_base + SMMU_NS_CB0_FSR);

	/* Retry or terminate any stalled transactions */
	if (fsr & SMMU_CB_FSR_SS) {
		writel(SMMU_CB_RESUME_TERMINATE,
				smmu_dev->reg_base + SMMU_NS_CB0_RESUME);
	}

	/*
	 * Because ivp dma error or cause error may cause many smmu fault
	 * continuously, there we only report once.
	 */
	ivp_info("Disable smmu irq");
	writel(SMMU_ISR_W_CB0_SCTLR, (smmu_dev->reg_base + SMMU_NS_CB0_SCTLR));
	writel(SMMU_ISR_W_CR0, (smmu_dev->reg_base + SMMU_NS_CR0));

	ivp_err("ivp dma cause a smmu fault!\n");
	if (smmu_dev->err_handler) {
		smmu_dev->err_handler();
	} else {
	}

	return IRQ_HANDLED; /*lint !e527*/
}

int ivp_iommu_fault_handler_t(
	struct iommu_domain *pdomain __attribute__((unused)),
	struct device *pdev __attribute__((unused)),
	unsigned long size __attribute__((unused)),
	int val __attribute__((unused)), void *data __attribute__((unused)))
{
	ivp_err("%s:iommu fault\n", __func__);
	return EOK;
}

static int ivp_smmu_probe(struct platform_device *pdev)
{
	struct device_node *np = pdev->dev.of_node;
	struct ivp_smmu_dev *smmu_dev  = NULL;
	struct resource *res = NULL;
	unsigned int *ver = NULL;

	ivp_info("%s: smmu driver start\n", __func__);

	smmu_dev = devm_kzalloc(&pdev->dev, sizeof(struct ivp_smmu_dev), GFP_KERNEL);
	if (!smmu_dev) {
		ivp_err("%s: devm_kzalloc is failed\n", __func__);
		return -ENOMEM;
	}
	smmu_dev->dev = &pdev->dev;
	smmu_dev->state = SMMU_STATE_DISABLE;

	/* get smmu version */
	ver = (unsigned int *)of_get_property(np, "hisi,smmu-version", NULL);
	if (ver) {
		smmu_dev->version = be32_to_cpu(*ver);
		ivp_info("%s: smmu version is %u\n", __func__, be32_to_cpu(*ver));
	}

	/* get IOMEM resource */
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		ivp_err("%s:platform_get_resource err\n", __func__);
		return -ENOENT;/*lint !e429*/
	}

	smmu_dev->reg_base = devm_ioremap_resource(&pdev->dev, res);
	if (IS_ERR(smmu_dev->reg_base)) {
		ivp_err("%s: remap resource err\n", __func__);
		return PTR_ERR(smmu_dev->reg_base);/*lint !e429*/
	}
	smmu_dev->reg_size = resource_size(res);

	/* get IRQ resource */
	res = platform_get_resource(pdev, IORESOURCE_IRQ, 0);
	if (!res) {
		ivp_err("%s: get IRQ IS failed\n", __func__);
		return -ENOENT;/*lint !e429*/
	}
	smmu_dev->irq = (unsigned int)res->start;
	smmu_dev->isr = ivp_smmu_isr;

	/**
	 * get physical pgd base address
	 */
	dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64)); /*lint !e598 !e648*/
	smmu_dev->pgd_base = (unsigned long)hisi_domain_get_ttbr(smmu_dev->dev);

	/**
	 * for the ivp subsys, only support:
	 * Context Bank:0; Virtual Machine ID:0; CB attribute:S1_TRANS_S2_BYPASS
	 */
	smmu_dev->cbidx = SMMU_CB_IDX_IVP;
	smmu_dev->vmid  = SMMU_CB_VMID_IVP;
	smmu_dev->cbar  = SMMU_CBAR_TYPE_S1_TRANS_S2_BYPASS;
	smmu_dev->err_handler = NULL;
	spin_lock_init(&smmu_dev->spinlock);
	g_smmu_dev = smmu_dev;

	ivp_info("%s: smmu driver probes finish\n", __func__);

	if (!client_ivp)
		client_ivp = dsm_register_client(&dev_ivp);

	return EOK;
}

static int ivp_smmu_remove(struct platform_device *pdev __attribute__((unused)))
{
	struct ivp_smmu_dev *smmu_dev = g_smmu_dev;

	if (!smmu_dev) {
		ivp_err("%s: smmu_dev invalid\n", __func__);
		return -ENODEV;
	}

	/**
	 * clientpd-0b1(bit0):transcation disable (switch to bypass mode)
	 */
	writel(SMMU_REMOVE_W_CR0, (smmu_dev->reg_base + SMMU_NS_CR0));

	g_smmu_dev = NULL;

	return EOK;
}

static const struct of_device_id of_smmu_match_tbl[] = {
	{
		.compatible = "hisi,ivp-smmu",
	},
	{ },
};

static struct platform_driver ivp_smmu_driver = {
	.driver = {
		.owner = THIS_MODULE,
		.name = "ivp-smmu",
		.of_match_table = of_smmu_match_tbl,
	},
	.probe  =  ivp_smmu_probe,
	.remove =  ivp_smmu_remove,
};

static int __init ivp_smmu_init(void)
{
	return platform_driver_register(&ivp_smmu_driver);
}

static void __exit ivp_smmu_exit(void)
{
	return platform_driver_unregister(&ivp_smmu_driver);
}

void ivp_smmu_set_err_handler(struct ivp_smmu_dev *smmu_dev,
	smmu_err_handler_t err_func)
{
	if (smmu_dev)
		smmu_dev->err_handler = err_func;
}
module_init(ivp_smmu_init);
module_exit(ivp_smmu_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("hisilicon");
//lint -restore
