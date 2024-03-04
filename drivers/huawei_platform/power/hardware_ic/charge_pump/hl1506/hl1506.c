/*
 * hl1506.c
 *
 * charge-pump hl1506 driver
 *
 * Copyright (c) 2019-2020 Huawei Technologies Co., Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 */

#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <chipset_common/hwpower/power_i2c.h>
#include <huawei_platform/log/hw_log.h>
#include <huawei_platform/power/hardware_ic/charge_pump.h>
#include <huawei_platform/power/wireless_power_supply.h>
#include <chipset_common/hwpower/power_devices_info.h>

#include "hl1506.h"

#define HWLOG_TAG cp_hl1506
HWLOG_REGIST();

static struct hl1506_dev_info *g_hl1506_di;

static int hl1506_read_byte(u8 reg, u8 *data)
{
	struct hl1506_dev_info *di = g_hl1506_di;
	int i;

	if (!di) {
		hwlog_err("chip not init\n");
		return -EIO;
	}

	for (i = 0; i < CP_I2C_RETRY_CNT; i++) {
		if (!power_i2c_u8_read_byte(di->client, reg, data))
			return 0;
		usleep_range(9500, 10500); /* 10ms */
	}

	return -EIO;
}

static int hl1506_write_byte(u8 reg, u8 data)
{
	struct hl1506_dev_info *di = g_hl1506_di;
	int i;

	if (!di) {
		hwlog_err("chip not init\n");
		return -EIO;
	}

	for (i = 0; i < CP_I2C_RETRY_CNT; i++) {
		if (!power_i2c_u8_write_byte(di->client, reg, data))
			return 0;
		usleep_range(9500, 10500); /* 10ms */
	}

	return -EIO;
}

static int hl1506_write_mask(u8 reg, u8 mask, u8 shift, u8 value)
{
	int ret;
	u8 val = 0;

	ret = hl1506_read_byte(reg, &val);
	if (ret)
		return ret;

	val &= ~mask;
	val |= ((value << shift) & mask);

	return hl1506_write_byte(reg, val);
}

static int hl1506_read_mask(u8 reg, u8 mask, u8 shift, u8 *value)
{
	int ret;
	u8 val = 0;

	ret = hl1506_read_byte(reg, &val);
	if (ret)
		return ret;

	val &= mask;
	val >>= shift;
	*value = val;

	return ret;
}

static int hl1506_i2c_init(void)
{
	int ret;

	/* i2c init */
	ret = hl1506_write_byte(HL1506_I2C_INIT_REG, HL1506_I2C_INIT_CLR);
	usleep_range(500, 1500); /* 1ms */
	ret += hl1506_write_byte(HL1506_I2C_INIT_REG, HL1506_I2C_INIT_RST);

	return ret;
}

static int hl1506_device_match(void)
{
	return hl1506_i2c_init();
}

static int hl1506_chip_init(void)
{
	int ret;

	/* i2c init */
	ret = hl1506_i2c_init();
	/* PMID  Vlimit MAX */
	ret += hl1506_write_byte(HL1506_REG_00, HL1506_00_INIT_VAL);
	/* ilimit 1.5A,  Force ByPass */
	ret += hl1506_write_byte(HL1506_REG_01, HL1506_01_INIT_VAL);
	/* cp-ck 600kHz */
	ret += hl1506_write_byte(HL1506_REG_02, HL1506_02_INIT_VAL);
	/* host enabel, PMID ov 11V */
	ret += hl1506_write_byte(HL1506_REG_03, HL1506_03_INIT_VAL);

	return ret;
}

static int hl1506_set_bp_mode(void)
{
	int ret;

	ret = hl1506_write_mask(HL1506_REG_01, HL1506_01_FORCE_CP_MASK,
		HL1506_01_FORCE_CP_SHIFT, HL1506_01_FORCE_CP_DIS);
	ret += hl1506_write_mask(HL1506_REG_01, HL1506_01_FORCE_BP_MASK,
		HL1506_01_FORCE_BP_SHIFT, HL1506_01_FORCE_BP_EN);

	return ret;
}

static int hl1506_set_cp_mode(void)
{
	return hl1506_write_mask(HL1506_REG_01, HL1506_01_FORCE_CP_MASK,
		HL1506_01_FORCE_CP_SHIFT, HL1506_01_FORCE_CP_EN);
}

static bool hl1506_is_cp_open(void)
{
	int ret;
	u8 status = 0;

	ret = hl1506_read_mask(HL1506_REG_04, HL1506_04_BPCP_MODE_MASK,
		HL1506_04_BPCP_MODE_SHIFT, &status);
	if (!ret && !status)
		return true;

	return false;
}

static bool hl1506_is_bp_open(void)
{
	int ret;
	u8 status = 0;

	ret = hl1506_read_mask(HL1506_REG_04, HL1506_04_BPCP_MODE_MASK,
		HL1506_04_BPCP_MODE_SHIFT, &status);
	if (!ret && status)
		return true;

	return false;
}

static struct charge_pump_ops hl1506_ops = {
	.chip_name       = "hl1506",
	.chip_init       = hl1506_chip_init,
	.set_bp_mode     = hl1506_set_bp_mode,
	.set_cp_mode     = hl1506_set_cp_mode,
	.is_cp_open      = hl1506_is_cp_open,
	.is_bp_open      = hl1506_is_bp_open,
};

static int hl1506_probe(struct i2c_client *client,
	const struct i2c_device_id *id)
{
	int ret;
	struct hl1506_dev_info *di = NULL;
	struct power_devices_info_data *power_dev_info = NULL;

	if (!client)
		return -ENODEV;

	di = devm_kzalloc(&client->dev, sizeof(*di), GFP_KERNEL);
	if (!di)
		return -ENOMEM;

	g_hl1506_di = di;
	di->dev = &client->dev;
	di->client = client;
	i2c_set_clientdata(client, di);

	wlps_control(WLPS_PROBE_PWR, WLPS_CTRL_ON);
	usleep_range(9500, 10500); /* wait 10ms for power supply */
	ret = hl1506_device_match();
	wlps_control(WLPS_PROBE_PWR, WLPS_CTRL_OFF);
	if (ret) {
		hwlog_err("device mismatch\n");
		goto dev_check_fail;
	}

	ret = charge_pump_ops_register(&hl1506_ops);
	if (ret)
		goto ops_register_fail;

	power_dev_info = power_devices_info_register();
	if (power_dev_info) {
		power_dev_info->dev_name = di->dev->driver->name;
		power_dev_info->dev_id = 0;
		power_dev_info->ver_id = 0;
	}

	return 0;

ops_register_fail:
dev_check_fail:
	devm_kfree(&client->dev, di);
	di = NULL;
	g_hl1506_di = NULL;
	return ret;
}

MODULE_DEVICE_TABLE(i2c, charge_pump_hl1506);
static const struct of_device_id hl1506_of_match[] = {
	{
		.compatible = "charge_pump_hl1506",
		.data = NULL,
	},
	{},
};

static const struct i2c_device_id hl1506_i2c_id[] = {
	{ "charge_pump_hl1506", 0 }, {}
};

static struct i2c_driver hl1506_driver = {
	.probe = hl1506_probe,
	.id_table = hl1506_i2c_id,
	.driver = {
		.owner = THIS_MODULE,
		.name = "charge_pump_hl1506",
		.of_match_table = of_match_ptr(hl1506_of_match),
	},
};

static int __init hl1506_init(void)
{
	return i2c_add_driver(&hl1506_driver);
}

static void __exit hl1506_exit(void)
{
	i2c_del_driver(&hl1506_driver);
}

rootfs_initcall(hl1506_init);
module_exit(hl1506_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("hl1506 module driver");
MODULE_AUTHOR("Huawei Technologies Co., Ltd.");
