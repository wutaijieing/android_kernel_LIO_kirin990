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

/*
 * 版权所有（c）华为技术有限公司 2015-2019
 * 功能描述: OSA PID/FID头文件
 */
#ifndef _V_ID_H
#define _V_ID_H

#include "v_id_def.h"
#include "vos_pid_def.h"
#include "product_config.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */

/*lint -e778*/
/* To decide to use dynamic FID/PID or static FID/PID, if using static */
#define VOS_USE_DYNAMIC_FIDPID VOS_NO

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
extern VOS_UINT32 HPA_FIDInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 GAS_FidPidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 WAS_FidPidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 LLC_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 GDLCST_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 WRLCMAC_UlFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 WRLCMAC_DlFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I0_WuepsRabmFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I0_NAS_MML_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 CNAS_FidInit(enum VOS_InitPhaseDefine initPhase);
extern VOS_UINT32 I0_NAS_MSCC_FidInit(enum VOS_InitPhaseDefine initPhase);
extern VOS_UINT32 CSS_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I0_WuepsCmFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 USIMM_FID_Init(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 WuepsSIFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I0_WuepsTafFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 WuepsSleepFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 WuepsAwakeFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 UE_FIDInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 GRM_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 RRC_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 PS_OM_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 CM_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 LMAC_RLC_UlFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 LMAC_RLC_DlFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 LHPA_FidPidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 COMM_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 MSP_DiagFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 msp_chr_fid_init(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I0_MSP_L4FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I0_MSP_FtmFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 Spy_FID_Init(enum VOS_InitPhaseDefine ip);
#ifdef CBT_ENABLED
extern VOS_UINT32 OM_NoSigFidInit(enum VOS_InitPhaseDefine ip);
#endif
extern VOS_UINT32 FC_CcoreFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 CDS_FidInit(enum VOS_InitPhaseDefine phase);
#if (FEATURE_ON == FEATURE_PC5_DATA_CHANNEL)
extern VOS_UINT32 VMAC_RLC_UlFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 VMAC_RLC_DlFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 VRRC_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 VHPA_FidPidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 VNAS_FidInit(enum VOS_InitPhaseDefine ip);
#endif
extern VOS_UINT32 PSUPMEM_FidInit(enum VOS_InitPhaseDefine phase);
#if (FEATURE_ON == FEATURE_BASTET)
extern VOS_UINT32 BST_FidInit(enum VOS_InitPhaseDefine phase);
#endif
extern VOS_UINT32 NDCLIENT_FidInit(enum VOS_InitPhaseDefine phase);

extern VOS_UINT32 CAS_1X_FidPidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 CAS_HRPD_FidPidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 CTTF_1X_RevFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 CTTF_1X_FwdFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 CTTF_HRPD_RevFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 CTTF_HRPD_FwdFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 CTTF_HRPD_SigFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 PPPC_FidInit(enum VOS_InitPhaseDefine ip);

extern VOS_UINT32 UPHY_HPA_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 LINKH_FidInit(enum VOS_InitPhaseDefine initPhrase);
extern VOS_UINT32 CPROC_1X_FidInit(enum VOS_InitPhaseDefine initPhrase);
extern VOS_UINT32 CPROC_HRPD_FidInit(enum VOS_InitPhaseDefine initPhrase);
extern VOS_UINT32 CPROC_RM_FidInit(enum VOS_InitPhaseDefine initPhrase);

extern VOS_UINT32 RTT_StubFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 RTT_MatchFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 RTT_PreemptFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 RTT_OnlineFidInit(enum VOS_InitPhaseDefine ip);

extern VOS_UINT32 CBTFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 PAMOM_FID_Init(enum VOS_InitPhaseDefine ip);

#if (FEATURE_MM_TASK_COMBINE == FEATURE_ON)
extern VOS_UINT32 I0_NAS_NRCM_FidInit(enum VOS_InitPhaseDefine initPhrase);
extern VOS_UINT32 I0_NAS_NREAP_FidInit(enum VOS_InitPhaseDefine initPhrase);
#endif

#if (FEATURE_NRMM_TASK_TYPE == FEATURE_ON)
extern VOS_UINT32 I0_NAS_NRMM_FidInit(enum VOS_InitPhaseDefine initPhrase);
extern VOS_UINT32 I0_NAS_NRCM_FidInit(enum VOS_InitPhaseDefine initPhrase);
extern VOS_UINT32 I0_NAS_NREAP_FidInit(enum VOS_InitPhaseDefine initPhrase);
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_TDS) /* TDS begin */
extern VOS_UINT32 process_pdc_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 process_mac_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 hl100_SndCmd_FidInit(enum VOS_InitPhaseDefine ip);
#endif /* (FEATURE_ON == FEATURE_UE_MODE_TDS) TDS end */

#endif /* (OSA_CPU_CCPU == VOS_OSA_CPU) */

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
extern VOS_UINT32 APP_NDIS_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 DMS_DsFidInit(enum VOS_InitPhaseDefine ip);

extern VOS_UINT32 RNIC_FidInit(enum VOS_InitPhaseDefine ip);

#if (FEATURE_PPP == FEATURE_ON)
extern VOS_UINT32 PPP_FidInit(enum VOS_InitPhaseDefine ip);
#endif

extern VOS_UINT32 NFEXT_FidInit(enum VOS_InitPhaseDefine ip);

#if (FEATURE_ACPU_FC_POINT_REG == FEATURE_ON)
extern VOS_UINT32 FC_AcoreFidInit(enum VOS_InitPhaseDefine phase);
#endif

#if (FEATURE_ACPU_STAT == FEATURE_ON)
extern VOS_UINT32 CPULOAD_FidInit(enum VOS_InitPhaseDefine phase);
#endif

extern VOS_UINT32 MSP_AppDiagFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 msp_app_chr_fid_init(enum VOS_InitPhaseDefine ip);

#if (FEATURE_ON == FEATURE_BASTET)
extern VOS_UINT32 BASTET_CommFidInit(enum VOS_InitPhaseDefine ip);
#endif

#ifdef CBT_ENABLED
extern VOS_UINT32 CBTAppFidInit(enum VOS_InitPhaseDefine ip);
#endif
extern VOS_UINT32 USIMM_APP_FID_Init(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 PAMOM_APP_FID_Init(enum VOS_InitPhaseDefine ip);
#if (defined(CONFIG_HISI_BALONG_EXTRA_MODEM_MBB))
extern VOS_UINT32 VOICE_AGENT_FidInit(enum VOS_InitPhaseDefine ip);
#endif

#if (FEATURE_PC5_DATA_CHANNEL == FEATURE_ON)
extern VOS_UINT32 Pc5ChqEst_FidInit(enum VOS_InitPhaseDefine initPhrase);
#endif

#endif /* (OSA_CPU_ACPU == VOS_OSA_CPU) */

/* NR */
#if (OSA_CPU_NRCPU == VOS_OSA_CPU)
extern VOS_UINT32 I0_NAS_NRMM_FidInit(enum VOS_InitPhaseDefine initPhase);
extern VOS_UINT32 I0_NAS_NRCM_FidInit(enum VOS_InitPhaseDefine initPhase);
extern VOS_UINT32 I0_NAS_NREAP_FidInit(enum VOS_InitPhaseDefine initPhase);
extern VOS_UINT32 I0_NRRC_FidInit(enum VOS_InitPhaseDefine initPhase);
extern VOS_UINT32 PAMOM_NR_FID_Init(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 NPDCP_RLC_UlFidInit(enum VOS_InitPhaseDefine phase);
extern VOS_UINT32 NPDCP_RLC_UlRelFidInit(enum VOS_InitPhaseDefine phase);
extern VOS_UINT32 NPDCP_DL_NRT_FidInit(enum VOS_InitPhaseDefine phase);
extern VOS_UINT32 L2MA_FidInit(enum VOS_InitPhaseDefine phase);
extern VOS_UINT32 NRLC_DlNrtFidInit(enum VOS_InitPhaseDefine phase);
extern VOS_UINT32 SDAP_FidInit(enum VOS_InitPhaseDefine phase);

#if (FEATURE_ON == FEATURE_NPHY_STUB_ESL)
extern VOS_UINT32 I0_NPHY_STUB_FidInit(enum VOS_InitPhaseDefine phase);
#endif

extern VOS_UINT32 RTTSTUB_NR_FidInit(enum VOS_InitPhaseDefine ip);

extern VOS_UINT32 MSP_DiagNrmFidInit(enum VOS_InitPhaseDefine ip);

#if (FEATURE_ON == FEATURE_CHR_OM)
extern VOS_UINT32 msp_chr_fid_init(enum VOS_InitPhaseDefine ip);
#endif

#ifdef CBT_ENABLED
extern VOS_UINT32 NBT_FidInit(enum VOS_InitPhaseDefine ip);
#endif
extern VOS_UINT32 NRCBT_FidInit(enum VOS_InitPhaseDefine ip);

#endif

#if (OSA_CPU_L2HAC == VOS_OSA_CPU)
extern VOS_UINT32 NL2_DECODE_FidInit(enum VOS_InitPhaseDefine phase);
extern VOS_UINT32 NMACRLC_UL_RT_FidInit(enum VOS_InitPhaseDefine phase);
extern VOS_UINT32 MSP_DiagHacFidInit(enum VOS_InitPhaseDefine ip);
#endif

#ifdef HPA_ITT
extern VOS_UINT32 MAPS_Stub_Fid_InitFun(enum VOS_InitPhaseDefine initPhase);
#endif /* HPA_ITT */

#ifdef USP_2_0
extern VOS_UINT32 WHPA_FidPidInit(enum VOS_InitPhaseDefine ip);
#endif /* USP_2_0 */

#if (VOS_WIN32 == VOS_OS_VER)
extern VOS_UINT32 WuepsATFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 RNIC_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 CBTAppFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 USIMM_APP_FID_Init(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 PAMOM_APP_FID_Init(enum VOS_InitPhaseDefine ip);
#endif /* (VOS_WIN32 == VOS_OS_VER) */

#if (FEATURE_ON == FEATURE_DATA_SERVICE_NEW_PLATFORM)
extern VOS_UINT32 ADS_FidInit(enum VOS_InitPhaseDefine ip);
#else
extern VOS_UINT32 ADS_UL_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 ADS_DL_FidInit(enum VOS_InitPhaseDefine ip);
#endif

extern VOS_UINT32 TAF_AGENT_FidInit(enum VOS_InitPhaseDefine phase);

/* DSP 上移  */
extern VOS_UINT32 UPHY_APM_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 UPHY_UCOM_WphyFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 UPHY_UCOM_GphyFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 UPHY_UCOM_GphyLowFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 UPHY_DRX_SleepFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 UPHY_DRX_WakeFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 UPHY_IDLE_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 UPHY_UCOM_UpaFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 UPHY_INIT_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 RCM_DRV_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 BBA_RFM_InitFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 BBA_CAL_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 BBA_CALC_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 UPHY_MNTN_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 BBA_RFM_LpcFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 BBA_RFMAGENT_FidInit(enum VOS_InitPhaseDefine ip);

#if ((FEATURE_ON == FEATURE_UE_MODE_NR) && (!defined(FEATURE_NPHY_ESLPS)))
extern VOS_UINT32 UPHY_ActiveFidInit(enum VOS_InitPhaseDefine ip);
#endif
#if defined(FEATURE_NPHY_START)
extern VOS_UINT32 NPHY_STARTUP_FidInit(enum VOS_InitPhaseDefine ip);
#endif

/* 5G ARM NRPHY StartUp */
extern VOS_UINT32 NRPHY_STARTUP_FidInit(enum VOS_InitPhaseDefine ip); /* DSP 上移 end */

/* Voice in ACPU start */
extern VOS_UINT32 VOICE_NormalFidInit(enum VOS_InitPhaseDefine initPhrase);
extern VOS_UINT32 VOICE_RtFidInit(enum VOS_InitPhaseDefine initPhrase); /* Voice in ACPU end */

extern VOS_UINT32 IMSA_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 IMSVA_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 IPSMNTN_CcoreFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 MTC_FidInit(enum VOS_InitPhaseDefine ip);

extern VOS_UINT32 TLPHY_RTTAGENT_FidInit(enum VOS_InitPhaseDefine phase);

extern VOS_UINT32 NPHY_RTTAGENTNR_FidInit(enum VOS_InitPhaseDefine phase);

/* RRM */
extern VOS_UINT32 RRM_FidInit(enum VOS_InitPhaseDefine phase);
extern VOS_UINT32 WPDCPBMC_FidInit(enum VOS_InitPhaseDefine ip);

/* MODEM ID DEFINITION */
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
extern VOS_UINT32 I1_USIMM_FID_Init(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_WuepsSIFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_WuepsSleepFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_WuepsAwakeFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_GAS_FidPidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_NAS_MML_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_WuepsCmFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_WuepsRabmFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_WuepsTafFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 GDLCST_I1FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 LLC_I1FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 GRM_I1FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_UPHY_UCOM_GphyFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_UPHY_UCOM_GphyLowFidInit(enum VOS_InitPhaseDefine ip);
#if (FEATURE_MODEM1_SUPPORT_WCDMA == FEATURE_ON)
extern VOS_UINT32 I1_UPHY_UCOM_WphyFidInit(enum VOS_InitPhaseDefine ip);
#endif
extern VOS_UINT32 I1_UPHY_APM_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_UPHY_DRX_SleepFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_UPHY_DRX_WakeFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_UPHY_IDLE_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_NAS_MSCC_FidInit(enum VOS_InitPhaseDefine phase);

extern VOS_UINT32 I1_CSIMA_FidInit(enum VOS_InitPhaseDefine ip);

#if (FEATURE_MODEM1_SUPPORT_WCDMA == FEATURE_ON)
extern VOS_UINT32 I1_WAS_FidPidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 WRLCMAC_I1UlFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 WRLCMAC_I1DlFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 WPDCPBMC_I1FidInit(enum VOS_InitPhaseDefine ip);
#endif

#if (FEATURE_MODEM1_SUPPORT_LTE == FEATURE_ON)
extern VOS_UINT32 I1_MSP_L4FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_MSP_FtmFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_RRC_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_PS_OM_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_LHPA_FidPidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_IMSA_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_IMSVA_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 LMAC_RLC_2ndModemUlFidInit(enum VOS_InitPhaseDefine phase);
extern VOS_UINT32 LMAC_RLC_2ndModemDlFidInit(enum VOS_InitPhaseDefine phase);
extern VOS_UINT32 I1_CM_FidInit(enum VOS_InitPhaseDefine ip);
#else
extern VOS_UINT32 I1_NAS_ESM_FidInit(enum VOS_InitPhaseDefine ip);
#endif

extern VOS_UINT32 I2_USIMM_FID_Init(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_WuepsSIFidInit(enum VOS_InitPhaseDefine ip);

#if (3 == MULTI_MODEM_NUMBER) /* three modems */
extern VOS_UINT32 I2_WuepsSleepFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_WuepsAwakeFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_GAS_FidPidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_NAS_MML_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_WuepsCmFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_WuepsRabmFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_WuepsTafFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 GDLCST_I2FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 LLC_I2FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 GRM_I2FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_UPHY_UCOM_GphyFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_UPHY_UCOM_GphyLowFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_UPHY_APM_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_UPHY_DRX_SleepFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_UPHY_DRX_WakeFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_UPHY_IDLE_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_NAS_MSCC_FidInit(enum VOS_InitPhaseDefine phase);
extern VOS_UINT32 I2_CSIMA_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_NAS_ESM_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_RTT_STUB_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_RTT_MATCH_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_RTT_PREEMPT_FidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I2_RTT_ONLINE_FidInit(enum VOS_InitPhaseDefine ip);

#endif /* ( 3 == MULTI_MODEM_NUMBER ) */
#endif /* ( FEATURE_MULTI_MODEM == FEATURE_ON ) */

#if (((FEATURE_MULTI_MODEM == FEATURE_ON) || defined(HINGDFE_SEPARATE)) && (defined(GUC_RTT_TEST_ENABLED)))
extern VOS_UINT32 I1_RTT_StubFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_RTT_MatchFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_RTT_PreemptFidInit(enum VOS_InitPhaseDefine ip);
extern VOS_UINT32 I1_RTT_OnlineFidInit(enum VOS_InitPhaseDefine ip);
#endif

extern VOS_UINT32 CMMCA_FidInit(enum VOS_InitPhaseDefine ip);

#if (FEATURE_ON == FEATURE_EDA_SUPPORT)
extern VOS_UINT32 EDA_FidInit(enum VOS_InitPhaseDefine ip);
#endif

#if (FEATURE_HIFI_MANAGE == FEATURE_ON)
extern VOS_UINT32 HIFI_MANAGE_FidInit(enum VOS_InitPhaseDefine ip);
#endif

/* ************************ MACRO & TYPE DEFINITION ************************** */
/* VOS编程模式下的用户模块划分信息 */
#if (OSA_CPU_CCPU == VOS_OSA_CPU)
BEGIN_FID_DEFINITION()
/*
 * add your FID here, e.g.
 * DEFINE_FID(VOS_FID_APP0)
 * DEFINE_FID(VOS_FID_APPx)
 */
DEFINE_FID(MSP_FID_DIAG_CCPU) /* 提醒! 新增FID请按顺序添加到g_psCommFidModemIdTbl表中 */
DEFINE_FID(MSP_FID_CHR_CCPU)
DEFINE_FID(I0_MAPS_USIMM_FID)
DEFINE_FID(I0_MAPS_SI_FID)
DEFINE_FID(I0_UEPS_FID_SL)
DEFINE_FID(I0_UEPS_FID_RM)
DEFINE_FID(I0_UEPS_FID_RR)
DEFINE_FID(I0_UEPS_FID_GRM)
DEFINE_FID(WUEPS_FID_HPA)
DEFINE_FID(I0_WUEPS_FID_MM)
DEFINE_FID(I0_WUEPS_FID_CM) /* 提醒! 新增FID请按顺序添加到g_psCommFidModemIdTbl表中 */
DEFINE_FID(I0_WUEPS_FID_RABMSM)
DEFINE_FID(I0_WUEPS_FID_TAF)
DEFINE_FID(WUEPS_FID_SPY)
DEFINE_FID(UEPS_FID_FLOWCTRL_C)
DEFINE_FID(I0_UEPS_FID_RM_DL)
#ifdef CBT_ENABLED
DEFINE_FID(WUEPS_FID_NOSIG)
#endif
DEFINE_FID(UEPS_FID_CDS)
DEFINE_FID(UEPS_FID_NDCLIENT)
DEFINE_FID(I0_UEPS_FID_DC)
DEFINE_FID(I0_WUEPS_FID_SLEEP) /* 提醒! 新增FID请按顺序添加到g_psCommFidModemIdTbl表中 */
DEFINE_FID(I0_WUEPS_FID_AWAKE)
DEFINE_FID(I0_DSP_FID_APM)
DEFINE_FID(DSP_FID_UPHY_MNTN)
DEFINE_FID(DSP_FID_UPA)
DEFINE_FID(I0_DSP_FID_WPHY)
DEFINE_FID(I0_DSP_FID_GPHY)
DEFINE_FID(I0_DSP_FID_GPHYLOW)
DEFINE_FID(I0_DSP_FID_SLEEP)
DEFINE_FID(I0_DSP_FID_WAKE)
DEFINE_FID(I0_DSP_FID_IDLE)
DEFINE_FID(DSP_FID_RCM) /* 提醒! 新增FID请按顺序添加到g_psCommFidModemIdTbl表中 */
DEFINE_FID(DSP_FID_STARTUP)
#if (FEATURE_ON == FEATURE_EASYRF)
DEFINE_FID(DSP_FID_RFM)
DEFINE_FID(DSP_FID_BBA_CAL)
#ifdef CBT_ENABLED
DEFINE_FID(DSP_FID_BBA_CALC)
#endif
DEFINE_FID(DSP_FID_BBA_RFM_LPC)
#endif
DEFINE_FID(UEPS_FID_IPS_CCORE)
DEFINE_FID(I0_WUEPS_FID_CSIMA)
DEFINE_FID(WUEPS_FID_CMMCA)
DEFINE_FID(CCPU_FID_CBT)
DEFINE_FID(CCPU_FID_PAM_OM)
DEFINE_FID(UEPS_FID_MTC)
DEFINE_FID(I0_UEPS_FID_MSCC) /* 提醒! 新增FID请按顺序添加到g_psCommFidModemIdTbl表中 */
DEFINE_FID(UEPS_FID_CSS)

DEFINE_FID(UEPS_FID_RRM)
DEFINE_FID(I0_UEPS_FID_PB)
DEFINE_FID(I0_UEPS_FID_WRR)
#if (FEATURE_MULTI_MODEM == FEATURE_ON)
DEFINE_FID(I1_MAPS_USIMM_FID) /* 提醒! 新增FID请按顺序添加到g_psCommFidModemIdTbl表中 */
DEFINE_FID(I1_MAPS_SI_FID)
DEFINE_FID(I1_WUEPS_FID_SLEEP)
DEFINE_FID(I1_WUEPS_FID_AWAKE)
DEFINE_FID(I1_UEPS_FID_RR)
DEFINE_FID(I1_WUEPS_FID_MM)
DEFINE_FID(I1_WUEPS_FID_CM)
DEFINE_FID(I1_WUEPS_FID_RABMSM)
DEFINE_FID(I1_WUEPS_FID_TAF)
DEFINE_FID(I1_UEPS_FID_SL)
DEFINE_FID(I1_UEPS_FID_DC)
DEFINE_FID(I1_UEPS_FID_GRM)
DEFINE_FID(I1_DSP_FID_APM)
#if (FEATURE_MODEM1_SUPPORT_WCDMA == FEATURE_ON)
DEFINE_FID(I1_DSP_FID_WPHY) /* 提醒! 新增FID请按顺序添加到g_psCommFidModemIdTbl表中 */
#endif
DEFINE_FID(I1_DSP_FID_GPHY)
DEFINE_FID(I1_DSP_FID_GPHYLOW)
DEFINE_FID(I1_DSP_FID_SLEEP)
DEFINE_FID(I1_DSP_FID_WAKE)
DEFINE_FID(I1_DSP_FID_IDLE)
DEFINE_FID(I1_UEPS_FID_MSCC)
DEFINE_FID(I1_WUEPS_FID_CSIMA)
DEFINE_FID(I1_PS_FID_CM)
#if (FEATURE_MODEM1_SUPPORT_WCDMA == FEATURE_ON)
DEFINE_FID(I1_UEPS_FID_WRR)
DEFINE_FID(I1_UEPS_FID_RM)
DEFINE_FID(I1_UEPS_FID_RM_DL)
DEFINE_FID(I1_UEPS_FID_PB) /* 提醒! 新增FID请按顺序添加到g_psCommFidModemIdTbl表中 */
#endif

#if (FEATURE_MODEM1_SUPPORT_LTE == FEATURE_ON)
DEFINE_FID(I1_MSP_L4_FID)
DEFINE_FID(I1_MSP_FTM_FID)
DEFINE_FID(I1_PS_FID_RRC)
DEFINE_FID(I1_PS_FID_OM)
DEFINE_FID(I1_PS_FID_RM_UL)
DEFINE_FID(I1_PS_FID_RM_DL)
DEFINE_FID(I1_PS_FID_HPA)
DEFINE_FID(I1_PS_FID_IMSA)
DEFINE_FID(I1_PS_FID_IMSVA)
#endif

DEFINE_FID(I2_MAPS_USIMM_FID) /* 提醒! 新增FID请按顺序添加到g_psCommFidModemIdTbl表中 */
DEFINE_FID(I2_MAPS_SI_FID)

#if (3 == MULTI_MODEM_NUMBER) /* three modems */
DEFINE_FID(I2_WUEPS_FID_SLEEP)
DEFINE_FID(I2_WUEPS_FID_AWAKE)
DEFINE_FID(I2_UEPS_FID_RR)
DEFINE_FID(I2_WUEPS_FID_MM)
DEFINE_FID(I2_WUEPS_FID_CM)
DEFINE_FID(I2_WUEPS_FID_RABMSM)
DEFINE_FID(I2_WUEPS_FID_TAF)
DEFINE_FID(I2_UEPS_FID_SL)
DEFINE_FID(I2_UEPS_FID_DC)
DEFINE_FID(I2_UEPS_FID_GRM)
DEFINE_FID(I2_DSP_FID_APM)
DEFINE_FID(I2_DSP_FID_GPHY)
DEFINE_FID(I2_DSP_FID_GPHYLOW)
DEFINE_FID(I2_DSP_FID_SLEEP)
DEFINE_FID(I2_DSP_FID_WAKE)
DEFINE_FID(I2_DSP_FID_IDLE)
DEFINE_FID(I2_UEPS_FID_MSCC)
DEFINE_FID(I2_WUEPS_FID_CSIMA)
DEFINE_FID(I2_PS_FID_CM)
#endif
#endif /* ( FEATURE_MULTI_MODEM == FEATURE_ON ) */

#if (((FEATURE_MULTI_MODEM == FEATURE_ON) || defined(HINGDFE_SEPARATE)) && (defined(GUC_RTT_TEST_ENABLED)))
DEFINE_FID(I1_DSP_FID_RTTSTUB)
DEFINE_FID(I1_DSP_FID_RTTMATCH)
DEFINE_FID(I1_DSP_FID_RTTPREEMPT)
DEFINE_FID(I1_DSP_FID_RTTONLINE)
#endif
DEFINE_FID(I0_MSP_L4_FID) /* 提醒! 新增FID请按顺序添加到g_psCommFidModemIdTbl表中 */
DEFINE_FID(I0_MSP_FTM_FID)
DEFINE_FID(I0_PS_FID_RRC)
DEFINE_FID(I0_PS_FID_CM)
DEFINE_FID(I0_PS_FID_OM)
DEFINE_FID(I0_PS_FID_RM_UL)
DEFINE_FID(I0_PS_FID_RM_DL)
DEFINE_FID(I0_PS_FID_HPA)
DEFINE_FID(I0_PS_FID_IMSA)
DEFINE_FID(I0_PS_FID_IMSVA)

#if (FEATURE_ON == FEATURE_EDA_SUPPORT)
DEFINE_FID(PS_FID_EDA) /* 提醒! 新增FID请按顺序添加到g_psCommFidModemIdTbl表中 */
#endif

#if (!defined(CHIP_BB_HI6210))
DEFINE_FID(TLPHY_FID_RTTAGENT)
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_TDS)
DEFINE_FID(HL1_FID_SNDCMD)
DEFINE_FID(TPS_FID_MAC)
DEFINE_FID(TPS_FID_PDC)
#endif

#if (FEATURE_ON == FEATURE_UE_MODE_CDMA)
DEFINE_FID(UEPS_FID_CAS1X) /* 提醒! 新增FID请按顺序添加到g_psCommFidModemIdTbl表中 */
DEFINE_FID(UEPS_FID_CASHRPD)
DEFINE_FID(MSPS_FID_CTTF_1X_REV)
DEFINE_FID(MSPS_FID_CTTF_1X_FWD)
DEFINE_FID(CTTF_FID_HRPD_REV)
DEFINE_FID(CTTF_FID_HRPD_FWD)
DEFINE_FID(CTTF_FID_HRPD_SIG)
DEFINE_FID(CPROC_FID_1X)
DEFINE_FID(CPROC_FID_HRPD)
DEFINE_FID(CPROC_FID_RM)
DEFINE_FID(UEPS_FID_CNAS)
#endif
#if ((FEATURE_ON == FEATURE_UE_MODE_CDMA) || (FEATURE_ON == FEATURE_PPP))
DEFINE_FID(MSPS_FID_PPPC)
#endif
#if ((FEATURE_ON == FEATURE_CSDR) || (FEATURE_ON == FEATURE_GSM_SDR))
DEFINE_FID(DSP_FID_UPHY_HPA)
#endif
#if (defined(GUC_RTT_TEST_ENABLED))
DEFINE_FID(I0_DSP_FID_RTTSTUB)
DEFINE_FID(I0_DSP_FID_RTTMATCH)
DEFINE_FID(I0_DSP_FID_RTTPREEMPT)
DEFINE_FID(I0_DSP_FID_RTTONLINE)
#endif
#if ((FEATURE_ON == FEATURE_UE_MODE_CDMA) || (FEATURE_ON == FEATURE_GSM_SDR) || (FEATURE_ON == FEATURE_EASYRF))
DEFINE_FID(LINKH_FID_ARM)
#endif

#ifdef HPA_ITT
DEFINE_FID(MAPS_STUB_FID) /* 提醒! 新增FID请按顺序添加到g_psCommFidModemIdTbl表中 */
#endif                    /* HPA_ITT */
#if (FEATURE_ON == FEATURE_BASTET)
DEFINE_FID(UEPS_FID_BASTET)
#endif

#if ((FEATURE_ON == FEATURE_UE_MODE_NR) && (!defined(FEATURE_NPHY_ESLPS)))
DEFINE_FID(PHY_FID_PROCESS)
#endif
#if defined(FEATURE_NPHY_START)
DEFINE_FID(NPHY_FID_STARTUP)
#endif
DEFINE_FID(UEPS_FID_PSUPMEM)

#if (VOS_WIN32 == VOS_OS_VER)
DEFINE_FID(WUEPS_FID_AT)
DEFINE_FID(WUEPS_FID_ITTSTUB)
#if (FEATURE_ON == FEATURE_DATA_SERVICE_NEW_PLATFORM)
DEFINE_FID(ACPU_FID_ADS_UL)
#else
DEFINE_FID(ACPU_FID_ADS_UL)
DEFINE_FID(ACPU_FID_ADS_DL)
#endif
DEFINE_FID(ACPU_FID_CSD)
DEFINE_FID(ACPU_FID_TAFAGENT)
DEFINE_FID(MSP_APP_DS_FID)
DEFINE_FID(ACPU_FID_CBT)
DEFINE_FID(ACPU_FID_USIMM)
DEFINE_FID(ACPU_FID_PAM_OM) /* 提醒! 新增FID请按顺序添加到g_psCommFidModemIdTbl表中 */
#if (FEATURE_MM_TASK_COMBINE != FEATURE_ON)
DEFINE_FID(I0_UEPS_FID_NRMM)
DEFINE_FID(I0_UEPS_FID_NRCM)
DEFINE_FID(I0_UEPS_FID_NREAP)
#endif
#endif

#if (FEATURE_MM_TASK_COMBINE == FEATURE_ON)
DEFINE_FID(I0_UEPS_FID_NRCM)
DEFINE_FID(I0_UEPS_FID_NREAP)
#endif

#if (FEATURE_NRMM_TASK_TYPE == FEATURE_ON)
DEFINE_FID(I0_UEPS_FID_NRMM)
DEFINE_FID(I0_UEPS_FID_NRCM)
DEFINE_FID(I0_UEPS_FID_NREAP)
#endif

#if (FEATURE_ON == FEATURE_PC5_DATA_CHANNEL)
DEFINE_FID(I0_PS_FID_VRRC)
DEFINE_FID(I0_PS_FID_VRM_UL)
DEFINE_FID(I0_PS_FID_VRM_DL)
DEFINE_FID(I0_PS_FID_VHPA)
DEFINE_FID(I0_PS_FID_VNAS)
#endif

END_FID_DEFINITION()
#endif /* (OSA_CPU_CCPU == VOS_OSA_CPU) */

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
BEGIN_FID_DEFINITION()
/*
 * add your FID here, e.g.
 * DEFINE_FID(VOS_FID_APP0)
 * DEFINE_FID(VOS_FID_APPx)
 */
DEFINE_FID(MSP_FID_DIAG_ACPU)
#if (FEATURE_ON == FEATURE_CHR_OM)
DEFINE_FID(MSP_FID_CHR_ACPU)
#endif
DEFINE_FID(MSP_APP_DS_FID)
DEFINE_FID(PS_FID_APP_NDIS_PPP_DIPC)
DEFINE_FID(ACPU_FID_NFEXT)
#if (FEATURE_ON == FEATURE_DATA_SERVICE_NEW_PLATFORM)
DEFINE_FID(ACPU_FID_ADS_UL)
#else
DEFINE_FID(ACPU_FID_ADS_UL)
DEFINE_FID(ACPU_FID_ADS_DL)
#endif
DEFINE_FID(ACPU_FID_TAFAGENT)

#if (FEATURE_ACPU_STAT == FEATURE_ON)
DEFINE_FID(ACPU_FID_CPULOAD)
#endif

#if (FEATURE_ACPU_FC_POINT_REG == FEATURE_ON)
DEFINE_FID(UEPS_FID_FLOWCTRL_A)
#endif

#ifdef CBT_ENABLED
DEFINE_FID(ACPU_FID_CBT)
#endif
DEFINE_FID(ACPU_FID_USIMM)
DEFINE_FID(ACPU_FID_PAM_OM)

#if (FEATURE_VOICE_UP == FEATURE_ON)
DEFINE_FID(ACPU_FID_MED_RT)
DEFINE_FID(ACPU_FID_MED)
#endif

#if (FEATURE_ON == FEATURE_BASTET)
DEFINE_FID(ACPU_FID_BASTET_COMM)
#endif

#if (FEATURE_ON == FEATURE_PPP)
DEFINE_FID(ACPU_FID_PPP)
#endif

#if (defined(CONFIG_HISI_BALONG_EXTRA_MODEM_MBB))
DEFINE_FID(ACPU_FID_VOICE_AGENT)
#endif

#if (FEATURE_PC5_DATA_CHANNEL == FEATURE_ON)
DEFINE_FID(ACPU_FID_PC5_CHQEST)
#endif

#if (FEATURE_HIFI_MANAGE == FEATURE_ON)
DEFINE_FID(ACPU_FID_HIFI_MANAGE)
#endif

END_FID_DEFINITION()
#endif /* (OSA_CPU_ACPU == VOS_OSA_CPU) */

#if (OSA_CPU_NRCPU == VOS_OSA_CPU)
BEGIN_FID_DEFINITION()
/*
 * add your FID here, e.g.
 * DEFINE_FID(VOS_FID_APP0)
 * DEFINE_FID(VOS_FID_APPx)
 */
DEFINE_FID(NRCPU_FID_PAM_OM)
DEFINE_FID(UEPS_FID_NRRC)
/* 5G用户面 NR CCPU */
DEFINE_FID(I0_UEPS_FID_L2MA)
DEFINE_FID(I0_UEPS_FID_NPDCPRLC_UL)
DEFINE_FID(I0_UEPS_FID_NPDCP_DL_NRT)
DEFINE_FID(I0_UEPS_FID_NRLC_DL_NRT)
DEFINE_FID(I0_UEPS_FID_NPDCPRLC_UL_REL)
#if (FEATURE_ON == FEATURE_NPHY_STUB_ESL)
DEFINE_FID(UEPS_FID_NPHY_STUB)
#endif
DEFINE_FID(MSP_FID_DIAG_NRM)
DEFINE_FID(I0_UEPS_FID_NRMM)
DEFINE_FID(NRPHY_FID_STARTUP)
DEFINE_FID(I0_UEPS_FID_NRCM)
#ifdef CBT_ENABLED
DEFINE_FID(NRCBT_FID_NOSIG)
#endif
DEFINE_FID(NRCBT_FID_CBT)
DEFINE_FID(I0_UEPS_FID_NREAP)
DEFINE_FID(I0_UEPS_FID_SDAP)
DEFINE_FID(NRAGENT_FID_AGENT)
DEFINE_FID(DSP_FID_BBA_RFM_AGENT_LPC)
#if (FEATURE_ON == FEATURE_CHR_OM)
DEFINE_FID(MSP_FID_CHR_NRCCPU)
#endif
DEFINE_FID(DSP_FID_RTTSTUB_NR)
END_FID_DEFINITION()
#endif /* (OSA_CPU_NRCPU == VOS_OSA_CPU) */

#if (OSA_CPU_L2HAC == VOS_OSA_CPU)
BEGIN_FID_DEFINITION()
/* 5G用户面 L2HAC */
DEFINE_FID(I0_UEPS_FID_NL2_DECODE)
DEFINE_FID(I0_UEPS_FID_NMACRLC_UL_RT)
DEFINE_FID(MSP_FID_DIAG_HAC)
END_FID_DEFINITION()
#endif /* (OSA_CPU_L2HAC == VOS_OSA_CPU) */

#if (OSA_CPU_CCPU == VOS_OSA_CPU)
#define VOS_PID_DOPRAEND (VOS_PID_CPU_ID_0_DOPRAEND)
#define VOS_PID_BUTT (VOS_CPU_ID_0_PID_BUTT)
#endif

#if (OSA_CPU_ACPU == VOS_OSA_CPU)
#define VOS_PID_DOPRAEND (VOS_PID_CPU_ID_1_DOPRAEND)
#define VOS_PID_BUTT (VOS_CPU_ID_1_PID_BUTT)
#endif

#if (OSA_CPU_NRCPU == VOS_OSA_CPU)
#define VOS_PID_DOPRAEND (VOS_PID_CPU_ID_2_DOPRAEND)
#define VOS_PID_BUTT (VOS_CPU_ID_2_PID_BUTT)
#endif

#if (OSA_CPU_L2HAC == VOS_OSA_CPU)
#define VOS_PID_DOPRAEND (VOS_PID_CPU_ID_16_DOPRAEND)
#define VOS_PID_BUTT (VOS_CPU_ID_16_PID_BUTT)
#endif

/* 为了和MAPS2000兼容, 暂时转定义下述PID */
#define WUEPS_PID_RRCF                  WUEPS_PID_WRR
/* GU NAS PID转定义 */
#define WUEPS_PID_LMM                   PS_PID_MM
#define WUEPS_PID_ESM                   PS_PID_ESM
#define PS_PID_APP                      PS_PID_OM

#if defined(INSTANCE_1)
#define MAPS_USIMM_FID   I1_MAPS_USIMM_FID
#define MAPS_SI_FID      I1_MAPS_SI_FID
#define WUEPS_FID_SLEEP  I1_WUEPS_FID_SLEEP
#define WUEPS_FID_AWAKE  I1_WUEPS_FID_AWAKE
#define UEPS_FID_RR      I1_UEPS_FID_RR
#define WUEPS_FID_MM     I1_WUEPS_FID_MM
#define WUEPS_FID_CM     I1_WUEPS_FID_CM
#define WUEPS_FID_RABMSM I1_WUEPS_FID_RABMSM
#define WUEPS_FID_TAF    I1_WUEPS_FID_TAF
#define UEPS_FID_SL      I1_UEPS_FID_SL
#define UEPS_FID_DC      I1_UEPS_FID_DC
#define UEPS_FID_GRM     I1_UEPS_FID_GRM
#define DSP_FID_APM      I1_DSP_FID_APM
#if (FEATURE_MODEM1_SUPPORT_WCDMA == FEATURE_ON)
#define DSP_FID_WPHY     I1_DSP_FID_WPHY
#else
#define DSP_FID_WPHY     I0_DSP_FID_WPHY
#endif
#define DSP_FID_GPHY     I1_DSP_FID_GPHY
#define DSP_FID_GPHYLOW  I1_DSP_FID_GPHYLOW
#define DSP_FID_SLEEP    I1_DSP_FID_SLEEP
#define DSP_FID_WAKE     I1_DSP_FID_WAKE
#define DSP_FID_IDLE     I1_DSP_FID_IDLE
#define UEPS_FID_MSCC    I1_UEPS_FID_MSCC
#define WUEPS_FID_CSIMA  I1_WUEPS_FID_CSIMA
#define PS_FID_CM        I1_PS_FID_CM
#if (FEATURE_MODEM1_SUPPORT_WCDMA == FEATURE_ON)
#define UEPS_FID_WRR     I1_UEPS_FID_WRR
#define UEPS_FID_RM      I1_UEPS_FID_RM
#define UEPS_FID_RM_DL   I1_UEPS_FID_RM_DL
#define UEPS_FID_PB      I1_UEPS_FID_PB
#endif

#if (FEATURE_MODEM1_SUPPORT_LTE == FEATURE_ON)
#define MSP_L4_FID       I1_MSP_L4_FID
#define MSP_FTM_FID      I1_MSP_FTM_FID
#define PS_FID_RRC       I1_PS_FID_RRC
#define PS_FID_OM        I1_PS_FID_OM
#define PS_FID_HPA       I1_PS_FID_HPA
#define PS_FID_RM_UL     I1_PS_FID_RM_UL
#define PS_FID_RM_DL     I1_PS_FID_RM_DL
#define PS_FID_IMSA      I1_PS_FID_IMSA
#define PS_FID_IMSVA     I1_PS_FID_IMSVA
#else
#define MSP_L4_FID       I0_MSP_L4_FID
#define MSP_FTM_FID      I0_MSP_FTM_FID
#endif
#if (((FEATURE_MULTI_MODEM == FEATURE_ON) || defined(HINGDFE_SEPARATE)) && (defined(GUC_RTT_TEST_ENABLED)))
#define DSP_FID_RTTSTUB     I1_DSP_FID_RTTSTUB
#define DSP_FID_RTTMATCH    I1_DSP_FID_RTTMATCH
#define DSP_FID_RTTPREEMPT  I1_DSP_FID_RTTPREEMPT
#define DSP_FID_RTTONLINE   I1_DSP_FID_RTTONLINE
#endif
#elif defined(INSTANCE_2)
#define MAPS_USIMM_FID   I2_MAPS_USIMM_FID
#define MAPS_SI_FID      I2_MAPS_SI_FID
#define WUEPS_FID_SLEEP  I2_WUEPS_FID_SLEEP
#define WUEPS_FID_AWAKE  I2_WUEPS_FID_AWAKE
#define UEPS_FID_RR      I2_UEPS_FID_RR
#define WUEPS_FID_MM     I2_WUEPS_FID_MM
#define WUEPS_FID_CM     I2_WUEPS_FID_CM
#define WUEPS_FID_RABMSM I2_WUEPS_FID_RABMSM
#define WUEPS_FID_TAF    I2_WUEPS_FID_TAF
#define UEPS_FID_SL      I2_UEPS_FID_SL
#define UEPS_FID_DC      I2_UEPS_FID_DC
#define UEPS_FID_GRM     I2_UEPS_FID_GRM
#define DSP_FID_APM      I2_DSP_FID_APM
#define DSP_FID_GPHY     I2_DSP_FID_GPHY
#define DSP_FID_GPHYLOW  I2_DSP_FID_GPHYLOW
#define DSP_FID_SLEEP    I2_DSP_FID_SLEEP
#define DSP_FID_WAKE     I2_DSP_FID_WAKE
#define DSP_FID_IDLE     I2_DSP_FID_IDLE
#define UEPS_FID_MSCC    I2_UEPS_FID_MSCC
#define WUEPS_FID_CSIMA  I2_WUEPS_FID_CSIMA
#define PS_FID_CM        I2_PS_FID_CM
#define DSP_FID_WPHY     I0_DSP_FID_WPHY
#define MSP_L4_FID       I0_MSP_L4_FID
#define MSP_FTM_FID      I0_MSP_FTM_FID
#else
#define MAPS_USIMM_FID   I0_MAPS_USIMM_FID
#define MAPS_SI_FID      I0_MAPS_SI_FID
#define WUEPS_FID_SLEEP  I0_WUEPS_FID_SLEEP
#define WUEPS_FID_AWAKE  I0_WUEPS_FID_AWAKE
#define UEPS_FID_RR      I0_UEPS_FID_RR
#define UEPS_FID_WRR     I0_UEPS_FID_WRR
#define UEPS_FID_RM      I0_UEPS_FID_RM
#define UEPS_FID_RM_DL   I0_UEPS_FID_RM_DL
#define UEPS_FID_PB      I0_UEPS_FID_PB
#define WUEPS_FID_MM     I0_WUEPS_FID_MM
#define WUEPS_FID_CM     I0_WUEPS_FID_CM
#define WUEPS_FID_RABMSM I0_WUEPS_FID_RABMSM
#define WUEPS_FID_TAF    I0_WUEPS_FID_TAF
#define UEPS_FID_SL      I0_UEPS_FID_SL
#define UEPS_FID_DC      I0_UEPS_FID_DC
#define UEPS_FID_GRM     I0_UEPS_FID_GRM
#define DSP_FID_APM      I0_DSP_FID_APM
#define DSP_FID_WPHY     I0_DSP_FID_WPHY
#define DSP_FID_GPHY     I0_DSP_FID_GPHY
#define DSP_FID_GPHYLOW  I0_DSP_FID_GPHYLOW
#define DSP_FID_SLEEP    I0_DSP_FID_SLEEP
#define DSP_FID_WAKE     I0_DSP_FID_WAKE
#define DSP_FID_IDLE     I0_DSP_FID_IDLE
#define UEPS_FID_MSCC    I0_UEPS_FID_MSCC
#define WUEPS_FID_CSIMA  I0_WUEPS_FID_CSIMA
#define PS_FID_CM        I0_PS_FID_CM
#define MSP_L4_FID       I0_MSP_L4_FID
#define MSP_FTM_FID      I0_MSP_FTM_FID
#define PS_FID_RRC       I0_PS_FID_RRC
#define PS_FID_OM        I0_PS_FID_OM
#define PS_FID_HPA       I0_PS_FID_HPA
#define PS_FID_RM_UL     I0_PS_FID_RM_UL
#define PS_FID_RM_DL     I0_PS_FID_RM_DL
#define PS_FID_IMSA      I0_PS_FID_IMSA
#define PS_FID_IMSVA     I0_PS_FID_IMSVA
#if (defined(GUC_RTT_TEST_ENABLED))
#define DSP_FID_RTTSTUB     I0_DSP_FID_RTTSTUB
#define DSP_FID_RTTMATCH    I0_DSP_FID_RTTMATCH
#define DSP_FID_RTTPREEMPT  I0_DSP_FID_RTTPREEMPT
#define DSP_FID_RTTONLINE   I0_DSP_FID_RTTONLINE
#endif
#endif

#if (VOS_WIN32 == VOS_OS_VER)
extern VOS_UINT32 WuepsFidITTStubInit(enum VOS_InitPhaseDefine ip);
#endif

/*lint +e778*/

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */

#endif /* _V_ID_H */
