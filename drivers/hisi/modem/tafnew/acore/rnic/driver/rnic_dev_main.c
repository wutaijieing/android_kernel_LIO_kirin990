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

#include <linux/module.h>
#include <linux/errno.h>
#include <linux/skbuff.h>
#include <linux/etherdevice.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/version.h>
#include <net/sock.h>
#include "securec.h"
#include "rnic_dev.h"
#include "rnic_dev_debug.h"
#include "rnic_dev_config.h"
#include "rnic_dev_handle.h"
#include "rnic_policy_manage.h"


unsigned int rnic_dev_log_level = RNIC_DEV_LOG_LVL_HIGH | RNIC_DEV_LOG_LVL_ERR;

struct rnic_dev_context_s rnic_dev_context;

static const uint8_t rnic_dev_dst_mac_base[ETH_ALEN] = {
	0x58,0x02,0x03,0x04,0x05,0x06
};

static const uint8_t rnic_dev_src_mac_base[ETH_ALEN] = {
	0x00,0x11,0x09,0x64,0x01,0x01
};

static const struct rnic_dev_name_param_s rnic_dev_name_param_table[] = {
	RNIC_DEV_NAME_ELEMENT(RMNET0),
	RNIC_DEV_NAME_ELEMENT(RMNET1),
	RNIC_DEV_NAME_ELEMENT(RMNET2),
	RNIC_DEV_NAME_ELEMENT(RMNET3),
	RNIC_DEV_NAME_ELEMENT(RMNET4),
	RNIC_DEV_NAME_ELEMENT(RMNET5),
	RNIC_DEV_NAME_ELEMENT(RMNET6),
	RNIC_DEV_NAME_ELEMENT(RMNET7),
	RNIC_DEV_NAME_ELEMENT(RMNET8),
	RNIC_DEV_NAME_ELEMENT(RMNET9),
	RNIC_DEV_NAME_ELEMENT(RMNET10),
	RNIC_DEV_NAME_ELEMENT(RMNET11),
#if ((FEATURE_IMS == FEATURE_ON) && (FEATURE_ACPU_PHONE_MODE == FEATURE_ON))
	RNIC_DEV_NAME_ELEMENT(RMNET_IMS00),
	RNIC_DEV_NAME_ELEMENT(RMNET_IMS10),
	RNIC_DEV_NAME_ELEMENT(RMNET_EMC0),
	RNIC_DEV_NAME_ELEMENT(RMNET_EMC1),
	RNIC_DEV_NAME_ELEMENT(RMNET_R_IMS00),
	RNIC_DEV_NAME_ELEMENT(RMNET_R_IMS01),
	RNIC_DEV_NAME_ELEMENT(RMNET_R_IMS10),
	RNIC_DEV_NAME_ELEMENT(RMNET_R_IMS11),
	RNIC_DEV_NAME_ELEMENT(RMNET_TUN00),
	RNIC_DEV_NAME_ELEMENT(RMNET_TUN01),
	RNIC_DEV_NAME_ELEMENT(RMNET_TUN02),
	RNIC_DEV_NAME_ELEMENT(RMNET_TUN03),
	RNIC_DEV_NAME_ELEMENT(RMNET_TUN04),
	RNIC_DEV_NAME_ELEMENT(RMNET_TUN10),
	RNIC_DEV_NAME_ELEMENT(RMNET_TUN11),
	RNIC_DEV_NAME_ELEMENT(RMNET_TUN12),
	RNIC_DEV_NAME_ELEMENT(RMNET_TUN13),
	RNIC_DEV_NAME_ELEMENT(RMNET_TUN14)
#endif /* FEATURE_IMS == FEATURE_ON && FEATURE_ACPU_PHONE_MODE == FEATURE_ON */
};

/*
 * rnic_check_rmnet_data - Check if netdevice is used for data service.
 * @devid: id of netdeivce
 */
STATIC bool rnic_check_rmnet_data(uint8_t devid)
{
	if (devid <= RNIC_DEV_ID_DATA_MAX)
		return true;

	return false;
}

/*
 * rnic_check_rmnet_iwlan - Check if netdevice is used for iwlan service.
 * @devid: id of netdeivce
 */
STATIC bool rnic_check_rmnet_iwlan(uint8_t devid)
{
#if ((FEATURE_ON == FEATURE_IMS) && (FEATURE_ON == FEATURE_ACPU_PHONE_MODE))
	if (devid >= RNIC_DEV_ID_RMNET_R_IMS00 &&
	    devid <= RNIC_DEV_ID_RMNET_R_IMS11)
		return true;
#endif

	return false;
}

/*
 * rnic_dev_stop - Open netdevice.
 * @netdev: netdevice
 *
 * Return 0 on success, negative on failure.
 */
STATIC int rnic_dev_open(struct net_device *netdev)
{
	struct rnic_dev_priv_s *priv = netdev_priv(netdev);

	if (priv->napi_enable)
		napi_enable(&priv->napi);

	netif_start_queue(netdev);

	return 0;
}

/*
 * rnic_dev_stop - Stop netdevice.
 * @netdev: netdevice
 *
 * Return 0 on success, negative on failure.
 */
STATIC int rnic_dev_stop(struct net_device *netdev)
{
	struct rnic_dev_priv_s *priv = netdev_priv(netdev);

	netif_stop_queue(netdev);

	if (priv->napi_enable) {
		napi_disable(&priv->napi);
		skb_queue_purge(&priv->napi_queue);
	}

	return 0;
}

/*
 * rnic_dev_change_mtu - Transmit skb from network.
 * @skb: sk buffer
 * @netdev: netdevice
 *
 * Return NETDEV_TX_OK.
 */
STATIC netdev_tx_t rnic_dev_start_xmit(struct sk_buff *skb,
				       struct net_device *netdev)
{
	struct rnic_dev_priv_s *priv = netdev_priv(netdev);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 0))
	/* The value is a bit-shift of 1 second,
	 * so 4 is ~64ms of queued data. Only affects local TCP
	 * sockets.
	 */
	sk_pacing_shift_update(skb->sk, 4);
#endif

	rnic_tx_hanlde(priv, skb);

	return NETDEV_TX_OK;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0))
/*
 * rnic_dev_change_mtu - Change MTU of netdevice.
 * @netdev: netdevice
 * @new_mtu: new MTU
 *
 * Return 0 on success, negative on failure.
 */
STATIC int rnic_dev_change_mtu(struct net_device *netdev, int new_mtu)
{
	struct rnic_dev_priv_s *priv = netdev_priv(netdev);
	int max_mtu;

	max_mtu = rnic_check_rmnet_iwlan(priv->devid) ?
		  RNIC_MAX_MTU : RNIC_DEFAULT_MTU;
	if (new_mtu > max_mtu)
		return -EINVAL;

	netdev->mtu = (unsigned int)new_mtu;

	return 0;
}
#endif

/*
 * rnic_dev_get_stats - Get stats of netdeivce.
 * @netdev: netdevice
 */
STATIC struct net_device_stats *rnic_dev_get_stats(struct net_device *netdev)
{
	struct rnic_dev_priv_s *priv = netdev_priv(netdev);

	return &priv->stats;
}

STATIC const struct net_device_ops rnic_dev_ops = {
	.ndo_open		= rnic_dev_open,
	.ndo_stop		= rnic_dev_stop,
	.ndo_start_xmit		= rnic_dev_start_xmit,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0))
	.ndo_change_mtu		= rnic_dev_change_mtu,
#endif
	.ndo_get_stats		= rnic_dev_get_stats,
};

/*
 * rnic_get_devid_by_name - Get netdeive structure.
 * @devid: id of netdevice
 */
struct net_device *rnic_get_netdev_by_devid(uint8_t devid)
{
	return rnic_dev_context.netdev[devid];
}

/*
 * rnic_get_priv - Get private info of netdeive.
 * @devid: id of netdevice
 */
struct rnic_dev_priv_s *rnic_get_priv(uint8_t devid)
{
	return rnic_dev_context.priv[devid];
}

/*
 * rnic_get_devid_by_name - Get id of netdeivce.
 * @name: name of netdevice
 * @devid: id of netdevice
 *
 * Return 0 on success, negative on failure.
 */
int rnic_get_devid_by_name(const char *name, uint8_t *devid)
{
	struct rnic_dev_priv_s *priv = NULL;
	struct net_device *netdev = NULL;

	if (name == NULL || devid == NULL) {
		RNIC_LOGE("invalid parameter.");
		return -EINVAL;
	}

	netdev = dev_get_by_name(&init_net, name);
	if (netdev == NULL) {
		RNIC_LOGE("netdev not found.");
		return -ENODEV;
	}

	priv = netdev_priv(netdev);
	*devid = priv->devid;
	dev_put(netdev);

	return 0;
}

/*
 * rnic_get_name_param - Get device name config.
 * @devid: id of netdevice
 */
STATIC const struct rnic_dev_name_param_s *rnic_get_name_param(uint8_t devid)
{
	const struct rnic_dev_name_param_s *name_param = NULL;
	uint8_t i;

	for (i = 0; i < ARRAY_SIZE(rnic_dev_name_param_table); i++) {
		if (rnic_dev_name_param_table[i].devid == devid) {
			name_param = &rnic_dev_name_param_table[i];
			break;
		}
	}

	return name_param;
}

/*
 * rnic_platform_remove - Driver probe function.
 * @pdev: platform device
 *
 * Return 0 on success, negative on failure.
 */
STATIC int rnic_platform_probe(struct platform_device *pdev)
{
	struct rnic_dev_context_s *dev_ctx = RNIC_DEV_CTX();
	uint8_t devid;

	dma_set_mask_and_coherent(&pdev->dev, DMA_BIT_MASK(64));//lint !e506 !e648 !e778

	for (devid = 0; devid < RNIC_DEV_ID_BUTT; devid++) {
		if (dev_ctx->priv[devid] != NULL)
			dev_ctx->priv[devid]->dev = &pdev->dev;
		else
			RNIC_LOGE("priv is null.");
	}

	return 0;
}

/*
 * rnic_platform_remove - Driver remove function.
 * @pdev: platform device
 *
 * Return 0 on success, negative on failure.
 */
STATIC int rnic_platform_remove(struct platform_device *pdev)
{
	return 0;
}

STATIC const struct of_device_id rnic_match_table[] = {
	{.compatible = "hisilicon,hisi-rnic", },
	{ },
};

STATIC struct platform_driver rnic_platform_driver = {
	.probe  = rnic_platform_probe,
	.remove = rnic_platform_remove,
	.driver = {
		.name = "hisi-rnic",
		.of_match_table = of_match_ptr(rnic_match_table),
	},
};

/*
 * rnic_create_netdev - Destory netdevice.
 */
STATIC void rnic_cleanup(void)
{
	struct rnic_dev_context_s *dev_ctx = RNIC_DEV_CTX();
	struct rnic_dev_priv_s *priv = NULL;
	struct net_device *netdev = NULL;
	uint32_t devid;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(4, 10, 0))
	rnic_cpu_hotplug_deinit();
#else
	rnic_cpuhp_deinit();
#endif

	platform_driver_unregister(&rnic_platform_driver);

	for (devid = 0; devid < RNIC_DEV_ID_BUTT; devid++) {
		netdev = dev_ctx->netdev[devid];
		if (netdev != NULL) {
			priv = netdev_priv(netdev);
			rnic_cpumasks_deinit(&priv->lb_policy);

			unregister_netdev(netdev);
			free_netdev(netdev);
		}

		dev_ctx->netdev[devid] = NULL;
		dev_ctx->priv[devid] = NULL;
	}

	dev_ctx->ready = false;
}

/*
 * rnic_create_netdev - Create netdevice.
 *
 * Return 0 on success, negative on failure.
 */
/*lint -save -e801*/
int rnic_create_netdev(void)
{
	struct rnic_dev_context_s *dev_ctx = RNIC_DEV_CTX();
	struct rnic_dev_priv_s *priv = NULL;
	struct net_device *netdev = NULL;
	const struct rnic_dev_name_param_s *name_param = NULL;
	uint8_t dst_mac[ETH_ALEN], src_mac[ETH_ALEN];
	int rc = 0;
	uint8_t devid;

	for (devid = 0; devid < RNIC_DEV_ID_BUTT; devid++) {
		name_param = rnic_get_name_param(devid);
		if (name_param == NULL) {
			rc = -ENODEV;
			goto err_name_param;
		}

		memcpy_s(dst_mac, ETH_ALEN, rnic_dev_dst_mac_base, ETH_ALEN);
		memcpy_s(src_mac, ETH_ALEN, rnic_dev_src_mac_base, ETH_ALEN);
		dst_mac[ETH_ALEN - 1] += devid;
		src_mac[ETH_ALEN - 1] += devid;

		netdev = alloc_etherdev(sizeof(struct rnic_dev_priv_s));
		if (netdev == NULL) {
			RNIC_LOGE("alloc etherdev fail, devid: %d.", devid);
			rc = -ENOMEM;
			goto err_alloc_netdev;
		}

		scnprintf(netdev->name, IFNAMSIZ, "%s%s",
			  rnic_dev_name_param_table[devid].prefix,
			  rnic_dev_name_param_table[devid].suffix);
		netdev->flags &= ~(IFF_BROADCAST | IFF_MULTICAST);
		netdev->mtu = RNIC_DEFAULT_MTU;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 10, 0))
		netdev->max_mtu = rnic_check_rmnet_iwlan(devid) ?
				  RNIC_MAX_MTU : RNIC_DEFAULT_MTU;
#endif
		memcpy_s(netdev->dev_addr, ETH_ALEN, dst_mac, ETH_ALEN);
		netdev->netdev_ops = &rnic_dev_ops;

		priv = (struct rnic_dev_priv_s *)netdev_priv(netdev);
		priv->netdev = netdev;
		priv->devid = devid;

		priv->napi_enable = false;
		priv->gro_enable  = false;
		priv->napi_weight = 16;
		priv->napi_queue_length = 10000;
		skb_queue_head_init(&priv->input_queue);
		skb_queue_head_init(&priv->napi_queue);
		skb_queue_head_init(&priv->rps_pend_queue);
		skb_queue_head_init(&priv->process_queue);
		netif_napi_add(netdev, &priv->napi, rnic_napi_poll, 16);


		memcpy_s(priv->v4_eth.h_dest, ETH_ALEN,
			 dst_mac, ETH_ALEN);
		memcpy_s(priv->v4_eth.h_source, ETH_ALEN,
			 src_mac, ETH_ALEN);
		priv->v4_eth.h_proto = htons(ETH_P_IP);/*lint !e778*/

		memcpy_s(priv->v6_eth.h_dest, ETH_ALEN,
			 dst_mac, ETH_ALEN);
		memcpy_s(priv->v6_eth.h_source, ETH_ALEN,
			 src_mac, ETH_ALEN);
		priv->v6_eth.h_proto = htons(ETH_P_IPV6);/*lint !e778*/

		spin_lock_init(&priv->lb_policy.lb_lock);
		INIT_WORK(&priv->napi_work, rnic_napi_dispatcher_cb);
		atomic_set(&priv->lb_policy.napi_cpu, 0);

		if (rnic_check_rmnet_data(devid)) {
			if (rnic_cpumasks_init(&priv->lb_policy))
				RNIC_LOGE("cpumasks init failed, devid: %d.", devid);
		}


		netdev->features |= NETIF_F_SG;
		netdev->hw_features |= NETIF_F_SG;

		rc = register_netdev(netdev);
		if (rc) {
			RNIC_LOGE("register netdev fail, devid: %d.", devid);
			free_netdev(netdev);
			goto err_register_netdev;
		}

		dev_ctx->netdev[devid] = netdev;
		dev_ctx->priv[devid] = priv;
		dev_ctx->priv[devid]->lb_policy.priv = priv;
	}

	if (!alloc_cpumask_var(&dev_ctx->rcc_cfg.rps_cpumask, GFP_KERNEL))
		RNIC_LOGE("rps_cpumask init failed.");

	rc = platform_driver_register(&rnic_platform_driver);
	if (rc) {
		RNIC_LOGE("register platform driver failed.");
		goto err_register_platfrom_driver;
	}

	rnic_pm_init();

	dev_ctx->ready = true;
	if (dev_ctx->dev_notifier_func != NULL)
		dev_ctx->dev_notifier_func();



	return 0;

err_register_platfrom_driver:
err_register_netdev:
err_alloc_netdev:
err_name_param:
	rnic_cleanup();

	return rc;
}
/*lint -restore +e801*/

/*
 * rnic_init - Module init function.
 *
 * Return 0 on success, negative on failure.
 */
int __init rnic_init(void)
{
	int rc;

	rc = rnic_create_netdev();
	RNIC_LOGH("%s.", (!rc)? "succ" : "fail");

	return rc;
}

/*
 * rnic_init - Module exit function.
 */
STATIC void __exit rnic_exit(void)
{
	rnic_cleanup();
	RNIC_LOGH("succ.");
}

module_param(rnic_dev_log_level, uint, S_IRUGO | S_IWUSR);
MODULE_PARM_DESC(rnic_dev_log_level, "rnic device log level");
#ifndef CONFIG_HISI_BALONG_MODEM_MODULE
module_init(rnic_init);
module_exit(rnic_exit);
#endif

