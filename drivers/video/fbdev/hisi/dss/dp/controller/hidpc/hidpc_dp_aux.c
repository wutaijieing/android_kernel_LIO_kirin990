/*
 * Copyright (c) 2016 Synopsys, Inc.
 *
 * Synopsys DP TX Linux Software Driver and documentation (hereinafter,
 * "Software") is an Unsupported proprietary work of Synopsys, Inc. unless
 * otherwise expressly agreed to in writing between Synopsys and you.
 *
 * The Software IS NOT an item of Licensed Software or Licensed Product under
 * any End User Software License Agreement or Agreement for Licensed Product
 * with Synopsys or any supplement thereto. You are permitted to use and
 * redistribute this Software in source and binary forms, with or without
 * modification, provided that redistributions of source code must retain this
 * notice. You may not view, use, disclose, copy or distribute this file or
 * any information contained herein except pursuant to this license grant from
 * Synopsys. If you do not agree with this notice, including the disclaimer
 * below, then you are not authorized to use the Software.
 *
 * THIS SOFTWARE IS BEING DISTRIBUTED BY SYNOPSYS SOLELY ON AN "AS IS" BASIS
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE HEREBY DISCLAIMED. IN NO EVENT SHALL SYNOPSYS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

/*
 * Copyright (c) 2017 Hisilicon Tech. Co., Ltd. Integrated into the Hisilicon display system.
 */

#include "hidpc_dp_aux.h"
#include "hidpc_dp_core.h"
#include "hidpc_reg.h"
#include "hisi_dp.h"
#include "hisi_fb.h"
#include "hisi_fb_def.h"

#define DPTX_NO_DEBUG_REG
#if defined(CONFIG_HISI_FB_V510) || defined(CONFIG_HISI_FB_V600)
static int dptx_handle_aux_reply(struct dp_ctrl *dptx)
{
	uint32_t auxsts;
	uint32_t status;
	uint32_t auxm;
	uint32_t br;
	int count;

	count = 0;
	while (1) {
		auxsts = dptx_readl(dptx, DPTX_AUX_STS);

		if (!(auxsts & DPTX_AUX_STS_REPLY_RECEIVED))
			break;

		count++;
		if (count > 5000)
			return -ETIMEDOUT;

		udelay(1);
	};

	/*lint -e648*/
	auxsts = dptx_readl(dptx, DPTX_AUX_STS);

	status = (auxsts & DPTX_AUX_STS_STATUS_MASK) >>
		DPTX_AUX_STS_STATUS_SHIFT;

	auxm = (auxsts & DPTX_AUX_STS_AUXM_MASK) >>
		DPTX_AUX_STS_AUXM_SHIFT;
	br = (auxsts & DPTX_AUX_STS_BYTES_READ_MASK) >>
		DPTX_AUX_STS_BYTES_READ_SHIFT;
	/*lint -e648*/
	HISI_FB_DEBUG("[DP] 0x%08x: sts=%d, auxm=%d, br=%d, replyrcvd=%d, replyerr=%d, timeout=%d, disconn=%d\n",
		      auxsts, status, auxm, br,
		      !!(auxsts & DPTX_AUX_STS_REPLY_RECEIVED),
		      !!(auxsts & DPTX_AUX_STS_REPLY_ERR),
		      !!(auxsts & DPTX_AUX_STS_TIMEOUT),
		      !!(auxsts & DPTX_AUX_STS_SINK_DWA));

	switch (status) {
	case DPTX_AUX_STS_STATUS_ACK:
		HISI_FB_DEBUG("[DP] DPTX_AUX_STS_STATUS_ACK\n");
		break;
	case DPTX_AUX_STS_STATUS_NACK:
		HISI_FB_DEBUG("[DP] DPTX_AUX_STS_STATUS_NACK\n");
		break;
	case DPTX_AUX_STS_STATUS_DEFER:
		HISI_FB_DEBUG("[DP] DPTX_AUX_STS_STATUS_DEFER\n");
		break;
	case DPTX_AUX_STS_STATUS_I2C_NACK:
		HISI_FB_DEBUG("[DP] DPTX_AUX_STS_STATUS_I2C_NACK\n");
		break;
	case DPTX_AUX_STS_STATUS_I2C_DEFER:
		HISI_FB_DEBUG("[DP] DPTX_AUX_STS_STATUS_I2C_DEFER\n");
		break;
	default:
		HISI_FB_ERR("[DP] Invalid AUX status 0x%x\n", status);
		break;
	}

	dptx->aux.data[0] = dptx_readl(dptx, DPTX_AUX_DATA0);
	dptx->aux.data[1] = dptx_readl(dptx, DPTX_AUX_DATA1);
	dptx->aux.data[2] = dptx_readl(dptx, DPTX_AUX_DATA2);
	dptx->aux.data[3] = dptx_readl(dptx, DPTX_AUX_DATA3);
	dptx->aux.sts = auxsts;

	return 0;
}
#else
static int dptx_handle_aux_reply(struct dp_ctrl *dptx)
{
	int retval;

	retval = dptx_wait(dptx,
			(atomic_read(&dptx->aux.event) ||
			atomic_read(&dptx->aux.abort)),
			DPTX_AUX_TIMEOUT);

	atomic_set(&dptx->aux.event, 0);

	return retval;
}
#endif
/*lint -save -e* */
static void dptx_aux_clear_data(struct dp_ctrl *dptx)
{
	hisi_check_and_no_retval((dptx == NULL), ERR, "[DP] NULL Pointer\n");

	dptx_writel(dptx, DPTX_AUX_DATA0, 0);
	dptx_writel(dptx, DPTX_AUX_DATA1, 0);
	dptx_writel(dptx, DPTX_AUX_DATA2, 0);
	dptx_writel(dptx, DPTX_AUX_DATA3, 0);
}

static int dptx_aux_read_data(struct dp_ctrl *dptx, uint8_t *bytes, uint32_t len)
{
	uint32_t i;
	uint32_t *data = NULL;

	hisi_check_and_return((dptx == NULL), -EINVAL, ERR, "[DP] dptx is NULL\n");
	hisi_check_and_return((bytes == NULL), -EINVAL, ERR, "[DP] bytes is NULL\n");

	data = dptx->aux.data;
	for (i = 0; i < len; i++) {
		if ((i / 4) > 3)
			return -EINVAL;

		bytes[i] = (data[i / 4] >> ((i % 4) * 8)) & 0xff;
	}

	return len;
}

static int dptx_aux_write_data(struct dp_ctrl *dptx, uint8_t const *bytes,
	uint32_t len)
{
	uint32_t i;
	uint32_t data[4];

	memset(data, 0, sizeof(uint32_t) * 4);

	hisi_check_and_return((dptx == NULL), -EINVAL, ERR, "[DP] dptx is NULL\n");
	hisi_check_and_return((bytes == NULL), -EINVAL, ERR, "[DP] bytes is NULL\n");

	for (i = 0; i < len; i++) {
		if ((i / 4) > 3)
			return -EINVAL;

		data[i / 4] |= (bytes[i] << ((i % 4) * 8));
	}

	dptx_writel(dptx, DPTX_AUX_DATA0, data[0]);
	dptx_writel(dptx, DPTX_AUX_DATA1, data[1]);
	dptx_writel(dptx, DPTX_AUX_DATA2, data[2]);
	dptx_writel(dptx, DPTX_AUX_DATA3, data[3]);

	return len;
}

static uint32_t dptx_get_aux_cmd(bool rw, bool i2c, bool mot,
	bool addr_only, uint32_t addr, uint32_t len)
{
	uint32_t type;
	uint32_t auxcmd;

	type = rw ? DPTX_AUX_CMD_TYPE_READ : DPTX_AUX_CMD_TYPE_WRITE;

	if (!i2c)
		type |= DPTX_AUX_CMD_TYPE_NATIVE;

	if (i2c && mot)
		type |= DPTX_AUX_CMD_TYPE_MOT;

	auxcmd = ((type << DPTX_AUX_CMD_TYPE_SHIFT) |
		  (addr << DPTX_AUX_CMD_ADDR_SHIFT) |
		  ((len - 1) << DPTX_AUX_CMD_REQ_LEN_SHIFT));

	if (addr_only)
		auxcmd |= DPTX_AUX_CMD_I2C_ADDR_ONLY;

	return auxcmd;
}

static int check_aux_status(struct dp_ctrl *dptx, bool *try_again)
{
	uint32_t status;
	uint32_t br;

	status = (dptx->aux.sts & DPTX_AUX_STS_STATUS_MASK) >>
		DPTX_AUX_STS_STATUS_SHIFT;

	br = (dptx->aux.sts & DPTX_AUX_STS_BYTES_READ_MASK) >>
		DPTX_AUX_STS_BYTES_READ_SHIFT;

	switch (status) {
	case DPTX_AUX_STS_STATUS_ACK:
		HISI_FB_DEBUG("[DP] AUX Success\n");
		if (br == 0) {
			HISI_FB_ERR("[DP] BR=0, Retry\n");
			dptx_soft_reset(dptx, DPTX_SRST_CTRL_AUX);
			*try_again = true;
		}
		break;
	case DPTX_AUX_STS_STATUS_NACK:
	case DPTX_AUX_STS_STATUS_I2C_NACK:
		HISI_FB_INFO("[DP] AUX Nack\n");
		return -ECONNREFUSED;
	case DPTX_AUX_STS_STATUS_I2C_DEFER:
	case DPTX_AUX_STS_STATUS_DEFER:
		HISI_FB_INFO("[DP] AUX Defer\n");
		*try_again = true;
		break;
	default:
		HISI_FB_ERR("[DP] AUX Status Invalid\n");
		dptx_soft_reset(dptx, DPTX_SRST_CTRL_AUX);
		*try_again = true;
		break;
	}

	return 0;
}

int dptx_aux_rw(struct dp_ctrl *dptx, bool rw, bool i2c, bool mot,
	bool addr_only, uint32_t addr, uint8_t *bytes, uint32_t len)
{
	int retval;
	int tries = 0;
	uint32_t auxcmd;
	bool try_again = false;

	hisi_check_and_return((dptx == NULL), -EINVAL, ERR, "[DP] dptx is NULL\n");
	hisi_check_and_return((bytes == NULL), -EINVAL, ERR, "[DP] bytes is NULL\n");

again:
	mdelay(1);
	tries++;
	if (tries > 100) {
		HISI_FB_ERR("[DP] AUX exceeded retries\n");
		return -EAGAIN;
	}

	HISI_FB_DEBUG("[DP] device addr=0x%08x, len=%d, try=%d\n",
					addr, len, tries);

	if ((len > 16) || (len == 0)) {
		HISI_FB_ERR("[DP] AUX read/write len must be 1-15, len=%d\n", len);
		return -EINVAL;
	}

	dptx_aux_clear_data(dptx);

	if (!rw)
		dptx_aux_write_data(dptx, bytes, len);

	auxcmd = dptx_get_aux_cmd(rw, i2c, mot, addr_only, addr, len);
	dptx_writel(dptx, DPTX_AUX_CMD, auxcmd);

	retval = dptx_handle_aux_reply(dptx);
	if (retval == -ETIMEDOUT) {
		HISI_FB_ERR("AUX timed out\n");
		return retval;
	}
	if (retval == -ESHUTDOWN) {
		HISI_FB_INFO("[DP] AUX aborted on driver shutdown\n");
		return retval;
	}

	if (atomic_read(&dptx->aux.abort)) {
		HISI_FB_INFO("[DP] AUX aborted\n");
		return -ETIMEDOUT;
	}

	try_again = false;
	retval = check_aux_status(dptx, &try_again);
	if (retval)
		return retval;
	if (try_again)
		goto again;

	if (rw)
		dptx_aux_read_data(dptx, bytes, len);

	return 0;
}

/*lint -restore*/
