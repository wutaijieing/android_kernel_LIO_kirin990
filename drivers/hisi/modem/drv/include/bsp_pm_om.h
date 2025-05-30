/*
 * Copyright (C) Huawei Technologies Co., Ltd. 2012-2015. All rights reserved.
 * foss@huawei.com
 *
 * If distributed as part of the Linux kernel, the following license terms
 * apply:
 *
 * * This program is free software; you can redistribute it and/or modify
 * * it under the terms of the GNU General Public License version 2 and
 * * only version 2 as published by the Free Software Foundation.
 * *
 * * This program is distributed in the hope that it will be useful,
 * * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * * GNU General Public License for more details.
 * *
 * * You should have received a copy of the GNU General Public License
 * * along with this program; if not, write to the Free Software
 * * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA
 *
 * Otherwise, the following license terms apply:
 *
 * * Redistribution and use in source and binary forms, with or without
 * * modification, are permitted provided that the following conditions
 * * are met:
 * * 1) Redistributions of source code must retain the above copyright
 * *    notice, this list of conditions and the following disclaimer.
 * * 2) Redistributions in binary form must reproduce the above copyright
 * *    notice, this list of conditions and the following disclaimer in the
 * *    documentation and/or other materials provided with the distribution.
 * * 3) Neither the name of Huawei nor the names of its contributors may
 * *    be used to endorse or promote products derived from this software
 * *    without specific prior written permission.
 *
 * * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef  __BSP_PM_OM_H__
#define  __BSP_PM_OM_H__

#ifdef __cplusplus
extern "C"
{
#endif
#include <product_config.h>
#include <osl_types.h>
#include <osl_spinlock.h>
#include <osl_list.h>
#include <mdrv_pm.h>
#include <mdrv_pm_om.h>
#include <mdrv_ipc.h>
#include <bsp_icc.h>

#ifndef PMOM_CAT_APP
#include <bsp_ring_buffer.h>
#endif
#include <bsp_modem_log.h>
#include <bsp_print.h>
#ifndef CCPU_CORE_NUM
#define CCPU_CORE_NUM 1
#endif

/*
 * 模块id，增加或修改模块ID的同时需要同步修改:
 * 1) 标识模块的魔数      : enum PM_OM_MAGIC的定义
 * 2) 模块与魔数之间映射表: g_magic_tbl
 */
enum PM_OM_MOD_ID
{
    PM_OM_PMA  = 0,
    PM_OM_PMC  = 1,
    PM_OM_PMM  = 2,
    PM_OM_DPM  = 3,
    PM_OM_ABB  = 4,
    PM_OM_BBP  = 5,
    PM_OM_DSP  = 6,
    PM_OM_PMU  = 7,
    PM_OM_WAKE = 8,
    PM_OM_CPUF = 9,
    PM_OM_CCLK = 10,
    PM_OM_REGU = 11,
    PM_OM_ACLK = 12,
    PM_OM_AIPC = 13,
    PM_OM_CIPC = 14,
    PM_OM_AIPF = 15,
    PM_OM_CIPF = 16,
    PM_OM_AOSA = 17,
    PM_OM_COSA = 18,
    PM_OM_CMSP = 19,
    PM_OM_NVA  = 20,
    PM_OM_NVC  = 21,
    PM_OM_ADPPOWER = 22,
    PM_OM_AICC = 23,
    PM_OM_CICC = 24,
    PM_OM_ARFILE = 25,
    PM_OM_PMLG = 26,
    PM_OM_TCXO = 27,
    PM_OM_RFFE = 28,
    PM_OM_CDSP = 29,
    PM_OM_MDRV = 30,
    PM_OM_IQI = 31,
    /* 32~48预留给上层, 模块ID不可以超过64 */
    PM_OM_MOD_ID_MAX
};

/*
 * 模块魔数，以ascii码值表示的u32整型数据，注意:
 * 1) 是小端字节序
 * 2) 修改或者增加enum PM_OM_MOD_ID定义的时候，需要同步修改枚举
 */
enum PM_OM_MAGIC
{
    /* 各模块对应魔数 */
    PM_OM_MAGIC_PMA  = 0x20414D50, /* 0-PMA  : PM ACORE  */
    PM_OM_MAGIC_PMC  = 0x20434D50, /* 1-PMC  : PM CCORE  */
    PM_OM_MAGIC_PMM  = 0x204D4D50, /* 2-PMM  : PM MCORE  */
    PM_OM_MAGIC_DPM  = 0x204D5044, /* 3-DPM  : DPM */
    PM_OM_MAGIC_ABB  = 0x20424241, /* 4-ABB  : ABB */
    PM_OM_MAGIC_BBP  = 0x20504242, /* 5-BBP  : BBP */
    PM_OM_MAGIC_DSP  = 0x20505344, /* 6-DSP  : DSP */
    PM_OM_MAGIC_PMU  = 0x20554D50, /* 7-PMU  : PMU */
    PM_OM_MAGIC_WAKE = 0x454B4157, /* 8-WAKE : WAKELOCK */
    PM_OM_MAGIC_CPUF = 0x46555043, /* 9-CPUF : CPUFREQ */
    PM_OM_MAGIC_CCLK = 0x4B4C4343, /*10-CCLK : CLK CCORE */
    PM_OM_MAGIC_REGU = 0x55474552, /*11-REG  : REGULATOR */
    PM_OM_MAGIC_ACLK = 0x4B4C4341, /*12-ACLK : CLK ACORE */
    PM_OM_MAGIC_AIPC = 0x43504941, /*13-AIPC : IPC ACORE */
    PM_OM_MAGIC_CIPC = 0x43504943, /*14-CIPC : IPC CCORE */
    PM_OM_MAGIC_AIPF = 0x46504941, /*15-AIPF : IPF ACORE */
    PM_OM_MAGIC_CIPF = 0x46504943, /*16-CIPF : IPF CCORE */
    PM_OM_MAGIC_NVA  = 0x2041564E, /*20-NVA : NV */
    PM_OM_MAGIC_NVC  = 0x2043564E, /*21-NVC : NV */
    PM_OM_MAGIC_ADPPOWER  = 0x50504441,/*22-ADPP: ADPPOWER*/
    PM_OM_MAGIC_AICC = 0x43434941, /*23-AICC: ICC ACORE */
    PM_OM_MAGIC_CICC = 0x43434943, /*24-CICC: ICC CCORE  */
    PM_OM_MAGIC_ARFILE=0x45494652, /*25-FILE: RFILE ACORE*/
    PM_OM_MAGIC_PMLG = 0x474C4D50, /*26-PMLG: LOG FILE  */
    PM_OM_MAGIC_TCXO = 0x4F584354, /*27-TCXO: CCORE  */
    PM_OM_MAGIC_RFFE = 0x45464652, /*28-RFFE: CCORE*/
    PM_OM_MAGIC_CDSP = 0x50534443, /*29-CDSP: CDSP*/
    PM_OM_MAGIC_MDRV = 0x5652444D, /*30-MDRV: ALL DRV PM INFO */
    /* pm om内部使用魔数 */
    PM_OM_MAGIC_PMDP = 0x50444D50, /* PMDP: DUMP FILE */
    PM_OM_MAGIC_PLOF = 0x464F4C50, /* PLOF: PM LOG OVERFLOW  */
};

enum PM_OM_MEM_CTRL
{
    PM_OM_USE_SHARED_DDR = 0,
    PM_OM_USE_NORMAL_DDR = 1
};

/*****************************************************************************
* 函 数 名  : bsp_pm_log
*
* 功能描述  : 输出流程信息到log区
*
* 输入参数  : mod_id    模块魔数，使用enum PM_OM_MOD_ID枚举类型定义的值
*            data_len    输出数据长度
*            data         输出数据
* 输出参数  : 无
*
* 返 回 值  : 0，执行成功；非0，失败
*****************************************************************************/
int bsp_pm_log(u32 mod_id, u32 data_len , const void *data);

/*****************************************************************************
* 函 数 名  : bsp_pm_log_type
*
* 功能描述  : 输出流程信息到log区(带typeid，方便脚本统一解析)
*
* 输入参数  : mod_id    模块魔数，使用enum PM_OM_MOD_ID枚举类型定义的值
*            type        模块内部typeid，方便脚本统一解析
*            data_len    输出数据长度
*            data         输出数据
* 输出参数  : 无
*
* 返 回 值  : 0，执行成功；非0，失败
*****************************************************************************/
int bsp_pm_log_type(u32 mod_id, u32 type, u32 data_len , const void *data);

/*****************************************************************************
* 函 数 名  : bsp_pm_log_addr_get
*
* 功能描述  : 获取log区基地址
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 非NULL，执行成功；NULL，失败，无log区
*****************************************************************************/
void *bsp_pm_log_addr_get(void);

/*****************************************************************************
* 函 数 名  : bsp_pm_dump_get
*
* 功能描述  : dump区分配
*
* 输入参数  : mod_id    模块魔数，使用enum PM_OM_MOD_ID枚举类型定义的值
*             len       待获取dump区的大小
*
* 输出参数  : 无
*
* 返 回 值  : 非NULL，执行成功；NULL，失败
*****************************************************************************/
void *bsp_pm_dump_get(u32 mod_id, u32 len);

int bsp_pm_om_log_init(void);
int bsp_pm_om_dump_init(void);

void bsp_pm_om_wakeup_stat(void);
void bsp_pm_om_wakeup_log(void);
/* 模块错误码 */
#define PM_OM_OK            (0)
#define PM_OM_ERR           (-1)
#define PM_OM_ERR_FULL      (-2)
#define PM_OM_ERR_INIT      (-3)
#define PM_OM_ERR_LOG_OFF   (-4)
#define PM_OM_ERR_MOD_OFF   (-5)
#define PM_OM_ERR_FW_TRI    (-6)
#define PM_OM_ERR_LEN_LIMIT (-7)


#define PM_DUMP_CCORE_DRX_SIZE      (0x2000)

/* 各模块在CCORE DRX DUMP区偏移 */
#define CDRX_DUMP_WAKE_OFFSET       (0)
#define CDRX_DUMP_CPUID_OFFSET       (4)
#define CDRX_DUMP_PM_OFFSET         (16)
#define CDRX_DUMP_PM_SIZE           (0x200)
#define CDRX_DUMP_CPUF_OFFSET       (CDRX_DUMP_PM_OFFSET+CDRX_DUMP_PM_SIZE)
#define CDRX_DUMP_CPUF_SIZE         (0x40)
#define CDRX_DUMP_DPM_OFFSET        (CDRX_DUMP_CPUF_OFFSET+CDRX_DUMP_CPUF_SIZE)
#define CDRX_DUMP_DPM_SIZE          (0x700)
#define CDRX_DUMP_RSRACC_OFFSET        (CDRX_DUMP_DPM_OFFSET+CDRX_DUMP_DPM_SIZE)
#ifdef CONFIG_RSR_ACC
#define CDRX_DUMP_RSRACC_SIZE          (0x400)
#else
#define CDRX_DUMP_RSRACC_SIZE          (0x0)
#endif
#define CDRX_DUMP_WAKELOCK_TIME_OFFSET (CDRX_DUMP_RSRACC_OFFSET+CDRX_DUMP_RSRACC_SIZE)
#define CDRX_DUMP_WAKELOCK_TIME_SIZE   (128)
#define CDRX_DUMP_IPC_OFFSET           (CDRX_DUMP_WAKELOCK_TIME_OFFSET+CDRX_DUMP_WAKELOCK_TIME_SIZE)
#define CDRX_DUMP_IPC_SIZE             (4*(IPC_INT_BUTTOM+IPC_SEM_BUTTOM))
#define CDRX_DUMP_ICC_OFFSET           (CDRX_DUMP_IPC_OFFSET+CDRX_DUMP_IPC_SIZE)
#define CDRX_DUMP_ICC_SIZE             (4*ICC_CHN_ID_MAX)
#ifdef CONFIG_NRCCPU_PM_DEBUG
#define NRPM_WAKEUP_DEBUG_SIZE (65*4)/*each 4 byte,65: 64 wakeup irq can set,and 1 for record latest wakeup irq*/
#else
#define NRPM_WAKEUP_DEBUG_SIZE 0
#endif
#define NRPM_WAKEUP_DEBUG_OFFSET (CDRX_DUMP_ICC_OFFSET+CDRX_DUMP_ICC_SIZE)

#define CPUIDLE_STAMP_OFFSET           (NRPM_WAKEUP_DEBUG_OFFSET+NRPM_WAKEUP_DEBUG_SIZE)
#define CPUIDLE_STAMP_SIZE             (4*CCPU_CORE_NUM*0x6*0x2)/*each stamp 4byte,each cpu record 6 stamp,0x2 means 32K stamp and hrt stamp*/

#define CCPU_WAKEUP_IRQ_NUM_MAX 50
#define CCPU_WAKEUP_IRQ_NUM_SEND 15
/* STAMP */
#define STAMP_START_ADDR            (0)
#define PM_DPM_FAIL_COUNT           (4+STAMP_START_ADDR)
/*start form pm_suspend()*/
#define STAMP_PM_SUSPEND_START      (4+PM_DPM_FAIL_COUNT)
#define STAMP_DPM_SUSPEND_FAIL      (4+STAMP_PM_SUSPEND_START)
#define STAMP_AFTER_DPM_SUSPEND     (4+STAMP_DPM_SUSPEND_FAIL)  /* dpm_suspend may fail and goto resume without being stamped.*/
#define STAMP_AFTER_UART_SUSPEND    (4+STAMP_AFTER_DPM_SUSPEND)
/* pm enter */
#define STAMP_PM_ENTER_START        (4+STAMP_AFTER_UART_SUSPEND)
#define STAMP_AFTER_DISABLE_GIC     (4+STAMP_PM_ENTER_START)
#define STAMP_AFTER_BAK_GIC         (4+STAMP_AFTER_DISABLE_GIC)
#define STAMP_AFTER_UTRACE_SUSPEND  (4+STAMP_AFTER_BAK_GIC)
#define STAMP_AFTER_TCXO_SUSPEND    (4+STAMP_AFTER_UTRACE_SUSPEND)
#define STAMP_AFTER_PIN_POWERDOWN   (4+STAMP_AFTER_TCXO_SUSPEND)
/*in sleep ASM power down*/
#define STAMP_SLEEP_ASM_ENTER       (4+STAMP_AFTER_PIN_POWERDOWN)
#define STAMP_BAK_COREG_BEGIN       (4+STAMP_SLEEP_ASM_ENTER)
#define STAMP_BAK_COREG_END         (4+STAMP_BAK_COREG_BEGIN)
#define STAMP_BAK_MMUREG_BEGIN      (4+STAMP_BAK_COREG_END)
#define STAMP_BAK_MMUREG_END        (4+STAMP_BAK_MMUREG_BEGIN)
#define STAMP_BEFORE_SEND_IPC       (4+STAMP_BAK_MMUREG_END)
#define STAMP_AFTER_SEND_IPC        (4+STAMP_BEFORE_SEND_IPC)

#define STAMP_AFTER_WFI_NOP         (4+STAMP_AFTER_SEND_IPC)   /* after wfi, should not be stamped */

/*in sleep ASM power up*/
#define STAMP_PWRUP_CODE_BEGIN      (4+STAMP_AFTER_WFI_NOP)
#define STAMP_RSTR_MMUREG_BEGIN     (4+STAMP_PWRUP_CODE_BEGIN)
#define STAMP_RSTR_MMUREG_END       (4+STAMP_RSTR_MMUREG_BEGIN)
#define STAMP_RSTR_COREG_BEGIN      (4+STAMP_RSTR_MMUREG_END)
#define STAMP_RSTR_COREG_END        (4+STAMP_RSTR_COREG_BEGIN)
/* out from sleep ASM,in pm enter */
#define STAMP_SLEEP_ASM_OUT         (4+STAMP_RSTR_COREG_END)
#define STAMP_AFTER_PIN_NORMAL      (4+STAMP_SLEEP_ASM_OUT)
#define STAMP_AFTER_TCXO_RESUME     (4+STAMP_AFTER_PIN_NORMAL)
#define STAMP_AFTER_UTRACE_RESUME   (4+STAMP_AFTER_TCXO_RESUME)
#define STAMP_AFTER_SLEEPOPS_RESUME    (4+STAMP_AFTER_UTRACE_RESUME)
#define STAMP_AFTER_ENABLE_GIC      (4+STAMP_AFTER_SLEEPOPS_RESUME)
#define STAMP_PM_ENTER_END          (4+STAMP_AFTER_ENABLE_GIC)
/* out from pm enter, in pm suspend*/
#define STAMP_AFTER_UART_RESUME     (4+STAMP_PM_ENTER_END)
#define STAMP_BEFORE_DPM_RESUME     (4+STAMP_AFTER_UART_RESUME)
#define STAMP_AFTER_DPM_RESUME      (4+STAMP_BEFORE_DPM_RESUME) /* after dpm_resume, pm_suspend return */

#define STAMP_DEBUG_1               (4*4+STAMP_AFTER_DPM_RESUME)
#define STAMP_DEBUG_2               (4+STAMP_DEBUG_1)
#define PM_SLICE_OVERFLOW_CNT       STAMP_DEBUG_1
#define PM_SLICE_OVERFLOW_STAMP     STAMP_DEBUG_2
#define PM_MODEM_LOG_ADDR_OFFSET   (4+STAMP_DEBUG_2)
#define PM_OUT_COUNT                (PM_MODEM_LOG_ADDR_OFFSET)
#define PM_ENTER_COUNT                (4+PM_OUT_COUNT)
#define PM_SUSPEND_MAXTIME                (4+PM_ENTER_COUNT)
#define PM_SUSPEND_MINTIME                (4+PM_SUSPEND_MAXTIME)
#define PM_SUSPEND_AVETIME                (4+PM_SUSPEND_MINTIME)
#define PM_RESUME_MAXTIME                (4+PM_SUSPEND_AVETIME)
#define PM_RESUME_MINTIME                (4+PM_RESUME_MAXTIME)
#define PM_RESUME_AVETIME               (4+PM_RESUME_MINTIME)
#define PM_SYS_WORK_TOTAL_TIME      (4+PM_RESUME_AVETIME)
#define PM_SYS_SLEEP_TOTAL_TIME     (4+PM_SYS_WORK_TOTAL_TIME)
#define PM_GIC_SUS_FAIL_TIME        (4+PM_SYS_SLEEP_TOTAL_TIME)
#define WAKEUP_INT_NUM              (4+PM_GIC_SUS_FAIL_TIME)
#define WAKEUP_INT_DUMP_SIZE        0xFC/*0X4+0XF0+0X8*/
#define WAKEUP_LATEST_INTER_NAME    (4+WAKEUP_INT_NUM)
#define WAKEUP_LATEST_INTER_SIZE    (0x8)
#define WAKEUP_IRQ_DEBUG            (WAKEUP_LATEST_INTER_SIZE+WAKEUP_LATEST_INTER_NAME)
#define WAKEUP_IRQ_DEBUG_SIZE       0xF0/*max 15 wakeup irq ,each 0x10 byte to record name and wake cnt*/
#define CHECK_STATUS_START          (WAKEUP_IRQ_DEBUG+WAKEUP_IRQ_DEBUG_SIZE) /*addr_start flag 0x5050a0a0*/
#define CHECK_CRG_CLKSTAT1          (4+CHECK_STATUS_START)
#define CHECK_CRG_CLKSTAT2          (4+CHECK_CRG_CLKSTAT1)
#define CHECK_CRG_CLKSTAT3          (4+CHECK_CRG_CLKSTAT2)
#define CHECK_CRG_CLKSTAT4          (4+CHECK_CRG_CLKSTAT3)
#define CHECK_CRG_CLKSTAT5          (4+CHECK_CRG_CLKSTAT4)
#define CHECK_PWR_STAT1             (4+CHECK_CRG_CLKSTAT5)
#define L2CACHE_RESUME_READY_OFFSET   (4+CHECK_PWR_STAT1)
#define L2CACHE_RESUME_READY_SIZE   (0X4*CCPU_CORE_NUM)

#if ((L2CACHE_RESUME_READY_OFFSET+L2CACHE_RESUME_READY_SIZE)>CDRX_DUMP_PM_SIZE)/*每次新添加。最后要检测是否越界*/
#error "pm dump size overflow"
#endif

#define PM_STAMP_START_FLAG     (0X5555AAAA)
#define pm_wakeirq_name_len 8
struct pm_wakeup_irq_info
{
    char name[pm_wakeirq_name_len];
    u32 wake_cnt;
    u32 wakeup_total_time;
};
struct pm_stat_info
{
    u32 resume_cnt;
    u32 suspend_cnt;
    u32 suspend_max;
    u32 suspend_min;
    u32 suspend_ave;
    u32 resume_max;
    u32 resume_min;
    u32 resume_ave;
    u32 sys_work_total_time;
    u32 sys_sleep_total_time;
    u32 gic_suspend_fail_cnt;
    u32 wakeup_int_cnt;/*ccore wakeup irq number*/
    char latest_wakeup_int_name[pm_wakeirq_name_len];
    struct pm_wakeup_irq_info wakeup_int_array[CCPU_WAKEUP_IRQ_NUM_SEND];
};

#define DRV_PM_INFO_SIZE  (sizeof(struct pm_stat_info))
#define DRV_INFO_DATA_LEN (DRV_PM_INFO_SIZE + CDRX_DUMP_WAKELOCK_TIME_SIZE)

/* dpm打点信息 */
#define CDRX_DUMP_DPM_STAMPS_OFFSET (CDRX_DUMP_DPM_OFFSET)
#define CDRX_DUMP_DPM_STAMPS_SIZE   (0x400)


/* dpm统计信息     各个device的失败次数 最大s/r时长 */
#define CDRX_DUMP_DPM_INFOS_OFFSET  (CDRX_DUMP_DPM_STAMPS_OFFSET + CDRX_DUMP_DPM_STAMPS_SIZE)
#define CDRX_DUMP_DPM_INFOS_SIZE    (0x300)
#define CDRX_DUMP_DPM_INFOS_END     (CDRX_DUMP_DPM_INFOS_OFFSET+CDRX_DUMP_DPM_INFOS_SIZE)

#define dpm_debug_char_num  (8)

/* dpm统计信息     各个device的失败次数 最大s/r时长 */
struct dpm_device_info
{
    char device_name[dpm_debug_char_num];
    u32 device_addr;
    u32 fail_cnt;
    u32 max_s;
    u32 max_r;
};

/**************++++++++++++++++++ 以下内容为模块内使用，外部组件不可以引用++++++++++++++++*************** */
/*dump区域格式
+------------------------+---------+ -->ALL: 160KB(acore/ccore:64KB, mcore:32KB)
|struct pm_om_file_header|         |
+------------------------+---------+ -->
|sub region1             |reg1 len |
+------------------------+---------+ -->
|sub region2             |reg2 len |
+------------------------+---------+ -->
*/
#define PM_LOG_OFF          (u8)(0)
#define PM_OM_INIT_MAGIC    (u32)(0x12345678)
#define PM_OM_PROT_MAGIC1   (u32)(0x5A5A5AA5)
#define PM_OM_PROT_MAGIC2   (u32)(0xA5A5A55A)
#define PM_OM_LOG_MAX_LEN   (u32)(512)
#define PM_OM_SOCP_OFFSET   (0x100000) /* socp占用的1MB空间 */
/*log区格式
+------------------------+---------+ -->ALL: 200K
|struct pm_om_file_header|         |
+------------------------+---------+ -->
|global serial number    |         |
+------------------------+---------+ -->
|nv config info          |         |
+------------------------+---------+ -->
|ring buffer info        |         |
+------------------------+---------+ -->
|log area                |         |
+------------------------+---------+ -->
*/
#define PM_OM_LOG_AREA      (char *)((unsigned long)SHM_BASE_ADDR + SHM_OFFSET_PM_OM)
#define PM_OM_BIT_64(x)     ((u64)(((u64)0x1)<<((u32)(x))))

#define pm_om_spin_lock(lock,flags) \
do{\
    spin_lock_irqsave(lock, flags);\
    (void)bsp_ipc_spin_lock(IPC_SEM_PM_OM_LOG);\
}while(0)

#define pm_om_spin_unlock(lock,flags) \
do{\
    (void)bsp_ipc_spin_unlock(IPC_SEM_PM_OM_LOG);\
    spin_unlock_irqrestore(lock, flags);\
}while(0)

#define pmom_pr_debug(fmt, ...)    \
    (bsp_debug("<%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define pmom_pr_warn(fmt, ...)      \
    (bsp_wrn("<%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define pmom_pr_info(fmt, ...)      \
    (bsp_info("<%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define pmom_pr_err(fmt, ...)      \
    (bsp_err("<%s> "fmt, __FUNCTION__, ##__VA_ARGS__))
#define pmom_print(fmt, ...)      \
    (bsp_err(fmt, ##__VA_ARGS__))

/*
 * struct pm_om_entry - The pm_om file header entry table structure
 * @magic    : indicating module's name defined by enum PM_OM_MAGIC
 * @offset    : offset to pm_om file header
 * @len        : length of module's length among DUMP/LOG area
 */
struct pm_om_head_entry
{
    u32  magic;
    u32  len;
    long offset;
};

/*
 * struct pm_om_header - The pm_om file header structure
 * @magic    : indicating DUMP or LOG file
 * @len        : file length
 * @data    : module private data which adjoin to len and stored in DUMP/LOG area
 */
struct pm_om_file_header
{
    u32 magic; /* "pmdp" or "pmlg" */
    u32 len;
    u32 version;
    u32 reserved;
};

struct pm_om_dump_cfg
{
    struct pm_om_file_header file_head;
    struct pm_om_head_entry  entry_tbl[PM_OM_MOD_ID_MAX];
};

struct pm_om_dump
{
    u32                   init_flag;
    char                  *base;
    char                  *sub_region;
    u32                   region_cnt;
    spinlock_t            lock;
    struct pm_om_dump_cfg *cfg;
};

struct pm_om_nv
{
    u32 mem_ctrl:1;       /* 控制bit位:使用哪种类型的memory */
    u32 reserved:31;      /* 控制bit位:保留 */
    u8  log_threshold[4]; /* acore/ccore/mcore触发写文件的阀值, 数值表示百分比 */
    u64 mod_sw_bitmap;    /* 各个模块log功能控制开关 */
}__attribute__ ((packed));

struct pm_om_smem_cfg
{
    struct pm_om_file_header file_head;
    u32                      sn;
    struct pm_om_nv          nv_cfg;
    struct log_mem           mem_info;
};/*lint !e959*/

struct pm_om_log
{
    u32                   init_flag;
    u32                   buf_is_full;
    u32                   threshold;
    spinlock_t            lock;
    struct ring_buffer    rb;
    struct pm_om_smem_cfg *smem;
};

struct pm_om_log_header
{
    u32 magic;
    u32 sn;
    u32 typeid;
    u32 timestamp;
    u32 length;
};

struct pm_info_list
{
    struct pm_info_usr_data usr_data;
    pm_info_cbfun cb_func;
    struct list_head entry;
};

struct pm_info_ctrl
{
    struct list_head list;
    spinlock_t       lock;
    u32              init_flag;
};

struct pm_om_ctrl
{
    struct pm_om_log    log;
    struct pm_om_dump   dump;
    struct pm_info_ctrl pm_info;
    void *debug;
    void *platform;
};

typedef u32 pm_om_icc_data_type;

static inline u32 __attribute__((unused))pm_om_log_mod_is_off(u64 mod_sw_bitmap, u32 mod_id)
{
    return (0 == (mod_sw_bitmap & PM_OM_BIT_64(mod_id)));
}

static inline void __attribute__((unused))pm_om_log_mod_off(u64 *mod_sw_bitmap, u32 mod_id)
{
    (*mod_sw_bitmap) &= (~(PM_OM_BIT_64(mod_id)));
}

static inline void __attribute__((unused))pm_om_log_mod_on(u64 *mod_sw_bitmap, u32 mod_id)
{
    (*mod_sw_bitmap) |= (PM_OM_BIT_64(mod_id));
}

static inline void __attribute__((unused))pm_om_log_sw_set(u64 *mod_sw_bitmap, u64 value)
{
    (*mod_sw_bitmap) = value;
}

struct pm_om_ctrl *pm_om_ctrl_get(void);

int bsp_pm_info_stat_register(pm_info_cbfun pcbfun, struct pm_info_usr_data *usr_data);

/* debug函数 */
void pm_om_log_on(void);
void pm_om_log_off(void);
void pm_om_dbg_on(void);
void pm_om_dbg_off(void);
void modem_log_fwrite_trigger_force(void);

#ifdef __cplusplus
}
#endif

#endif   /* __BSP_PM_OM_H__ */
