
#ifndef __NV_ID_PAM_H__
#define __NV_ID_PAM_H__

#include "vos.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif

/*
 * 功能说明: PAM组件公共NV项ID枚举
 */
enum PAM_NV_Id {
    NV_ITEM_USIM_TEMP_PROTECT_NEW       = 40,
    NV_ITEM_OM_PORT_TYPE                = 45,
    NV_ITEM_SCI_DSDA_CFG                = 128,
    NV_ITEM_USIM_APP_PRIORITY_CFG       = 4001,
    NV_ITEM_NV_SC_PERS_CTRL_CFG         = 4008,
    NV_ITEM_USIM_UICC_APP_PRIORITY_CFG  = 4011,
    NV_ITEM_USIM_DEBUG_MODE_SET         = 4043,
    NV_ITEM_USIM_PB_CTRL_INFO           = 8244,
    NV_ITEM_TERMINALPROFILE_SET         = 8261,
    NV_ITEM_NV_HUAWEI_DOUBLE_IMSI_CFG_I = 50041,

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    en_NV_Item_ESN_MEID                    = 9283,
#endif
};

#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
