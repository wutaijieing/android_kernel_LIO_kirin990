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

#include "AtParseSCmd.h"
#include "AtCheckFunc.h"

#include "msp_errno.h"


/* AT S命令初始状态表 */
AT_SubState g_atSCmdNoneStateTab[] = {
    /* 如果当前状态是ATCMD_NONE_STATE，atCheckCharS成功，则进入AT_S_CMD_NAME_STATE */
    { At_CheckCharS, AT_S_CMD_NAME_STATE },

    /* 子状态表结束 */
    { NULL, AT_BUTT_STATE },
};

/* AT S命令名状态表 */
AT_SubState g_atSCmdNameStateTab[] = {
    /* 如果当前状态是AT_S_CMD_NAME_STATE，atCheck_num成功，则进入AT_S_CMD_NAME_STATE */
    { At_CheckDigit, AT_S_CMD_NAME_STATE },

    /* 如果当前状态是AT_S_CMD_NAME_STATE，atCheck_equ成功，则进入AT_S_CMD_SET_STATE */
    { At_CheckEqu, AT_S_CMD_SET_STATE },

    /* 如果当前状态是AT_S_CMD_NAME_STATE，atCheck_req成功，则进入AT_S_CMD_READ_STATE */
    { At_CheckReq, AT_S_CMD_READ_STATE },

    /* 子状态表结束 */
    { NULL, AT_BUTT_STATE },
};

/* AT S命令设置状态表 */
AT_SubState g_atSCmdSetStateTab[] = {
    /* 如果当前状态是AT_S_CMD_SET_STATE，atCheck_num成功，则进入AT_S_CMD_PARA_STATE */
    { At_CheckDigit, AT_S_CMD_PARA_STATE },

    /* 如果当前状态是AT_S_CMD_SET_STATE，atCheck_req成功，则进入AT_S_CMD_TEST_STATE */
    { At_CheckReq, AT_S_CMD_TEST_STATE },

    /* 子状态表结束 */
    { NULL, AT_BUTT_STATE },
};

/* AT S命令参数状态表 */
AT_SubState g_atSCmdParaStateTab[] = {
    /* 如果当前状态是AT_S_CMD_PARA_STATE，atCheck_num成功，则进入AT_S_CMD_PARA_STATE */
    { At_CheckDigit, AT_S_CMD_PARA_STATE },

    /* 子状态表结束 */
    { NULL, AT_BUTT_STATE },
};

/* AT S命令主状态表 */
AT_MainState g_atSCmdMainStateTab[] = {
    /* 如果当前状态是AT_NONE_STATE，则进入g_atSCmdNoneStateTab子状态表 */
    { AT_NONE_STATE, g_atSCmdNoneStateTab },

    /* 如果当前状态是ATCMD_B_CMD_STATE，则进入g_atSCmdNameStateTab子状态表 */
    { AT_S_CMD_NAME_STATE, g_atSCmdNameStateTab },

    /* 如果当前状态是AT_S_CMD_SET_STATE，则进入g_atSCmdSetStateTab子状态表 */
    { AT_S_CMD_SET_STATE, g_atSCmdSetStateTab },

    /* 如果当前状态是AT_S_CMD_PARA_STATE，则进入g_atSCmdParaStateTab子状态表 */
    { AT_S_CMD_PARA_STATE, g_atSCmdParaStateTab },

    /* 主状态表结束 */
    { AT_BUTT_STATE, NULL },
};



VOS_UINT32 atParseSCmd(VOS_UINT8 *data, VOS_UINT16 len)
{
    AT_StateType currState = AT_NONE_STATE; /*  设置初始状态 */
    AT_StateType newState  = AT_NONE_STATE; /*  设置初始状态 */
    VOS_UINT8   *currPtr   = data;          /*  指向当前正在处理的字符 */
    VOS_UINT8   *copyPtr   = data;          /*  拷贝内容的起始指针 */
    VOS_UINT16   length    = 0;             /*  记录当前已经处理的字符个数 */

    /* 依次分析字符串中的每个字符 */
    /* 依次比较每个字符 */
    while ((length++ < len) && (g_atParseCmd.paraIndex < AT_MAX_PARA_NUMBER)) {
        currState = newState; /*  当前状态设置为新状态 */

        /*  根据当前处理的字符和当前状态查表得到新状态 */
        newState = atFindNextMainState(g_atSCmdMainStateTab, *currPtr, currState);

        switch (newState) {
            /*  状态处理 */
            case AT_S_CMD_NAME_STATE:                                     /*  S命令名状态 */
                if (currState != newState) {                              /*  新状态部分不等于当前状态 */
                    g_atParseCmd.cmdFmtType = AT_BASIC_CMD_TYPE;          /*  设置命令类型 */
                    g_atParseCmd.cmdOptType = AT_CMD_OPT_SET_CMD_NO_PARA; /*  设置命令操作类型 */
                    copyPtr                 = currPtr;                    /*  准备接收S命令名 */
                }
                break;

            case AT_S_CMD_SET_STATE: /* AT S命令等号状态 */

                /* 存储命令名到全局变量中 */
                g_atParseCmd.cmdOptType = AT_CMD_OPT_SET_PARA_CMD; /* 设置命令操作类型 */

                g_atParseCmd.cmdName.cmdNameLen = (VOS_UINT16)(currPtr - copyPtr);
                if (g_atParseCmd.cmdName.cmdNameLen < sizeof(g_atParseCmd.cmdName.cmdName)) {
                    atRangeCopy(g_atParseCmd.cmdName.cmdName, copyPtr, currPtr);
                } else {
                    return AT_ERROR; /* 返回错误 */
                }
                break;

            case AT_S_CMD_PARA_STATE:        /* AT S命令参数状态 */
                if (currState != newState) { /* 新状态部分不等于当前状态 */
                    copyPtr = currPtr;       /* 准备接收 S类型命令名 */
                }
                break;

            case AT_S_CMD_READ_STATE: /* AT S命令查询参数状态 */

                /* 存储命令名到全局变量中 */
                g_atParseCmd.cmdName.cmdNameLen = (VOS_UINT16)(currPtr - copyPtr);
                if (g_atParseCmd.cmdName.cmdNameLen < sizeof(g_atParseCmd.cmdName.cmdName)) {
                    atRangeCopy(g_atParseCmd.cmdName.cmdName, copyPtr, currPtr);
                } else {
                    return AT_ERROR; /* 返回错误 */
                }

                g_atParseCmd.cmdOptType = AT_CMD_OPT_READ_CMD; /* 设置命令操作类型 */

                break;

            case AT_S_CMD_TEST_STATE:                          /* AT S命令测试参数状态 */
                g_atParseCmd.cmdOptType = AT_CMD_OPT_TEST_CMD; /* 设置命令操作类型 */
                break;

            default:
                return AT_ERROR; /* 返回错误 */
        }
        currPtr++; /*  继续分析下一个字符 */
    }

    switch (newState) {
        /*  根据最后的状态判断 */
        case AT_S_CMD_PARA_STATE: /* AT S命令参数状态 */

            /* 存储参数到全局变量中 */
            if (atfwParseSaveParam(copyPtr, (VOS_UINT16)(currPtr - copyPtr)) != ERR_MSP_SUCCESS) {
                return AT_ERROR;
            }

            break;

        case AT_S_CMD_NAME_STATE: /* AT S命令状态 */
            /* 存储命令名到全局变量中 */
            g_atParseCmd.cmdName.cmdNameLen = (VOS_UINT16)(currPtr - copyPtr);
            if (g_atParseCmd.cmdName.cmdNameLen < sizeof(g_atParseCmd.cmdName.cmdName)) {
                atRangeCopy(g_atParseCmd.cmdName.cmdName, copyPtr, currPtr);
            } else {
                return AT_ERROR; /* 返回错误 */
            }
            break;

        case AT_S_CMD_SET_STATE:  /* 无效状态 */
        case AT_S_CMD_READ_STATE: /* 初始状态 */
        case AT_S_CMD_TEST_STATE: /* 初始状态 */
            break;

        default:
            return AT_ERROR; /* 返回错误 */
    }
    return AT_SUCCESS; /*  返回正确 */
}

