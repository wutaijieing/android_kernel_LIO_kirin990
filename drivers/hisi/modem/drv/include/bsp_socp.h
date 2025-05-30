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

#ifndef _BSP_SOCP_H
#define _BSP_SOCP_H

#include "osl_common.h"
#include "mdrv_socp_common.h"
#ifdef __KERNEL__
#include "acore_nv_id_drv.h"
#include "acore_nv_stru_drv.h"
#endif
#include "bsp_trace.h"

#ifdef __cplusplus
extern "C"
{
#endif

/**************************************************************************
  错误码定义
**************************************************************************/
#define BSP_ERR_SOCP_BASE            BSP_DEF_ERR(BSP_MODU_SOCP, 0)
#define BSP_ERR_SOCP_NULL            (BSP_ERR_SOCP_BASE + 0x1)
#define BSP_ERR_SOCP_NOT_INIT        (BSP_ERR_SOCP_BASE + 0x2)
#define BSP_ERR_SOCP_MEM_ALLOC       (BSP_ERR_SOCP_BASE + 0x3)
#define BSP_ERR_SOCP_SEM_CREATE      (BSP_ERR_SOCP_BASE + 0x4)
#define BSP_ERR_SOCP_TSK_CREATE      (BSP_ERR_SOCP_BASE + 0x5)
#define BSP_ERR_SOCP_INVALID_CHAN    (BSP_ERR_SOCP_BASE + 0x6)
#define BSP_ERR_SOCP_INVALID_PARA    (BSP_ERR_SOCP_BASE + 0x7)
#define BSP_ERR_SOCP_NO_CHAN         (BSP_ERR_SOCP_BASE + 0x8)
#define BSP_ERR_SOCP_SET_FAIL        (BSP_ERR_SOCP_BASE + 0x9)
#define BSP_ERR_SOCP_TIMEOUT         (BSP_ERR_SOCP_BASE + 0xa)
#define BSP_ERR_SOCP_NOT_8BYTESALIGN (BSP_ERR_SOCP_BASE + 0xb)
#define BSP_ERR_SOCP_CHAN_RUNNING    (BSP_ERR_SOCP_BASE + 0xc)
#define BSP_ERR_SOCP_CHAN_MODE       (BSP_ERR_SOCP_BASE + 0xd)
#define BSP_ERR_SOCP_DEST_CHAN       (BSP_ERR_SOCP_BASE + 0xe)
#define BSP_ERR_SOCP_DECSRC_SET      (BSP_ERR_SOCP_BASE + 0xf)

#define SOCP_MAX_MEM_SIZE            (200 *1024 *1024)
#define SOCP_MIN_MEM_SIZE            (1 *1024 *1024)
#define SOCP_MAX_TIMEOUT             1200     /*MS*/
#define SOCP_MIN_TIMEOUT             10       /*MS*/
#define SOCP_RESERVED_TRUE           1
#define SOCP_RESERVED_FALSE          0

typedef u32 (*socp_compress_isr)     (void);
typedef u32 (*socp_compress_event_cb)(socp_event_cb event_cb);
typedef u32 (*socp_compress_read_cb) (socp_read_cb read_cb);
typedef u32 (*socp_compress_enable)  (u32 chanid);
typedef u32 (*socp_compress_disable) (u32 chanid);
typedef u32 (*socp_compress_set)     (u32 chanid, SOCP_CODER_DEST_CHAN_S* attr);
typedef u32 (*socp_compress_getbuffer) (SOCP_BUFFER_RW_STRU *pRingBuffer);
typedef u32 (*socp_compress_readdone)(u32 u32Size);
typedef u32 (*socp_compress_clear)   (u32 chanid);

typedef struct socp_compress_ops
{
    socp_compress_isr       isr;
    socp_compress_event_cb  register_event_cb;
    socp_compress_read_cb   register_read_cb;
    socp_compress_enable    enable;
    socp_compress_disable   disable;
    socp_compress_set       set;
    socp_compress_getbuffer getbuffer;
    socp_compress_readdone  readdone;
    socp_compress_clear     clear;
}socp_compress_ops_stru;

#if (FEATURE_SOCP_DECODE_INT_TIMEOUT == FEATURE_ON)
typedef enum timeout_module
{
    DECODE_TIMEOUT_INT_TIMEOUT = 0,
    DECODE_TIMEOUT_DEC_INT_TIMEOUT = 1,
    DECODE_TIMEOUT_BUTTON = 2,

} decode_timeout_module_e;

#endif

typedef struct
{
    SOCP_VOTE_TYPE_ENUM_U32 type;
}socp_vote_req_stru;

typedef struct
{
    u32 vote_rst;   /* 1:成功，非1:失败 */
}socp_vote_cnf_stru;

struct socp_enc_dst_log_cfg
{
    void*           virt_addr;       /* SOCP编码目的通道数据虚拟BUFFER、在32位系统上是4字节，在64位系统上是8字节 */
    unsigned long   phy_addr;        /* SOCP编码目的通道数据物理BUFFER地址 */
    unsigned int    buff_size;       /* SOCP编码目的通道数据BUFFER大小 */
    unsigned int    over_time;       /* NVE中设置的超时时间 */
    unsigned int    log_on_flag;     /* 底软配置buffer功能启动标志(SOCP_DST_CHAN_CFG_TYPE_ENUM) */
    unsigned int    cur_time_out;    /* SOCP编码目的通道数据传输超时时间 */
    unsigned int    flush_flag;
    unsigned int    mem_log_cfg;
    unsigned int    current_mode;
    unsigned int    cps_mode;
};

typedef struct {
    void*           virt_addr;      /* SOCP编码目的通道数据虚拟BUFFER、在32位系统上是4字节，在64位系统上是8字节 */
    unsigned long   phy_addr;       /* SOCP编码目的通道数据物理BUFFER地址 */
    unsigned int    buff_size;      /* SOCP编码目的通道数据BUFFER大小 */
    unsigned int    buff_useable;    /* 预留的kernel buffer是否可用的标志 */
    unsigned int    timeout;        /* SOCP编码目的通道数据传输超时时间 */
    unsigned int    init_flag;      /* 预留内存获取流程完成标志 */
}socp_rsv_mem_s;


#ifdef ENABLE_BUILD_SOCP
/*****************************************************************************
* 函 数 名  : socp_init
*
* 功能描述  : 模块初始化函数
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : 初始化成功的标识码
*****************************************************************************/
s32 socp_init(void);

/*****************************************************************************
* 函 数 名  : bsp_socp_encdst_dsm_init
*
* 功能描述  : socp编码目的端中断状态初始化
* 输入参数  : enc_dst_chan_id: 编码目的端通道号
*             b_enable: 初始化的中断状态
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
void bsp_socp_encdst_dsm_init(u32 enc_dst_chan_id, u32 b_enable);

/*****************************************************************************
* 函 数 名  : bsp_socp_data_send_manager
*
* 功能描述  : socp编码目的端上报数据
* 输入参数  : enc_dst_chan_id: 编码目的端通道号
*             b_enable: 中断使能
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
void bsp_socp_data_send_manager(u32 enc_dst_chan_id, u32 b_enable);

/*****************************************************************************
 函 数 名  : bsp_socp_coder_set_src_chan
 功能描述  : 此接口完成SOCP编码器源通道的分配，根据编码器源通道参数设置通道属性，连接目标通道，返回函数执行结果。
 输入参数  : src_attr:编码器源通道参数结构体指针。
             pSrcChanID:申请到的源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码源通道分配成功。
             SOCP_ERROR:编码源通道分配失败。
*****************************************************************************/
s32 bsp_socp_coder_set_src_chan(SOCP_CODER_SRC_ENUM_U32 src_chan_id, SOCP_CODER_SRC_CHAN_S *src_attr);

/*****************************************************************************
 函 数 名  : bsp_socp_coder_set_dest_chan_attr
 功能描述  : 此接口完成某一编码目标通道的配置，返回函数执行的结果。
 输入参数  : dst_chan_id:SOCP编码器的目标通道ID。
             dst_attr:SOCP编码器目标通道参数结构体指针。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码目的通道设置成功。
             SOCP_ERROR:编码目的通道设置失败。
*****************************************************************************/
s32 bsp_socp_coder_set_dest_chan_attr(u32 dst_chan_id, SOCP_CODER_DEST_CHAN_S *dst_attr);

/*****************************************************************************
 函 数 名      : bsp_socp_decoder_set_dest_chan
 功能描述  :此接口完成SOCP解码器目标通道的分配，
                根据解码目标通道参数设置通道属性，
                并连接源通道，返回函数执行结果。
 输入参数  : attr:解码器目标通道参数结构体指针
                         pDestChanID:申请到的目标通道ID
 输出参数  : 无。
 返 回 值      : SOCP_OK:解码目的通道分配成功。
                             SOCP_ERROR:解码目的通道设置失败。
*****************************************************************************/
s32 bsp_socp_decoder_set_dest_chan(SOCP_DECODER_DST_ENUM_U32 dst_chan_id, SOCP_DECODER_DEST_CHAN_STRU *attr);

/*****************************************************************************
 函 数 名      : bsp_socp_decoder_set_src_chan_attr
 功能描述  :此接口完成某一解码源通道的配置，返回函数执行的结果。
 输入参数  : src_chan_id:解码器源通道ID
                         input_attr:解码器源通道参数结构体指针
 输出参数  : 无。
 返 回 值      : SOCP_OK:解码源通道设置成功
                             SOCP_ERROR:解码源通道设置失败
*****************************************************************************/
s32 bsp_socp_decoder_set_src_chan_attr ( u32 src_chan_id,SOCP_DECODER_SRC_CHAN_STRU *input_attr);

/*****************************************************************************
 函 数 名      : bsp_socp_decoder_get_err_cnt
 功能描述  :此接口给出解码通道中四种异常情况的计数值。
 输入参数  : chan_id:解码器通道ID
                         err_cnt:解码器异常计数结构体指针
 输出参数  : 无。
 返 回 值      : SOCP_OK:返回异常计数成功
                             SOCP_ERROR:返回异常计数失败
*****************************************************************************/
s32 bsp_socp_decoder_get_err_cnt (u32 chan_id, SOCP_DECODER_ERROR_CNT_STRU *err_cnt);

/*****************************************************************************
 函 数 名  : bsp_socp_free_channel
 功能描述  : 此接口根据通道ID释放分配的编解码通道。
 输入参数  : chan_id:通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:通道释放成功。
             SOCP_ERROR:通道释放失败。
*****************************************************************************/
s32 bsp_socp_free_channel(u32 chan_id);

/*****************************************************************************
* 函 数 名  : socp_clean_encsrc_chan
*
* 功能描述  : 清空编码源通道，同步V9 SOCP接口
*
* 输入参数  : src_chan_id       编码通道号
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/
u32 bsp_socp_clean_encsrc_chan(SOCP_CODER_SRC_ENUM_U32 src_chan_id);

/*****************************************************************************
 函 数 名  : bsp_socp_register_event_cb
 功能描述  : 此接口为给定通道注册事件回调函数。
 输入参数  : chan_id:通道ID。
             event_cb:事件回调函数，参考socp_event_cb函数定义
 输出参数  : 无。
 返 回 值  : SOCP_OK:注册事件回调函数成功。
             SOCP_ERROR:注册事件回调函数失败。
*****************************************************************************/
s32 bsp_socp_register_event_cb(u32 chan_id, socp_event_cb event_cb);

/*****************************************************************************
 函 数 名  : bsp_socp_start
 功能描述  : 此接口用于源通道，启动编码或者解码。
 输入参数  : src_chan_id:源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码或解码启动成功。
             SOCP_ERROR:编码或解码启动失败。
*****************************************************************************/
s32 bsp_socp_start(u32 src_chan_id);

/*****************************************************************************
 函 数 名  : bsp_socp_stop
 功能描述  : 此接口用于源通道，停止编码或者解码。
 输入参数  : src_chan_id:源通道ID。
 输出参数  : 无。
 返 回 值  : SOCP_OK:编码或解码停止成功。
             SOCP_ERROR:编码或解码停止失败。
*****************************************************************************/
s32 bsp_socp_stop(u32 src_chan_id);

/*****************************************************************************
 函 数 名      : bsp_socp_set_timeout
 功能描述  :此接口设置超时阈值。
 输入参数  : time_out:超时阈值

 输出参数  : 无。
 返 回 值      : SOCP_OK:设置超时时间阈值成功。
                             SOCP_ERROR:设置超时时间阈值失败
*****************************************************************************/
s32 bsp_socp_set_timeout (SOCP_TIMEOUT_EN_ENUM_UIN32 time_out_en, u32 time_out);

/*****************************************************************************
 函 数 名   : bsp_socp_set_dec_pkt_lgth
 功能描述  :设置解码包长度极限值
 输入参数  : pkt_length:解码包长度极值

 输出参数  : 无。
 返 回 值      : SOCP_OK:设置成功。
                    其他值:设置失败
*****************************************************************************/
s32 bsp_socp_set_dec_pkt_lgth(SOCP_DEC_PKTLGTH_STRU *pkt_length);

/*****************************************************************************
 函 数 名   : bsp_socp_set_debug
 功能描述  :设置解码源通道的debug模式
 输入参数  : chan_id:通道ID
                  debug_en: debug标识
 输出参数  : 无。
 返 回 值      : SOCP_OK:设置成功。
                     其他值:设置失败
*****************************************************************************/
s32 bsp_socp_set_debug(u32 dst_chan_id, u32 debug_en);

/*****************************************************************************
 函 数 名      : bsp_socp_get_write_buff
 功能描述  :此接口用于获取写数据buffer。
 输入参数  : src_chan_id:源通道ID
                  p_rw_buff:           :写数据buffer

 输出参数  : 无。
 返 回 值      : SOCP_OK:获取写数据buffer成功。
                             SOCP_ERROR:获取写数据buffer失败
*****************************************************************************/
s32 bsp_socp_get_write_buff( u32 src_chan_id, SOCP_BUFFER_RW_STRU *p_rw_buff);

/*****************************************************************************
 函 数 名      : bsp_socp_write_done
 功能描述  :该接口用于数据的写操作，提供写入数据的长度。
 输入参数  : src_chan_id:源通道ID
                  write_size:   已写入数据的长度
 输出参数  : 无。
 返 回 值      : SOCP_OK:写入数据成功。
                             SOCP_ERROR:写入数据失败
*****************************************************************************/
s32 bsp_socp_write_done(u32 src_chan_id, u32 write_size);


/*****************************************************************************
 函 数 名      : bsp_socp_register_rd_cb
 功能描述  :该接口用于注册从RD缓冲区中读取数据的回调函数。
 输入参数  : src_chan_id:源通道ID
                  rd_cb:  事件回调函数
 输出参数  : 无。
 返 回 值      : SOCP_OK:注册RD环形缓冲区读数据回调函数成功。
                             SOCP_ERROR:注册RD环形缓冲区读数据回调函数失败
*****************************************************************************/
s32 bsp_socp_register_rd_cb(u32 src_chan_id, socp_rd_cb rd_cb);

/*****************************************************************************
 函 数 名      : bsp_socp_get_rd_buffer
 功能描述  :该此接口用于获取RD buffer的数据指针。
 输入参数  : src_chan_id:源通道ID
                  p_rw_buff:  RD buffer
 输出参数  : 无。
 返 回 值      : SOCP_OK:获取RD环形缓冲区成功
                             SOCP_ERROR:获取RD环形缓冲区失败
*****************************************************************************/
s32 bsp_socp_get_rd_buffer( u32 src_chan_id,SOCP_BUFFER_RW_STRU *p_rw_buff);

/*****************************************************************************
 函 数 名      : bsp_socp_read_rd_done
 功能描述  :此接口用于上层通知SOCP驱动，从RD buffer中实际读取的数据。
 输入参数  : src_chan_id:源通道ID
                  rd_size:  从RD buffer中实际读取的数据长度
 输出参数  : 无。
 返 回 值      : SOCP_OK:读取RDbuffer中的数据成功
                             SOCP_ERROR:读取RDbuffer中的数据失败
*****************************************************************************/
s32 bsp_socp_read_rd_done(u32 src_chan_id, u32 rd_size);

/*****************************************************************************
 函 数 名      : bsp_socp_register_read_cb
 功能描述  :该接口用于注册读数据的回调函数。
 输入参数  : dst_chan_id:目标通道ID
                  read_cb: 事件回调函数
 输出参数  : 无。
 返 回 值      : SOCP_OK:注册读数据回调函数成功
                             SOCP_ERROR:注册读数据回调函数失败
*****************************************************************************/
s32 bsp_socp_register_read_cb( u32 dst_chan_id, socp_read_cb read_cb);

/*****************************************************************************
 函 数 名      : bsp_socp_register_read_cb
 功能描述  :该此接口用于获取读数据缓冲区指针。
 输入参数  : dst_chan_id:目标通道ID
                  read_cb: 读数据buffer
 输出参数  : 无。
 返 回 值      : SOCP_OK:获取读数据缓冲区成功。
                             SOCP_ERROR:获取读数据缓冲区成功。
*****************************************************************************/
s32 bsp_socp_get_read_buff(u32 dst_chan_id,SOCP_BUFFER_RW_STRU *pBuffer);

/*****************************************************************************
 函 数 名      : bsp_socp_read_data_done
 功能描述  :该接口用于上层告诉SOCP驱动，从目标通道中读走的实际数据。
 输入参数  : dst_chan_id:目标通道ID
                  read_size: 已读出数据的长度
 输出参数  : 无。
 返 回 值      : SOCP_OK:读数据成功。
                             SOCP_ERROR:读数据失败
*****************************************************************************/
s32 bsp_socp_read_data_done(u32 dst_chan_id,u32 read_size);

/*****************************************************************************
 函 数 名      : bsp_socp_set_bbp_enable
 功能描述  :使能或停止BBP通道。
 输入参数  : b_enable:通道ID
 输出参数  : 无。
 返 回 值      : SOCP_OK:设置成功。
                   其他值:设置失败
*****************************************************************************/
s32 bsp_socp_set_bbp_enable(int b_enable);

/*****************************************************************************
 函 数 名      : bsp_socp_set_bbp_ds_mode
 功能描述  :设置BBP DS通道数据溢出处理模式。
 输入参数  : ds_mode:DS通道数据溢出时处理模式设置
 输出参数  : 无。
 返 回 值      : SOCP_OK:设置成功。
                   其他值:设置失败
*****************************************************************************/
s32 bsp_socp_set_bbp_ds_mode(SOCP_BBP_DS_MODE_ENUM_UIN32 ds_mode);

/*****************************************************************************
* 函 数 名  : bsp_socp_get_state
*
* 功能描述  : 获取SOCP状态
*
* 返 回 值  : SOCP_IDLE    空闲
*             SOCP_BUSY    忙碌
*****************************************************************************/
SOCP_STATE_ENUM_UINT32 bsp_socp_get_state(void);


/*****************************************************************************
* 函 数 名  : bsp_socp_vote
* 功能描述  : SOCP投票接口，根据投票结果决定SOCP是否睡眠，该接口只在A核提供
* 输入参数  : id --- 投票组件ID，type --- 投票类型
* 输出参数  : 无
* 返 回 值  : BSP_S32 0 --- 投票成功，0xFFFFFFFF --- 投票失败
*****************************************************************************/
BSP_S32 bsp_socp_vote(SOCP_VOTE_ID_ENUM_U32 id, SOCP_VOTE_TYPE_ENUM_U32 type);

/*****************************************************************************
* 函 数 名  : bsp_socp_vote_to_mcore
* 功能描述  : SOCP投票接口，该接口只在C核提供，适配LDSP首次加载的SOCP上电需求
* 输入参数  : type --- 投票类型
* 输出参数  : 无
* 返 回 值  : BSP_S32 0 --- 投票成功，0xFFFFFFFF --- 投票失败
*****************************************************************************/
BSP_S32 bsp_socp_vote_to_mcore(SOCP_VOTE_TYPE_ENUM_U32 type);

/*****************************************************************************
* 函 数 名  : bsp_socp_get_log_cfg
* 功能描述  : 获取log配置
* 输入参数  :
* 输出参数  :
* 返 回 值  :
*****************************************************************************/
struct socp_enc_dst_log_cfg * bsp_socp_get_log_cfg(void);
/*****************************************************************************
* 函 数 名  : bsp_socp_get_sd_logcfg
* 功能描述  : 获取配置
* 输入参数  :
* 输出参数  :
* 返 回 值  :
*****************************************************************************/
u32 bsp_socp_get_sd_logcfg(SOCP_ENC_DST_BUF_LOG_CFG_STRU* cfg);
/*****************************************************************************
* 函 数 名  : socp_set_clk_autodiv_enable
* 功能描述  : 调用clk接口clk_disable_unprepare将bypass置0，即开自动降频
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 无
* 注    意  :
              clk_prepare_enable 接口与 clk_disable_unprepare 接口必须成对使用
              clk的自动降频默认处于打开状态，所以
              必须先进行 clk_prepare_enable 才能进行 clk_disable_unprepare 操作
*****************************************************************************/
void bsp_socp_set_clk_autodiv_enable(void);

/*****************************************************************************
* 函 数 名  : socp_set_clk_autodiv_disable
* 功能描述  : 调用clk接口clk_prepare_enable将bypass置1，即关自动降频
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  : 无
* 注    意  :
              clk_prepare_enable 接口与 clk_disable_unprepare 接口必须成对使用
              clk的自动降频默认处于打开状态，所以
              必须先进行 clk_prepare_enable 才能进行 clk_disable_unprepare 操作
*****************************************************************************/
void bsp_socp_set_clk_autodiv_disable(void);


#if (FEATURE_SOCP_DECODE_INT_TIMEOUT == FEATURE_ON)
/*****************************************************************************
* 函 数 名  : bsp_socp_set_decode_timeout_register
* 功能描述  :选择解码超时寄存器
* 输入参数  : 无
* 输出参数  : 无
* 返 回 值  :
*****************************************************************************/
s32 bsp_socp_set_decode_timeout_register(decode_timeout_module_e module);
#endif
/*****************************************************************************
* 函 数 名  : bsp_socp_set_enc_dst_threshold
* 功能描述  :
* 输入参数  :
* 输出参数  :
* 返 回 值  :
*****************************************************************************/
void bsp_socp_set_enc_dst_threshold(bool mode,u32 dst_chan_id);

/*****************************************************************************
* 函 数 名  : bsp_socp_set_ind_mode
*
* 功能描述  : 设置上报模式接口
*
* 输入参数  : 模式参数
*
* 输出参数  : 无
*
* 返 回 值  : BSP_S32 BSP_OK:成功 BSP_ERROR:失败
*****************************************************************************/
s32 bsp_socp_set_ind_mode(SOCP_IND_MODE_ENUM mode);

/*****************************************************************************
* 函 数 名  : bsp_socp_get_log_ind_mode
*
* 功能描述  : 获取当前上报模式
*
* 输入参数  : 模式参数
*
* 输出参数  : 无
*
* 返 回 值  : BSP_S32 BSP_OK:成功 BSP_ERROR:失败
*****************************************************************************/
s32  bsp_socp_get_log_ind_mode(u32 *log_ind_mode);
/*****************************************************************************
* 函 数 名  : bsp_report_ind_mode_ajust
*
* 功能描述  : 上报模式调整
*
* 输入参数  : 模式参数
*
* 输出参数  : 无
*
* 返 回 值  : BSP_S32 BSP_OK:成功 BSP_ERROR:失败
*****************************************************************************/
s32 bsp_report_ind_mode_ajust(SOCP_IND_MODE_ENUM mode);

/*****************************************************************************
* 函 数 名  :  bsp_socp_encsrc_chan_open
*
* 功能描述  : 打开SOCP编码源通道开关
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : void
*****************************************************************************/
void bsp_socp_encsrc_chan_open(u32 src_chan_id);

/*****************************************************************************
* 函 数 名  : bsp_socp_encsrc_chan_close
*
* 功能描述  : 关闭socp编码源通道开关，并且等待socp空闲后退出
              该接口用于防止socp访问下电区域
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : void
*****************************************************************************/
void bsp_socp_encsrc_chan_close(u32 src_chan_id);

/*****************************************************************************
* 函 数 名  : bsp_socp_dump_save
*
* 功能描述  : 异常前存储socp寄存器
*****************************************************************************/
void bsp_socp_dump_save(void);

/*****************************************************************************
* 函 数 名  : bsp_socp_check_state
*
* 功能描述  : 判断SOCP通道关闭以后的状态，提供给TCM下电流程中使用
*
* 输入参数  : 源通道号
*
* 输出参数  : 无
*
* 返 回 值  : SOCP状态
*****************************************************************************/
s32 bsp_socp_check_state(u32 src_chan_id);

/*****************************************************************************
 函 数 名  : bsp_socp_update_bbp_ptr
 功能描述  : 该此接口用于读写指针绝对地址改为相对地址
 输入参数  : ulSrcChanId:源通道ID

 输出参数  : 无。
 返 回 值  : 无
*****************************************************************************/
void bsp_socp_update_bbp_ptr(u32 src_chan_id);


u32 bsp_get_socp_ind_dst_int_slice(void);
/*****************************************************************************
 函 数 名      : bsp_clear_socp_buff
 功能描述  : 该此接口用于清空SOCP源buff
 输入参数  : src_chan_id:通道id
 输出参数  : 无。
 返 回 值     : 无
*****************************************************************************/
s32 bsp_clear_socp_buff(u32 src_chan_id);

/*****************************************************************************
* 函 数 名  : bsp_socp_soft_free_encdst_chan
*
* 功能描述  : 软释放编码目的通道
*
* 输入参数  : enc_dst_chan_id       编码通道号
*
* 输出参数  : 无
*
* 返 回 值  : 释放成功与否的标识码
*****************************************************************************/
s32 bsp_socp_soft_free_encdst_chan(u32 enc_dst_chan_id);

/*****************************************************************************
* 函 数 名  : bsp_SocpEncDstQueryIntInfo
*
* 功能描述  : 提供给diag_debug查询socp数据通道目的端中断信息
*
* 输入参数  : 无
* 输出参数  :
*
* 返 回 值  : 无
*****************************************************************************/
void bsp_SocpEncDstQueryIntInfo(u32 *trf_info, u32 *thrh_ovf_info);

/*****************************************************************************
* 函 数 名  : bsp_clear_socp_encdst_int_info
*
* 功能描述  : 清空socp目的端上溢统计值
*
* 输入参数  : 无
* 输出参数  :
*
* 返 回 值  : 无
*****************************************************************************/
void bsp_clear_socp_encdst_int_info(void);

s32 socp_dst_channel_enable(u32 dst_chan_id);

s32 socp_dst_channel_disable(u32 dst_chan_id);
s32 bsp_socp_dst_trans_id_disable(u32 dst_chan_id);


#ifdef __KERNEL__
/*****************************************************************************
 函 数 名      : bsp_socp_set_rate_ctrl
 功能描述  : 该此接口用于设置SOCP流控配置
 输入参数  : p_rate_ctrl:流控配置
 输出参数  : 无。
 返 回 值     : 无
*****************************************************************************/
s32 bsp_socp_set_rate_ctrl(DRV_DIAG_RATE_STRU *p_rate_ctrl);
#endif

#else

static inline void bsp_socp_encsrc_chan_open(u32 src_chan_id)
{
    return;
}

static inline void bsp_socp_encsrc_chan_close(u32 src_chan_id)
{
    return;
}

static inline s32 bsp_socp_check_state(u32 src_chan_id)
{
	return 0;
}

static inline void bsp_socp_dump_save(void)
{
    return;
}

static inline s32 bsp_socp_get_write_buff( u32 src_chan_id, SOCP_BUFFER_RW_STRU *p_rw_buff)
{
    return 0;
}

static inline s32 bsp_socp_write_done(u32 src_chan_id, u32 write_size)
{
    return 0;
}
static inline s32 bsp_socp_coder_set_dest_chan_attr(u32 dst_chan_id, SOCP_CODER_DEST_CHAN_S *dst_attr)
{
    return 0;
}
static inline s32 bsp_socp_coder_set_src_chan(SOCP_CODER_SRC_ENUM_U32 src_chan_id, SOCP_CODER_SRC_CHAN_S *src_attr)
{
    return 0;
}

static inline s32 bsp_socp_start(u32 src_chan_id)
{
    return 0;
}
static inline u32 bsp_get_socp_ind_dst_int_slice(void)
{
    return 0;
}
static inline s32 bsp_socp_set_ind_mode(SOCP_IND_MODE_ENUM mode)
{
	return 0;
}
static inline s32 bsp_socp_get_read_buff(u32 dst_chan_id,SOCP_BUFFER_RW_STRU *p_buffer)
{
	return 0;
}
static inline u32 bsp_socp_get_sd_logcfg(SOCP_ENC_DST_BUF_LOG_CFG_STRU* cfg)
{
	return 0;
}
static inline s32 bsp_socp_read_data_done(u32 dst_chan_id,u32 read_size)
{
	return 0;
}
static inline s32 bsp_socp_register_read_cb( u32 dst_chan_id, socp_read_cb read_cb)
{
	return 0;
}
static inline s32 bsp_socp_register_event_cb(u32 chan_id, socp_event_cb event_cb)
{
	return 0;
}
static inline void bsp_socp_encdst_dsm_init(u32 enc_dst_chan_id, u32 b_enable)
{

}

static inline s32 bsp_clear_socp_buff(u32 src_chan_id)
{
    return 0;
}
static inline s32 bsp_socp_soft_free_encdst_chan(u32 enc_dst_chan_id)
{
    return 0;
}
static inline void socp_m3_init(void)
{
    return;
}

static inline void bsp_SocpEncDstQueryIntInfo(u32 *trf_info, u32 *thrh_ovf_info)
{
    return;
}

static inline void bsp_clear_socp_encdst_int_info(void)
{
    return;
}

static inline s32 socp_dst_channel_enable(u32 dst_chan_id)
{
    return 0;
}

static inline s32 socp_dst_channel_disable(u32 dst_chan_id)
{
    return 0;
}

static inline s32 bsp_socp_set_rate_ctrl(DRV_DIAG_RATE_STRU *p_rate_ctrl)
{
    return 0;
}
static inline s32 bsp_socp_dst_trans_id_disable(u32 dst_chan_id)
{
    return 0;

}

#endif

/*****************************************************************************
* 函 数 名  : bsp_socp_register_compress
*
* 功能描述  : 压缩接口函数注册到socp中
*
* 输入参数  : 压缩接口参数
*
* 输出参数  : 无
*
* 返 回 值  : BSP_S32 BSP_OK:成功 BSP_ERROR:失败
*****************************************************************************/

s32 bsp_socp_register_compress(socp_compress_ops_stru *ops);

/*****************************************************************************
* 函 数 名  : bsp_deflate_get_log_ind_mode
*
* 功能描述  : 获取当前压缩上报模式接口
*
* 输入参数  : 上报模式
*
* 输出参数  : 无
*
* 返 回 值  : BSP_S32 BSP_OK:成功 BSP_ERROR:失败
*****************************************************************************/
s32  bsp_deflate_get_log_ind_mode(u32 *log_ind_mode);
/*****************************************************************************
* 函 数 名  : bsp_deflate_cfg_ind_mode
*
* 功能描述  : 压缩上报模式接口
*
* 输入参数  : 上报模式
*
* 输出参数  : 无
*
* 返 回 值  : BSP_S32 BSP_OK:成功 BSP_ERROR:失败
*****************************************************************************/

s32 bsp_deflate_set_ind_mode(SOCP_IND_MODE_ENUM mode);
/*****************************************************************************
* 函 数 名  : bsp_socp_compress_disable
*
* 功能描述  : 压缩使能接口
*
* 输入参数  : 目的通道号
*
* 输出参数  : 无
*
* 返 回 值  : BSP_S32 BSP_OK:成功 BSP_ERROR:失败
*****************************************************************************/
s32 bsp_socp_compress_disable(u32 dst_chan_id);
/*****************************************************************************
* 函 数 名  : bsp_socp_compress_enable
*
* 功能描述  : 压缩使能接口
*
* 输入参数  : 目的通道号
*
* 输出参数  : 无
*
* 返 回 值  : BSP_S32 BSP_OK:成功 BSP_ERROR:失败
*****************************************************************************/
s32 bsp_socp_compress_enable(u32 dst_chan_id);

s32 bsp_socp_set_cfg_ind_mode(SOCP_IND_MODE_ENUM mode);
s32 bsp_socp_get_cfg_ind_mode(u32 *Cfg_ind_mode);
s32 bsp_socp_set_cps_ind_mode(DEFLATE_IND_COMPRESSS_ENUM mode);
s32 bsp_socp_get_cps_ind_mode(u32 *cps_ind_mode);
s32 bsp_socp_compress_status(void);

#ifdef __cplusplus
}
#endif

#endif /* end of _BSP_SOCP_H*/


