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

#include "ATCmdProc.h"
#include "AtCheckFunc.h"
#include "AtParseCmd.h"
#include "at_common.h"


/*
 * 协议栈打印打点方式下的.C文件宏定义
 */
#define THIS_FILE_ID PS_FILE_ID_AT_PARSECMD_C

/*
 * 3 函数、变量声明
 */


AT_StateType atFindNextSubState(AT_SubState *subStateTab, VOS_UINT8 inputChar)
{
    VOS_UINT16 tabIndex = 0; /* 子状态表索引 */

    /* 依次比较子状态的每一项直至结束 */
    while (subStateTab[tabIndex].nextState != AT_BUTT_STATE) {
        if (subStateTab[tabIndex].funcName(inputChar) == AT_SUCCESS) { /* 判断输入字符是否匹配 */
            return subStateTab[tabIndex].nextState;                    /* 返回匹配的子状态 */
        }
        tabIndex++; /* 子状态表索引递增 */
    }
    return AT_BUTT_STATE;
}


AT_StateType atFindNextMainState(AT_MainState *mainStateTab, VOS_UINT8 inputChar, AT_StateType inputState)
{
    VOS_UINT16 tabIndex = 0; /* 子状态表索引 */

    /* 依次比较主状态的每一项直至结束 */
    while (mainStateTab[tabIndex].currState != AT_BUTT_STATE) {
        if (inputState == mainStateTab[tabIndex].currState) { /* 判断输入状态是否匹配 */
            /* 如果状态匹配,则根据输入字符寻找下一个子状态 */
            return atFindNextSubState(mainStateTab[tabIndex].subStateTab, inputChar);
        }
        tabIndex++;
    }
    return AT_BUTT_STATE;
}


TAF_UINT32 At_Auc2ul(TAF_UINT8 *nptr, TAF_UINT16 len, TAF_UINT32 *rtn)
{
    TAF_UINT32 c;          /* current Char */
    TAF_UINT32 total  = 0; /* current total */
    TAF_UINT32 length = 0; /* current Length */

    c = (TAF_UINT32)*nptr++;

    while (length++ < len) {
        if ((c >= '0') && (c <= '9')) { /* 字符检查 */
            /* 0xFFFFFFFF 等于 4294967295 */
            if (((total == 429496729) && (c > '5')) || (total > 429496729)) {
                return AT_FAILURE;
            }
            total = (10 * total) + (c - '0');       /* accumulate digit */
            c     = (TAF_UINT32)(TAF_UINT8)*nptr++; /* get next Char */
        } else {
            return AT_FAILURE;
        }
    }

    *rtn = total; /* return result, negated if necessary */
    return AT_SUCCESS;
}

TAF_UINT32 At_String2Hex(TAF_UINT8 *nptr, TAF_UINT16 len, TAF_UINT32 *rtn)
{
    TAF_UINT32 c;          /* current Char */
    TAF_UINT32 total  = 0; /* current total */
    TAF_UINT8  length = 0; /* current Length */

    c = (TAF_UINT32)*nptr++;

    while (length++ < len) {
        if ((c >= '0') && (c <= '9')) {
            c = c - '0';
        } else if ((c >= 'a') && (c <= 'f')) {
            c = (c - 'a') + 10;
        } else if ((c >= 'A') && (c <= 'F')) {
            c = (c - 'A') + 10;
        } else {
            return AT_FAILURE;
        }

        if (total > 0x0FFFFFFF) { /* 发生反转 */
            return AT_FAILURE;
        } else {
            total = (total << 4) + c;               /* accumulate digit */
            c     = (TAF_UINT32)(TAF_UINT8)*nptr++; /* get next Char */
        }
    }

    *rtn = total; /* return result, negated if necessary */
    return AT_SUCCESS;
}
/*
 * Description: 把字符串中的某一段拷贝到指定地址,pDst指示目的地址,pucBegain
 *              指示开始地址,pEnd指示结束地址
 * History:
 *  1.Date: 2005-04-19
 *    Author:
 *    Modification: Created function
 */
TAF_VOID At_RangeCopy(TAF_UINT8 *dst, TAF_UINT8 *begain, TAF_UINT8 *end)
{
    /* 依次拷贝到目的地址,直至结束 */
    while (begain < end) {
        *dst++ = *begain++;
    }
}

TAF_UINT32 At_UpString(TAF_UINT8 *data, TAF_UINT16 len)
{
    TAF_UINT8 *tmp    = data; /* current Char */
    TAF_UINT16 chkLen = 0;

    if (len == 0) {
        return AT_FAILURE;
    }

    while (chkLen++ < len) {
        if ((*tmp >= 'a') && (*tmp <= 'z')) {
            *tmp = *tmp - 0x20;
        }
        tmp++;
    }
    return AT_SUCCESS;
}


VOS_UINT32 atRangeToU32(VOS_UINT8 *begain, VOS_UINT8 *end)
{
    VOS_UINT32 total = 0; /* current total */
    VOS_UINT32 rst;

    /* 输入参数检查 */
    if (begain >= end) {
        return total;
    }

    rst = atAuc2ul(begain, (VOS_UINT16)(end - begain), &total);

    if (rst != AT_SUCCESS) {
        total = 0;
    }

    return total;
}


VOS_VOID atRangeCopy(VOS_UINT8 *dst, VOS_UINT8 *begain, VOS_UINT8 *end)
{
    /* 依次拷贝到目的地址,直至结束 */
    while (begain < end) {
        *dst++ = *begain++;
    }
}

/*
 * 功能描述: 把十六进制字符串转成无符号整型值
 *    AT_FAILURE: 输入字符串中有非数字字符，或数值溢出
 *    AT_SUCCESS: 成功
 */
LOCAL VOS_UINT32 auc2ulHex(VOS_UINT8 *nptr, VOS_UINT16 len, VOS_UINT32 *rtn)
{
    VOS_UINT8  c;                 /* current Char */
    VOS_UINT32 total  = 0;        /* current total */
    VOS_UINT16 length = 2;        /* current Length */
    VOS_UINT8 *pcTmp  = nptr + 2; /* 从0x后开始比较 */

    /* 参数指针由调用者保证不为NULL, 该处不做判断 */

    c = *pcTmp++;

    while (length++ < len) {
        /* 0xFFFFFFFF */
        if (total > 0xFFFFFFF) {
            return AT_FAILURE;
        }

        /* 字符检查 */
        if (isdigit(c)) {
            total = AT_CHECK_BASE_HEX * total + (c - '0'); /* accumulate digit */
            c     = *pcTmp++;                              /* get next Char */
        } else if (c >= 'A' && c <= 'F') {
            total = AT_CHECK_BASE_HEX * total + (c - 'A' + 10); /* accumulate digit */
            c     = *pcTmp++;                                   /* get next Char */
        } else if (c >= 'a' && c <= 'f') {
            total = AT_CHECK_BASE_HEX * total + (c - 'a' + 10); /* accumulate digit */
            c     = *pcTmp++;                                   /* get next Char */
        } else {
            return AT_FAILURE;
        }
    }

    *rtn = total; /* return result, negated if necessary */

    return AT_SUCCESS;
}


/*
 * 功能描述: 把十进制字符串转成无符号整型值
 *    AT_FAILURE: 输入字符串中有非数字字符，或数值溢出
 *    AT_SUCCESS: 成功
 */
VOS_UINT32 auc2ulDec(VOS_UINT8 *nptr, VOS_UINT16 len, VOS_UINT32 *rtn)
{
    VOS_UINT32 c;             /* current Char */
    VOS_UINT32 total  = 0;    /* current total */
    VOS_UINT16 length = 0;    /* current Length */
    VOS_UINT8 *pcTmp  = nptr; /* 从0x后开始比较 */

    /* 参数指针由调用者保证不为NULL, 该处不做判断 */

    c = (VOS_UINT32)*pcTmp++;

    while (length++ < len) {
        /* 字符检查 */
        if (isdigit(c)) {
            /* 0xFFFFFFFF 等于 4294967295 */
            if (((total == 429496729) && (c > '5')) || (total > 429496729)) {
                return AT_FAILURE;
            }

            total = AT_CHECK_BASE_DEC * total + (c - '0'); /* accumulate digit */
            c     = (VOS_UINT32)(VOS_UINT8)*pcTmp++;       /* get next Char */
        } else {
            return AT_FAILURE;
        }
    }

    *rtn = total; /* return result, negated if necessary */

    return AT_SUCCESS;
}

/*
 * 功能描述: 把字符串转成无符号整型值
 *    AT_FAILURE: 输入字符串中有非数字字符，或数值溢出
 *    AT_SUCCESS: 成功
 */
VOS_UINT32 atAuc2ul(VOS_UINT8 *nptr, VOS_UINT16 len, VOS_UINT32 *rtn)
{
    /* 进入该函数前，所有参数已进行检查，保证不为NULL */

    if (nptr == NULL || len == 0 || rtn == NULL) {
        return AT_FAILURE;
    }

    if (*nptr == '0') {
        if (len > 2 && ((*(nptr + 1) == 'x') || (*(nptr + 1) == 'X'))) {
            return auc2ulHex(nptr, len, rtn);
        } else {
        }
    }

    return auc2ulDec(nptr, len, rtn);
}


VOS_VOID At_ul2Auc(VOS_UINT32 value, TAF_UINT16 len, VOS_UINT8 *rtn)
{
    VOS_UINT32 tempValue;

    if (len == 0) {
        return;
    }

    while (value != 0) {
        tempValue = value % 10;
        value /= 10;
        *(rtn + len - 1) = '0' + (VOS_UINT8)tempValue;
        len--;

        if (len == 0) {
            return;
        }
    }

    while (len > 0) {
        *(rtn + len - 1) = '0';
        len--;
    }

    return;
}


VOS_VOID* At_HeapAllocD(VOS_UINT32 size)
{
    VOS_VOID *ret = NULL;

    if ((size == 0) || (size > (1024 * 1024))) {
        return NULL;
    }

#if (VOS_VXWORKS == VOS_OS_VER)
    ret = (VOS_VOID *)malloc(size);
#elif (VOS_LINUX == VOS_OS_VER)
    ret = (VOS_VOID *)kmalloc(size, GFP_KERNEL);
#else
    ret = (VOS_VOID *)malloc(size);
#endif

    return ret;
}


VOS_VOID At_HeapFreeD(VOS_UINT8 *addr)
{
    if (addr == NULL) {
        return;
    }

#if (VOS_VXWORKS == VOS_OS_VER)
    free(addr);
#elif (VOS_LINUX == VOS_OS_VER)
    kfree(addr);
#else
    free(addr);
#endif

    return;
}

