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


/*lint --e{537} */
#include <bsp_pm_om.h>
#include "icc_core.h"
#include "icc_platform.h"
#include <securec.h>

#undef THIS_MODU
#define THIS_MODU mod_icc
struct icc_pm_om {
    struct icc_channel_packet packet;
    read_cb_func read_cb;
    void *read_context;
};

struct icc_dbg g_icc_dbg = {0};

extern struct icc_control g_icc_ctrl;

s32 bsp_icc_debug_register(u32 channel_id, funcptr_1 debug_routine, int param)
{
    struct icc_channel_vector *vector = NULL;

    if ((GET_CHN_ID(channel_id) >= ICC_CHN_ID_MAX) || g_icc_ctrl.channels[GET_CHN_ID(channel_id)] == NULL ||
        (GET_FUNC_ID(channel_id) >= g_icc_ctrl.channels[GET_CHN_ID(channel_id)]->func_size)) {
        icc_print_error("invalide parameter! channel_id=0x%x\n", channel_id);
        return ICC_INVALID_PARA;
    }
    /*lint --e{409} */
    if (g_icc_ctrl.channels[GET_CHN_ID(channel_id)]->vector == NULL) {
        icc_print_error("vector NULL\n");
        return ICC_NULL_PTR;
    }
    vector = &(g_icc_ctrl.channels[GET_CHN_ID(channel_id)]->vector[GET_FUNC_ID(channel_id)]);
    vector->pm_debug.debug_routine = debug_routine;
    vector->pm_debug.para = param;

    return ICC_OK;
}

void run_icc_pm_debug_callback(void)
{
    u32 i = 0, j = 0;
    struct icc_channel *channel = NULL;
    struct icc_channel_vector *vector = NULL;
    for (i = 0; i < ICC_CHN_ID_MAX; i++) {
        channel = g_icc_ctrl.channels[i];
        if (channel == NULL) {
            continue;
        }
        for (j = 0; j < channel->func_size; j++) {
            vector = &(channel->vector[j]);
            if ((vector != NULL) && (vector->pm_debug.debug_routine != NULL)) /*lint !e774 */
            {
                (void)vector->pm_debug.debug_routine(vector->pm_debug.para);
            }
        }
    }
}


static inline void icc_pm_om_log(struct icc_channel_packet *packet, read_cb_func read_cb, void *read_context)
{
    struct icc_pm_om pm_om;

    errno_t err = EOK;
    err = memset_s((void *)&pm_om, sizeof(pm_om), 0, sizeof(pm_om));
    if (err != EOK) {
        icc_print_error("<icc_pm_om_log> memset_s failed, err = %d\n", err);
        return;
    }
    err = memcpy_s((void *)&pm_om.packet, sizeof(*packet), (void *)packet, sizeof(*packet));
    if (err != EOK) {
        icc_print_error("<icc_pm_om_log> memcpy_s failed, err = %d\n", err);
        return;
    }
    pm_om.read_cb = read_cb;
    pm_om.read_context = read_context;
    (void)bsp_pm_log(PM_OM_AICC, sizeof(pm_om), (void *)&pm_om);
}

int icc_channel_packet_dump(struct icc_channel_packet *packet)
{
    struct icc_channel *channel = NULL;
    const char *name = NULL;
    struct icc_channel_vector *vector = NULL;
    read_cb_func read_cb = NULL;
    void *read_context = NULL;
    int ret = ICC_OK;

    if ((GET_CHN_ID(packet->channel_id) >= ICC_CHN_ID_MAX) || (!g_icc_ctrl.channels[GET_CHN_ID(packet->channel_id)]) ||
        (GET_FUNC_ID(packet->channel_id) >= g_icc_ctrl.channels[GET_CHN_ID(packet->channel_id)]->func_size)) {
        icc_print_error("para err,chan_id=0x%x\n", GET_CHN_ID(packet->channel_id));
        return ICC_ERR;
    }
    channel = g_icc_ctrl.channels[GET_CHN_ID(packet->channel_id)];
    name = channel->name;
    vector = &channel->vector[GET_FUNC_ID(packet->channel_id)];
    read_cb = vector->read_cb;
    read_context = vector->read_context;

    if ((0 == g_icc_dbg.msg_print_sw) && (0 == g_icc_ctrl.wake_up_flag) && (0 == g_icc_ctrl.sleep_flag)) {
        return ret;
    }

    if (1 == g_icc_ctrl.wake_up_flag) {
        g_icc_ctrl.wake_up_flag = 0;
        icc_pm_om_log(packet, read_cb, read_context);
        printk(KERN_ERR "[C SR]icc recv msg dump\n");
    }

    if (1 == g_icc_ctrl.sleep_flag) {
        g_icc_ctrl.sleep_flag = 0;
    }

    icc_print_info("<icc[%s] begin>\n", name);
    icc_print_info("channel_id  : 0x%x\n", packet->channel_id);
    icc_print_info("len         : 0x%x\n", packet->len);
    icc_print_info("src_cpu_id  : 0x%x\n", packet->src_cpu_id);
    icc_print_info("timestamp   : 0x%x\n", packet->timestamp);
    icc_print_info("task_id     : 0x%x\n", packet->task_id);
    icc_print_info("read_cb     : %lx\n", (long unsigned int)(uintptr_t)read_cb);
    icc_print_info("read_context: %lx\n", (long unsigned int)(uintptr_t)read_context);
    icc_print_info("<icc[%s] end>\n", name);
    return ret;
}

void icc_packet_print(struct icc_channel_packet *packet)
{
    icc_print_info("<icc_packet begin>\n");
    icc_print_info("channel_id  : 0x%x\n", packet->channel_id);
    icc_print_info("len         : 0x%x\n", packet->len);
    icc_print_info("src_cpu_id  : 0x%x\n", packet->src_cpu_id);
    icc_print_info("timestamp   : 0x%x\n", packet->timestamp);
    icc_print_info("task_id     : 0x%x\n", packet->task_id);
    icc_print_info("<icc_packet end\n");
}

void icc_dbg_print_sw(u32 sw)
{
    g_icc_dbg.msg_print_sw = sw;
}

void icc_dbg_info_print(const char *fifo_name, u32 channel_id, u8 *data, u32 data_len)
{
    if (g_icc_dbg.msg_print_sw) {
        icc_print_error("%s: channel[0x%x], msg[0x%lx]: 0x%x, len[%d]:\n", fifo_name, channel_id, (long unsigned int)(uintptr_t)data, *(u32 *)data,
                        data_len);
    }
}

/* 如果在中断中，则返回0 */
u32 icc_taskid_get(void)
{
    if (in_interrupt()) /*lint !e737 */
    {
        return 0;
    } else {
        return (u32)(current->pid);
    }
}

void icc_msg_queue_init(struct icc_msg_fifo *queue)
{
    queue->front = queue->rear = 0;
    queue->size = 0;
}

static void icc_send_msg_queue_in(struct icc_msg_fifo *queue, struct icc_uni_msg_info *msg)
{
    unsigned int dest_max = (unsigned int)sizeof(struct icc_uni_msg_info);
    errno_t err = EOK;
    /* when full, abandon old message */
    if ((queue->front == queue->rear) && (queue->size == ICC_STAT_MSG_NUM)) {
        queue->front = (queue->front + 1) % ICC_STAT_MSG_NUM;
        queue->size = queue->size - 1;
    }

    err = memcpy_s((void *)(&queue->msg[queue->rear]), dest_max, msg, sizeof(*msg));
    if (err != EOK) {
        icc_print_error("<icc_send_msg_queue_in> memcpy_s failed, err = %d\n", err);
        return;
    }
    queue->rear = (queue->rear + 1) % ICC_STAT_MSG_NUM;
    queue->size = queue->size + 1;
}

static void icc_recv_msg_queue_in(struct icc_msg_fifo_recv *queue, struct icc_recv_msg_info *msg)
{
    unsigned int dest_max = (unsigned int)sizeof(struct icc_recv_msg_info);
    errno_t err = EOK;
    /* when full, abandon old message */
    if ((queue->front == queue->rear) && (queue->size == ICC_STAT_MSG_NUM)) {
        queue->front = (queue->front + 1) % ICC_STAT_MSG_NUM;
        queue->size = queue->size - 1;
    }

    err = memcpy_s((void *)(&queue->msg[queue->rear]), dest_max, msg, sizeof(*msg));
    if (err != EOK) {
        icc_print_error("<icc_recv_msg_queue_in> memcpy_s failed, err = %d\n", err);
        return;
    }
    queue->rear = (queue->rear + 1) % ICC_STAT_MSG_NUM;
    queue->size = queue->size + 1;
}


void icc_dump_hook(void)
{ /*lint --e{539}*/
    char *dump_buf = g_icc_dbg.dump_buf_addr;
    u32 dump_size = g_icc_dbg.dump_buf_size;
    errno_t err = EOK;

    /* do nothing, if om init fail */
    if (0 == dump_size || NULL == dump_buf) {
        return;
    }

    err = memcpy_s((void *)g_icc_dbg.dump_buf_addr, dump_size, (void *)&g_icc_dbg.msg_stat,
                   sizeof(struct icc_msg_info));
    if (err != EOK) {
        return;
    }

    return;
}

void icc_dump_init(void)
{
    /* reg the dump callback to om */
    if (BSP_ERROR == bsp_dump_register_hook("ICC", (dump_hook)icc_dump_hook)) {
        goto err_ret; /*lint !e801 */
    }

    g_icc_dbg.dump_buf_addr = (char *)bsp_dump_register_field(ICC_DUMP_SAVE_MOD, "ICC",ICC_DUMP_EXT_SIZE,
                                                              0x0001);
    if (g_icc_dbg.dump_buf_addr == NULL) {
        goto err_ret; /*lint !e801 */
    }
    g_icc_dbg.dump_buf_size = ICC_DUMP_EXT_SIZE;

    return;

err_ret:
    g_icc_dbg.dump_buf_addr = NULL;
    g_icc_dbg.dump_buf_size = 0;
    icc_print_error("try to use dump fail!\n");
    return;
}

s32 icc_debug_channel_info_init(struct icc_channel *icc_channel)
{
    struct icc_channel_info *channel = NULL;
    struct icc_channel_stat_info *sub_channel = NULL;

    channel = (struct icc_channel_info *)osl_malloc(sizeof(struct icc_channel_info));
    if (channel == NULL) {
        icc_print_error("malloc icc_channel_info memory fail!\n");
        return (s32)ICC_ERR;
    }

    /* 收发子通道一同分配 */
    sub_channel = (struct icc_channel_stat_info *)osl_malloc(sizeof(struct icc_channel_stat_info) *
                                                             icc_channel->func_size * 2); /*lint !e838 */
    if (sub_channel == NULL) {
        osl_free((void *)channel);
        icc_print_error("malloc icc_channel_stat_info memory fail!\n");
        return (s32)ICC_ERR;
    }

    /* channel init */
    (void)memset_s((void *)channel, sizeof(struct icc_channel_info), 0, sizeof(struct icc_channel_info));
    channel->id = icc_channel->id;
    channel->recv.func_size = icc_channel->func_size;
    channel->send.func_size = icc_channel->func_size;

    /* sub channel init */
    (void)memset_s((void *)sub_channel, sizeof(struct icc_channel_stat_info) * icc_channel->func_size * 2, 0,
                   sizeof(struct icc_channel_stat_info) * icc_channel->func_size * 2); /*lint !e665 */
    channel->send.sub_chn = sub_channel;
    channel->recv.sub_chn = &(sub_channel[icc_channel->func_size]);

    g_icc_dbg.channel_stat[channel->id] = channel;

    return ICC_OK;
}

s32 icc_debug_init(u32 channel_num)
{
    u32 i = 0;
    s32 ret;

    (void)memset_s(&g_icc_dbg, sizeof(struct icc_dbg), 0, sizeof(struct icc_dbg));

    for (i = 0; i < channel_num; i++) /*lint !e838 */
    {
        /* 使用g_icc_ctrl而不是g_icc_init_info，因为测试编进去以后，统计通道不需要再处理 */
        if (!g_icc_ctrl.channels[i]) {
            continue;
        }

        ret = icc_debug_channel_info_init(g_icc_ctrl.channels[i]);
        if (ret == ICC_ERR) {
            icc_print_error("malloc icc_channel_info memory fail!\n");
            return (s32)ICC_ERR;
        }
    }

    icc_dump_init();

    return ICC_OK;
}

static void icc_channel_msg_stat(struct icc_channel_stat_info *channel, u32 msg_len, u32 recv_task_id)
{
    if (channel == NULL) {
        return;
    }
    channel->task_id = recv_task_id;
    channel->sum_len += msg_len;
    channel->sum_num++;
}

void icc_debug_in_isr(void)
{
    g_icc_dbg.ipc_int_cnt++;
}

void icc_debug_before_send(struct icc_channel_packet *packet)
{
    /* 发送数据包的任务ID及时间戳 */
    packet->timestamp = bsp_get_slice_value();
    packet->task_id = icc_taskid_get();
}

void icc_debug_after_send(struct icc_channel *channel, struct icc_channel_packet *packet, u8 *data)
{
    struct icc_uni_msg_info msg_tx = {0};
    msg_tx.duration_prev = packet->timestamp;
    msg_tx.send_task_id = packet->task_id;
    msg_tx.write_pos = channel->fifo_send->write;
    msg_tx.read_pos = channel->fifo_send->read;
    msg_tx.len = packet->len;
    msg_tx.channel_id = packet->channel_id;
    msg_tx.recv_task_id = 0;
    msg_tx.duration_post = bsp_get_slice_value();

    if ((GET_CHN_ID(msg_tx.channel_id) >= ICC_CHN_ID_MAX) || (!g_icc_ctrl.channels[GET_CHN_ID(msg_tx.channel_id)]) ||
        (GET_FUNC_ID(msg_tx.channel_id) >= g_icc_ctrl.channels[GET_CHN_ID(msg_tx.channel_id)]->func_size)) {
        icc_print_error("para err, chan_id=0x%x.\n", msg_tx.channel_id);
        return;
    }
    icc_send_msg_queue_in(&(g_icc_dbg.msg_stat.send), &msg_tx);
    icc_channel_msg_stat(&(g_icc_dbg.channel_stat[GET_CHN_ID(msg_tx.channel_id)]->send.total), msg_tx.len,
                         msg_tx.send_task_id);
    icc_channel_msg_stat(
        &(g_icc_dbg.channel_stat[GET_CHN_ID(msg_tx.channel_id)]->send.sub_chn[GET_FUNC_ID(msg_tx.channel_id)]),
        msg_tx.len, msg_tx.send_task_id);

    icc_dbg_info_print("fifo_send", msg_tx.channel_id, data, packet->len);
}

static struct icc_recv_msg_info g_msg_rx = {0};

void icc_debug_before_recv(struct icc_channel_packet *pkg_header)
{
    g_msg_rx.before_recv_slice = bsp_get_slice_value();
    g_msg_rx.channel_id = pkg_header->channel_id;
    g_msg_rx.send_task_id = pkg_header->task_id;
    g_msg_rx.len = pkg_header->len;
    g_icc_dbg.send_cpu_id = pkg_header->src_cpu_id;
    (void)icc_channel_packet_dump(pkg_header);
}

void icc_debug_in_read_cb(u32 channel_id, u8 *buf, u32 buf_len, u32 read_ptr, u32 write_ptr)
{
    g_msg_rx.in_read_cb_slice = bsp_get_slice_value();
    g_msg_rx.recv_task_id = icc_taskid_get();
    g_msg_rx.write_pos = write_ptr;
    g_msg_rx.read_pos = read_ptr;
    icc_dbg_info_print("fifo_recv", channel_id, buf, buf_len);
}

#define ICC_PERF_COUNT_MAX (10000)
void icc_debug_after_recv(struct icc_channel_packet *pkg_header)
{
    u32 delta_slice = 0;
    u32 channel_id = pkg_header->channel_id;

    g_msg_rx.after_recv_slice = bsp_get_slice_value();
    delta_slice = get_timer_slice_delta(g_msg_rx.before_recv_slice, g_msg_rx.after_recv_slice);
    if (delta_slice > g_msg_rx.perf.max_readcb_consume_slice) {
        g_msg_rx.perf.max_readcb_consume_slice = delta_slice;
        g_msg_rx.perf.max_readcb_channel_id = channel_id;
    }

    if (g_msg_rx.perf.cnt >= ICC_PERF_COUNT_MAX) {
        g_msg_rx.perf.cnt = 0;
        g_msg_rx.perf.sum = 0;
    }

    delta_slice = get_timer_slice_delta(pkg_header->timestamp, g_msg_rx.after_recv_slice);
    g_msg_rx.perf.cnt++;
    g_msg_rx.perf.sum += delta_slice;
    g_msg_rx.perf.avg = g_msg_rx.perf.sum / g_msg_rx.perf.cnt;

    if ((GET_CHN_ID(channel_id) >= ICC_CHN_ID_MAX) || (!g_icc_ctrl.channels[GET_CHN_ID(channel_id)]) ||
        (GET_FUNC_ID(channel_id) >= g_icc_ctrl.channels[GET_CHN_ID(channel_id)]->func_size)) {
        icc_print_error("para err, chan_id=0x%x.\n", channel_id);
        return;
    }
    icc_recv_msg_queue_in(&(g_icc_dbg.msg_stat.recv), &g_msg_rx);
    icc_channel_msg_stat(&(g_icc_dbg.channel_stat[GET_CHN_ID(channel_id)]->recv.total), g_msg_rx.len,
                         g_msg_rx.recv_task_id);
    icc_channel_msg_stat(&(g_icc_dbg.channel_stat[GET_CHN_ID(channel_id)]->recv.sub_chn[GET_FUNC_ID(channel_id)]),
                         g_msg_rx.len, g_msg_rx.recv_task_id);
}

