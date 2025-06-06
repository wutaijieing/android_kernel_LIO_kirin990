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

#ifndef __BSP_OM_ENUM_H__
#define __BSP_OM_ENUM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* field id定义只允许添加，不允许删除，枚举定义删除之后，id值需要保留，新添加id需要跳过删除id */
/* 后续新增id放到DYNAMIC区域，添加AP新的field，放到DUMP_AP_FIELD_END之前，添加CP新的field，放到DUMP_CP_FIELD_END之前 */
/* 添加M3新的field，放到DUMP_M3_FIELD_END之前 */
typedef enum{
#ifndef CONFIG_HISI_BALONG_EXTRA_MODEM
    /*非A+B平台modem ap使用的field id在此添加*/
    DUMP_MODEMAP_FIELD_BEGIN     = (0x010F0000),
    DUMP_MODEMAP_BASE_INFO       = (DUMP_MODEMAP_FIELD_BEGIN),
    DUMP_MODEMAP_TASK_SWITCH     = (0x010F0001),
    DUMP_MODEMAP_INTLOCK         = (0x010F0002),
    DUMP_MODEMAP_TASK_STACK      = (0x010F0003),
    DUMP_MODEMAP_INT_STACK       = (0x010F0004),
    DUMP_MODEMAP_ALLTASK         = (0x010F0005),
    DUMP_MODEMAP_ALLTASK_TCB     = (0x010F0006),
    DUMP_MODEMAP_PRINT           = (0x010F0007),
    DUMP_MODEMAP_REGS            = (0x010F0008),
    DUMP_MODEMAP_CPUVIEW         = (0x010F0009),
    DUMP_MODEMAP_USER_DATA       = (0x010F000B),
    DUMP_MODEMAP_LP              = (0x01100003),
    DUMP_MODEMAP_LP_BUSERROR     = (0x01100004),
    DUMP_MODEMAP_ICC             = (0x01100005),
    DUMP_MODEMAP_KERNEL_LOG      = (0x01100007),
    DUMP_MODEMAP_PM_OM           = (0x01100008),
    DUMP_MODEMAP_NV              = (0x01100009),
    DUMP_MODEMAP_CPUFREQ         = (0x0110000a),
    DUMP_MODEMAP_BASE_INFO_SMP   = (0x0110000b),
    DUMP_MODEMAP_CPRESET         = (0x0110000c),
    DUMP_MODEMAP_IPF             = (0x0110000d),
    DUMP_MODEMAP_COLD_PATCH      = (0x0110000e),
    DUMP_MODEMAP_KO_DUMP         = (0x0110000f),
    DUMP_MODEMAP_MAA             = (0x01100010),
    DUMP_MODEMAP_HISOCK_DUMP     = (0x01100011),
    DUMP_MODEMAP_PCIE_RC         = (0x01100012),
    DUMP_MODEMAP_APR             = (0x01100013),
    DUMP_MODEMAP_PCDEV           = (0x01100014),
    DUMP_MODEMAP_DEBUG           = (0x01100015),
    DUMP_MODEMAP_CBOOT           = (0x01100016),
    DUMP_MODEMAP_SEC_DUMP        = (0x01100017),
    DUMP_MODEMAP_LAST_KMSG       = (0x01100018),
    DUMP_MODEMAP_DIAG_PPM        = (0x01100019),
    DUMP_MODEMAP_RFILE           = (0x0110001a),
    DUMP_MODEMAP_DUMP_LOG_RECORD = (0x0110001b),
    DUMP_MODEMAP_MLOADER         = (0x0110001c),
    DUMP_MODEMAP_FIELD_END       = (0x01ffffff),
#else
    /*A+B平台modem ap使用的field id在此添加*/
    DUMP_MODEMAP_FIELD_BEGIN     = (0x0C0F0000),
    DUMP_MODEMAP_BASE_INFO       = (DUMP_MODEMAP_FIELD_BEGIN),
    DUMP_MODEMAP_TASK_SWITCH     = (0x0C0F0001),
    DUMP_MODEMAP_INTLOCK         = (0x0C0F0002),
    DUMP_MODEMAP_TASK_STACK      = (0x0C0F0003),
    DUMP_MODEMAP_INT_STACK       = (0x0C0F0004),
    DUMP_MODEMAP_ALLTASK         = (0x0C0F0005),
    DUMP_MODEMAP_ALLTASK_TCB     = (0x0C0F0006),
    DUMP_MODEMAP_PRINT           = (0x0C0F0007),
    DUMP_MODEMAP_REGS            = (0x0C0F0008),
    DUMP_MODEMAP_CPUVIEW         = (0x0C0F0009),
    DUMP_MODEMAP_USER_DATA       = (0x0C0F000B),
    DUMP_MODEMAP_LP              = (0x0C100003),
    DUMP_MODEMAP_LP_BUSERROR     = (0x0C100004),
    DUMP_MODEMAP_ICC             = (0x0C100005),
    DUMP_MODEMAP_KERNEL_LOG      = (0x0C100007),
    DUMP_MODEMAP_PM_OM           = (0x0C100008),
    DUMP_MODEMAP_NV              = (0x0C100009),
    DUMP_MODEMAP_CPUFREQ         = (0x0C10000a),
    DUMP_MODEMAP_BASE_INFO_SMP   = (0x0C10000b),
    DUMP_MODEMAP_CPRESET         = (0x0C10000c),
    DUMP_MODEMAP_IPF             = (0x0C10000d),
    DUMP_MODEMAP_COLD_PATCH      = (0x0C10000e),
    DUMP_MODEMAP_KO_DUMP         = (0x0C10000f),
    DUMP_MODEMAP_MAA             = (0x0C100010),
    DUMP_MODEMAP_HISOCK_DUMP     = (0x0C100011),
    DUMP_MODEMAP_PCIE_RC         = (0x0C100012),
    DUMP_MODEMAP_APR             = (0x0C100013),
    DUMP_MODEMAP_PCDEV           = (0x0C100014),
    DUMP_MODEMAP_DEBUG           = (0x0C100015),
    DUMP_MODEMAP_DIAG_PPM        = (0x0C100016),
    DUMP_MODEMAP_PMU             = (0x0C100017),
    DUMP_MODEMAP_MLOADER         = (0x0C100018),
    DUMP_MODEMAP_FIELD_END       = (0x0Cffffff),

#endif
    /*LR CP FIELD IDs*/
    DUMP_LRCCPU_FIELD_BEGIN          = (0x02000000),
    DUMP_LRCCPU_BASE_INFO            = DUMP_LRCCPU_FIELD_BEGIN,
    DUMP_LRCCPU_TASK_SWITCH          = (0x02000001),
    DUMP_LRCCPU_INTLOCK              = (0x02000002),
    DUMP_LRCCPU_TASK_STACK           = (0x02000003),
    DUMP_LRCCPU_SYSTEM_STACK         = (0x02000004),
    DUMP_LRCCPU_ALLTASK_NAME         = (0x02000005),
    DUMP_LRCCPU_ALLTASK_TCB          = (0x02000006),
    DUMP_LRCCPU_PRINT                = (0x02000007),
    DUMP_LRCCPU_DMESG                = (0x02000008),
    DUMP_LRCCPU_REGS                 = (0x02000009),
    DUMP_LRCCPU_CPUVIEW              = (0x0200000A),
    DUMP_LRCCPU_USER_DATA            = (0x0200000C),
    DUMP_LRCCPU_BBE16_TCM            = (0x0200000D),
    DUMP_LRCCPU_DRX                  = (0x0200000E),
    DUMP_LRCCPU_TASK_TCB             = (0x0200000F),
    DUMP_LRCCPU_RTOSCK_CDA           = (0x02000010),
    DUMP_LRCCPU_SIM0                 = (0x02000064),
    DUMP_LRCCPU_SIM1                 = (0x02000065),
    DUMP_LRCCPU_LP                   = (0x02000067),
    DUMP_LRCCPU_LP_BUSERROR          = (0x02000068),
    DUMP_LRCCPU_UTRACE0              = (0x02000069),
    DUMP_LRCCPU_UTRACE1              = (0x0200006a),
    DUMP_LRCCPU_UTRACE2              = (0x0200006b),
    DUMP_LRCCPU_UTRACE3              = (0x0200006c),
    DUMP_LRCCPU_ICC                  = (0x0200006E),
    DUMP_LRCCPU_AMON                 = (0x02000070),
    DUMP_LRCCPU_RUN_TRACE            = (0x02000072),
    DUMP_LRCCPU_SYSCTRL              = (0x02000074),
    DUMP_LRCCPU_PM_OM                = (0x02000075),
    DUMP_LRCCPU_SAVE_MOD_DUAL_MODEM  = (0x02000076),
    DUMP_LRCCPU_CIPHER               = (0x02000077),
    DUMP_LRCCPU_IPF                  = (0x02000078),
    DUMP_LRCCPU_PSAM                 = (0x02000079),
    DUMP_LRCCPU_PDLOCK               = (0x0200007a),
    DUMP_LRCCPU_NV                   = (0x0200007b),
    DUMP_LRCCPU_OS_SYS               = (0x0200007C),
    DUMP_LRCCPU_TASK_SWITCH1         = (0x0200007d),
    DUMP_LRCCPU_TASK_SWITCH2         = (0x0200007e),
    DUMP_LRCCPU_TASK_SWITCH3         = (0x0200007f),
    DUMP_LRCCPU_SYSTEM_STACK1        = (0x02000080),
    DUMP_LRCCPU_SYSTEM_STACK2        = (0x02000081),
    DUMP_LRCCPU_SYSTEM_STACK3        = (0x02000082),
    DUMP_LRCCPU_SEM_INFO             = (0x02000083),
    DUMP_LRCCPU_NOC                  = (0x02000084),
    DUMP_LRCCPU_EDMA                 = (0x02000085),
    DUMP_LRCCPU_CPM_LTE0             = (0x02000086),
    DUMP_LRCCPU_CPM_LTE1             = (0x02000087),
    DUMP_LRCCPU_CPM_TDS              = (0x02000088),
    DUMP_LRCCPU_CPM_NXP              = (0x02000089),
    DUMP_LRCCPU_CPM_1X               = (0x0200008A),
    DUMP_LRCCPU_DUMP_DEBUG           = (0x02000090),
    DUMP_LRCCPU_EXCINFO              = (0x02000091),
    DUMP_LRCCPU_REBOOTCONTEX         = (0x02000092),
    DUMP_LRCCPU_WDT                  = (0x02000093),
    DUMP_LRCCPU_FREQ                 = (0x02000094),
    DUMP_LRCCPU_PMU                  = (0x02000095),
    DUMP_LRCCPU_FIQ                  = (0x02000096),
    DUMP_LRCCPU_DSP0                 = (0x02000097),
    DUMP_LRCCPU_BBP                  = (0x02000098),
    DDUMP_LRCCPU_DSP1                = (0x02000099),
    DDUMP_LRCCPU_LTEV                = (0x0200009a),
    DUMP_LRCCPU_EICC                 = (0x0200009b),
    /*0x02F00000-- 专用于smp修改新增的特定格式段其他模块不得使用*/
    DUMP_LRCCPU_BASE_INFO_SMP        = (0x02f00000),
    DUMP_LRCCPU_CPUINFO              = (0x02f00001),
    DUMP_LRCCPU_CPUINFO1             = (0x02f00002),
    DUMP_LRCCPU_CPUINFO2             = (0x02f00003),
    DUMP_LRCCPU_CPUINFO3             = (0x02f00004),
    DUMP_LRCCPU_TASK_TCB_SMP         = (0x02f00005),
    DUMP_LRCCPU_DEBUGREG_CPU         = (0x02f00006),/*0x02f00006 ~ 0x02f0000d :8个预留给Debug Reg保存使用*/
    DUMP_LRCCPU_DEBUGREG_CPU_END     = (0x02f0000d),
    DUMP_LRCCPU_TASK_TCB_SMP_NEW     = (0x02f0000e),
    DUMP_LRCCPU_SEC_DUMP_TARNS       = (0x02f0000f),
    DUMP_LRCCPU_NRCPM_NR             = (0x02f00010),
    DUMP_LRCCPU_NRCPM_DSP            = (0x02f00011),
    DUMP_LRCCPU_FIELD_LLT            = (0x02fffffe),
    DUMP_LRCCPU_FIELD_END            = (0x02ffffff),

    /*AP kernel FIELD IDs,对应原来位于system/kernel/driver/hisi目录下的代码，现在位于modem/drv/acore/hisi目录下的代码*/
    DUMP_KERNEL_FIELD_BEGIN     = (0x03000000),
    DUMP_KERNEL_BASE_INFO       = (DUMP_KERNEL_FIELD_BEGIN),
    DUMP_KERNEL_TASK_SWITCH     = (0x030F0001),
    DUMP_KERNEL_INTLOCK         = (0x030F0002),
    DUMP_KERNEL_TASK_STACK      = (0x030F0003),
    DUMP_KERNEL_INT_STACK       = (0x030F0004),
    DUMP_KERNEL_TASK_NAME       = (0x030F0005),
    DUMP_KERNEL_ALLTASK_TCB     = (0x030F0006),
    DUMP_KERNEL_PRINT           = (0x030F0007),
    DUMP_KERNEL_REGS            = (0x030F0008),
    DUMP_KERNEL_CPUVIEW         = (0x030F0009),
    DUMP_KERNEL_USER_DATA       = (0x030F000B),
    DUMP_KERNEL_AMON            = (0x030F000C),
    DUMP_KERNEL_RUNNING_TRACE   = (0x030F000D),/*0x030F000D ~ 0x030F0014 :8个预留给running trace使用*/
    DUMP_KERNEL_CPU_INFO        = (0x030F0015),/*0x030F0015 ~ 0x030F001D :8个预留给cpu info使用*/
    DUMP_KERNEL_BASE_INFO1      = (0x030F001E),/*SMP 系统新增扩展baseinfo ID*/
    DUMP_KERNEL_TASK_TCB        = (0x030F001F),
    DUMP_KERNEL_DEBUGREG_CPU    = (0x030F0020),/*0x030F0020 ~ 0x030F0027 :8个预留给Debug Reg保存使用*/
    DUMP_KERNEL_DEBUGREG_CPU_END= (0x030F0027),
    DUMP_KERNEL_USB             = (0x03100000),
    DUMP_KERNEL_UTRACE          = (0x03100001),
    DUMP_KERNEL_DRX             = (0x03100002),
    DUMP_KERNEL_UTRACE0         = (0x03100003),
    DUMP_KERNEL_UTRACE1         = (0x03100004),
    DUMP_KERNEL_UTRACE2         = (0x03100005),
    DUMP_KERNEL_UTRACE3         = (0x03100006),
    DUMP_KERNEL_NOC             = (0x03100007),
    DUMP_KERNEL_PDLOCK          = (0x03100008),
    DUMP_KERNEL_PCIE_EP         = (0x03100009),
    DUMP_KERNEL_DRV_HIFI        = (0x0310000A),
    DUMP_KERNEL_GMAC            = (0x03100010),
    DUMP_KERNEL_FIELD_END       = (0x03ffffff),

    /*M3 FIELD IDs*/
    DUMP_M3_FIELD_BEGIN         = (0x04000000),
    DUMP_M3_LP_BUSERROR         = (DUMP_M3_FIELD_BEGIN),
    DUMP_M3_ICC                 = (0x04000001),
    DUMP_M3_PM_OM               = (0x04000002),
    DUMP_M3_BASE_INFO           = (0x04000020),
    DUMP_M3_TASK_SWITCH         = (0x04000021),
    DUMP_M3_INTLOCK             = (0x04000022),
    DUMP_M3_TASK_STACK          = (0x04000023),
    DUMP_M3_INT_STACK           = (0x04000024),
    DUMP_M3_ALLTASK             = (0x04000025),
    DUMP_M3_ALLTASK_TCB         = (0x04000026),
    DUMP_M3_LOG                 = (0x04000027),
    DUMP_M3_PRINT               = (0x04000028),
    DUMP_M3_REGS                = (0x04000029),
    DUMP_M3_TCM0                = (0x0400002A),
    DUMP_M3_TCM1                = (0x0400002B),
    DUMP_M3_DDRMNT              = (0x0400002C),
    DUMP_M3_APB_SPI             = (0x0400002d),
    DUMP_M3_BBIC                = (0x0400002e),
    DUMP_M3_IRQ                 = (0x0400002F),
    DUMP_M3_PMCTRL                 = (0x04000030),
    DUMP_M3_BOOST               = (0x04000064),
    DUMP_M3_AO_SYSCTRL          = (0x04000065),
    DUMP_M3_AO_CRG              = (0x04000066),
    DUMP_M3_PD_SYSCTRL          = (0x04000067),
    DUMP_M3_PERI_SYSCTRL        = (0x04000068),
    DUMP_M3_PERI_CRG            = (0x04000069),
    /*0x04100000--0x04200000 专用于smp修改新增的段其他模块不得使用*/
    DUMP_M3_SMP_START           = (0x04100000),
    DUMP_M3_CPUINFO             = (DUMP_M3_SMP_START),
    DUMP_M3_BASEINFO_SMP        = (DUMP_M3_SMP_START+1),
    DUMP_M3_SMP_END             = (0x041FFFFF),
    DUMP_M3_FIELD_END           = (0x04FFFFFF),

    /* SHARE FIELD IDs */
    DUMP_SHARE_FIELD_BEGIN      = (0x05000000),
    DUMP_SHARE_GLOBAL_INFO      = (DUMP_SHARE_FIELD_BEGIN),
    DUMP_SHARE_LOAD_INFO        = (0x05000001),
    DUMP_SHARE_FASTBOOT_INFO    = (0x05000002),
    DUMP_SHARE_FIELD_END        = (0x05FFFFFF),

    /* TEEOS FIELD IDs */
    DUMP_TEE_FIELD_BEGIN        = (0x06000000),
    DUMP_TEE_FIELD_LOG          = (0x06000001),
    DUMP_TEE_FIELD_END          = (0x06FFFFFF),

    /*NR CCPU FIELD IDs*/
    DUMP_NRCCPU_FIELD_BEGIN          = (0x07000000),
    DUMP_NRCCPU_BASE_INFO            = DUMP_NRCCPU_FIELD_BEGIN,
    DUMP_NRCCPU_TASK_SWITCH          = (0x07000001),
    DUMP_NRCCPU_INTLOCK              = (0x07000002),
    DUMP_NRCCPU_TASK_STACK           = (0x07000003),
    DUMP_NRCCPU_SYSTEM_STACK         = (0x07000004),
    DUMP_NRCCPU_ALLTASK_NAME         = (0x07000005),
    DUMP_NRCCPU_ALLTASK_TCB          = (0x07000006),
    DUMP_NRCCPU_PRINT                = (0x07000007),
    DUMP_NRCCPU_DMESG                = (0x07000008),
    DUMP_NRCCPU_REGS                 = (0x07000009),
    DUMP_NRCCPU_CPUVIEW              = (0x0700000A),
    DUMP_NRCCPU_USER_DATA            = (0x0700000C),
    DUMP_NRCCPU_BBE16_TCM            = (0x0700000D),
    DUMP_NRCCPU_DRX                  = (0x0700000E),
    DUMP_NRCCPU_TASK_TCB             = (0x0700000F),
    DUMP_NRCCPU_RTOSCK_CDA           = (0x07000010),
    DUMP_NRCCPU_SIM0                 = (0x07000064),
    DUMP_NRCCPU_SIM1                 = (0x07000065),
    DUMP_NRCCPU_LP                   = (0x07000067),
    DUMP_NRCCPU_LP_BUSERROR          = (0x07000068),
    DUMP_NRCCPU_UTRACE0              = (0x07000069),
    DUMP_NRCCPU_UTRACE1              = (0x0700006a),
    DUMP_NRCCPU_UTRACE2              = (0x0700006b),
    DUMP_NRCCPU_UTRACE3              = (0x0700006c),
    DUMP_NRCCPU_ICC                  = (0x0700006E),
    DUMP_NRCCPU_AMON                 = (0x07000070),
    DUMP_NRCCPU_RUN_TRACE            = (0x07000072),
    DUMP_NRCCPU_SYSCTRL              = (0x07000074),
    DUMP_NRCCPU_PM_OM                = (0x07000075),
    DUMP_NRCCPU_SAVE_MOD_DUAL_MODEM  = (0x07000076),
    DUMP_NRCCPU_CIPHER               = (0x07000077),
    DUMP_NRCCPU_IPF                  = (0x07000078),
    DUMP_NRCCPU_PSAM                 = (0x07000079),
    DUMP_NRCCPU_PDLOCK               = (0x0700007a),
    DUMP_NRCCPU_NV                   = (0x0700007b),
    DUMP_NRCCPU_OS_SYS               = (0x0700007C),
    DUMP_NRCCPU_TASK_SWITCH1         = (0x0700007d),
    DUMP_NRCCPU_TASK_SWITCH2         = (0x0700007e),
    DUMP_NRCCPU_TASK_SWITCH3         = (0x0700007f),
    DUMP_NRCCPU_SYSTEM_STACK1        = (0x07000080),
    DUMP_NRCCPU_SYSTEM_STACK2        = (0x07000081),
    DUMP_NRCCPU_SYSTEM_STACK3        = (0x07000082),
    DUMP_NRCCPU_SEM_INFO             = (0x07000083),
    DUMP_NRCCPU_NOC                  = (0x07000084),
    DUMP_NRCCPU_EDMA                 = (0x07000085),
    DUMP_NRCCPU_CPM_LTE0             = (0x07000086),
    DUMP_NRCCPU_CPM_LTE1             = (0x07000087),
    DUMP_NRCCPU_CPM_TDS              = (0x07000088),
    DUMP_NRCCPU_CPM_NXP              = (0x07000089),
    DUMP_NRCCPU_CPM_1X               = (0x0700008A),
    DUMP_NRCCPU_DUMP_DEBUG           = (0x07000090),
    DUMP_NRCCPU_EXCINFO              = (0x07000091),
    DUMP_NRCCPU_REBOOTCONTEX         = (0x07000092),
    DUMP_NRCCPU_WDT                  = (0x07000093),
    DUMP_NRCCPU_FREQ                 = (0x07000094),
    DUMP_NRCCPU_PMU                  = (0x07000095),
    DUMP_NRCCPU_FIQ                  = (0x07000096),
    DUMP_NRCCPU_DSP0                 = (0x07000097),
    DUMP_NRCCPU_BBP                  = (0x07000098),
    DUMP_NRCCPU_NRCPM_NR             = (0x07000099),
    DUMP_NRCCPU_NRCPM_NXP0           = (0x0700009a),
    DUMP_NRCCPU_EICC                 = (0x0700009b),
    DUMP_NRCCPU_CRG_MDM5G            = (0x0700009c),
    DUMP_NRCCPU_SYSCTRL_MDM5G        = (0x0700009d),
    DUMP_NRCCPU_SYSCTRL_CCPU5G       = (0x0700009e),
    DUMP_NRCCPU_PMCTRL       = (0x0700009f),
    /*0x02F00000-- 专用于smp修改新增的特定格式段其他模块不得使用*/
    DUMP_NRCCPU_BASE_INFO_SMP        = (0x07f00000),
    DUMP_NRCCPU_CPUINFO              = (0x07f00001),
    DUMP_NRCCPU_CPUINFO1             = (0x07f00002),
    DUMP_NRCCPU_CPUINFO2             = (0x07f00003),
    DUMP_NRCCPU_CPUINFO3             = (0x07f00004),
    DUMP_NRCCPU_TASK_TCB_SMP         = (0x07f00005),
    DUMP_NRCCPU_DEBUGREG_CPU         = (0x07f00006),/*0x02f00006 ~ 0x02f0000d :8个预留给Debug Reg保存使用*/
    DUMP_NRCCPU_DEBUGREG_CPU_END     = (0x07f0000d),
    DUMP_NRCCPU_TASK_TCB_SMP_NEW     = (0x07f0000e),
    DUMP_NRCCPU_SEC_DUMP_TARNS       = (0x07f0000f),
    DUMP_NRCCPU_FIELD_LLT            = (0x07fffffe),
    DUMP_NRCCPU_FIELD_END            = (0x07ffffff),

    /*NRL2HAC FIELD IDs*/
    DUMP_NRL2HAC_FIELD_BEGIN          = (0x08000000),
    DUMP_NRL2HAC_BASE_INFO            = DUMP_NRL2HAC_FIELD_BEGIN,
    DUMP_NRL2HAC_TASK_SWITCH          = (0x08000001),
    DUMP_NRL2HAC_INTLOCK              = (0x08000002),
    DUMP_NRL2HAC_TASK_STACK           = (0x08000003),
    DUMP_NRL2HAC_SYSTEM_STACK         = (0x08000004),
    DUMP_NRL2HAC_ALLTASK_NAME         = (0x08000005),
    DUMP_NRL2HAC_ALLTASK_TCB          = (0x08000006),
    DUMP_NRL2HAC_PRINT                = (0x08000007),
    DUMP_NRL2HAC_DMESG                = (0x08000008),
    DUMP_NRL2HAC_REGS                 = (0x08000009),
    DUMP_NRL2HAC_CPUVIEW              = (0x0800000A),
    DUMP_NRL2HAC_USER_DATA            = (0x0800000C),
    DUMP_NRL2HAC_BBE16_TCM            = (0x0800000D),
    DUMP_NRL2HAC_DRX                  = (0x0800000E),
    DUMP_NRL2HAC_TASK_TCB             = (0x0800000F),
    DUMP_NRL2HAC_RTOSCK_CDA           = (0x08000010),
    DUMP_NRL2HAC_SIM0                 = (0x08000064),
    DUMP_NRL2HAC_SIM1                 = (0x08000065),
    DUMP_NRL2HAC_LP                   = (0x08000067),
    DUMP_NRL2HAC_LP_BUSERROR          = (0x08000068),
    DUMP_NRL2HAC_UTRACE0              = (0x08000069),
    DUMP_NRL2HAC_UTRACE1              = (0x0800006a),
    DUMP_NRL2HAC_UTRACE2              = (0x0800006b),
    DUMP_NRL2HAC_UTRACE3              = (0x0800006c),
    DUMP_NRL2HAC_ICC                  = (0x0800006E),
    DUMP_NRL2HAC_AMON                 = (0x08000070),
    DUMP_NRL2HAC_RUN_TRACE            = (0x08000072),
    DUMP_NRL2HAC_SYSCTRL              = (0x08000074),
    DUMP_NRL2HAC_PM_OM                = (0x08000075),
    DUMP_NRL2HAC_SAVE_MOD_DUAL_MODEM  = (0x08000076),
    DUMP_NRL2HAC_CIPHER               = (0x08000077),
    DUMP_NRL2HAC_IPF                  = (0x08000078),
    DUMP_NRL2HAC_PSAM                 = (0x08000079),
    DUMP_NRL2HAC_PDLOCK               = (0x0800007a),
    DUMP_NRL2HAC_NV                   = (0x0800007b),
    DUMP_NRL2HAC_OS_SYS               = (0x0800007C),
    DUMP_NRL2HAC_TASK_SWITCH1         = (0x0800007d),
    DUMP_NRL2HAC_TASK_SWITCH2         = (0x0800007e),
    DUMP_NRL2HAC_TASK_SWITCH3         = (0x0800007f),
    DUMP_NRL2HAC_SYSTEM_STACK1        = (0x08000080),
    DUMP_NRL2HAC_SYSTEM_STACK2        = (0x08000081),
    DUMP_NRL2HAC_SYSTEM_STACK3        = (0x08000082),
    DUMP_NRL2HAC_SEM_INFO             = (0x08000083),
    DUMP_NRL2HAC_NOC                  = (0x08000084),
    DUMP_NRL2HAC_EDMA                 = (0x08000085),
    DUMP_NRL2HAC_CPM_LTE0             = (0x08000086),
    DUMP_NRL2HAC_CPM_LTE1             = (0x08000087),
    DUMP_NRL2HAC_CPM_TDS              = (0x08000088),
    DUMP_NRL2HAC_CPM_NXP              = (0x08000089),
    DUMP_NRL2HAC_CPM_1X               = (0x0800008A),
    DUMP_NRL2HAC_DUMP_DEBUG           = (0x08000090),
    DUMP_NRL2HAC_EXCINFO              = (0x08000091),
    DUMP_NRL2HAC_REBOOTCONTEX         = (0x08000092),
    DUMP_NRL2HAC_WDT                  = (0x08000093),
    DUMP_NRL2HAC_FREQ                 = (0x08000094),
    DUMP_NRL2HAC_PMU                  = (0x08000095),
    DUMP_NRL2HAC_FIQ                  = (0x08000096),
    DUMP_NRL2HAC_DSP0                 = (0x08000097),
    DUMP_NRL2HAC_EICC                  = (0x08000098),
    /*0x02F00000-- 专用于smp修改新增的特定格式段其他模块不得使用*/
    DUMP_NRL2HAC_BASE_INFO_SMP        = (0x08f00000),
    DUMP_NRL2HAC_CPUINFO              = (0x08f00001),
    DUMP_NRL2HAC_CPUINFO1             = (0x08f00002),
    DUMP_NRL2HAC_CPUINFO2             = (0x08f00003),
    DUMP_NRL2HAC_CPUINFO3             = (0x08f00004),
    DUMP_NRL2HAC_TASK_TCB_SMP         = (0x08f00005),
    DUMP_NRL2HAC_DEBUGREG_CPU         = (0x08f00006),/*0x02f00006 ~ 0x02f0000d :8个预留给Debug Reg保存使用*/
    DUMP_NRL2HAC_DEBUGREG_CPU_END     = (0x08f0000d),
    DUMP_NRL2HAC_TASK_TCB_SMP_NEW     = (0x08f0000e),
    DUMP_NRL2HAC_FIELD_LLT            = (0x08fffffe),
    DUMP_NRL2HAC_FIELD_END            = (0x08FFFFFF),


    DUMP_NRHL1C_FIELD_BEGIN          = (0x09000000),
    DUMP_NRHL1C_FIELD_END            = (0x09FFFFFF),

    DUMP_NRPHY_FIELD_BEGIN           = (0x0a000000),
    DUMP_NRPHY_FIELD_END             = (0x0aFFFFFF),

    DUMP_RF_FIELD_BEGIN              = (0x0b000000),
    DUMP_RF_FIELD_END                = (0x0bFFFFFF),

    DUMP_PHONEAP_FIELD_BEGIN         = (0x0c000000),
    DUMP_PHONEAP_FIELD_END           = (0x0cFFFFFF),

    DUMP_MDM_LPMCU_FIELD_BEGIN       = (0x0d000000),
    DUMP_MDM_LPMCU_FIELD_END         = (0x0d00ffff),
}DUMP_SAVE_MOD_ENUM;


#define AP_TRACE_ID(id)                 (DUMP_KERNEL_UTRACE##id)

#define NR_TRACE_ID(id)                 (DUMP_NRCCPU_UTRACE0+id)

#define LR_TRACE_ID(id)                 (DUMP_LRCCPU_UTRACE0+id)
#ifdef __OS_NRCCPU__
#define CP_TRACE_ID(id)                 NR_TRACE_ID(id)
#else
#define CP_TRACE_ID(id)                 LR_TRACE_ID(id)
#endif

#define HAC_TRACE_ID(id)                 (DUMP_NRL2HAC_UTRACE0+id)


typedef enum {
    TEEOS_ERRNO_LOAD_SEC_IMAGE  = 0x83000000,   /* modem单独复位中,加载安全镜像失败 */
    TEEOS_ERRNO_BUTT            = 0x83ffffff
}dump_teeos_errno_e;

typedef enum {
    HIFI_ERRNO_MODEM_RESET      = 0x84000000,   /* modem单独复位中HIFI回调失败 */
    HIFI_ERRNO_BUTT             = 0x84ffffff
}dump_hifi_errno_e;

typedef enum {
    LPM3_ERRNO_MODEM_RESET      = 0x85000000,   /* modem单独复位中M3异常 */
    LPM3_ERRNO_BUTT             = 0x85ffffff
}dump_mcore_errno_e;

#ifdef __cplusplus
}
#endif

#endif /* __BSP_OM_ENUM_H__ */


