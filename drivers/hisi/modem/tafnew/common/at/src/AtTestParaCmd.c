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

#include "AtCheckFunc.h"
#include "ATCmdProc.h"
#include "AtDataProc.h"
#include "taf_tafm_remote.h"
#include "AtTestParaCmd.h"

#include "taf_app_mma.h"
#include "securec.h"


#define THIS_FILE_ID PS_FILE_ID_AT_TESTPARACMD_C


VOS_UINT32 At_TestCgdcont(VOS_UINT8 indexNum)
{
    VOS_UINT16 length = 0;

#if (FEATURE_LTE == FEATURE_ON)
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
    length += (VOS_UINT16)At_sprintf(
        AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
        "%s: (0-31),\"IP\",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1),,,,,,(0-2),,(0,1),(0,1),(0,1),(0,1)\r\n",
        g_parseContext[indexNum].cmdElement->cmdName);

    if (AT_GetIpv6Capability() != AT_IPV6_CAPABILITY_IPV4_ONLY) {
        length += (TAF_UINT16)At_sprintf(
            AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress, (TAF_CHAR *)g_atSndCodeAddress + length,
            "%s: (0-31),\"IPV6\",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1),,,,,,(0-2),,(0,1),(0,1),(0,1),(0,1)\r\n",
            g_parseContext[indexNum].cmdElement->cmdName);
        length += (TAF_UINT16)At_sprintf(
            AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress, (TAF_CHAR *)g_atSndCodeAddress + length,
            "%s: (0-31),\"IPV4V6\",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1),,,,,,(0-2),,(0,1),(0,1),(0,1),(0,1)\r\n",
            g_parseContext[indexNum].cmdElement->cmdName);
    }

    length += (VOS_UINT16)At_sprintf(
        AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress + length,
        "%s: (0-31),\"PPP\",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1),,,,,,(0-2),,(0,1),(0,1),(0,1),(0,1)",
        g_parseContext[indexNum].cmdElement->cmdName);
#else
    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                     "%s: (0-31),\"IP\",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1)\r\n",
                                     g_parseContext[indexNum].cmdElement->cmdName);

    if (AT_GetIpv6Capability() != AT_IPV6_CAPABILITY_IPV4_ONLY) {
        length += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                         (TAF_CHAR *)g_atSndCodeAddress + length,
                                         "%s: (0-31),\"IPV6\",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1)\r\n",
                                         g_parseContext[indexNum].cmdElement->cmdName);
        length += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                         (TAF_CHAR *)g_atSndCodeAddress + length,
                                         "%s: (0-31),\"IPV4V6\",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1)\r\n",
                                         g_parseContext[indexNum].cmdElement->cmdName);
    }

    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                     (VOS_CHAR *)g_atSndCodeAddress + length,
                                     "%s: (0-31),\"PPP\",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1)",
                                     g_parseContext[indexNum].cmdElement->cmdName);
#endif
#else
    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                     "%s: (1-11),\"IP\",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1)\r\n",
                                     g_parseContext[indexNum].cmdElement->cmdName);

    if (AT_GetIpv6Capability() != AT_IPV6_CAPABILITY_IPV4_ONLY) {
        length += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                         (TAF_CHAR *)g_atSndCodeAddress + length,
                                         "%s: (1-11),\"IPV6\",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1)\r\n",
                                         g_parseContext[indexNum].cmdElement->cmdName);
        length += (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                         (TAF_CHAR *)g_atSndCodeAddress + length,
                                         "%s: (1-11),\"IPV4V6\",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1)\r\n",
                                         g_parseContext[indexNum].cmdElement->cmdName);
    }

    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                     (VOS_CHAR *)g_atSndCodeAddress + length,
                                     "%s: (1-11),\"PPP\",,,(0-2),(0-3),(0,1),(0,1),(0-2),(0,1),(0,1)",
                                     g_parseContext[indexNum].cmdElement->cmdName);

#endif

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 At_TestCgdscont(VOS_UINT8 indexNum)
{
    VOS_UINT16 length = 0;

#if (FEATURE_LTE == FEATURE_ON)
    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                     "%s: (1-31),(0-31),(0-2),(0-3),(0-1)",
                                     g_parseContext[indexNum].cmdElement->cmdName);
#else
    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                     "%s: (1-11),(0-11),(0-2),(0-3),(0-1)",
                                     g_parseContext[indexNum].cmdElement->cmdName);

#endif

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 At_TestCgtft(VOS_UINT8 indexNum)
{
    VOS_UINT16 length = 0;

    if (AT_IsSupportReleaseRst(AT_ACCESS_STRATUM_REL11)) {
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
        length += (VOS_UINT16)At_sprintf(
            AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
            "%s: \"IP\",(1-16),(0-255),,(0-255),,,(00000000-FFFFFFFF),,(00000-FFFFF),(0-3),,(0-255)\r\n",
            g_parseContext[indexNum].cmdElement->cmdName);
#else
        length +=
            (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                   "%s: \"IP\",(1-16),(0-255),,(0-255),,,(00000000-FFFFFFFF),,(00000-FFFFF),(0-3),\r\n",
                                   g_parseContext[indexNum].cmdElement->cmdName);
#endif
    } else {
        length +=
            (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                   "%s: \"IP\",(1-16),(0-255),,(0-255),,,(00000000-FFFFFFFF),,(00000-FFFFF),(0-3)\r\n",
                                   g_parseContext[indexNum].cmdElement->cmdName);
    }

    if (AT_GetIpv6Capability() != AT_IPV6_CAPABILITY_IPV4_ONLY) {
        if (AT_IsSupportReleaseRst(AT_ACCESS_STRATUM_REL11)) {
#if (FEATURE_UE_MODE_NR == FEATURE_ON)
            length += (TAF_UINT16)At_sprintf(
                AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress, (TAF_CHAR *)g_atSndCodeAddress + length,
                "%s: \"IPV6\",(1-16),(0-255),,(0-255),,,(00000000-FFFFFFFF),,(00000-FFFFF),(0-3),,(0-255)\r\n",
                g_parseContext[indexNum].cmdElement->cmdName);
            length += (TAF_UINT16)At_sprintf(
                AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress, (TAF_CHAR *)g_atSndCodeAddress + length,
                "%s: \"IPV4V6\",(1-16),(0-255),,(0-255),,,(00000000-FFFFFFFF),,(00000-FFFFF),(0-3),,(0-255)",
                g_parseContext[indexNum].cmdElement->cmdName);
#else
            length += (TAF_UINT16)At_sprintf(
                AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress, (TAF_CHAR *)g_atSndCodeAddress + length,
                "%s: \"IPV6\",(1-16),(0-255),,(0-255),,,(00000000-FFFFFFFF),,(00000-FFFFF),(0-3),\r\n",
                g_parseContext[indexNum].cmdElement->cmdName);
            length += (TAF_UINT16)At_sprintf(
                AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress, (TAF_CHAR *)g_atSndCodeAddress + length,
                "%s: \"IPV4V6\",(1-16),(0-255),,(0-255),,,(00000000-FFFFFFFF),,(00000-FFFFF),(0-3),",
                g_parseContext[indexNum].cmdElement->cmdName);
#endif
        } else {
            length += (TAF_UINT16)At_sprintf(
                AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress, (TAF_CHAR *)g_atSndCodeAddress + length,
                "%s: \"IPV6\",(1-16),(0-255),,(0-255),,,(00000000-FFFFFFFF),,(00000-FFFFF),(0-3)\r\n",
                g_parseContext[indexNum].cmdElement->cmdName);
            length += (TAF_UINT16)At_sprintf(
                AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress, (TAF_CHAR *)g_atSndCodeAddress + length,
                "%s: \"IPV4V6\",(1-16),(0-255),,(0-255),,,(00000000-FFFFFFFF),,(00000-FFFFF),(0-3)",
                g_parseContext[indexNum].cmdElement->cmdName);
        }
    }

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 At_TestCgact(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                    "%s: (0,1)", g_parseContext[indexNum].cmdElement->cmdName);
    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 At_TestCgcmod(VOS_UINT8 indexNum)
{

    /* CGEQNEG的测试命令和CGCMODE的返回值相同，使用相同的函数 */
    return At_TestCgeqnegPara(indexNum);

}


VOS_UINT32 At_TestCgpaddr(VOS_UINT8 indexNum)
{
    /* 执行命令操作 */
    if (TAF_PS_GetPdpContextInfo(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0) != VOS_OK) {
        return AT_ERROR;
    }

    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGPADDR_TEST;

    return AT_WAIT_ASYNC_RETURN;
}


VOS_UINT32 At_TestCgautoPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = 0;

    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                     (VOS_CHAR *)g_atSndCodeAddress + length, "%s: (0,1)",
                                     g_parseContext[indexNum].cmdElement->cmdName);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 At_TestAuhtdata(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

#if (FEATURE_LTE == FEATURE_ON)
    length = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                    "%s: (0-31),(0-2),,", g_parseContext[indexNum].cmdElement->cmdName);
#else
    length = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                    "%s: (1-11),(0-2),,", g_parseContext[indexNum].cmdElement->cmdName);
#endif

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 At_TestNdisDup(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = 0;

#if (FEATURE_LTE == FEATURE_ON)
    length = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                    "%s: (1-20),(0-1)", g_parseContext[indexNum].cmdElement->cmdName);
#else
    length = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                    "%s: (1-16),(0-1)", g_parseContext[indexNum].cmdElement->cmdName);
#endif

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 At_TestCgeqos(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                    "%s: (1-31),(0-9),(0-10485760),(0-10485760),(0-10485760),(0-10485760)",
                                    g_parseContext[indexNum].cmdElement->cmdName);
    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

#if (FEATURE_UE_MODE_NR == FEATURE_ON)

VOS_UINT32 At_TestC5gqos(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                    "%s: (1-31),(0-254),(0-4294967295),(0-4294967295),(0-4294967295),(0-4294967295)",
                                    g_parseContext[indexNum].cmdElement->cmdName);
    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 At_TestC5gQosRdp(VOS_UINT8 indexNum)
{
    return At_TestCgeqnegPara(indexNum);
}


VOS_UINT32 At_TestC5gPNssai(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                    "%s: (0-159),", g_parseContext[indexNum].cmdElement->cmdName);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 At_TestC5gNssai(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                    "%s: (0-319),", g_parseContext[indexNum].cmdElement->cmdName);
    g_atSendDataBuff.bufLen = length;
    return AT_OK;
}


VOS_UINT32 At_TestC5gNssaiRdp(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                    "%s: (0,1,2,3),", g_parseContext[indexNum].cmdElement->cmdName);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

#endif

VOS_UINT32 At_TestCgeqosrdp(VOS_UINT8 indexNum)
{
    return At_TestCgeqnegPara(indexNum);
}


VOS_UINT32 At_TestCgcontrdp(VOS_UINT8 indexNum)
{
    return At_TestCgeqnegPara(indexNum);
}


VOS_UINT32 At_TestCgscontrdp(VOS_UINT8 indexNum)
{
    return At_TestCgeqnegPara(indexNum);
}


VOS_UINT32 At_TestCgtftrdp(VOS_UINT8 indexNum)
{
    return At_TestCgeqnegPara(indexNum);
}


VOS_UINT32 AT_TestChrgEnablePara(VOS_UINT8 indexNum)
{
    VOS_INT32 chargeEnable;

    chargeEnable = AT_GetMiscChargeState(); /* BSP_TBAT_CHRStGet()) */
    /* 只有TRUE/FLASE的返回值是有效的 */
    if ((chargeEnable == TRUE) || (chargeEnable == FALSE)) {
        g_atSendDataBuff.bufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                                         (TAF_CHAR *)g_atSndCodeAddress, "%s:%d", "^TCHRENABLE",
                                                         chargeEnable);
        return AT_OK;
    } else {
        return AT_ERROR;
    }
}


VOS_UINT32 AT_TestCmutPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length = 0;

    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                     (VOS_CHAR *)g_atSndCodeAddress + length, "%s: (0-1)",
                                     g_parseContext[indexNum].cmdElement->cmdName);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}

/* 如下函数并非此项目中新增, 只是从AtExtendCmd.c文件中移动到此文件而已 */


VOS_UINT32 At_TestVtsPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = 0;

    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                     (VOS_CHAR *)g_atSndCodeAddress + length, "+VTS: (0-9,A-D,*,#)");
    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 At_TestChldPara(VOS_UINT8 indexNum)
{
    g_atSendDataBuff.bufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                                     (VOS_CHAR *)g_atSndCodeAddress, "%s: (0,1,1x,2,2x,3,4,5)",
                                                     g_parseContext[indexNum].cmdElement->cmdName);

    return AT_OK;
}


VOS_UINT32 At_TestCuus1Para(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = 0;

    length = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                    (VOS_CHAR *)g_atSndCodeAddress + length, "+CUUS1:(0,1),(0,1),(0-6),(0-4),(0-3)");

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 At_TestCcwaPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = 0;

    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                     (VOS_CHAR *)g_atSndCodeAddress + length, "%s: (0,1)",
                                     g_parseContext[indexNum].cmdElement->cmdName);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 At_TestCcfcPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = 0;

    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                     (VOS_CHAR *)g_atSndCodeAddress + length, "%s: (0-5)",
                                     g_parseContext[indexNum].cmdElement->cmdName);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 At_TestCusdPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = 0;

    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                     (VOS_CHAR *)g_atSndCodeAddress + length, "%s: (0-2)",
                                     g_parseContext[indexNum].cmdElement->cmdName);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 At_TestCpwdPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = 0;

    length += (VOS_UINT16)At_sprintf(
        AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress + length,
        "%s: (\"P2\",8),(\"SC\",8),(\"AO\",4),(\"OI\",4),(\"OX\",4),(\"AI\",4),(\"IR\",4),(\"AB\",4),(\"AG\",4),(\"AC\",4)",
        g_parseContext[indexNum].cmdElement->cmdName);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 At_TestClckPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = 0;

    length += (VOS_UINT16)At_sprintf(
        AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress + length,
        "%s: (\"SC\",\"AO\",\"OI\",\"OX\",\"AI\",\"IR\",\"AB\",\"AG\",\"AC\",\"FD\",\"PN\",\"PU\",\"PP\")",
        g_parseContext[indexNum].cmdElement->cmdName);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 AT_TestCpbsPara(VOS_UINT8 indexNum)
{
    if (SI_PB_Query(g_atClientTab[indexNum].clientId, 1) == TAF_SUCCESS) {
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}


VOS_UINT32 AT_TestCpbrPara(VOS_UINT8 indexNum)
{
    if (SI_PB_Query(g_atClientTab[indexNum].clientId, 0) == TAF_SUCCESS) {
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}

VOS_UINT32 AT_TestCnmaPara(VOS_UINT8 indexNum)
{
    VOS_INT         bufLen;
    AT_ModemSmsCtx *smsCtx = VOS_NULL_PTR;

    smsCtx = AT_GetModemSmsCtxAddrFromClientId(indexNum);

    if (smsCtx->cmgfMsgFormat == AT_CMGF_MSG_FORMAT_PDU) { /* TEXT方式参数检查 */
        bufLen = snprintf_s((TAF_CHAR *)g_atSndCodeAddress, AT_CMD_MAX_LEN + 20 - 3, (AT_CMD_MAX_LEN + 20 - 3) - 1,
                            "%s: (0-2)", g_parseContext[indexNum].cmdElement->cmdName);
        if (bufLen < 0) {
            return AT_ERROR;
        }

        g_atSendDataBuff.bufLen = (VOS_UINT16)bufLen;
    } else {
        g_atSendDataBuff.bufLen = 0;
    }

    return AT_OK;
}


VOS_UINT32 AT_TestCpasPara(VOS_UINT8 indexNum)
{
    g_atSendDataBuff.bufLen = (TAF_UINT16)At_sprintf(AT_CMD_MAX_LEN, (TAF_CHAR *)g_atSndCodeAddress,
                                                     (TAF_CHAR *)g_atSndCodeAddress, "%s: (0-5)",
                                                     g_parseContext[indexNum].cmdElement->cmdName);

    return AT_OK;
}
#if ((FEATURE_GCBS == FEATURE_ON) || (FEATURE_WCBS == FEATURE_ON))

VOS_UINT32 AT_TestCscbPara(VOS_UINT8 indexNum)
{
    VOS_INT bufLen;

    bufLen = snprintf_s((VOS_CHAR *)g_atSndCodeAddress,
                        AT_CMD_MAX_LEN + AT_CMD_PADDING_LEN - AT_SEND_DATA_BUFF_OFFSET,
                        (AT_CMD_MAX_LEN + AT_CMD_PADDING_LEN - AT_SEND_DATA_BUFF_OFFSET) - 1,
                        "%s: (0,1)", g_parseContext[indexNum].cmdElement->cmdName);
    if (bufLen < 0) {
        return AT_ERROR;
    }

    g_atSendDataBuff.bufLen = (VOS_UINT16)bufLen;

    return AT_OK;
}
#endif


VOS_UINT32 AT_TestCpmsPara(VOS_UINT8 indexNum)
{
    VOS_INT         length;
    AT_ModemSmsCtx *smsCtx = VOS_NULL_PTR;

    smsCtx = AT_GetModemSmsCtxAddrFromClientId(indexNum);

    if (smsCtx->msgMeStorageStatus == MN_MSG_ME_STORAGE_ENABLE) {
        length = snprintf_s((VOS_CHAR *)g_atSndCodeAddress,
                            AT_CMD_MAX_LEN + AT_CMD_PADDING_LEN - AT_SEND_DATA_BUFF_OFFSET,
                            (AT_CMD_MAX_LEN + AT_CMD_PADDING_LEN - AT_SEND_DATA_BUFF_OFFSET) - 1,
                            "%s: (\"SM\",\"ME\"),(\"SM\",\"ME\"),(\"SM\",\"ME\")",
                            g_parseContext[indexNum].cmdElement->cmdName);
    } else {
        length = snprintf_s((VOS_CHAR *)g_atSndCodeAddress,
                            AT_CMD_MAX_LEN + AT_CMD_PADDING_LEN - AT_SEND_DATA_BUFF_OFFSET,
                            (AT_CMD_MAX_LEN + AT_CMD_PADDING_LEN - AT_SEND_DATA_BUFF_OFFSET) - 1,
                            "%s: (\"SM\"),(\"SM\"),(\"SM\")", g_parseContext[indexNum].cmdElement->cmdName);
    }

    if (length < 0) {
        return AT_ERROR;
    }

    g_atSendDataBuff.bufLen = (VOS_UINT16)length;

    return AT_OK;
}

#if (FEATURE_CSG == FEATURE_ON)

VOS_UINT32 AT_TestCsgIdSearchPara(VOS_UINT8 indexNum)
{
    /* 核间通信最大缓存4K,需要分段查询分段上报结果 */
    TAF_MMA_PlmnListPara plmnListPara;

    plmnListPara.qryNum    = TAF_MMA_MAX_CSG_ID_LIST_NUM;
    plmnListPara.currIndex = 0;

    if (TAF_MMA_CsgListSearchReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &plmnListPara) == VOS_TRUE) {
        /* 设置当前操作类型 */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CSG_LIST_SEARCH;

        /* 返回命令处理挂起状态 */
        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}
#endif


VOS_UINT32 At_TestCopsPara(TAF_UINT8 indexNum)
{
    TAF_MMA_PlmnListPara plmnListPara;

    plmnListPara.qryNum    = TAF_MMA_MAX_PLMN_NAME_LIST_NUM;
    plmnListPara.currIndex = 0;

#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)
    if (At_CheckCurrRatModeIsCL((VOS_UINT8)(g_atClientTab[indexNum].clientId)) == VOS_TRUE) {
        return AT_CME_OPERATION_NOT_ALLOWED_IN_CL_MODE;
    }
#endif

    if (Taf_PhonePlmnList(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, &plmnListPara) == VOS_TRUE) {
        /* 设置当前操作类型 */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_COPS_TEST;

        /* 返回命令处理挂起状态 */
        return AT_WAIT_ASYNC_RETURN;
    } else {
        return AT_ERROR;
    }
}


VOS_UINT32 At_TestCpolPara(VOS_UINT8 indexNum)
{
    VOS_UINT32 rst;

    AT_ModemNetCtx *netCtx = VOS_NULL_PTR;

    /* 参数检查 */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_TEST_CMD) {
        return AT_ERROR;
    }

    netCtx = AT_GetModemNetCtxAddrFromClientId(indexNum);

    rst = TAF_MMA_TestCpolReq(WUEPS_PID_AT, g_atClientTab[indexNum].clientId, 0, netCtx->prefPlmnType);

    if (rst == VOS_TRUE) {
        /* 设置AT模块实体的状态为等待异步返回 */
        g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CPOL_TEST;

        return AT_WAIT_ASYNC_RETURN;
    }

    return AT_ERROR;
}


VOS_UINT32 At_TestCgeqnegPara(VOS_UINT8 indexNum)
{
    /*
     * CGEQNEG的测试命令和CGACT的查询命令的功能类似,都要获取当前各CID的激活
     * 情况,向TAF查询当前各CID的激活情况
     */
    if (TAF_PS_GetPdpContextState(WUEPS_PID_AT, AT_PS_BuildExClientId(g_atClientTab[indexNum].clientId), 0) != VOS_OK) {
        return AT_ERROR;
    }

    /* 设置当前操作类型 */
    g_atClientTab[indexNum].cmdCurrentOpt = AT_CMD_CGEQNEG_TEST;
    return AT_WAIT_ASYNC_RETURN; /* 返回命令处理挂起状态 */

    /* 接下来到TAF事件上报的地方,获取当前哪些CID被激活,并打印当前哪些CID被激活. */

}


VOS_UINT32 At_TestCgeqreqPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                    "%s: \"IP\",%s%s", g_parseContext[indexNum].cmdElement->cmdName,
                                    CGEQREQ_TEST_CMD_PARA_STRING, g_atCrLf);

    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                     (VOS_CHAR *)g_atSndCodeAddress + length, "%s: \"IPV6\",%s%s",
                                     g_parseContext[indexNum].cmdElement->cmdName, CGEQREQ_TEST_CMD_PARA_STRING,
                                     g_atCrLf);


    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                     (VOS_CHAR *)g_atSndCodeAddress + length, "%s: \"IPV4V6\",%s",
                                     g_parseContext[indexNum].cmdElement->cmdName, CGEQREQ_TEST_CMD_PARA_STRING);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 At_TestCgansPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = 0;

    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                     (VOS_CHAR *)g_atSndCodeAddress + length, "%s: (0,1),(\"NULL\")",
                                     g_parseContext[indexNum].cmdElement->cmdName);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 At_TestCgansExtPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = 0;

    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                     (VOS_CHAR *)g_atSndCodeAddress + length, "%s: (0,1)",
                                     g_parseContext[indexNum].cmdElement->cmdName);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 At_TestCgdnsPara(VOS_UINT8 indexNum)
{

    /* CGDNS的测试命令和CGCMODE的返回值相同，使用相同的函数 */
    return At_TestCgeqnegPara(indexNum);

}


VOS_UINT32 At_TestCgdataPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = 0;

    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                     (VOS_CHAR *)g_atSndCodeAddress + length, "%s: (\"NULL\")",
                                     g_parseContext[indexNum].cmdElement->cmdName);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}


VOS_UINT32 AT_TestCbgPara(VOS_UINT8 indexNum)
{
    g_atSendDataBuff.bufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                                     (VOS_CHAR *)g_atSndCodeAddress, "%s: (0,1)",
                                                     g_parseContext[indexNum].cmdElement->cmdName);

    return AT_OK;
}


VOS_UINT32 AT_TestNCellMonitorPara(VOS_UINT8 indexNum)
{
    g_atSendDataBuff.bufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                                     (VOS_CHAR *)g_atSndCodeAddress, "%s: (0,1)",
                                                     g_parseContext[indexNum].cmdElement->cmdName);

    return AT_OK;
}


VOS_UINT32 AT_TestRefclkfreqPara(VOS_UINT8 indexNum)
{
    /* 通道检查 */
    if (AT_IsApPort(indexNum) == VOS_FALSE) {
        return AT_ERROR;
    }

    /* 打印输出^REFCLKFREQ支持的参数设置范围 */
    g_atSendDataBuff.bufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                                     (VOS_CHAR *)g_atSndCodeAddress, "%s: (0,1)",
                                                     g_parseContext[indexNum].cmdElement->cmdName);

    return AT_OK;
}

#if (FEATURE_IMS == FEATURE_ON)

VOS_UINT32 AT_TestCiregPara(VOS_UINT8 indexNum)
{
    g_atSendDataBuff.bufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                                     (VOS_CHAR *)g_atSndCodeAddress, "%s: (0-2)",
                                                     g_parseContext[indexNum].cmdElement->cmdName);

    return AT_OK;
}


VOS_UINT32 AT_TestCirepPara(VOS_UINT8 indexNum)
{
    g_atSendDataBuff.bufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                                     (VOS_CHAR *)g_atSndCodeAddress, "%s: (0,1)",
                                                     g_parseContext[indexNum].cmdElement->cmdName);

    return AT_OK;
}
#endif

#if (FEATURE_AT_HSUART == FEATURE_ON)

VOS_UINT32 AT_TestIprPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = (VOS_UINT16)At_sprintf(
        AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
        "%s: (0,300,600,1200,2400,4800,9600,19200,38400,57600,115200,230400,460800,921600,2764800,4000000)",
        g_parseContext[indexNum].cmdElement->cmdName);

    g_atSendDataBuff.bufLen = length;
    return AT_OK;
}


VOS_UINT32 AT_TestIcfPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                    "%s: (1-6),(0-1)", g_parseContext[indexNum].cmdElement->cmdName);

    g_atSendDataBuff.bufLen = length;
    return AT_OK;
}


VOS_UINT32 AT_TestIfcPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length;

    length = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress, (VOS_CHAR *)g_atSndCodeAddress,
                                    "%s: (0,2),(0,2)", g_parseContext[indexNum].cmdElement->cmdName);

    g_atSendDataBuff.bufLen = length;
    return AT_OK;
}
#endif


VOS_UINT32 AT_TestUECenterPara(VOS_UINT8 indexNum)
{
    g_atSendDataBuff.bufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                                     (VOS_CHAR *)g_atSndCodeAddress, "%s: (0,1)",
                                                     g_parseContext[indexNum].cmdElement->cmdName);

    return AT_OK;
}


VOS_UINT32 At_TestFclass(VOS_UINT8 indexNum)
{
    AT_WARN_LOG("At_TestFclass: Not support this command!");
    (VOS_VOID)indexNum;
    return AT_ERROR;
}


VOS_UINT32 At_TestGci(VOS_UINT8 indexNum)
{
    AT_WARN_LOG("At_TestGci: Not support this command!");
    (VOS_VOID)indexNum;
    return AT_ERROR;
}



VOS_UINT32 AT_TestMipPara(VOS_UINT8 indexNum)
{
    g_atSendDataBuff.bufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                                     (VOS_CHAR *)g_atSndCodeAddress, "%s: (0-2)",
                                                     g_parseContext[indexNum].cmdElement->cmdName);

    return AT_OK;
}


VOS_UINT32 At_TestCesqPara(VOS_UINT8 indexNum)
{
    VOS_UINT16 length = 0;

    length += (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                     (VOS_CHAR *)g_atSndCodeAddress + length,
                                     "%s: (0-63,99),(0-7,99),(0-96,255),(0-49,255),(0-34,255),(0-97,255)",
                                     g_parseContext[indexNum].cmdElement->cmdName);

    g_atSendDataBuff.bufLen = length;

    return AT_OK;
}
#if (FEATURE_UE_MODE_CDMA == FEATURE_ON)

VOS_UINT32 AT_TestClDbDomainStatusPara(VOS_UINT8 indexNum)
{
    /* 参数检查 */
    if (g_atParseCmd.cmdOptType != AT_CMD_OPT_TEST_CMD) {
        return AT_ERROR;
    }

    g_atSendDataBuff.bufLen = (VOS_UINT16)At_sprintf(AT_CMD_MAX_LEN, (VOS_CHAR *)g_atSndCodeAddress,
                                                     (VOS_CHAR *)g_atSndCodeAddress, "%s: (0-1),(0-1),(0-1)",
                                                     g_parseContext[indexNum].cmdElement->cmdName);

    return AT_OK;
}
#endif
