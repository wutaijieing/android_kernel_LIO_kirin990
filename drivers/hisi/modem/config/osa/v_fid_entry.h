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


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#ifndef _V_IDDEF_INC
#define _V_IDDEF_INC

/* 单板和PC工程C核请配置这里 */
#if (OSA_CPU_CCPU == VOS_OSA_CPU)
/* FID静态信息表 */
FID_StaticTableType g_aFidStaticTable[VOS_FID_BUTT] = {
    {DOPRA_FID_SYS,         "fSYS",          VOS_NULL_PTR,           VOS_START_FIRST,    0,      0  },
    /* 以上是OSA 保留配置请不要动 */
    {MSP_FID_DIAG_CCPU,     "mspDiag",       MSP_DiagFidInit,        VOS_START_THIRD,    8192,   0  },
    {MSP_FID_CHR_CCPU,     "mspChr",         msp_chr_fid_init,         VOS_START_THIRD,    4096,   0  },
    {I0_MAPS_USIMM_FID,     "USIMM_FID",     USIMM_FID_Init,         VOS_START_THIRD,    8192,   0  },
    {I0_MAPS_SI_FID,        "SI_FID",        WuepsSIFidInit,         VOS_START_THIRD,    0,      0  },
    {I0_UEPS_FID_SL,        "I0_SL_FID",     LLC_FidInit,           VOS_START_THIRD,    16384,  0  },
    {I0_UEPS_FID_RM,        "I0_RM_FID",     WRLCMAC_UlFidInit, VOS_START_THIRD,    16384,  0  },
    {I0_UEPS_FID_RR,        "I0_RR_FID",     GAS_FidPidInit,         VOS_START_THIRD,    16384,  0  },
    {I0_UEPS_FID_GRM,       "I0_GRM_FID",    GRM_FidInit,           VOS_START_THIRD,    16384,  0  },
    {WUEPS_FID_HPA,         "HPA_FID",       HPA_FIDInit,            VOS_START_THIRD,    2560,   0  },
    {I0_WUEPS_FID_MM,       "I0_MM_FID",     I0_NAS_MML_FidInit,     VOS_START_THIRD,    16384,  0  },
    {I0_WUEPS_FID_CM,       "I0_CM_FID",     I0_WuepsCmFidInit,      VOS_START_THIRD,    16384,  0  },
    {I0_WUEPS_FID_RABMSM,   "I0_RABMSM_FID", I0_WuepsRabmFidInit,    VOS_START_THIRD,    16384,  0  },
    {I0_WUEPS_FID_TAF,      "I0_TAF_FID",    I0_WuepsTafFidInit,     VOS_START_THIRD,    0,      0  },
    {WUEPS_FID_SPY,         "SPY",           Spy_FID_Init,           VOS_START_THIRD,    6144,   0  },
    {UEPS_FID_FLOWCTRL_C,   "CCpuFlowCtrl",  FC_CcoreFidInit,       VOS_START_THIRD,    8192,   0  },
    {I0_UEPS_FID_RM_DL,     "I0_RM_DL_FID",  WRLCMAC_DlFidInit, VOS_START_THIRD,    0,      0  },
#ifdef CBT_ENABLED
    {WUEPS_FID_NOSIG,       "OM_NOSIG",      OM_NoSigFidInit,        VOS_START_THIRD,    0,      0  },
#endif
    {UEPS_FID_CDS,          "CDS_FID",       CDS_FidInit,            VOS_START_THIRD,    16384,  0  },
    {UEPS_FID_NDCLIENT,     "NCCLIENT_FID",  NDCLIENT_FidInit,       VOS_START_THIRD,    8192,   0  },
    {I0_UEPS_FID_DC,        "I0_DC_FID",     GDLCST_FidInit,            VOS_START_THIRD,    16384,  0  },
    {WUEPS_FID_SLEEP,       "SLEEP",         WuepsSleepFidInit,      VOS_START_THIRD,    4096,   0  },
    {WUEPS_FID_AWAKE,       "AWAKE",         WuepsAwakeFidInit,      VOS_START_THIRD,    4096,   0  },
    {I0_DSP_FID_APM,        "DSP_APM0",      UPHY_APM_FidInit,       VOS_START_THIRD,    8192,   0  },
    {DSP_FID_UPHY_MNTN,     "DSP_MNTN",      UPHY_MNTN_FidInit,      VOS_START_THIRD,    8192,   0  },
    {DSP_FID_UPA,           "DSP_UPA",       UPHY_UCOM_UpaFidInit,   VOS_START_THIRD,    8192,   0  },
    {I0_DSP_FID_WPHY,       "DSP_WPHY0",     UPHY_UCOM_WphyFidInit,  VOS_START_THIRD,    8192,   0  },
    {I0_DSP_FID_GPHY,       "DSP_GPHY0",     UPHY_UCOM_GphyFidInit,  VOS_START_THIRD,    8192,   0  },
    {I0_DSP_FID_GPHYLOW,    "DSP_GPHYLOW0",  UPHY_UCOM_GphyLowFidInit,  VOS_START_THIRD, 8192,   0  },
    {I0_DSP_FID_SLEEP,      "DSP_SLEEP0",    UPHY_DRX_SleepFidInit,  VOS_START_THIRD,    8192,   0  },
    {I0_DSP_FID_WAKE,       "DSP_WAKE0",     UPHY_DRX_WakeFidInit,   VOS_START_THIRD,    8192,   0  },
    {I0_DSP_FID_IDLE,       "DSP_IDLE0",     UPHY_IDLE_FidInit,      VOS_START_THIRD,    8192,   0  },
    {DSP_FID_RCM,           "DSP_RCM",       RCM_DRV_FidInit,        VOS_START_THIRD,    8192,   0  },
#if (FEATURE_ON == FEATURE_LTE_R12)
    {DSP_FID_STARTUP,       "DSP_STARTUP",   UPHY_INIT_FidInit,      VOS_START_THIRD,    16384,   0  },
#else
    {DSP_FID_STARTUP,       "DSP_STARTUP",   UPHY_INIT_FidInit,      VOS_START_THIRD,    8192,   0  },
#endif
#if ( FEATURE_ON == FEATURE_EASYRF )
    {DSP_FID_RFM,           "DSP_RFM",       BBA_RFM_InitFidInit,    VOS_START_THIRD,    8192,   0  },
    {DSP_FID_BBA_CAL,       "DSP_BBA_CAL",   BBA_CAL_FidInit,        VOS_START_THIRD,    8192,   0  },
#ifdef CBT_ENABLED
    {DSP_FID_BBA_CALC,      "DSP_BBA_CALC",  BBA_CALC_FidInit,       VOS_START_THIRD,    8192,   0  },
#endif
    {DSP_FID_BBA_RFM_LPC,   "BBA_RFM_LPC",   BBA_RFM_LpcFidInit,     VOS_START_THIRD,    8192,   0  },
#endif
    {UEPS_FID_IPS_CCORE,    "IPS_CCORE",     IPSMNTN_CcoreFidInit, VOS_START_THIRD,    8192,   0  },
    {I0_WUEPS_FID_CSIMA,    "CSIMA_FID",     VOS_NULL_PTR,           VOS_START_THIRD,    8192,   0  },
    {WUEPS_FID_CMMCA,       "CMMCA",         CMMCA_FidInit,          VOS_START_THIRD,    4096,   0  },
    {CCPU_FID_CBT,          "CBT",           CBTFidInit,             VOS_START_THIRD,    8192,   0  },
    {CCPU_FID_PAM_OM,       "PAM_OM",        PAMOM_FID_Init,         VOS_START_THIRD,    8192,   0  },
    {UEPS_FID_MTC,          "MTC",           MTC_FidInit,            VOS_START_THIRD,    4096,   0  },
    {I0_UEPS_FID_MSCC,      "I0_MSCC_FID",   I0_NAS_MSCC_FidInit,    VOS_START_THIRD,    0,      0  },
    {UEPS_FID_CSS,          "UEPS_FID_CSS",  CSS_FidInit,            VOS_START_THIRD,    8192,   0  },
    {UEPS_FID_RRM,          "RRM_FID",       RRM_FidInit,            VOS_START_THIRD,    8192,   0  },
    {I0_UEPS_FID_PB,        "I0_PB_FID",     WPDCPBMC_FidInit,            VOS_START_THIRD,    16384,  0  },
    {I0_UEPS_FID_WRR,       "I0_WRR_FID",    WAS_FidPidInit,         VOS_START_THIRD,    16384,  0  },
#if ( FEATURE_MULTI_MODEM == FEATURE_ON )
    {I1_MAPS_USIMM_FID,     "USIMM_FID1",    I1_USIMM_FID_Init,      VOS_START_THIRD,    8192,   0  },
    {I1_MAPS_SI_FID,        "SI_FID1",       I1_WuepsSIFidInit,      VOS_START_THIRD,    0,      0  },
    {I1_WUEPS_FID_SLEEP,    "I1_SLEEP",      I1_WuepsSleepFidInit,   VOS_START_THIRD,    4096,   0  },
    {I1_WUEPS_FID_AWAKE,    "I1_AWAKE",      I1_WuepsAwakeFidInit,   VOS_START_THIRD,    4096,   0  },
    {I1_UEPS_FID_RR,        "RR_FID1",       I1_GAS_FidPidInit,      VOS_START_THIRD,    16384,  0  },
    {I1_WUEPS_FID_MM,       "MM_FID1",       I1_NAS_MML_FidInit,     VOS_START_THIRD,    16384,  0  },
    {I1_WUEPS_FID_CM,       "CM_FID1",       I1_WuepsCmFidInit,      VOS_START_THIRD,    16384,  0  },
    {I1_WUEPS_FID_RABMSM,   "RABMSM_FID1",   I1_WuepsRabmFidInit,    VOS_START_THIRD,    16384,  0  },
    {I1_WUEPS_FID_TAF,      "TAF_FID1",      I1_WuepsTafFidInit,     VOS_START_THIRD,    0,      0  },
    {I1_UEPS_FID_SL,        "SL_FID1",       LLC_I1FidInit,        VOS_START_THIRD,    16384,  0  },
    {I1_UEPS_FID_DC,        "DC_FID1",       GDLCST_I1FidInit,         VOS_START_THIRD,    16384,  0  },
    {I1_UEPS_FID_GRM,       "GRM_FID1",      GRM_I1FidInit,        VOS_START_THIRD,    16384,  0  },
    {I1_DSP_FID_APM,        "DSP_APM1",      I1_UPHY_APM_FidInit,    VOS_START_THIRD,    8192,   0  },
#if ( FEATURE_MODEM1_SUPPORT_WCDMA == FEATURE_ON )
    {I1_DSP_FID_WPHY,       "DSP_WPHY1",     I1_UPHY_UCOM_WphyFidInit, VOS_START_THIRD,   8192,   0  },
#endif
    {I1_DSP_FID_GPHY,       "DSP_GPHY1",     I1_UPHY_UCOM_GphyFidInit, VOS_START_THIRD,   8192,   0  },
    {I1_DSP_FID_GPHYLOW,    "DSP_GPHYLOW1",  I1_UPHY_UCOM_GphyLowFidInit, VOS_START_THIRD, 8192,   0 },
    {I1_DSP_FID_SLEEP,      "DSP_SLEEP1",    I1_UPHY_DRX_SleepFidInit, VOS_START_THIRD,   8192,   0  },
    {I1_DSP_FID_WAKE,       "DSP_WAKE1",     I1_UPHY_DRX_WakeFidInit, VOS_START_THIRD,   8192,   0  },
    {I1_DSP_FID_IDLE,       "DSP_IDLE1",     I1_UPHY_IDLE_FidInit,   VOS_START_THIRD,    8192,   0  },
    {I1_UEPS_FID_MSCC,      "MSCC_FID1",     I1_NAS_MSCC_FidInit,    VOS_START_THIRD,    0,      0  },
    {I1_WUEPS_FID_CSIMA,    "CSIMA_FID1",    VOS_NULL_PTR,           VOS_START_THIRD,    8192,   0  },
#if ( FEATURE_MODEM1_SUPPORT_LTE == FEATURE_ON )
    {I1_PS_FID_CM,          "LCM_FID1",      I1_CM_FidInit,              VOS_START_THIRD,    16384,  0  },
#else
    {I1_PS_FID_CM,          "LCM_FID1",      I1_NAS_ESM_FidInit,         VOS_START_THIRD,    16384,  0  },
#endif
#if ( FEATURE_MODEM1_SUPPORT_WCDMA == FEATURE_ON )
    {I1_UEPS_FID_WRR,       "I1_WRR_FID",    I1_WAS_FidPidInit,          VOS_START_THIRD,    16384,  0  },
    {I1_UEPS_FID_RM,        "I1_RM_FID",     WRLCMAC_I1UlFidInit,  VOS_START_THIRD,    16384,  0  },
    {I1_UEPS_FID_RM_DL,     "I1_RM_DL_FID",  WRLCMAC_I1DlFidInit,  VOS_START_THIRD,    0,      0  },
    {I1_UEPS_FID_PB,        "I1_PB_FID",     WPDCPBMC_I1FidInit,             VOS_START_THIRD,    16384,  0  },
#endif
#if ( FEATURE_MODEM1_SUPPORT_LTE == FEATURE_ON )
    {I1_MSP_L4_FID,         "mspL4_1",       I1_MSP_L4FidInit,       VOS_START_THIRD,    16384,  0  },
    {I1_MSP_FTM_FID,        "mspFTM1",       I1_MSP_FtmFidInit,      VOS_START_THIRD,    8192,   0  },
    {I1_PS_FID_RRC,         "I1_LRRC_FID",      I1_RRC_FidInit,             VOS_START_THIRD,    16384,  0  },
    {I1_PS_FID_OM,          "I1_LPSOM_FID",     I1_PS_OM_FidInit,           VOS_START_THIRD,    4096,   0  },
    {I1_PS_FID_RM_UL,       "I1_MACRLCUL_FID",  LMAC_RLC_2ndModemUlFidInit, VOS_START_THIRD,    16384,  0  },
    {I1_PS_FID_RM_DL,       "I1_MACRLCDL_FID",  LMAC_RLC_2ndModemDlFidInit, VOS_START_THIRD,    16384,  0  },
    {I1_PS_FID_HPA,         "I1_LHPA_FID",      I1_LHPA_FidPidInit,         VOS_START_THIRD,    4096,   0  },
    {I1_PS_FID_IMSA,        "I1_IMSA_FID",      I1_IMSA_FidInit,            VOS_START_THIRD,    16384,  0  },
    {I1_PS_FID_IMSVA,       "I1_IMSVA_FID",     I1_IMSVA_FidInit,           VOS_START_THIRD,    8192,   0  },
#endif

    {I2_MAPS_USIMM_FID,     "USIMM_FID2",    I2_USIMM_FID_Init,      VOS_START_THIRD,    8192,   0  },
    {I2_MAPS_SI_FID,        "SI_FID2",       I2_WuepsSIFidInit,      VOS_START_THIRD,    16384,  0  },

#if ( 3 == MULTI_MODEM_NUMBER )/* three modems */
    {I2_WUEPS_FID_SLEEP,    "I2_SLEEP",      I2_WuepsSleepFidInit,   VOS_START_THIRD,    4096,   0  },
    {I2_WUEPS_FID_AWAKE,    "I2_AWAKE",      I2_WuepsAwakeFidInit,   VOS_START_THIRD,    4096,   0  },
    {I2_UEPS_FID_RR,        "RR_FID2",       I2_GAS_FidPidInit,      VOS_START_THIRD,    16384,  0  },
    {I2_WUEPS_FID_MM,       "MM_FID2",       I2_NAS_MML_FidInit,     VOS_START_THIRD,    16384,  0  },
    {I2_WUEPS_FID_CM,       "CM_FID2",       I2_WuepsCmFidInit,      VOS_START_THIRD,    16384,  0  },
    {I2_WUEPS_FID_RABMSM,   "RABMSM_FID2",   I2_WuepsRabmFidInit,    VOS_START_THIRD,    16384,  0  },
    {I2_WUEPS_FID_TAF,      "TAF_FID2",      I2_WuepsTafFidInit,     VOS_START_THIRD,    0,      0  },
    {I2_UEPS_FID_SL,        "SL_FID2",       LLC_I2FidInit,        VOS_START_THIRD,    16384,  0  },
    {I2_UEPS_FID_DC,        "DC_FID2",       GDLCST_I2FidInit,         VOS_START_THIRD,    16384,  0  },
    {I2_UEPS_FID_GRM,       "GRM_FID2",      GRM_I2FidInit,        VOS_START_THIRD,    16384,  0  },
    {I2_DSP_FID_APM,        "DSP_APM2",      I2_UPHY_APM_FidInit,    VOS_START_THIRD,    8192,   0  },
    {I2_DSP_FID_GPHY,       "DSP_GPHY2",     I2_UPHY_UCOM_GphyFidInit, VOS_START_THIRD,   8192,   0  },
    {I2_DSP_FID_GPHYLOW,    "DSP_GPHYLOW2",  I2_UPHY_UCOM_GphyLowFidInit, VOS_START_THIRD, 8192,   0  },
    {I2_DSP_FID_SLEEP,      "DSP_SLEEP2",    I2_UPHY_DRX_SleepFidInit, VOS_START_THIRD,   8192,   0  },
    {I2_DSP_FID_WAKE,       "DSP_WAKE2",     I2_UPHY_DRX_WakeFidInit, VOS_START_THIRD,   8192,   0  },
    {I2_DSP_FID_IDLE,       "DSP_IDLE2",     I2_UPHY_IDLE_FidInit,   VOS_START_THIRD,    8192,   0  },
    {I2_UEPS_FID_MSCC,      "MSCC_FID2",     I2_NAS_MSCC_FidInit,    VOS_START_THIRD,    0,      0  },
    {I2_WUEPS_FID_CSIMA,    "CSIMA_FID2",    VOS_NULL_PTR,           VOS_START_THIRD,    8192,   0  },
    {I2_PS_FID_CM,          "LCM_FID2",      I2_NAS_ESM_FidInit,     VOS_START_THIRD,    16384,  0  },
#endif
#endif

#if(( ( FEATURE_MULTI_MODEM == FEATURE_ON ) || defined(HINGDFE_SEPARATE)) && (defined(GUC_RTT_TEST_ENABLED)))
    {I1_DSP_FID_RTTSTUB,    "RTT_STUB1",     I1_RTT_StubFidInit,    VOS_START_THIRD,    4096,   0  },
    {I1_DSP_FID_RTTMATCH,   "RTT_MATCH1",    I1_RTT_MatchFidInit,   VOS_START_THIRD,    4096,   0  },
    {I1_DSP_FID_RTTPREEMPT, "RTT_PREEMPT1",  I1_RTT_PreemptFidInit, VOS_START_THIRD,    4096,   0  },
    {I1_DSP_FID_RTTONLINE,  "RTT_ONLINE1",   I1_RTT_OnlineFidInit,  VOS_START_THIRD,    4096,   0  },
#endif

    {I0_MSP_L4_FID,         "mspL4_0",       I0_MSP_L4FidInit,       VOS_START_THIRD,    16384,  0  },
    {I0_MSP_FTM_FID,        "mspFTM0",       I0_MSP_FtmFidInit,      VOS_START_THIRD,    8192,   0  },
    {I0_PS_FID_RRC,         "LRRC_FID",      RRC_FidInit,            VOS_START_THIRD,    16384,  0  },
    {I0_PS_FID_CM,          "LCM_FID",       CM_FidInit,             VOS_START_THIRD,    16384,  0  },
    {I0_PS_FID_OM,          "LPSOM_FID",     PS_OM_FidInit,          VOS_START_THIRD,    4096,   0  },
    {I0_PS_FID_RM_UL,       "MACRLCUL_FID",  LMAC_RLC_UlFidInit,     VOS_START_THIRD,    16384,  0  },
    {I0_PS_FID_RM_DL,       "MACRLCDL_FID",  LMAC_RLC_DlFidInit,     VOS_START_THIRD,    16384,  0  },
    {I0_PS_FID_HPA,         "LHPA_FID",      LHPA_FidPidInit,        VOS_START_THIRD,    4096,   0  },
    {I0_PS_FID_IMSA,        "IMSA_FID",      IMSA_FidInit,           VOS_START_THIRD,    16384,  0  },
    {I0_PS_FID_IMSVA,       "IMSVA_FID",     IMSVA_FidInit,          VOS_START_THIRD,    8192,   0  },
#if (FEATURE_ON == FEATURE_EDA_SUPPORT)
    {PS_FID_EDA,            "EDA_FID",       EDA_FidInit,            VOS_START_THIRD,    8192,   0  },
#endif

#if (!defined(CHIP_BB_HI6210))
    {TLPHY_FID_RTTAGENT,   "TLPHY_RTTAGENT", TLPHY_RTTAGENT_FidInit, VOS_START_THIRD,    8192,   0  },
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_TDS)
    {HL1_FID_SNDCMD,        "SNDCMD_FID",    hl100_SndCmd_FidInit,   VOS_START_THIRD,    0,      0  },
    {TPS_FID_MAC,           "TPS_FID_MAC",   process_mac_FidInit,    VOS_START_THIRD,    0,      0  },
    {TPS_FID_PDC,           "TPS_FID_PDC",   process_pdc_FidInit,    VOS_START_THIRD,    0,      0  },
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
    {UEPS_FID_CAS1X,        "CAS1X_FID",     CAS_1X_FidPidInit,      VOS_START_THIRD,    16384,  0  },
    {UEPS_FID_CASHRPD,      "CASHRPD_FID",   CAS_HRPD_FidPidInit,    VOS_START_THIRD,    16384,  0  },
    {MSPS_FID_CTTF_1X_REV,  "1X_R_FID",      CTTF_1X_RevFidInit,     VOS_START_THIRD,    16384,  0  },
    {MSPS_FID_CTTF_1X_FWD,  "1X_F_FID",      CTTF_1X_FwdFidInit,     VOS_START_THIRD,    16384,  0  },
    {CTTF_FID_HRPD_REV,     "HRPD_R_FID",    CTTF_HRPD_RevFidInit,   VOS_START_THIRD,    16384,  0  },
    {CTTF_FID_HRPD_FWD,     "HRPD_F_FID",    CTTF_HRPD_FwdFidInit,   VOS_START_THIRD,    16384,  0  },
    {CTTF_FID_HRPD_SIG,     "HRPD_SIG_FID",  CTTF_HRPD_SigFidInit,   VOS_START_THIRD,    16384,  0  },
    {CPROC_FID_1X,          "CPROC_1X",      CPROC_1X_FidInit,       VOS_START_THIRD,    8192,   0  },
    {CPROC_FID_HRPD,        "CPROC_EVDO",    CPROC_HRPD_FidInit,     VOS_START_THIRD,    8192,   0  },
    {CPROC_FID_RM,          "CPROC_RM",      CPROC_RM_FidInit,       VOS_START_THIRD,    8192,   0  },
    {UEPS_FID_CNAS,         "CNAS_FID",      CNAS_FidInit,           VOS_START_THIRD,    16384,  0  },
#endif
#if ((FEATURE_ON == FEATURE_UE_MODE_CDMA) || (FEATURE_ON == FEATURE_PPP))
    {MSPS_FID_PPPC,         "PPPC_FID",      PPPC_FidInit,           VOS_START_THIRD,    16384,  0  },
#endif
#if ((FEATURE_ON == FEATURE_CSDR) || (FEATURE_ON == FEATURE_GSM_SDR))
    {DSP_FID_UPHY_HPA,      "UPHY_HPA",      UPHY_HPA_FidInit,       VOS_START_THIRD,    8192,   0  },
#endif
#ifdef GUC_RTT_TEST_ENABLED
    {I0_DSP_FID_RTTSTUB,    "RTT_STUB",      RTT_StubFidInit,       VOS_START_THIRD,    4096,   0  },
    {I0_DSP_FID_RTTMATCH,   "RTT_MATCH",     RTT_MatchFidInit,      VOS_START_THIRD,    4096,   0  },
    {I0_DSP_FID_RTTPREEMPT, "RTT_PREEMPT",   RTT_PreemptFidInit,    VOS_START_THIRD,    4096,   0  },
    {I0_DSP_FID_RTTONLINE,  "RTT_ONLINE",    RTT_OnlineFidInit,     VOS_START_THIRD,    4096,   0  },
#endif
#if ((FEATURE_ON == FEATURE_UE_MODE_CDMA) || (FEATURE_ON == FEATURE_GSM_SDR) || (FEATURE_ON == FEATURE_EASYRF))
    {LINKH_FID_ARM,         "LINKH_FID_ARM", LINKH_FidInit,          VOS_START_THIRD,    16384,  0  },
#endif
#if (FEATURE_ON == FEATURE_BASTET)
    {UEPS_FID_BASTET,       "BST_FidInit",   BST_FidInit,            VOS_START_THIRD,    4096,   0  },
#endif
#if ( ( FEATURE_ON == FEATURE_UE_MODE_NR ) && ( !defined(FEATURE_NPHY_ESLPS ) ) )
    {PHY_FID_PROCESS,       "PHY_ACTIVE",    UPHY_ActiveFidInit,     VOS_START_THIRD,    4096,   0  },
#endif
#if defined( FEATURE_NPHY_START)
    {NPHY_FID_STARTUP,      "NPHY_START",    NPHY_STARTUP_FidInit,   VOS_START_THIRD,    4096,   0  },
#endif
    {UEPS_FID_PSUPMEM,      "PSUPMEM_FID",   PSUPMEM_FidInit,        VOS_START_THIRD,    8192,   0  },
#if (VOS_WIN32 == VOS_OS_VER)
    {WUEPS_FID_AT,          "NAS_AT",        WuepsATFidInit,         VOS_START_THIRD,    6144,   0  },
    {WUEPS_FID_ITTSTUB,     "ITTSTUB",       WuepsFidITTStubInit,    VOS_START_THIRD,    10240,  0  },
#if (FEATURE_ON == FEATURE_DATA_SERVICE_NEW_PLATFORM)
    {ACPU_FID_ADS_UL,       "ADS_UL",        ADS_FidInit,            VOS_START_THIRD,    0,      0  },
#else
    {ACPU_FID_ADS_UL,       "ADS_UL",        ADS_UL_FidInit,         VOS_START_THIRD,    0,      0  },
    {ACPU_FID_ADS_DL,       "ADS_DL",        ADS_DL_FidInit,         VOS_START_THIRD,    0,      0  },
#endif
    {ACPU_FID_CSD,          "CSD",           CSD_FidInit,            VOS_START_THIRD,    0,      0  },
    {ACPU_FID_TAFAGENT,     "TAF_AGENT",     TAF_AGENT_FidInit,      VOS_START_THIRD,    6144,   0  },
    {MSP_APP_DS_FID,        "AppDS",         VOS_NULL_PTR,           VOS_START_THIRD,    8192,   0  },
    {ACPU_FID_CBT,          "CBT",           CBTAppFidInit,          VOS_START_THIRD,    8192,   0  },
    {ACPU_FID_USIMM,        "USIM",          USIMM_APP_FID_Init,     VOS_START_THIRD,    8192,   0  },
    {ACPU_FID_PAM_OM,       "PAM_OM",        PAMOM_APP_FID_Init,     VOS_START_THIRD,    8192,   0  },
#if (FEATURE_MM_TASK_COMBINE != FEATURE_ON)
    {I0_UEPS_FID_NRMM,      "NRMM_FID",      I0_NAS_NRMM_FidInit,    VOS_START_THIRD,    16384,  0  },
    {I0_UEPS_FID_NRCM,      "NRCM_FID",      I0_NAS_NRCM_FidInit,    VOS_START_THIRD,    16384,  0  },
    {I0_UEPS_FID_NREAP,     "NREAP_FID",     I0_NAS_NREAP_FidInit,   VOS_START_THIRD,    16384,  0  },
#endif
#endif

#if (FEATURE_MM_TASK_COMBINE == FEATURE_ON)
    {I0_UEPS_FID_NRCM,      "NRCM_FID",      I0_NAS_NRCM_FidInit,    VOS_START_THIRD,    16384,  0  },
    {I0_UEPS_FID_NREAP,     "NREAP_FID",     I0_NAS_NREAP_FidInit,   VOS_START_THIRD,    16384,  0  },
#endif

#if (FEATURE_NRMM_TASK_TYPE == FEATURE_ON)
    {I0_UEPS_FID_NRMM,      "NRMM_FID",      I0_NAS_NRMM_FidInit,    VOS_START_THIRD,    16384,  0  },
    {I0_UEPS_FID_NRCM,      "NRCM_FID",      I0_NAS_NRCM_FidInit,    VOS_START_THIRD,    16384,  0  },
    {I0_UEPS_FID_NREAP,     "NREAP_FID",     I0_NAS_NREAP_FidInit,   VOS_START_THIRD,    16384,  0  },
#endif

#if (FEATURE_ON == FEATURE_PC5_DATA_CHANNEL)
    {I0_PS_FID_VRRC,        "VRRC_FID",      VRRC_FidInit,           VOS_START_THIRD,    16384,  0  },
    {I0_PS_FID_VRM_UL,      "VMACRLCUL_FID", VMAC_RLC_UlFidInit,     VOS_START_THIRD,    16384,  0  },
    {I0_PS_FID_VRM_DL,      "VMACRLCDL_FID", VMAC_RLC_DlFidInit,     VOS_START_THIRD,    16384,  0  },
    {I0_PS_FID_VHPA,        "VHPA_FID",      VHPA_FidPidInit,        VOS_START_THIRD,    4096,   0  },
    {I0_PS_FID_VNAS,        "VNAS_FID",      VNAS_FidInit,           VOS_START_THIRD,    16384,  0  },
#endif
};
#endif

/* 单板A核请配置这里 */
#if (OSA_CPU_ACPU == VOS_OSA_CPU)
/* FID静态信息表 */
FID_StaticTableType g_aFidStaticTable[VOS_FID_BUTT] = {
    {DOPRA_FID_SYS,             "fSYS",            VOS_NULL_PTR,            VOS_START_FIRST,    0,     0  },
    /* 以上是OSA 保留配置请不要动 */
    {MSP_FID_DIAG_ACPU,         "mspAppDiag",      MSP_AppDiagFidInit,      VOS_START_THIRD,    16384, 0  },
#if (FEATURE_ON == FEATURE_CHR_OM)
    {MSP_FID_CHR_ACPU,          "mspAppChr",       msp_app_chr_fid_init,       VOS_START_THIRD,    4096,  0  },
#endif
    {MSP_APP_DS_FID,            "AppDS",           DMS_DsFidInit,           VOS_START_THIRD,    8192,  0  },
    {PS_FID_APP_NDIS_PPP_DIPC,  "AppNdisDipc",     APP_NDIS_FidInit,        VOS_START_THIRD,    16384, 0  },
    {ACPU_FID_NFEXT,            "ACPU_NFEXT",      NFEXT_FidInit,           VOS_START_THIRD,    8192,  0  },
#if (FEATURE_ON == FEATURE_DATA_SERVICE_NEW_PLATFORM)
    {ACPU_FID_ADS_UL,           "ADS_UL",          ADS_FidInit,             VOS_START_THIRD,    8192,  0  },
#else
    {ACPU_FID_ADS_UL,           "ADS_UL",          ADS_UL_FidInit,          VOS_START_THIRD,    8192,  0  },
    {ACPU_FID_ADS_DL,           "ADS_DL",          ADS_DL_FidInit,          VOS_START_THIRD,    5120,  0  },
#endif

    {ACPU_FID_TAFAGENT,         "TAF_AGENT",       TAF_AGENT_FidInit,       VOS_START_THIRD,    16384, 0  },

#if(FEATURE_ACPU_STAT == FEATURE_ON)
    {ACPU_FID_CPULOAD,          "CpuLoadMonitor",  CPULOAD_FidInit,         VOS_START_THIRD,    4096,  0  },
#endif

#if(FEATURE_ACPU_FC_POINT_REG == FEATURE_ON)
    {UEPS_FID_FLOWCTRL_A,       "ACpuFlowCtrl",    FC_AcoreFidInit,        VOS_START_THIRD,    8192,  0  },
#endif

#ifdef CBT_ENABLED
    {ACPU_FID_CBT,              "CBT",             CBTAppFidInit,           VOS_START_THIRD,    8192,  0  },
#endif
    {ACPU_FID_USIMM,            "USIM",            USIMM_APP_FID_Init,      VOS_START_THIRD,    8192,  0  },
    {ACPU_FID_PAM_OM,           "PAM_OM",          PAMOM_APP_FID_Init,      VOS_START_THIRD,    8192,  0  },

#if (FEATURE_VOICE_UP == FEATURE_ON)
    {ACPU_FID_MED_RT,           "MED_Voice_RT",    VOICE_RtFidInit,         VOS_START_THIRD,    8192,  0  },
    {ACPU_FID_MED,              "MED_Voice",       VOICE_NormalFidInit,     VOS_START_THIRD,    16384, 0  },
#endif

#if (FEATURE_ON == FEATURE_BASTET)
    {ACPU_FID_BASTET_COMM,      "BASTET_COMM",     BASTET_CommFidInit,      VOS_START_THIRD,    8192,  0  },
#endif

#if(FEATURE_PPP == FEATURE_ON)
    {ACPU_FID_PPP,              "ACPU_PPP",        PPP_FidInit,         VOS_START_THIRD,    16384, 0  },
#endif

#if (defined(CONFIG_HISI_BALONG_EXTRA_MODEM_MBB))
    {ACPU_FID_VOICE_AGENT,      "VOICE_AGENT",      VOICE_AGENT_FidInit,    VOS_START_THIRD,    16384, 0  },
#endif

#if (FEATURE_PC5_DATA_CHANNEL == FEATURE_ON)
    {ACPU_FID_PC5_CHQEST,      "ACPU_PC5_CHQEST",  Pc5ChqEst_FidInit,      VOS_START_THIRD,     8192,  0  },
#endif

#if (FEATURE_HIFI_MANAGE == FEATURE_ON)
    {ACPU_FID_HIFI_MANAGE,      "HIFI_MANAGE",      HIFI_MANAGE_FidInit,           VOS_START_THIRD,    16384, 0  },
#endif

};
#endif

/* 单板NR核请配置这里 */
#if (OSA_CPU_NRCPU == VOS_OSA_CPU)
/* FID静态信息表 */
FID_StaticTableType g_aFidStaticTable[VOS_FID_BUTT] = {
    {DOPRA_FID_SYS,                  "fSYS",                  VOS_NULL_PTR,             VOS_START_FIRST,    0,      0  },
    /* 以上是OSA 保留配置请不要动 */
    {NRCPU_FID_PAM_OM,               "PAM_OM",                PAMOM_NR_FID_Init,        VOS_START_THIRD,    8192,   0  },
    {UEPS_FID_NRRC,                  "NRRC_FID",              I0_NRRC_FidInit,          VOS_START_THIRD,    16384,  0  },
    {I0_UEPS_FID_L2MA,               "L2MA_FID",              L2MA_FidInit,             VOS_START_THIRD,    16384,  0  },
    {I0_UEPS_FID_NPDCPRLC_UL,        "NPDCP_RLC_UL",          NPDCP_RLC_UlFidInit,      VOS_START_THIRD,    16384,  0  },
    {I0_UEPS_FID_NPDCP_DL_NRT,       "NPDCP_DL_NRT",          NPDCP_DL_NRT_FidInit,     VOS_START_THIRD,    16384,  0  },
    {I0_UEPS_FID_NRLC_DL_NRT,        "NRLC_DL_NRT",           NRLC_DlNrtFidInit,        VOS_START_THIRD,    16384,  0  },
    {I0_UEPS_FID_NPDCPRLC_UL_REL,    "NPDCPRLC_UL_REL",        NPDCP_RLC_UlRelFidInit,   VOS_START_THIRD,    4096,  0  },
#if (FEATURE_ON == FEATURE_NPHY_STUB_ESL)
    {UEPS_FID_NPHY_STUB,             "NPHY_STUB_FID",         I0_NPHY_STUB_FidInit,     VOS_START_THIRD,    16384,  0  },
#endif
    {MSP_FID_DIAG_NRM,               "mspNrmDiag",            MSP_DiagNrmFidInit,       VOS_START_THIRD,    8192,   0  },
    {I0_UEPS_FID_NRMM,               "NRMM_FID",              I0_NAS_NRMM_FidInit,      VOS_START_THIRD,    16384,  0  },
    {NRPHY_FID_STARTUP,              "NRPHY_START",           NRPHY_STARTUP_FidInit,    VOS_START_THIRD,    8192,   0  },
    {I0_UEPS_FID_NRCM,               "NRCM_FID",              I0_NAS_NRCM_FidInit,      VOS_START_THIRD,    16384,  0  },
#ifdef CBT_ENABLED
    {NRCBT_FID_NOSIG,                "NRNOSIG",               NBT_FidInit,              VOS_START_THIRD,    8192,   0  },
#endif
    {NRCBT_FID_CBT,                  "NRCBT",                 NRCBT_FidInit,            VOS_START_THIRD,    8192,   0  },
    {I0_UEPS_FID_NREAP,              "NREAP_FID",             I0_NAS_NREAP_FidInit,     VOS_START_THIRD,    16384,  0  },
    {I0_UEPS_FID_SDAP,               "SDAP_FID",              SDAP_FidInit,             VOS_START_THIRD,    16384,  0  },
    {NRAGENT_FID_AGENT,              "NPHY_RTTAGENT",         NPHY_RTTAGENTNR_FidInit,  VOS_START_THIRD,    8192,   0  },
    {DSP_FID_BBA_RFM_AGENT_LPC,      "RFM_LPCAGENT",          BBA_RFMAGENT_FidInit,     VOS_START_THIRD,    8192,   0  },
#if (FEATURE_ON == FEATURE_CHR_OM)
    {MSP_FID_CHR_NRCCPU,             "mspChrNr",              msp_chr_fid_init,           VOS_START_THIRD,    4096,   0  },
#endif
    {DSP_FID_RTTSTUB_NR,             "RTT_STUB_NR",           RTTSTUB_NR_FidInit,       VOS_START_THIRD,    4096,   0  },
};
#endif

#if (OSA_CPU_L2HAC == VOS_OSA_CPU)
/* FID静态信息表 */
FID_StaticTableType g_aFidStaticTable[VOS_FID_BUTT] = {
    {DOPRA_FID_SYS,                  "fSYS",                  VOS_NULL_PTR,                   VOS_START_FIRST,    0,          0  },
    /* 以上是OSA 保留配置请不要动 */
    {I0_UEPS_FID_NL2_DECODE,         "NL2_DECODE_FID",        NL2_DECODE_FidInit,             VOS_START_THIRD,    10240,      0  },
    {I0_UEPS_FID_NMACRLC_UL_RT,      "NMACRLC_UL_FID",        NMACRLC_UL_RT_FidInit,          VOS_START_THIRD,    10240,      0  },
    {MSP_FID_DIAG_HAC,               "HacDiag",               MSP_DiagHacFidInit,             VOS_START_THIRD,    8192,       0  },
};
#endif

#endif /* _V_IDDEF_INC */

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */
