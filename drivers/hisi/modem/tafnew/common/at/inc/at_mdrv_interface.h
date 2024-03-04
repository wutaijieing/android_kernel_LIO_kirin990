/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2012-2019. All rights reserved.
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

#ifndef _AT_MDRV_INTERFACE_H_
#define _AT_MDRV_INTERFACE_H_

#include "vos.h"
#include "AtMntn.h"
#include "mdrv.h"
#include "mdrv_slt.h"
#if (!defined(MODEM_FUSION_VERSION)) || (defined(LLT_OS_VER))
#include "msp_diag.h"
#else
#include "mdrv_diag.h"
#endif

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*lint -esym(528,AT_SwitchDiagLogPort)*/
static inline VOS_UINT32 AT_SwitchDiagLogPort(VOS_UINT32 phyPort, VOS_UINT32 effect)
{
    AT_PR_LOGI("enter");
    return mdrv_diag_log_port_switch(phyPort, effect);
}


/*lint -esym(528,AT_SetDrvGpio)*/
static inline VOS_INT32 AT_SetDrvGpio(VOS_UINT8 group, VOS_UINT8 pin, VOS_UINT8 value)
{
    AT_PR_LOGI("enter");
    return DRV_GPIO_SET(group, pin, value);
}


/*lint -esym(528,AT_IsCryptoSecStarted)*/
static inline VOS_INT32 AT_IsCryptoSecStarted(VOS_UINT8 *data)
{
    AT_PR_LOGI("enter");
    return mdrv_crypto_sec_started(data);
}


/*lint -esym(528,AT_IsCryptoSecbootSupported)*/
static inline VOS_INT32 AT_IsCryptoSecbootSupported(VOS_UINT8 *data)
{
    AT_PR_LOGI("enter");
    return mdrv_crypto_secboot_supported(data);
}


/*lint -esym(528,AT_CryptoStartSecure)*/
static inline VOS_INT32 AT_CryptoStartSecure(VOS_VOID)
{
    AT_PR_LOGI("enter");
    return mdrv_crypto_start_secure();
}


/*lint -esym(528,AT_GetDloadInfo)*/
static inline DLOAD_VER_INFO_S* AT_GetDloadInfo(VOS_VOID)
{
    AT_PR_LOGI("enter");
    return mdrv_dload_get_info();
}


/*lint -esym(528,AT_SetDloadCurMode)*/
static inline VOS_VOID AT_SetDloadCurMode(DLOAD_MODE_E mode)
{
    AT_PR_LOGI("enter");
    mdrv_dload_set_curmode(mode);
}


/*lint -esym(528,AT_SetDloadSoftload)*/
static inline VOS_VOID AT_SetDloadSoftload(VOS_INT32 softLoad)
{
    AT_PR_LOGI("enter");
    mdrv_dload_set_softload(softLoad);
}


/*lint -esym(528,AT_GetEfuseDieid)*/
static inline VOS_INT32 AT_GetEfuseDieid(VOS_UINT8 *buf, VOS_INT32 length)
{
    AT_PR_LOGI("enter");
    return mdrv_efuse_get_dieid(buf, length);
}

/*lint -esym(528,AT_EfuseIoctl)*/
static inline VOS_INT32 AT_EfuseIoctl(VOS_INT32 cmd, VOS_INT32 arg, VOS_UINT8 *buf, VOS_INT32 len)
{
    AT_PR_LOGI("enter");
    return mdrv_efuse_ioctl(cmd, arg, buf, len);
}


/*lint -esym(528,AT_GetMiscBatteryState)*/
static inline VOS_INT32 AT_GetMiscBatteryState(BATT_STATE_S *batteryState)
{
    AT_PR_LOGI("enter");
    return mdrv_misc_get_battery_state(batteryState);
}


/*lint -esym(528,AT_GetMiscCbcState)*/
static inline VOS_INT32 AT_GetMiscCbcState(VOS_UINT8 *bcs, VOS_UINT8 *bcl)
{
    AT_PR_LOGI("enter");
    return mdrv_misc_get_cbc_state(bcs, bcl);
}

/*lint -esym(528,AT_GetMiscChargeState)*/
static inline VOS_INT32 AT_GetMiscChargeState(VOS_VOID)
{
    AT_PR_LOGI("enter");
    return mdrv_misc_get_charge_state();
}


/*lint -esym(528,AT_GetMiscChargingStatus)*/
static inline VOS_INT32 AT_GetMiscChargingStatus(VOS_VOID)
{
    AT_PR_LOGI("enter");
    return mdrv_misc_get_charging_status();
}


/*lint -esym(528,AT_SetMiscChargeState)*/
static inline VOS_VOID AT_SetMiscChargeState(unsigned long state)
{
    AT_PR_LOGI("enter");
    mdrv_misc_set_charge_state(state);
}

/*lint -esym(528,AT_SplyMiscBattery)*/
static inline VOS_INT32 AT_SplyMiscBattery(VOS_VOID)
{
    AT_PR_LOGI("enter");
    return mdrv_misc_sply_battery();
}


/*lint -esym(528,AT_IsMiscSupport)*/
static inline bsp_module_support_e AT_IsMiscSupport(bsp_module_type_e moduleType)
{
    AT_PR_LOGI("enter");
    return mdrv_misc_support_check(moduleType);
}


/*lint -esym(528,AT_CheckFactoryNv)*/
static inline VOS_UINT32 AT_CheckFactoryNv(VOS_UINT32 mode)
{
    AT_PR_LOGI("enter");
    return mdrv_nv_check_factorynv(mode);
}


/*lint -esym(528,AT_OmSystemError)*/
static inline VOS_VOID AT_OmSystemError(VOS_INT32 modId, VOS_INT32 arg1, VOS_INT32 arg2, VOS_CHAR *arg3,
                                        VOS_INT32 arg3Length)
{
    AT_PR_LOGI("enter");
    mdrv_om_system_error(modId, arg1, arg2, arg3, arg3Length);
}

/*lint -esym(528,AT_QueryPpmLogPort)*/
static inline VOS_UINT32 AT_QueryPpmLogPort(VOS_UINT32 *logPort)
{
    AT_PR_LOGI("enter");
    return mdrv_ppm_query_log_port(logPort);
}


/*lint -esym(528,AT_SdProcess)*/
static inline VOS_UINT32 AT_SdProcess(VOS_UINT32 type, VOS_UINT32 addr, VOS_UINT32 data, VOS_UINT8 *buf,
                                      VOS_UINT32 *err)
{
    AT_PR_LOGI("enter");
    return mdrv_sd_at_process(type, addr, data, buf, err);
}

/*lint -esym(528,AT_GetSdOpptStatus)*/
static inline VOS_UINT32 AT_GetSdOpptStatus(VOS_VOID)
{
    AT_PR_LOGI("enter");
    return mdrv_sd_get_opptstatus();
}



/*lint -esym(528,AT_SetModemState)*/
static inline VOS_INT32 AT_SetModemState(VOS_UINT32 state)
{
    AT_PR_LOGI("enter");
    return mdrv_set_modem_state(state);
}

/*lint -esym(528,AT_SltPcieTest)*/
static inline VOS_INT32 AT_SltPcieTest(VOS_VOID)
{
    AT_PR_LOGI("enter");
    return mdrv_slt_pcie_test();
}

/*lint -esym(528,AT_SltRgmiiTest)*/
static inline VOS_INT32 AT_SltRgmiiTest(VOS_VOID)
{
    AT_PR_LOGI("enter");
    return mdrv_slt_RGMII_test();
}

/*lint -esym(528,AT_SltSdcardTest)*/
static inline VOS_INT32 AT_SltSdcardTest(VOS_VOID)
{
    AT_PR_LOGI("enter");
    return mdrv_slt_sdcard_test();
}

/*lint -esym(528,AT_SltSgmiiTest)*/
static inline VOS_INT32 AT_SltSgmiiTest(VOS_VOID)
{
    AT_PR_LOGI("enter");
    return mdrv_slt_SGMII_test();
}

/*lint -esym(528,AT_SltSpiTest)*/
static inline VOS_INT32 AT_SltSpiTest(VOS_VOID)
{
    AT_PR_LOGI("enter");
    return mdrv_slt_spi_test();
}

/*lint -esym(528,AT_SltAcoreL2CacheTest)*/
static inline VOS_INT32 AT_SltAcoreL2CacheTest(VOS_VOID)
{
    AT_PR_LOGI("enter");
    return mdrv_slt_acore_l2cache_test();
}

/*lint -esym(528,AT_GetSocpCfgIndMode)*/
static inline VOS_INT32 AT_GetSocpCfgIndMode(VOS_UINT32 *mode)
{
    AT_PR_LOGI("enter");
    return mdrv_socp_get_cfg_ind_mode(mode);
}

/*lint -esym(528,AT_GetSocpCompressIndMode)*/
static inline VOS_INT32 AT_GetSocpCompressIndMode(VOS_UINT32 *mode)
{
    AT_PR_LOGI("enter");
    return mdrv_socp_get_cps_ind_mode(mode);
}



/*lint -esym(528,AT_GetSocpSdLogCfg)*/
static inline VOS_UINT32 AT_GetSocpSdLogCfg(SOCP_ENC_DST_BUF_LOG_CFG_STRU *cfg)
{
    AT_PR_LOGI("enter");
    return mdrv_socp_get_sd_logcfg(cfg);
}

/*lint -esym(528,AT_SetSocpCfgIndMode)*/
static inline VOS_INT32 AT_SetSocpCfgIndMode(SOCP_IND_MODE_ENUM mode)
{
    AT_PR_LOGI("enter");
    return mdrv_socp_set_cfg_ind_mode(mode);
}

/*lint -esym(528,AT_SetSocpCpsIndMode)*/
static inline VOS_INT32 AT_SetSocpCpsIndMode(DEFLATE_IND_COMPRESSS_ENUM mode)
{
    AT_PR_LOGI("enter");
    return mdrv_socp_set_cps_ind_mode(mode);
}


/*lint -esym(528,AT_RegisterSysbootResetNotify)*/
static inline VOS_INT32 AT_RegisterSysbootResetNotify(const VOS_CHAR *name, pdrv_reset_cbfun cbFun, VOS_INT32 userData,
                                                      VOS_INT32 prioLevel)
{
    AT_PR_LOGI("enter");
    return mdrv_sysboot_register_reset_notify(name, cbFun, userData, prioLevel);
}


/*lint -esym(528,AT_SysbootRestart)*/
static inline VOS_VOID AT_SysbootRestart(VOS_VOID)
{
    AT_PR_LOGI("enter");
    mdrv_sysboot_restart();
}

#if (VOS_WIN32 == VOS_OS_VER || OSA_CPU_CCPU == VOS_OSA_CPU)
/*lint -esym(528,AT_SysbootShutdown)*/
static inline VOS_VOID AT_SysbootShutdown(sysboot_shutdown_reason_e reason)
{
    AT_PR_LOGI("enter");
    mdrv_sysboot_shutdown(reason);
}
#else
/*lint -esym(528,AT_SysbootShutdown)*/
static inline VOS_VOID AT_SysbootShutdown(VOS_VOID)
{
    AT_PR_LOGI("enter");
    mdrv_sysboot_shutdown();
}
#endif


/*lint -esym(528,AT_GetTimerAccuracyTimestamp)*/
static inline VOS_INT32 AT_GetTimerAccuracyTimestamp(VOS_UINT32 *high32BitValue, VOS_UINT32 *low32BitValue)
{
    AT_PR_LOGI("enter");
    return mdrv_timer_get_accuracy_timestamp(high32BitValue, low32BitValue);
}


/*lint -esym(528,AT_GetWifiPACurMode)*/
static inline VOS_INT32 AT_GetWifiPACurMode(VOS_VOID)
{
    AT_PR_LOGI("enter");
    return WIFI_GET_PA_CUR_MODE();
}


/*lint -esym(528,AT_GetWifiPAMode)*/
static inline VOS_INT32 AT_GetWifiPAMode(VOS_VOID)
{
    AT_PR_LOGI("enter");
    return WIFI_GET_PA_MODE();
}


/*lint -esym(528,AT_GetWifiRxPacketReport)*/
static inline VOS_VOID AT_GetWifiRxPacketReport(VOS_UINT32 *ucastPkts, VOS_UINT32 *mcastPkts)
{
    AT_PR_LOGI("enter");
    WIFI_GET_RX_PACKET_REPORT(ucastPkts, mcastPkts);
}


/*lint -esym(528,AT_GetWifiStatus)*/
static inline VOS_INT32 AT_GetWifiStatus(VOS_VOID)
{
    AT_PR_LOGI("enter");
    return WIFI_GET_STATUS();
}


/*lint -esym(528,AT_GetWifiTcmdMode)*/
static inline VOS_INT32 AT_GetWifiTcmdMode(VOS_VOID)
{
    AT_PR_LOGI("enter");
    return WIFI_GET_TCMD_MODE();
}


/*lint -esym(528,AT_WifiPowerShutdown)*/
static inline VOS_INT32 AT_WifiPowerShutdown(VOS_VOID)
{
    AT_PR_LOGI("enter");
    return WIFI_POWER_SHUTDOWN();
}


/*lint -esym(528,AT_WifiPowerStart)*/
static inline VOS_INT32 AT_WifiPowerStart(VOS_VOID)
{
    AT_PR_LOGI("enter");
    return WIFI_POWER_START();
}


/*lint -esym(528,AT_SetWifiPaMode)*/
static inline VOS_INT32 AT_SetWifiPaMode(VOS_INT32 wifiPaMode)
{
    AT_PR_LOGI("enter");
    return WIFI_SET_PA_MODE(wifiPaMode);
}

#define AT_WIFI_TEST_CMD_CALL(cmd) do { \
    AT_TestWifiCmd(cmd, strlen(cmd)); \
} while (0)

/*lint -esym(528,AT_TestWifiCmd)*/
static inline VOS_VOID AT_TestWifiCmd(VOS_CHAR *cmdStr, VOS_UINT32 len)
{
    if (len != 0) {
        AT_PR_LOGI("enter");
    }
    WIFI_TEST_CMD(cmdStr);
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* _AT_MDRV_INTERFACE_H_ */
