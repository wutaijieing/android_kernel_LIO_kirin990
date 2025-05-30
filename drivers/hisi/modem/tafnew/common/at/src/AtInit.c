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
#include "AtInit.h"

/* 保护此头文件是因为包含了PS域相关的定义，但是本项目未修改PS域相关的，后续需要调整 */
#include "AtDataProc.h"
#include "AcpuReset.h"
#if (FEATURE_LTE == FEATURE_ON)
#include "mdrv_nvim.h"
#include "at_lte_common.h"
#endif
#include "nv_stru_was.h"
#include "nv_stru_gas.h"
#include "acore_nv_stru_msp.h"
#include "product_config.h"
#include "at_mdrv_interface.h"
#include "msp_diag_comm.h"
#include "GuNasLogFilter.h"
#include "ps_log_filter_interface.h"
#include "TafAcoreLogPrivacy.h"
#if (FEATURE_DATA_SERVICE_NEW_PLATFORM == FEATURE_ON)
#include "ads_dev_i.h"
#else
#include "ads_device_interface.h"
#endif


#include "ATCmdProc.h"
#include "securec.h"
#include "taf_type_def.h"
#include "AtMtCommFun.h"


#if (FEATURE_LTEV == FEATURE_ON)
#include "at_ltev_cmd.h"
#endif


/*
 * 协议栈打印打点方式下的.C文件宏定义
 */
#define THIS_FILE_ID PS_FILE_ID_AT_INIT_C
#define AT_UE_CATEGORY_MIN_VALID_VALUE 1
#define AT_UE_CATEGORY_MAX_VALID_VALUE 12
#define AT_UE_CATEGORYEXT_MIN_VALID_VALUE 13
#define AT_UE_CATEGORYEXT_MAX_VALID_VALUE 20
#define AT_UE_CATEGORYEXT2_MIN_VALID_VALUE 21
#define AT_UE_CATEGORYEXT2_MAX_VALID_VALUE 24
#define AT_UE_CATEGORYEXT3_MIN_VALID_VALUE 25
#define AT_UE_CATEGORYEXT3_MAX_VALID_VALUE 28


VOS_VOID AT_ReadPlatformNV(VOS_VOID)
{
    ModemIdUint16           modemID;
    PLATAFORM_RatCapability platFormRat;
    VOS_UINT8               ratIndex;
    AT_ModemSptRat         *atSptRatList = VOS_NULL_PTR;

    memset_s(&platFormRat, sizeof(platFormRat), 0x00, sizeof(PLATAFORM_RatCapability));

    for (modemID = 0; modemID < MODEM_ID_BUTT; modemID++) {
        atSptRatList = AT_GetSptRatFromModemId(modemID);

        /* 读取平台NV成功 */
        if (TAF_ACORE_NV_READ(modemID, NV_ITEM_PLATFORM_RAT_CAP, &platFormRat, sizeof(PLATAFORM_RatCapability)) ==
            NV_OK) {
            atSptRatList->platformSptGsm      = VOS_FALSE;
            atSptRatList->platformSptWcdma    = VOS_FALSE;
            atSptRatList->platformSptLte      = VOS_FALSE;
            atSptRatList->platformSptUtralTdd = VOS_FALSE;

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
            atSptRatList->platformSptNr = VOS_FALSE;
#endif

            platFormRat.ratNum = AT_MIN(platFormRat.ratNum, PLATFORM_MAX_RAT_NUM);
            for (ratIndex = 0; ratIndex < platFormRat.ratNum; ratIndex++) {
#if (FEATURE_LTE == FEATURE_ON)
                /* 平台支持LTE */
                if (platFormRat.ratList[ratIndex] == PLATFORM_RAT_LTE) {
                    atSptRatList->platformSptLte = VOS_TRUE;
                }
#endif
                /* 平台支持WCDMA */
                if (platFormRat.ratList[ratIndex] == PLATFORM_RAT_WCDMA) {
                    atSptRatList->platformSptWcdma = VOS_TRUE;
                }

#if (FEATURE_UE_MODE_TDS == FEATURE_ON)
                /* 平台支持TDS */
                if (platFormRat.ratList[ratIndex] == PLATFORM_RAT_TDS) {
                    atSptRatList->platformSptUtralTdd = VOS_TRUE;
                }
#endif
                /* 平台支持GSM */
                if (platFormRat.ratList[ratIndex] == PLATFORM_RAT_GSM) {
                    atSptRatList->platformSptGsm = VOS_TRUE;
                }

#if (FEATURE_UE_MODE_NR == FEATURE_ON)
                /* 平台支持NR */
                if (platFormRat.ratList[ratIndex] == PLATFORM_RAT_NR) {
                    atSptRatList->platformSptNr = VOS_TRUE;
                }
#endif
            }
        }
    }

    return;
}


VOS_VOID AT_ReadClientConfigNV(VOS_VOID)
{
    TAF_AT_NvimAtClientCfg  atClientCfg;
    AT_ClinetConfigDesc    *cfgDesc   = VOS_NULL_PTR;
    AT_ClientConfiguration *clientCfg = VOS_NULL_PTR;
    AT_ClientCfgMapTab     *cfgMapTbl = VOS_NULL_PTR;
    VOS_UINT8               i;

    memset_s(&atClientCfg, sizeof(atClientCfg), 0x00, sizeof(TAF_AT_NvimAtClientCfg));

    /*
     * NV项en_NV_Item_AT_CLIENT_CONFIG的
     * aucAtClientConfig[Index]
     * Index = 0 -- AT_CLIENT_TAB_PCUI_INDEX
     * Index = 1 -- AT_CLIENT_TAB_CTRL_INDEX
     * ......
     * 按index顺序递增
     * aucAtClientConfig[Index]
     * BIT0-BIT1对应一个client归属于哪个ModemId:00:表示modem0 01:表示modem1
     * BIT2对应一个client是否允许广播:0:表示不允许 1:表示允许
     */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_AT_CLIENT_CFG, &atClientCfg, sizeof(TAF_AT_NvimAtClientCfg)) != VOS_OK) {
        return;
    }

    cfgDesc = (AT_ClinetConfigDesc *)&(atClientCfg.atClientConfig[0]);

    for (i = 0; i < AT_GET_CLIENT_CFG_TAB_LEN(); i++) {
        cfgMapTbl = AT_GetClientCfgMapTbl(i);
        clientCfg = AT_GetClientConfig(cfgMapTbl->clientId);

#if (MULTI_MODEM_NUMBER >= 2)
        clientCfg->modemId = cfgDesc[cfgMapTbl->nvIndex].modemId;
#endif
        clientCfg->reportFlg = cfgDesc[cfgMapTbl->nvIndex].reportFlg;
    }

    return;
}


VOS_VOID AT_GetCpmsMtMem(ModemIdUint16 modemId, MN_MSG_MemStoreUint8 *smMemStore)
{
    VOS_UINT32              ret;
    TAF_NVIM_SmsServicePara smsServicePara;
    /*
     * 短信接收存储介质保存在NV配置文件的使能标志
     * VOS_TRUE        短信接收存储介质保存在NV配置文件
     * VOS_FALSE       短信接收存储介质不保存在NV配置文件，每次上电后恢复为SM存储
     */
    VOS_BOOL        bSmMemEnable;
    AT_ModemSmsCtx *smsCtx = VOS_NULL_PTR;

    memset_s(&smsServicePara, sizeof(smsServicePara), 0x00, sizeof(TAF_NVIM_SmsServicePara));

    *smMemStore = MN_MSG_MEM_STORE_SIM;

    ret = TAF_ACORE_NV_READ(modemId, NV_ITEM_SMS_SERVICE_PARA, &smsServicePara, MN_MSG_SRV_PARAM_LEN);
    if (ret != NV_OK) {
        AT_ERR_LOG("AT_GetCpmsMtMem: Read Service Parm From Nvim Failed");
        return;
    }

    bSmMemEnable = (smsServicePara.smsServicePara)[AT_MSG_SRV_SM_MEM_ENABLE_OFFSET];

    smsCtx = AT_GetModemSmsCtxAddrFromModemId(modemId);

    if (smsCtx->msgMeStorageStatus == MN_MSG_ME_STORAGE_ENABLE) {
        if (bSmMemEnable == VOS_TRUE) {
            *smMemStore = (smsServicePara.smsServicePara)[AT_MSG_SRV_RCV_SM_MEM_STORE_OFFSET];
        }
    }

    return;
}


VOS_VOID AT_ReadSmsMeStorageInfoNV(VOS_VOID)
{
    VOS_UINT32           ret;
    MN_MSG_MeStorageParm meStorageParm;
    ModemIdUint16        modemId;
    AT_ModemSmsCtx      *smsCtx = VOS_NULL_PTR;

    for (modemId = 0; modemId < MODEM_ID_BUTT; modemId++) {
        smsCtx = AT_GetModemSmsCtxAddrFromModemId(modemId);

        meStorageParm.meStorageStatus = MN_MSG_ME_STORAGE_DISABLE;
        meStorageParm.meStorageNum    = 0;

        /* 获取ME短信存储介质的支持状态 */
        ret = TAF_ACORE_NV_READ(modemId, NV_ITEM_SMS_ME_STORAGE_INFO, &meStorageParm, sizeof(meStorageParm));
        if ((ret == NV_OK) && (meStorageParm.meStorageStatus == MN_MSG_ME_STORAGE_ENABLE)) {
            smsCtx->msgMeStorageStatus = meStorageParm.meStorageStatus;
        }

        /* 获取短信及状态报告接收存储介质 */
        AT_GetCpmsMtMem(modemId, &(smsCtx->cpmsInfo.rcvPath.smMemStore));
        smsCtx->cpmsInfo.rcvPath.staRptMemStore = smsCtx->cpmsInfo.rcvPath.smMemStore;
    }

    return;
}


VOS_VOID AT_ReadSmsClass0TailorNV(VOS_VOID)
{
    VOS_UINT32              ret;
    MN_MSG_NvimClass0Tailor class0Tailor;

    class0Tailor.actFlg       = MN_MSG_NVIM_ITEM_INACTIVE;
    class0Tailor.class0Tailor = MN_MSG_CLASS0_DEF;

    /* 获取CLASS0类短信接收上报方式 */
    ret = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_SMS_CLASS0_TAILOR, &class0Tailor, sizeof(MN_MSG_NvimClass0Tailor));
    if ((ret == NV_OK) && (class0Tailor.actFlg == MN_MSG_NVIM_ITEM_ACTIVE)) {
        g_class0Tailor = class0Tailor.class0Tailor;
    } else {
        g_class0Tailor = MN_MSG_CLASS0_DEF;
    }

    return;
}


VOS_VOID AT_ReadSmsClosePathNV(VOS_VOID)
{
    TAF_NVIM_SmsClosePathCfg closePath;
    AT_ModemSmsCtx          *smsCtx = VOS_NULL_PTR;
    ModemIdUint16            modemId;
    VOS_UINT32               ret;

    memset_s(&closePath, sizeof(closePath), 0x00, sizeof(closePath));

    for (modemId = 0; modemId < MODEM_ID_BUTT; modemId++) {
        smsCtx = AT_GetModemSmsCtxAddrFromModemId(modemId);

        /* 获取ME短信存储介质的支持状态 */
        ret = TAF_ACORE_NV_READ(modemId, NV_ITEM_SMS_CLOSE_PATH, &closePath, sizeof(closePath));
        if ((ret == NV_OK) && (closePath.nvimValid == VOS_TRUE)) {
            smsCtx->localStoreFlg = closePath.smsClosePathFlg;
        }
    }
}


VOS_VOID AT_ReadMtCustomizeInfo(VOS_VOID)
{
    VOS_UINT32             ret;
    MN_MSG_MtCustomizeInfo mtCustomize;
    ModemIdUint16          modemId;
    AT_ModemSmsCtx        *smsCtx = VOS_NULL_PTR;

    memset_s(&mtCustomize, (VOS_SIZE_T)sizeof(mtCustomize), 0x00, (VOS_SIZE_T)sizeof(MN_MSG_MtCustomizeInfo));

    ret = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_SMS_MT_CUSTOMIZE_INFO, &mtCustomize, sizeof(mtCustomize));
    if (ret != NV_OK) {
        AT_INFO_LOG("AT_ReadMtCustomizeInfo: Fail to read ");
        return;
    }

    for (modemId = 0; modemId < MODEM_ID_BUTT; modemId++) {
        smsCtx = AT_GetModemSmsCtxAddrFromModemId(modemId);

        if (mtCustomize.actFlag == MN_MSG_NVIM_ITEM_ACTIVE) {
            smsCtx->smMeFullCustomize.actFlg      = VOS_TRUE;
            smsCtx->smMeFullCustomize.mtCustomize = mtCustomize.enMtCustomize;
        }
    }
    return;
}


VOS_VOID AT_ReadPortBuffCfgNV(VOS_VOID)
{
    AT_CommCtx        *commCtx = VOS_NULL_PTR;
    VOS_UINT32         ret;
    TAF_NV_PortBuffCfg smsBuffCfg;

    commCtx = AT_GetCommCtxAddr();

    memset_s(&smsBuffCfg, sizeof(smsBuffCfg), 0x00, sizeof(smsBuffCfg));

    ret = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_PORT_BUFF_CFG, &smsBuffCfg, sizeof(smsBuffCfg));

    if (ret == NV_OK) {
        if (smsBuffCfg.enableFlg < AT_PORT_BUFF_BUTT) {
            commCtx->portBuffCfg.smsBuffCfg = smsBuffCfg.enableFlg;
        } else {
            commCtx->portBuffCfg.smsBuffCfg = AT_PORT_BUFF_DISABLE;
        }
    } else {
        commCtx->portBuffCfg.smsBuffCfg = AT_PORT_BUFF_DISABLE;
    }

    return;
}


VOS_VOID AT_ReadSmsNV(VOS_VOID)
{
    AT_ReadSmsMeStorageInfoNV();

    AT_ReadSmsClass0TailorNV();

    AT_ReadSmsClosePathNV();

    AT_ReadMtCustomizeInfo();

    return;
}


VOS_VOID AT_ReadRoamCapaNV(VOS_VOID)
{
    NAS_NVIM_RoamCfgInfo roamCfgInfo;
    ModemIdUint16        modemId;
    AT_ModemNetCtx      *netCtx = VOS_NULL_PTR;

    memset_s(&roamCfgInfo, sizeof(roamCfgInfo), 0x00, sizeof(roamCfgInfo));

    for (modemId = 0; modemId < MODEM_ID_BUTT; modemId++) {
        netCtx = AT_GetModemNetCtxAddrFromModemId(modemId);

        if (TAF_ACORE_NV_READ(modemId, NV_ITEM_ROAM_CAPA, &roamCfgInfo, sizeof(NAS_NVIM_RoamCfgInfo)) == NV_OK) {
            if (roamCfgInfo.roamFeatureFlg > 1) {
                netCtx->roamFeature = AT_ROAM_FEATURE_OFF;
            } else {
                netCtx->roamFeature = roamCfgInfo.roamFeatureFlg;
            }
        } else {
            netCtx->roamFeature = AT_ROAM_FEATURE_OFF;
        }
    }
    return;
}


VOS_VOID AT_ReadPrivacyFilterCfgNv(VOS_VOID)
{
    AT_ModemPrivacyFilterCtx *filterCtx = VOS_NULL_PTR;
    NAS_NV_PrivacyFilterCfg   privacyFilterCfg;
    ModemIdUint16             modemId;

    /* 初始化 */
    memset_s(&privacyFilterCfg, sizeof(privacyFilterCfg), 0x00, sizeof(NAS_NV_PrivacyFilterCfg));

    for (modemId = 0; modemId < MODEM_ID_BUTT; modemId++) {
        /* 读取NV项 */
        if (TAF_ACORE_NV_READ(modemId, NV_ITEM_PRIVACY_LOG_FILTER_CFG, &privacyFilterCfg,
                              sizeof(NAS_NV_PrivacyFilterCfg)) != NV_OK) {
            privacyFilterCfg.filterEnableFlg = VOS_FALSE;
        }

        /* NV读取值保护 */
        if (privacyFilterCfg.filterEnableFlg == VOS_TRUE) {
            privacyFilterCfg.filterEnableFlg = VOS_TRUE;
        }

        filterCtx = AT_GetModemPrivacyFilterCtxAddrFromModemId(modemId);

        filterCtx->filterEnableFlg = VOS_TRUE;
    }

    return;
}


VOS_VOID AT_ReadSystemAppConfigNV(VOS_VOID)
{
    VOS_UINT8               *systemAppConfig = VOS_NULL_PTR;
    NAS_NVIM_SystemAppConfig sysAppConfig;

    sysAppConfig.sysAppConfigType = SYSTEM_APP_BUTT;

    /* 获取控制特性的NV上下文地址 */
    systemAppConfig = AT_GetSystemAppConfigAddr();

    /* 读取失败按默认值处理 */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_SYSTEM_APP_CONFIG, &sysAppConfig, sizeof(NAS_NVIM_SystemAppConfig)) !=
        VOS_OK) {
        *systemAppConfig = SYSTEM_APP_MP;
        return;
    }

    if (sysAppConfig.sysAppConfigType > SYSTEM_APP_ANDROID) {
        *systemAppConfig = SYSTEM_APP_MP;
    } else {
        *systemAppConfig = (VOS_UINT8)sysAppConfig.sysAppConfigType;
    }

    return;
}


VOS_VOID AT_ReadAtDislogPwdNV(VOS_VOID)
{
    VOS_UINT8              *systemAppConfig = VOS_NULL_PTR;
    TAF_AT_NvimDislogPwdNew dislogPwdNew;
    errno_t                 memResult;

    /* 按D25的做法  不保存权限标志, 则不用从NV中读取权限, 默认无权限 */
    g_ate5RightFlag = AT_E5_RIGHT_FLAG_NO;
    memset_s(&dislogPwdNew, sizeof(dislogPwdNew), 0x00, sizeof(dislogPwdNew));

    /* 读取NV项中当前产品形态 */
    systemAppConfig = AT_GetSystemAppConfigAddr();

    /* 新加NV项保存DISLOG密码(OPWORD使用) */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_AT_DISLOG_PWD_NEW, &dislogPwdNew, AT_OPWORD_PWD_LEN) == NV_OK) {
        memResult = memcpy_s((VOS_INT8 *)g_atOpwordPwd, AT_OPWORD_PWD_LEN + 1, (VOS_INT8 *)(&dislogPwdNew),
                             AT_OPWORD_PWD_LEN);
        TAF_MEM_CHK_RTN_VAL(memResult, AT_OPWORD_PWD_LEN + 1, AT_OPWORD_PWD_LEN);

        /* 跟上字符串结尾符 */
        g_atOpwordPwd[AT_OPWORD_PWD_LEN] = 0;

        /* 如果密码为空，且非AP-Modem形态，则取消密码保护 */
        if ((VOS_StrLen((VOS_CHAR *)g_atOpwordPwd) == 0) && (*systemAppConfig != SYSTEM_APP_ANDROID)) {
            /* NV中密码为空则输入任何密码都能获取权限 */
            g_ate5RightFlag = AT_E5_RIGHT_FLAG_YES;
        }
    } else {
        AT_WARN_LOG("AT_ReadAtDislogPwdNV:read NV_ITEM_AT_DISLOG_PWD_NEW failed");
    }
    memset_s(&dislogPwdNew, sizeof(dislogPwdNew), 0x00, sizeof(dislogPwdNew));
    return;
}


VOS_VOID AT_ReadAtRightPasswordNV(VOS_VOID)
{
    TAF_AT_NvimRightOpenFlag nvimRightOpenFlg;
    errno_t                  memResult;

    memset_s(&nvimRightOpenFlg, sizeof(nvimRightOpenFlg), 0x00, sizeof(nvimRightOpenFlg));

    /* 从NV中获取当前操作AT命令的权限 */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_AT_RIGHT_PASSWORD, &nvimRightOpenFlg, sizeof(nvimRightOpenFlg)) !=
        NV_OK) {
        /* 读取NV失败,采用默认密码 */
        memset_s(&g_atRightOpenFlg, sizeof(g_atRightOpenFlg), 0x00, sizeof(g_atRightOpenFlg));
        AT_WARN_LOG("AT_ReadAtRightPasswordNV:read NV_ITEM_AT_RIGHT_PASSWORD failed");
    }

    memResult = memcpy_s(&g_atRightOpenFlg, sizeof(AT_RightOpenFlagInfo), &nvimRightOpenFlg,
                         sizeof(AT_RightOpenFlagInfo));
    TAF_MEM_CHK_RTN_VAL(memResult, sizeof(AT_RightOpenFlagInfo), sizeof(AT_RightOpenFlagInfo));
    memset_s(&nvimRightOpenFlg, sizeof(nvimRightOpenFlg), 0x00, sizeof(nvimRightOpenFlg));
    return;
}


VOS_VOID AT_ReadAtDissdPwdNV(VOS_VOID)
{
    TAF_AT_NvimDissdPwd dissdPwd;
    errno_t             memResult;

    memset_s(&dissdPwd, sizeof(dissdPwd), 0x00, AT_DISSD_PWD_LEN);

    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_AT_DISSD_PWD, &dissdPwd, AT_DISSD_PWD_LEN) == NV_OK) {
        memResult = memcpy_s((VOS_INT8 *)g_ate5DissdPwd, AT_DISSD_PWD_LEN + 1, (VOS_INT8 *)(&dissdPwd),
                             AT_DISSD_PWD_LEN);
        TAF_MEM_CHK_RTN_VAL(memResult, AT_DISSD_PWD_LEN + 1, AT_DISSD_PWD_LEN);
        /* 跟上字符串结尾符 */
        g_ate5DissdPwd[AT_DISSD_PWD_LEN] = 0;
    } else {
        AT_WARN_LOG("AT_ReadAtDissdPwdNV:read NV_ITEM_AT_DISSD_PWD failed");
    }
    memset_s(&dissdPwd, sizeof(dissdPwd), 0x00, AT_DISSD_PWD_LEN);
    return;
}


VOS_VOID AT_ReadNotSupportRetValueNV(VOS_VOID)
{
    errno_t                       memResult;
    VOS_UINT32                    retLen;
    VOS_CHAR                      acRetVal[AT_NOTSUPPORT_STR_LEN + 1];
    TAF_AT_NotSupportCmdErrorText errorText;

    memset_s(&errorText, sizeof(errorText), 0x00, AT_NOTSUPPORT_STR_LEN);

    /*
     * 从NV中获取不支持命令的返回值，当为E5、LCARD、DONGLE时，
     * 该NV设置为"ERROR",其他平台设置为"COMMAND NOT SUPPORT"
     */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_NOT_SUPPORT_RET_VALUE, errorText.errorText, AT_NOTSUPPORT_STR_LEN) ==
        NV_OK) {
        memResult = memcpy_s(acRetVal, sizeof(acRetVal), errorText.errorText, AT_NOTSUPPORT_STR_LEN);
        TAF_MEM_CHK_RTN_VAL(memResult, sizeof(acRetVal), AT_NOTSUPPORT_STR_LEN);

        /* 跟上字符串结尾符 */
        acRetVal[AT_NOTSUPPORT_STR_LEN] = 0;
        retLen                          = VOS_StrLen(acRetVal);
        if (retLen > 0) {
            /* AT_NOTSUPPORT_STR_LEN + 4是gaucAtCmdNotSupportStr数组定义时的长度 */
            memResult = strcpy_s((VOS_CHAR *)g_atCmdNotSupportStr, AT_NOTSUPPORT_STR_LEN + 4, acRetVal);
            TAF_STRCPY_CHK_RTN_VAL_CONTINUE(memResult, AT_NOTSUPPORT_STR_LEN + 4, AT_NOTSUPPORT_STR_LEN + 4);
        }
    } else {
        AT_WARN_LOG("AT_ReadNotSupportRetValueNV:read NV_ITEM_NOT_SUPPORT_RET_VALUE failed");
    }

    return;
}


VOS_VOID AT_ReadE5NV(VOS_VOID)
{
    AT_ReadAtDislogPwdNV();

    AT_ReadAtRightPasswordNV();

    AT_ReadAtDissdPwdNV();

    AT_ReadNotSupportRetValueNV();

    return;
}


VOS_UINT32 AT_IsAbortCmdCharValid(VOS_UINT8 *abortCmdChar, VOS_UINT32 len)
{
    VOS_UINT32 i;

    /*
     * 由于在AT命令的处理的流程中, 会对输入的字符进行格式化处理, 大致会过滤掉不可见字符
     * (<0x20的ASCII字符,空格, S3, S5)等.
     * 如果NV中设置的ABORT命令字符串中包含这些字符会导致打断命令匹配失败, 故做容错处理,
     * 如果NV中设置的ABORT命令字符串中包含这些字符则认为设置无效, 使用默认值
     */

    /* 如果打断命令或打断回复为空指针，无效 */
    if (abortCmdChar == VOS_NULL_PTR) {
        return VOS_FALSE;
    }

    /* 如果打断命令或打断回复长度为0，无效 */
    if (len == 0) {
        return VOS_FALSE;
    }

    /* 如果打断命令或打断回复有不可见字符(<0x20的ASCII字符,空格, S3, S5)等，无效 */
    for (i = 0; i < len; i++) {
        if ((abortCmdChar[i] == g_atS3) || (abortCmdChar[i] == g_atS5) || (abortCmdChar[i] <= 0x20)) {
            return VOS_FALSE;
        }
    }

    return VOS_TRUE;
}


VOS_VOID AT_ReadAbortCmdParaNV(VOS_VOID)
{
    AT_ABORT_CmdPara *abortCmdPara   = VOS_NULL_PTR;
    VOS_UINT8         dfltAbortCmd[] = "AT";
    /* 命令比较时不区分大小写, 此处默认值定义为大小写均可 */
    VOS_UINT8            dfltAbortRsp[] = "OK";
    AT_NVIM_AbortCmdPara nvAbortCmdPara;
    VOS_UINT32           isAbortCmdValid;
    VOS_UINT32           isAbortRspValid;
    VOS_UINT32           rlst;
    VOS_UINT8            anyCharAbortCmd[] = "ANY";
    VOS_UINT32           len;
    errno_t              stringRet;

    /* 初始化 避免TQE告警 */
    memset_s(&nvAbortCmdPara, sizeof(nvAbortCmdPara), 0x00, sizeof(nvAbortCmdPara));

    /* 打断命令初始化 */
    abortCmdPara = AT_GetAbortCmdPara();
    memset_s(abortCmdPara, sizeof(AT_ABORT_CmdPara), 0x00, sizeof(AT_ABORT_CmdPara));

    /* 读取NV项中配置的打断命令和打断回复 */
    rlst = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_AT_ABORT_CMD_PARA, &nvAbortCmdPara, sizeof(AT_NVIM_AbortCmdPara));

    /* NV读取失败则使用默认值 */
    if (rlst != NV_OK) {
        /* 打断使能标志默认值为TRUE */
        abortCmdPara->abortEnableFlg = VOS_TRUE;

        /* 打断命令默认值 */
        stringRet = strncpy_s((VOS_CHAR *)(abortCmdPara->abortAtCmdStr), sizeof(abortCmdPara->abortAtCmdStr),
                              (VOS_CHAR *)dfltAbortCmd, AT_MAX_ABORT_CMD_STR_LEN);
        TAF_STRCPY_CHK_RTN_VAL_CONTINUE(stringRet, sizeof(abortCmdPara->abortAtCmdStr), AT_MAX_ABORT_CMD_STR_LEN);
        abortCmdPara->abortAtCmdStr[AT_MAX_ABORT_CMD_STR_LEN] = '\0';

        /* 打断回复默认值 */
        stringRet = strncpy_s((VOS_CHAR *)(abortCmdPara->abortAtRspStr), sizeof(abortCmdPara->abortAtRspStr),
                              (VOS_CHAR *)dfltAbortRsp, AT_MAX_ABORT_RSP_STR_LEN);
        TAF_STRCPY_CHK_RTN_VAL_CONTINUE(stringRet, sizeof(abortCmdPara->abortAtRspStr), AT_MAX_ABORT_RSP_STR_LEN);
        abortCmdPara->abortAtRspStr[AT_MAX_ABORT_RSP_STR_LEN] = '\0';

        AT_WARN_LOG("AT_ReadAbortCmdParaNV:read NV_ITEM_AT_ABORT_CMD_PARA failed");

        return;
    }

    /* 如果打断功能未使能, 则可以不用关注其它内容 */
    abortCmdPara->abortEnableFlg = nvAbortCmdPara.abortEnableFlg;
    if (abortCmdPara->abortEnableFlg != VOS_TRUE) {
        return;
    }

    /*
     * 将NV中打断命令的内容赋值到全局变量，赋值长度为AT_MAX_ABORT_CMD_STR_LEN，
     * 然后将数组第AT_MAX_ABORT_CMD_STR_LEN+1个字符赋值为结束符，
     * 对应数组下标为AT_MAX_ABORT_CMD_STR_LEN
     */
    stringRet = strncpy_s((VOS_CHAR *)(abortCmdPara->abortAtCmdStr), sizeof(abortCmdPara->abortAtCmdStr),
                          (VOS_CHAR *)(nvAbortCmdPara.abortAtCmdStr), AT_MAX_ABORT_CMD_STR_LEN);
    TAF_STRCPY_CHK_RTN_VAL_CONTINUE(stringRet, sizeof(abortCmdPara->abortAtCmdStr), AT_MAX_ABORT_CMD_STR_LEN);

    abortCmdPara->abortAtCmdStr[AT_MAX_ABORT_CMD_STR_LEN] = '\0';

    /*
     * 将NV中打断回复的内容赋值到全局变量，赋值长度为AT_MAX_ABORT_CMD_STR_LEN，
     * 然后将数组第AT_MAX_ABORT_RSP_STR_LEN+1个字符赋值为结束符，
     * 对应数组下标为AT_MAX_ABORT_RSP_STR_LEN
     */
    stringRet = strncpy_s((VOS_CHAR *)(abortCmdPara->abortAtRspStr), sizeof(abortCmdPara->abortAtRspStr),
                          (VOS_CHAR *)(nvAbortCmdPara.abortAtRspStr), AT_MAX_ABORT_RSP_STR_LEN);
    TAF_STRCPY_CHK_RTN_VAL_CONTINUE(stringRet, sizeof(abortCmdPara->abortAtRspStr), AT_MAX_ABORT_RSP_STR_LEN);

    abortCmdPara->abortAtRspStr[AT_MAX_ABORT_RSP_STR_LEN] = '\0';

    /* 判断打断命令的NV设置中是否有无效字符 */
    isAbortCmdValid = AT_IsAbortCmdCharValid(abortCmdPara->abortAtCmdStr,
                                             VOS_StrLen((VOS_CHAR *)(abortCmdPara->abortAtCmdStr)));

    isAbortRspValid = AT_IsAbortCmdCharValid(abortCmdPara->abortAtRspStr,
                                             VOS_StrLen((VOS_CHAR *)(abortCmdPara->abortAtRspStr)));

    /* 如果NV设置中含有不可见字符(空格, S3, S5, 小于x020的字符), 则使用默认值 */
    if (isAbortCmdValid != VOS_TRUE) {
        stringRet = strncpy_s((VOS_CHAR *)(abortCmdPara->abortAtCmdStr), sizeof(abortCmdPara->abortAtCmdStr),
                              (VOS_CHAR *)dfltAbortCmd, AT_MAX_ABORT_CMD_STR_LEN);
        TAF_STRCPY_CHK_RTN_VAL_CONTINUE(stringRet, sizeof(abortCmdPara->abortAtCmdStr), AT_MAX_ABORT_CMD_STR_LEN);
        abortCmdPara->abortAtCmdStr[AT_MAX_ABORT_CMD_STR_LEN] = '\0';
    }

    if (isAbortRspValid != VOS_TRUE) {
        stringRet = strncpy_s((VOS_CHAR *)(abortCmdPara->abortAtRspStr), sizeof(abortCmdPara->abortAtRspStr),
                              (VOS_CHAR *)dfltAbortRsp, AT_MAX_ABORT_RSP_STR_LEN);
        TAF_STRCPY_CHK_RTN_VAL_CONTINUE(stringRet, sizeof(abortCmdPara->abortAtRspStr), AT_MAX_ABORT_RSP_STR_LEN);
        abortCmdPara->abortAtRspStr[AT_MAX_ABORT_RSP_STR_LEN] = '\0';
    }

    len = VOS_StrLen((VOS_CHAR *)abortCmdPara->abortAtCmdStr);

    if (VOS_StrLen((VOS_CHAR *)anyCharAbortCmd) != len) {
        return;
    }

    if ((VOS_StrNiCmp((VOS_CHAR *)anyCharAbortCmd, (VOS_CHAR *)abortCmdPara->abortAtCmdStr, len) == 0)) {
        /* PS_MEM_SET的时候已经将其置成0，即为默认不支持任意字符打断 */
        abortCmdPara->anyAbortFlg = VOS_TRUE;
    }

    return;
}


VOS_VOID AT_ReadSysNV(VOS_VOID)
{
    AT_ReadRoamCapaNV();

    AT_ReadSystemAppConfigNV();

    AT_ReadE5NV();

    AT_ReadAbortCmdParaNV();

    return;
}


VOS_VOID AT_ReadCellSignReportCfgNV(VOS_VOID)
{
    AT_ModemNetCtx            *netCtx = VOS_NULL_PTR;
    NAS_NVIM_CellSignReportCfg cellSignReportCfg;
    ModemIdUint16              modemId;

    for (modemId = 0; modemId < MODEM_ID_BUTT; modemId++) {
        netCtx = AT_GetModemNetCtxAddrFromModemId(modemId);

        memset_s(&cellSignReportCfg, sizeof(cellSignReportCfg), 0x00, sizeof(cellSignReportCfg));

        if (TAF_ACORE_NV_READ(modemId, NV_ITEM_CELL_SIGN_REPORT_CFG, &cellSignReportCfg,
                              sizeof(NAS_NVIM_CellSignReportCfg)) != NV_OK) {
            AT_WARN_LOG("AT_ReadCellSignReportCfgNV:read NV_ITEM_CELL_SIGN_REPORT_CFG failed");
            return;
        }

        netCtx->cerssiReportType = cellSignReportCfg.signThreshold;

        if (cellSignReportCfg.signThreshold >= AT_CERSSI_REPORT_TYPE_BUTT) {
            netCtx->cerssiReportType = AT_CERSSI_REPORT_TYPE_NOT_REPORT;
        }

        netCtx->cerssiMinTimerInterval = cellSignReportCfg.minRptTimerInterval;

        if (cellSignReportCfg.minRptTimerInterval > AT_CERSSI_MAX_TIMER_INTERVAL) {
            netCtx->cerssiMinTimerInterval = 0;
        }
    }

    return;
}


VOS_UINT32 AT_ValidateWasCategory(AT_NvUeCapability *ueCapability)
{
    if (ueCapability->hSDSCHSupport != VOS_TRUE) {
        return VOS_ERR;
    }

    if ((ueCapability->hSDSCHPhyCategory < AT_UE_CATEGORY_MIN_VALID_VALUE) ||
        (ueCapability->hSDSCHPhyCategory > AT_UE_CATEGORY_MAX_VALID_VALUE)) {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_ValidateWasCategoryExt(AT_NvUeCapability *ueCapability)
{
    if ((ueCapability->hSDSCHSupport != VOS_TRUE) || (ueCapability->macEhsSupport != VOS_TRUE)) {
        return VOS_ERR;
    }

    if ((ueCapability->hSDSCHPhyCategoryExt < AT_UE_CATEGORYEXT_MIN_VALID_VALUE) ||
        (ueCapability->hSDSCHPhyCategoryExt > AT_UE_CATEGORYEXT_MAX_VALID_VALUE)) {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_ValidateWasCategoryExt2(AT_NvUeCapability *ueCapability)
{
    if ((ueCapability->hSDSCHSupport != VOS_TRUE) || (ueCapability->macEhsSupport != VOS_TRUE) ||
        (ueCapability->multiCellSupport != VOS_TRUE)) {
        return VOS_ERR;
    }

    if ((ueCapability->hSDSCHPhyCategoryExt2 < AT_UE_CATEGORYEXT2_MIN_VALID_VALUE) ||
        (ueCapability->hSDSCHPhyCategoryExt2 > AT_UE_CATEGORYEXT2_MAX_VALID_VALUE)) {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT32 AT_ValidateWasCategoryExt3(AT_NvUeCapability *ueCapability)
{
    if ((ueCapability->hSDSCHSupport != VOS_TRUE) || (ueCapability->macEhsSupport != VOS_TRUE) ||
        (ueCapability->multiCellSupport != VOS_TRUE) || (ueCapability->dcMimoSupport != VOS_TRUE)) {
        return VOS_ERR;
    }

    if ((ueCapability->hSDSCHPhyCategoryExt3 < AT_UE_CATEGORYEXT3_MIN_VALID_VALUE) ||
        (ueCapability->hSDSCHPhyCategoryExt3 > AT_UE_CATEGORYEXT3_MAX_VALID_VALUE)) {
        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_UINT8 AT_GetWasDefaultCategory(AT_NvUeCapability *ueCapability)
{
    AT_RateWcdmaCategoryUint8 rateCategory;

    switch (ueCapability->asRelIndicator) {
        case PS_PTL_VER_R5:
        case PS_PTL_VER_R6:
            rateCategory = AT_RATE_WCDMA_CATEGORY_10;
            break;

        case PS_PTL_VER_R7:
            rateCategory = AT_RATE_WCDMA_CATEGORY_18;
            break;

        case PS_PTL_VER_R8:
            rateCategory = AT_RATE_WCDMA_CATEGORY_28;
            break;

        default:
            rateCategory = AT_RATE_WCDMA_CATEGORY_10;
            break;
    }

    return rateCategory;
}


VOS_UINT8 AT_CalcWasCategory(AT_NvUeCapability *ueCapability)
{
    AT_RateWcdmaCategoryUint8 rateCategory;

    if (AT_ValidateWasCategoryExt3(ueCapability) == VOS_OK) {
        return ueCapability->hSDSCHPhyCategoryExt3;
    }

    if (AT_ValidateWasCategoryExt2(ueCapability) == VOS_OK) {
        return ueCapability->hSDSCHPhyCategoryExt2;
    }

    if (AT_ValidateWasCategoryExt(ueCapability) == VOS_OK) {
        return ueCapability->hSDSCHPhyCategoryExt;
    }

    if (AT_ValidateWasCategory(ueCapability) == VOS_OK) {
        return ueCapability->hSDSCHPhyCategory;
    }

    rateCategory = AT_GetWasDefaultCategory(ueCapability);

    return rateCategory;
}


VOS_VOID AT_ReadWasCapabilityNV(VOS_VOID)
{
    AT_NvUeCapability nvUeCapability;
    VOS_UINT32        result;

    memset_s(&nvUeCapability, sizeof(nvUeCapability), 0x00, sizeof(nvUeCapability));

    result = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_WAS_RADIO_ACCESS_CAPA_NEW, &nvUeCapability,
                               sizeof(AT_NvUeCapability));
    if (result != VOS_OK) {
        /* NV读取失败, 协议版本默认为R99 */
        g_atDlRateCategory.wasRelIndicator = PS_PTL_VER_R99;
        g_atDlRateCategory.wasCategory     = AT_RATE_WCDMA_R99;
    } else {
        g_atDlRateCategory.wasRelIndicator = (VOS_INT8)nvUeCapability.asRelIndicator;
        g_atDlRateCategory.wasCategory     = AT_CalcWasCategory(&nvUeCapability);
    }
}


VOS_VOID AT_CalcGasCategory(NVIM_GAS_HighMultiSlotClass *highMultislotclass, VOS_UINT16 gprsMultiSlotClass,
                            VOS_UINT16 egprsMultiSlotClass, VOS_UINT16 egprsFlag)
{
    /* 默认为VOS_FALSE */
    g_atDlRateCategory.gasMultislotClass33Flg = VOS_FALSE;

    if ((highMultislotclass->highMultislotClassFlg == VOS_TRUE) &&
        (highMultislotclass->highMultislotClass == AT_GAS_HIGH_MULTISLOT_CLASS_0)) {
        if ((egprsFlag == VOS_FALSE) && (gprsMultiSlotClass == AT_GAS_GRR_MULTISLOT_CLASS_MAX)) {
            g_atDlRateCategory.gasMultislotClass33Flg = VOS_TRUE;
        }

        if ((egprsFlag == VOS_TRUE) && (egprsMultiSlotClass == AT_GAS_GRR_MULTISLOT_CLASS_MAX)) {
            g_atDlRateCategory.gasMultislotClass33Flg = VOS_TRUE;
        }
    }

    return;
}


VOS_VOID AT_ReadGasCapabilityNV(VOS_VOID)
{
    VOS_UINT32                  result;
    NVIM_GAS_HighMultiSlotClass highMultislotclass;
    VOS_UINT16                  gprsMultiSlotClass;
    VOS_UINT16                  egprsMultiSlotClass;
    VOS_UINT16                  egprsFlag;

    highMultislotclass.highMultislotClassFlg = VOS_FALSE;
    highMultislotclass.highMultislotClass    = AT_GAS_HIGH_MULTISLOT_CLASS_0;

    gprsMultiSlotClass  = 0;
    egprsMultiSlotClass = 0;
    egprsFlag           = VOS_FALSE;

    result = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_EGPRS_FLAG, &egprsFlag, sizeof(VOS_UINT16));
    if (result != NV_OK || egprsFlag != VOS_FALSE) {
        egprsFlag = VOS_TRUE;
    }

    result = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_GPRS_MULTI_SLOT_CLASS, &gprsMultiSlotClass, sizeof(VOS_UINT16));

    /* 读取失败，默认多时隙能力等级为12 */
    if (result != NV_OK) {
        gprsMultiSlotClass = AT_GAS_GRR_MULTISLOT_CLASS_MAX;
    }

    /* 多时隙能力等级取值为1-12 */
    if ((gprsMultiSlotClass == 0) || (gprsMultiSlotClass > AT_GAS_GRR_MULTISLOT_CLASS_MAX)) {
        gprsMultiSlotClass = AT_GAS_GRR_MULTISLOT_CLASS_MAX;
    }

    result = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_EGPRS_MULTI_SLOT_CLASS, &egprsMultiSlotClass, sizeof(VOS_UINT16));
    if (result != NV_OK) {
        egprsMultiSlotClass = AT_GAS_GRR_MULTISLOT_CLASS_MAX;
    }

    /* 多时隙能力等级取值为1-12 */
    if ((egprsMultiSlotClass == 0) || (egprsMultiSlotClass > AT_GAS_GRR_MULTISLOT_CLASS_MAX)) {
        egprsMultiSlotClass = AT_GAS_GRR_MULTISLOT_CLASS_MAX;
    }

    result = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_GAS_HIGH_MULTI_SLOT_CLASS, &highMultislotclass,
                               sizeof(NVIM_GAS_HighMultiSlotClass));

    /* 读取失败，默认High Multislot Class 无效  */
    if (result != NV_OK) {
        return;
    }

    if ((highMultislotclass.highMultislotClassFlg == VOS_TRUE) &&
        (highMultislotclass.highMultislotClass != AT_GAS_HIGH_MULTISLOT_CLASS_0)) {
        highMultislotclass.highMultislotClass = AT_GAS_HIGH_MULTISLOT_CLASS_0;
    }

    AT_CalcGasCategory(&highMultislotclass, gprsMultiSlotClass, egprsMultiSlotClass, egprsFlag);

    return;
}


VOS_VOID AT_ReadPppDialErrCodeNV(VOS_VOID)
{
    NAS_NV_PppDialErrCode pppErrRpt;
    VOS_UINT32            result;

    pppErrRpt.status     = VOS_FALSE;
    pppErrRpt.errCodeRpt = PPP_DIAL_ERR_CODE_BUTT;

    result = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_PPP_DIAL_ERR_CODE, &pppErrRpt, sizeof(NAS_NV_PppDialErrCode));

    if ((result == NV_OK) && (pppErrRpt.status == VOS_TRUE) && (pppErrRpt.errCodeRpt == PPP_DIAL_ERR_CODE_ENABLE)) {
        g_pppDialErrCodeRpt = PPP_DIAL_ERR_CODE_ENABLE;
    } else {
        g_pppDialErrCodeRpt = PPP_DIAL_ERR_CODE_DISABLE;
    }

    return;
}


VOS_VOID AT_ReadReportRegActFlgNV(VOS_VOID)
{
    VOS_UINT32                 result;
    TAF_AT_NvimReportRegActFlg reportRegFlg;

    memset_s(&reportRegFlg, sizeof(reportRegFlg), 0x00, sizeof(TAF_AT_NvimReportRegActFlg));

    result = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_REPORT_REG_ACT_FLG, &(reportRegFlg.reportRegActFlg),
                               sizeof(reportRegFlg.reportRegActFlg));

    if ((result == NV_OK) && (reportRegFlg.reportRegActFlg == VOS_TRUE)) {
        g_reportCregActParaFlg = reportRegFlg.reportRegActFlg;
    } else {
        g_reportCregActParaFlg = VOS_FALSE;
    }

    return;
}


VOS_VOID AT_ReadCregAndCgregCiFourByteRptNV(VOS_VOID)
{
    NAS_NV_CregCgregCiFourByteRpt ciFourByteRpt;
    VOS_UINT32                    result;

    ciFourByteRpt.status     = VOS_FALSE;
    ciFourByteRpt.ciBytesRpt = 0;

    result = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_CREG_CGREG_CI_FOUR_BYTE_RPT, &ciFourByteRpt,
                               sizeof(NAS_NV_CregCgregCiFourByteRpt));

    if ((result == NV_OK) && (ciFourByteRpt.status == VOS_TRUE) &&
        (ciFourByteRpt.ciBytesRpt == CREG_CGREG_CI_RPT_FOUR_BYTE)) {
        g_ciRptByte = CREG_CGREG_CI_RPT_FOUR_BYTE;
    } else {
        g_ciRptByte = CREG_CGREG_CI_RPT_TWO_BYTE;
    }

    return;
}


VOS_VOID AT_ReadIpv6CapabilityNV(VOS_VOID)
{
    AT_NV_Ipv6Capability nvIpv6Capability;
    VOS_UINT32           rslt;
    AT_CommPsCtx        *commPsCtx = VOS_NULL_PTR;
    VOS_UINT32           ipv6CapabilityValid;

    memset_s(&nvIpv6Capability, sizeof(nvIpv6Capability), 0x00, sizeof(AT_NV_Ipv6Capability));

    commPsCtx = AT_GetCommPsCtxAddr();

    rslt = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_IPV6_CAPABILITY, &nvIpv6Capability, sizeof(AT_NV_Ipv6Capability));

    ipv6CapabilityValid = AT_PS_IsIpv6CapabilityValid(nvIpv6Capability.ipv6Capablity);

    if ((rslt == NV_OK) && (nvIpv6Capability.status == VOS_TRUE) && (ipv6CapabilityValid == VOS_TRUE)) {
        commPsCtx->ipv6Capability = nvIpv6Capability.ipv6Capablity;
    } else {
        commPsCtx->ipv6Capability = AT_IPV6_CAPABILITY_IPV4_ONLY;
    }

    return;
}


VOS_VOID AT_ReadIpv6AddrTestModeCfgNV(VOS_VOID)
{
    TAF_NVIM_Ipv6AddrTestModeCfg ipv6AddrTestModeCfg;
    AT_CommPsCtx                *commPsCtx = VOS_NULL_PTR;
    VOS_UINT32                   rslt;

    memset_s(&ipv6AddrTestModeCfg, sizeof(ipv6AddrTestModeCfg), 0x00, sizeof(TAF_NVIM_Ipv6AddrTestModeCfg));

    commPsCtx = AT_GetCommPsCtxAddr();

    rslt = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_IPV6_ADDRESS_TEST_MODE_CFG, &ipv6AddrTestModeCfg,
                             sizeof(ipv6AddrTestModeCfg));

    if (rslt == NV_OK) {
        commPsCtx->ipv6AddrTestModeCfg = ipv6AddrTestModeCfg.ipv6AddrTestModeCfg;
    } else {
        commPsCtx->ipv6AddrTestModeCfg = 0;
    }

    return;
}


VOS_VOID AT_ReadSharePdpInfoNV(VOS_VOID)
{
    TAF_NVIM_SharePdpInfo sharePdpInfo;
    AT_CommPsCtx         *commPsCtx = VOS_NULL_PTR;
    VOS_UINT32            rslt;

    memset_s(&sharePdpInfo, sizeof(sharePdpInfo), 0x00, sizeof(TAF_NVIM_SharePdpInfo));

    commPsCtx = AT_GetCommPsCtxAddr();

    rslt = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_SHARE_PDP_INFO, &sharePdpInfo, sizeof(sharePdpInfo));

    if ((rslt == NV_OK) && (sharePdpInfo.enableFlag == VOS_TRUE)) {
        commPsCtx->sharePdpFlag = sharePdpInfo.enableFlag;
    } else {
        commPsCtx->sharePdpFlag = VOS_FALSE;
    }

    return;
}


VOS_VOID AT_ReadDialConnectDisplayRateNV(VOS_VOID)
{
    VOS_UINT32                     result;
    AT_NVIM_DialConnectDisplayRate dialConnectDisplayRate;

    memset_s(&dialConnectDisplayRate, sizeof(dialConnectDisplayRate), 0x00, sizeof(dialConnectDisplayRate));

    result = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_DIAL_CONNECT_DISPLAY_RATE, &dialConnectDisplayRate,
                               sizeof(AT_NVIM_DialConnectDisplayRate));
    if (result != NV_OK) {
        memset_s(&g_dialConnectDisplayRate, sizeof(g_dialConnectDisplayRate), 0x00, sizeof(g_dialConnectDisplayRate));
        return;
    }

    g_dialConnectDisplayRate.dpaConnectRate   = dialConnectDisplayRate.dpaConnectRate;
    g_dialConnectDisplayRate.edgeConnectRate  = dialConnectDisplayRate.edgeConnectRate;
    g_dialConnectDisplayRate.gprsConnectRate  = dialConnectDisplayRate.gprsConnectRate;
    g_dialConnectDisplayRate.gsmConnectRate   = dialConnectDisplayRate.gsmConnectRate;
    g_dialConnectDisplayRate.reserve1         = dialConnectDisplayRate.reserve1;
    g_dialConnectDisplayRate.reserve2         = dialConnectDisplayRate.reserve2;
    g_dialConnectDisplayRate.reserve3         = dialConnectDisplayRate.reserve3;
    g_dialConnectDisplayRate.wcdmaConnectRate = dialConnectDisplayRate.wcdmaConnectRate;

    return;
}

LOCAL VOS_VOID AT_SetApnCustFormatCfg(TAF_NVIM_ApnCustomFormatCfg *apnNVCfg, AT_PS_ApnCustomFormatCfg *apnAtCfg)
{
    VOS_UINT32 charNum;
    VOS_UINT32 charNvNum;
    VOS_UINT32 i;

    apnAtCfg->protocolStringCheckFlag = (apnNVCfg->ucProtocolStringCheckFlag == VOS_TRUE) ? VOS_TRUE : VOS_FALSE;

    charNum   = 0;
    charNvNum = TAF_MIN(apnNVCfg->ucCustomCharNum, AT_PS_APN_CUSTOM_CHAR_MAX_NUM);
    for (i = 0; i < charNvNum; i++) {
        /* 控制字符,正反斜杠不容许设置为特殊字符 */
        if ((apnNVCfg->aucCustomChar[i] > AT_PS_ASCII_UINT_SEPARATOR) &&
            (apnNVCfg->aucCustomChar[i] != AT_PS_ASCII_DELETE) && (apnNVCfg->aucCustomChar[i] != AT_PS_ASCII_SLASH) &&
            (apnNVCfg->aucCustomChar[i] != AT_PS_ASCII_BACKSLASH)) {
            apnAtCfg->customChar[charNum] = apnNVCfg->aucCustomChar[i];
            charNum++;
        }
    }
    apnAtCfg->customCharNum = (VOS_UINT8)charNum;
}

VOS_VOID AT_ReadApnCustomFormatCfgNV(VOS_VOID)
{
    TAF_NVIM_ApnCustomFormatCfg apnCustomFormatCfg;
    AT_CommPsCtx               *commPsCtx = VOS_NULL_PTR;
    VOS_UINT32                  rslt;
    VOS_UINT32                  i;

    memset_s(&apnCustomFormatCfg, sizeof(apnCustomFormatCfg), 0x00, sizeof(TAF_NVIM_ApnCustomFormatCfg));

    commPsCtx = AT_GetCommPsCtxAddr();

    /* 多Modem分别处理 */
    for (i = 0; i < MODEM_ID_BUTT; i++) {
        rslt = TAF_ACORE_NV_READ(i, NV_ITEM_APN_CUSTOM_FORMAT_CFG, &apnCustomFormatCfg, sizeof(apnCustomFormatCfg));

        if (rslt != NV_OK) {
            commPsCtx->apnCustomFormatCfg[i].customCharNum           = 0;
            commPsCtx->apnCustomFormatCfg[i].protocolStringCheckFlag = VOS_FALSE;
        } else {
            AT_SetApnCustFormatCfg(&apnCustomFormatCfg, &(commPsCtx->apnCustomFormatCfg[i]));
        }
    }

    return;
}


VOS_VOID AT_ReadPsNV(VOS_VOID)
{
    /* 读取拨号错误码上报NV */
    AT_ReadPppDialErrCodeNV();

    /* 读取是否上报CREG/CGREG的ACT参数控制NV */
    AT_ReadReportRegActFlgNV();

    /* 读取CREG/CGREG的CI参数是否以4字节上报的控制NV(Vodafone需求) */
    AT_ReadCregAndCgregCiFourByteRptNV();

    /* 读取IPV6能力的NV, 目前IPV6能力只能支持到IPV4V6_OVER_ONE_PDP */
    AT_ReadIpv6CapabilityNV();

    AT_ReadIpv6AddrTestModeCfgNV();

    /* 读取拨号系统托盘显示速率定制NV */
    AT_ReadDialConnectDisplayRateNV();

    /* 读取WCDMA接入等级NV, 用于PS域拨号速率上报 */
    AT_ReadWasCapabilityNV();

    /* 读取GAS是否支持CLASS33 NV项目 */
    AT_ReadGasCapabilityNV();

    AT_ReadCellSignReportCfgNV();

    AT_ReadSharePdpInfoNV();

    AT_ReadApnCustomFormatCfgNV();

    return;
}


VOS_VOID AT_ReadCsNV(VOS_VOID)
{
    TAF_UssdNvimTransMode ussdTranMode;
    TAF_CCA_TelePara      timeInfo;
    ModemIdUint16         modemId;
    AT_ModemCcCtx        *ccCtx = VOS_NULL_PTR;
    AT_ModemSsCtx        *ssCtx = VOS_NULL_PTR;

    NAS_NVIM_CustomCallCfg callCfgInfo;

    for (modemId = 0; modemId < MODEM_ID_BUTT; modemId++) {
        ccCtx = AT_GetModemCcCtxAddrFromModemId(modemId);
        ssCtx = AT_GetModemSsCtxAddrFromModemId(modemId);

        memset_s(&timeInfo, sizeof(timeInfo), 0x00, sizeof(timeInfo));

        /* 读取语音自动应答 */
        if (TAF_ACORE_NV_READ(modemId, NV_ITEM_CCA_TELE_PARA, &timeInfo, sizeof(TAF_CCA_TelePara)) == NV_OK) {
            ccCtx->s0TimeInfo.s0TimerLen = timeInfo.s0TimerLen;
        }

        memset_s(&ussdTranMode, sizeof(ussdTranMode), 0x00, sizeof(ussdTranMode));

        /* 读取USSD是否透传设置 */
        if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_CUST_USSD_MODE, &ussdTranMode, sizeof(TAF_UssdNvimTransMode)) ==
            NV_OK) {
            if (ussdTranMode.status == VOS_TRUE) {
                ssCtx->ussdTransMode = ussdTranMode.ussdTransMode;
            }
        }

        /* 读取语音功能定制 */
        memset_s(&callCfgInfo, sizeof(callCfgInfo), 0x00, sizeof(callCfgInfo));

        if (TAF_ACORE_NV_READ(modemId, NV_ITEM_CUSTOM_CALL_CFG, &callCfgInfo, sizeof(NAS_NVIM_CustomCallCfg)) ==
            NV_OK) {
            /* 如果NV取值超过范围，使用默认值CVHU_MODE_0 */
            if (callCfgInfo.cvhuMode >= CVHU_MODE_BUTT) {
                ccCtx->cvhuMode = CVHU_MODE_0;
            } else {
                ccCtx->cvhuMode = callCfgInfo.cvhuMode;
            }
        }
    }

    AT_ReadSsNV();

    return;
}

#if (FEATURE_LTE == FEATURE_ON)

VOS_VOID AT_ReadLTENV(VOS_VOID)
{
    VOS_UINT32 result;

    memset_s(&g_rsrpCfg, sizeof(g_rsrpCfg), 0x00, sizeof(g_rsrpCfg));
    memset_s(&g_rscpCfg, sizeof(g_rscpCfg), 0x00, sizeof(g_rscpCfg));
    memset_s(&g_ecioCfg, sizeof(g_ecioCfg), 0x00, sizeof(g_ecioCfg));

    /* 从NV中获取门限值 */
    result = TAF_ACORE_NV_READ(MODEM_ID_0, EN_NV_ID_RSRP_CFG, &g_rsrpCfg, sizeof(NVIM_RsrpCfg));

    if (result != NV_OK) {
        PS_PRINTF_WARNING("read RSRP NV fail!\n");
        return;
    }

    /* 从NV中获取门限值 */
    result = TAF_ACORE_NV_READ(MODEM_ID_0, EN_NV_ID_RSCP_CFG, &g_rscpCfg, sizeof(NVIM_RscpCfg));

    if (result != NV_OK) {
        PS_PRINTF_WARNING("read RSCP NV fail!\n");
        return;
    }

    /* 从NV中获取门限值 */
    result = TAF_ACORE_NV_READ(MODEM_ID_0, EN_NV_ID_ECIO_CFG, &g_ecioCfg, sizeof(NVIM_EcioCfg));

    if (result != NV_OK) {
        PS_PRINTF_WARNING("read ECIO NV fail!\n");
        return;
    }

    return;
}

#endif


VOS_VOID AT_ReadAgpsNv(VOS_VOID)
{
    AT_ModemAgpsCtx      *agpsCtx = VOS_NULL_PTR;
    TAF_NVIM_XcposrrptCfg xcposrRptCfg;
    ModemIdUint16         modemId;

    memset_s(&xcposrRptCfg, (VOS_SIZE_T)sizeof(xcposrRptCfg), 0x00, (VOS_SIZE_T)sizeof(xcposrRptCfg));

    /* 读取NV项 */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_XCPOSRRPT_CFG, &xcposrRptCfg, sizeof(xcposrRptCfg)) == NV_OK) {
        for (modemId = MODEM_ID_0; modemId < MODEM_ID_BUTT; modemId++) {
            agpsCtx               = AT_GetModemAgpsCtxAddrFromModemId(modemId);
            agpsCtx->cposrReport  = xcposrRptCfg.cposrDefault;
            agpsCtx->xcposrReport = xcposrRptCfg.xcposrRptDefault;
        }
    } else {
        AT_ERR_LOG("AT_ReadAgpsNv: NV_Read Failed!");
    }

    return;
}


LOCAL VOS_VOID AT_ReadCustomUsimmCfg(VOS_VOID)
{
    VOS_UINT32              result;
    AT_CommCtx             *commCtx = VOS_NULL_PTR;
    NAS_NVIM_CustomUsimmCfg customUsimmCfg;

    commCtx = AT_GetCommCtxAddr();

    memset_s(&customUsimmCfg, sizeof(NAS_NVIM_CustomUsimmCfg), 0, sizeof(NAS_NVIM_CustomUsimmCfg));

    result = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_CUSTOM_USIMM_CFG, &customUsimmCfg, sizeof(NAS_NVIM_CustomUsimmCfg));

    if ((result != NV_OK) || ((customUsimmCfg.simsqEnable != 0) && (customUsimmCfg.simsqEnable != 1))) {
        memset_s(&commCtx->customUsimmCfg, sizeof(NAS_NVIM_CustomUsimmCfg), 0, sizeof(NAS_NVIM_CustomUsimmCfg));
        AT_WARN_LOG("AT_ReadCustomUsimmCfg: read nv failed!");
    } else {
        commCtx->customUsimmCfg.simsqEnable = customUsimmCfg.simsqEnable;
    }

    return;
}


VOS_VOID AT_ReadNV(VOS_VOID)
{
    /* 平台接入技术NV读取 */
    AT_ReadPlatformNV();

    /* client NV读取 */
    AT_ReadClientConfigNV();

    /* 读取系统相关的NV项 */
    AT_ReadSysNV();

    /* 读取PS域相关的NV项 */
    AT_ReadPsNV();

    /* 读取cs域（CC和SS相关的NV) */
    AT_ReadCsNV();

    /* 读取SMS相关的NV */
    AT_ReadSmsNV();

#if (FEATURE_LTE == FEATURE_ON)
    /* 读取LTE NV项 */
    AT_ReadLTENV();

#endif

#if (FEATURE_AT_HSUART == FEATURE_ON)
    /* 读取uart相关相关NV项 */
    AT_ReadUartCfgNV();
#endif

    AT_ReadPortBuffCfgNV();

    AT_ReadPrivacyFilterCfgNv();

    AT_ReadAgpsNv();

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
    AT_ReadCdmaModemSwitchNotResetCfgNv();
#endif

    AT_ReadCustomUsimmCfg();

    return;
}


VOS_UINT32 AT_ReadPhyNV(VOS_VOID)
{
    VOS_BOOL      bImeiIsNull;
    ModemIdUint16 modemId;

    for (modemId = 0; modemId < MODEM_ID_BUTT; modemId++) {
        if (AT_PhyNumIsNull(modemId, AT_PHYNUM_TYPE_IMEI, &bImeiIsNull) != AT_OK) {
            return VOS_ERR;
        }

        if (bImeiIsNull != VOS_TRUE) {
            return VOS_ERR;
        }
    }

    return VOS_OK;
}


VOS_VOID AT_InitDeviceCmd(VOS_VOID)
{
#if (FEATURE_UE_MODE_NR == FEATURE_OFF)
    AT_DEVICE_CmdCtrl *atDevCmdCtrl = VOS_NULL_PTR;

    atDevCmdCtrl = AT_GetDevCmdCtrl();
    memset_s(atDevCmdCtrl, sizeof(AT_DEVICE_CmdCtrl), 0x00, sizeof(AT_DEVICE_CmdCtrl));
#else
    AT_MT_Info *atMtInfoCtx = VOS_NULL_PTR;

    atMtInfoCtx = AT_GetMtInfoCtx();
    memset_s(atMtInfoCtx, sizeof(AT_MT_Info), 0x00, sizeof(AT_MT_Info));
    atMtInfoCtx->atInfo.rficTestResult       = AT_RFIC_MEM_TEST_NOT_START;
    atMtInfoCtx->rserTestResult              = (VOS_INT32)AT_SERDES_TEST_NOT_START;
    atMtInfoCtx->serdesTestAsyncInfo.result  = (VOS_INT16)AT_SERDES_TEST_RST_NOT_START;
    atMtInfoCtx->serdesTestAsyncInfo.cmdType = 0;
    atMtInfoCtx->serdesTestAsyncInfo.rptFlg  = 0;
    AT_SetUartTestState(AT_UART_TEST_NOT_START);
    AT_SetI2sTestState(AT_I2S_TEST_NOT_START);
#endif

    if (AT_ReadPhyNV() != VOS_OK) {
        return;
    }

    /* IMEI为全零时，认为DATALOCK解锁状态 */
    g_atDataLocked = VOS_FALSE;

    return;
}


VOS_VOID AT_InitStk(VOS_VOID)
{
    USIMM_StkCfg profile;

    memset_s(&profile, sizeof(profile), 0x00, sizeof(profile));

    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_TERMINALPROFILE_SET, &profile, sizeof(USIMM_StkCfg)) != NV_OK) {
        AT_ERR_LOG("AT_StkInit: read NV_ITEM_TERMINALPROFILE_SET fail.");

        g_stkFunctionFlag = VOS_FALSE;
    } else {
        g_stkFunctionFlag = profile.funcEnable;
    }

    return;
}


VOS_VOID AT_InitPara(VOS_VOID)
{
    VOS_UINT8      *systemAppConfig       = VOS_NULL_PTR;
    VOS_UINT8       atCmdNotSupportStr1[] = "ERROR";
    VOS_UINT8       atCmdNotSupportStr2[] = "COMMAND NOT SUPPORT";
    ModemIdUint16   modemId;
    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;
    errno_t         memResult;

    systemAppConfig = AT_GetSystemAppConfigAddr();

    /* 根据产品类型分别进行处理 */
    switch (*systemAppConfig) {
        /* E5 */
        case SYSTEM_APP_WEBUI:

            netCtx = AT_GetModemNetCtxAddrFromModemId(MODEM_ID_0);

            /* E5版本对不支持命令返回ERROR */
            /* AT_NOTSUPPORT_STR_LEN + 4是gaucAtCmdNotSupportStr数组定义时的长度 */
            memResult = memcpy_s((VOS_CHAR *)g_atCmdNotSupportStr, AT_NOTSUPPORT_STR_LEN + 4,
                                 (VOS_CHAR *)atCmdNotSupportStr1, sizeof(atCmdNotSupportStr1));
            TAF_MEM_CHK_RTN_VAL(memResult, AT_NOTSUPPORT_STR_LEN + 4, sizeof(atCmdNotSupportStr1));

            /* E5的默认格式改为数字格式 */
            netCtx->copsFormatType = AT_COPS_NUMERIC_TYPE;

            /* E5错误码默认错误编号 */
            g_atCmeeType = AT_CMEE_ERROR_CODE;

            break;

            /* STICK */
        case SYSTEM_APP_MP:

            netCtx = AT_GetModemNetCtxAddrFromModemId(MODEM_ID_0);

            /* 命令不支持提示字串 */
            /* AT_NOTSUPPORT_STR_LEN + 4是gaucAtCmdNotSupportStr数组定义时的长度 */
            memResult = memcpy_s((VOS_CHAR *)g_atCmdNotSupportStr, AT_NOTSUPPORT_STR_LEN + 4,
                                 (VOS_CHAR *)atCmdNotSupportStr2, sizeof(atCmdNotSupportStr2));
            TAF_MEM_CHK_RTN_VAL(memResult, AT_NOTSUPPORT_STR_LEN + 4, sizeof(atCmdNotSupportStr2));

            netCtx->copsFormatType = AT_COPS_LONG_ALPH_TYPE;

            /* 指示错误命令返回码类型 */
            g_atCmeeType = AT_CMEE_ERROR_CONTENT;

            break;

        /* PAD,目前没有，为保持与原代码一致性，暂按stick处理 */
        case SYSTEM_APP_ANDROID:

            /* 命令不支持提示字串 */
            /* AT_NOTSUPPORT_STR_LEN + 4是gaucAtCmdNotSupportStr数组定义时的长度 */
            memResult = memcpy_s((VOS_CHAR *)g_atCmdNotSupportStr, AT_NOTSUPPORT_STR_LEN + 4,
                                 (VOS_CHAR *)atCmdNotSupportStr2, sizeof(atCmdNotSupportStr2));
            TAF_MEM_CHK_RTN_VAL(memResult, AT_NOTSUPPORT_STR_LEN + 4, sizeof(atCmdNotSupportStr2));

            for (modemId = 0; modemId < MODEM_ID_BUTT; modemId++) {
                netCtx                 = AT_GetModemNetCtxAddrFromModemId(modemId);
                netCtx->copsFormatType = AT_COPS_LONG_ALPH_TYPE;
            }

            /* 指示错误命令返回码类型 */
            g_atCmeeType = AT_CMEE_ERROR_CODE;

            break;

        default:
            break;
    }

    return;
}


VOS_VOID AT_InitPort(VOS_VOID)
{
    /*
     * 为了保证PC回放工程和 NAS GTR PC工程中SDT USBCOM AT端口的注册client ID
     * 相同，必须让USB COM口第一个注册
     */
    /* USB PCUI口链路的建立 */
    At_UsbPcuiEst();

    /* USB Control口链路的建立 */
    At_UsbCtrEst();

    /* USB PCUI2口链路的建立 */
    At_UsbPcui2Est();

    /* UART口链路的建立 */
    AT_UartInitPort();

    /* NDIS MODEM口链路的建立 */
    AT_UsbNdisEst();

    /* APP 建立通道 */
    AT_AppComEst();

    AT_UsbNdisDataEst();

#if (FEATURE_AT_HSUART == FEATURE_ON)
    AT_HSUART_InitPort();
#endif

    /* NDIS MODEM口链路的建立 */
    AT_ModemEst();

    /* APP 建立通道 */
    AT_SockEst();

    DMS_PORT_Init();

#if (FEATURE_AT_HSUART == FEATURE_ON)
    AT_HSUART_ConfigDefaultPara();
#endif


    return;
}



VOS_UINT32 At_PidInit(enum VOS_InitPhaseDefine phase)
{
    switch (phase) {
        case VOS_IP_INITIAL:

            /* 初始化AT的上下文 */
            AT_InitCtx();

#if (FEATURE_PHONE_ENG_AT_CMD == FEATURE_ON)
            AT_InitWakelock();
#endif

#if (FEATURE_AT_HSUART == FEATURE_ON)
            /* 初始化UART相关的上下文 */
            AT_InitUartCtx();
#endif

            AT_InitPortBuffCfg();

            AT_InitTraceMsgTab();

            /* 初始化复位相关的上下文 */
            AT_InitResetCtx();

            /* 读取NV项 */
            AT_ReadNV();

            /* AT 解析器初始化 */
            At_ParseInit();

            /* 注册AT命令表 */
            At_RegisterBasicCmdTable();
            At_RegisterExCmdTable();
            At_RegisterExPrivateCmdTable();
            At_RegisterDeviceCmdTable();
#if (FEATURE_LTE == FEATURE_ON)
            At_RegisterDeviceCmdTLTable();
            At_RegisterTLCmdTable();
#endif
#if (FEATURE_LTEV == FEATURE_ON)
            At_RegisterLtevCmdTable();
#endif
            /* 装备初始化 */
            AT_InitDeviceCmd();

            /* STK在AT模块的初始化 */
            AT_InitStk();

            /* AT模块参数的初始化 */
            AT_InitPara();

            /* AT消息数量控制上下文初始化 */
            AT_InitMsgNumCtrlCtx();

            /* 端口初始化 */
            AT_InitPort();

            /* 初始化g_fcIdMaptoFcPri */
            AT_InitFcMap();

            /* 给低软注册回调函数，用于C核单独复位的处理 */
            AT_RegisterSysbootResetNotify(NAS_AT_FUNC_PROC_NAME, AT_CCpuResetCallback, 0, ACPU_RESET_PRIORITY_AT);

            /* 给低软注册回调函数，用于HIFI单独复位的处理 */
            hifireset_regcbfunc(NAS_AT_FUNC_PROC_NAME, AT_HifiResetCallback, 0, ACPU_RESET_PRIORITY_AT);
            AT_InitMntnCtx();
            AT_RegisterDumpCallBack();

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
            /* 注册层间消息过滤函数 */
            GUNAS_OM_LayerMsgReplaceCBRegACore();
            TAF_OM_LayerMsgLogPrivacyMatchRegAcore();
#endif
            break;

        default:
            break;
    }

    return VOS_OK;
}


VOS_VOID AT_ReadSsNV(VOS_VOID)
{
    memset_s(&g_atSsCustomizePara, sizeof(g_atSsCustomizePara), 0x00, sizeof(g_atSsCustomizePara));

    /* 读取SS定制的NV项到全局变量 */
    if ((TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_SS_CUSTOMIZE_PARA, &g_atSsCustomizePara, sizeof(g_atSsCustomizePara)) !=
         NV_OK) ||
        (g_atSsCustomizePara.status != VOS_TRUE)) {
        g_atSsCustomizePara.status = VOS_FALSE;
    }

    return;
}

#if (FEATURE_AT_HSUART == FEATURE_ON)

VOS_VOID AT_ReadUartCfgNV(VOS_VOID)
{
    AT_UartCtx    *uartCtx = VOS_NULL_PTR;
    TAF_NV_UartCfg uartNVCfg;
    VOS_UINT32     ret;

    memset_s(&uartNVCfg, sizeof(uartNVCfg), 0x00, sizeof(TAF_NV_UartCfg));

    uartCtx = AT_GetUartCtxAddr();

    /* 读取NV项 */
    ret = TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_UART_CFG, &uartNVCfg, sizeof(TAF_NV_UartCfg));

    if (ret == VOS_OK) {
        /* 检查NV中设置的波特率是否在支持的范围内 */
        ret = AT_HSUART_IsBaudRateValid(uartNVCfg.baudRate);
        if (ret == VOS_TRUE) {
            /* 将NV中的值赋给上下文全局变量 */
            uartCtx->phyConfig.baudRate = uartNVCfg.baudRate;
        }

        /* 检查NV中设置的帧格式是否在支持的范围内 */
        ret = AT_HSUART_ValidateCharFrameParam(uartNVCfg.frame.format, uartNVCfg.frame.parity);
        if (ret == VOS_TRUE) {
            uartCtx->phyConfig.frame.format = uartNVCfg.frame.format;
            uartCtx->phyConfig.frame.parity = uartNVCfg.frame.parity;
        }

        uartCtx->riConfig.smsRiOnInterval    = uartNVCfg.riConfig.smsRiOnInterval;
        uartCtx->riConfig.smsRiOffInterval   = uartNVCfg.riConfig.smsRiOffInterval;
        uartCtx->riConfig.voiceRiOnInterval  = uartNVCfg.riConfig.voiceRiOnInterval;
        uartCtx->riConfig.voiceRiOffInterval = uartNVCfg.riConfig.voiceRiOffInterval;
        uartCtx->riConfig.voiceRiCycleTimes  = uartNVCfg.riConfig.voiceRiCycleTimes;
    }

    return;
}
#endif

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)

VOS_VOID AT_ReadCdmaModemSwitchNotResetCfgNv(VOS_VOID)
{
    AT_ModemCdmamodemswitchCtx         *cdmaModemSwitchCtx = VOS_NULL_PTR;
    TAF_NVIM_CdmamodemswitchNotResetCfg cdmaModemSwitchNvCfg;
    ModemIdUint16                       modemId;

    /* 初始化 */
    memset_s(&cdmaModemSwitchNvCfg, (VOS_UINT32)sizeof(TAF_NVIM_CdmamodemswitchNotResetCfg), 0x00,
             (VOS_UINT32)sizeof(TAF_NVIM_CdmamodemswitchNotResetCfg));

    /* 读取NV项 */
    if (TAF_ACORE_NV_READ(MODEM_ID_0, NV_ITEM_CDMAMODEMSWITCH_NOT_RESET_CFG, &cdmaModemSwitchNvCfg,
                          (VOS_UINT32)sizeof(TAF_NVIM_CdmamodemswitchNotResetCfg)) != NV_OK) {
        cdmaModemSwitchNvCfg.enableFlg = VOS_FALSE;
    }

    if (cdmaModemSwitchNvCfg.enableFlg != VOS_TRUE) {
        cdmaModemSwitchNvCfg.enableFlg = VOS_FALSE;
    }

    for (modemId = 0; modemId < MODEM_ID_BUTT; modemId++) {
        cdmaModemSwitchCtx = AT_GetModemCdmaModemSwitchCtxAddrFromModemId(modemId);

        cdmaModemSwitchCtx->enableFlg = cdmaModemSwitchNvCfg.enableFlg;
    }

    return;
}
#endif

