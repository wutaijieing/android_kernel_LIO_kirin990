/* Copyright (c) 2019-2019, Hisilicon Tech. Co., Ltd. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */
#include "hisi_fb.h"

#if defined(CONFIG_HISI_PERIDVFS)
#include "peri_volt_poll.h"
#endif

#if defined(CONFIG_HISI_PERIDVFS) && !defined(SUPPORT_DCA_WITHOUT_VOLT)
int dss_get_peri_volt(int *curr_volt)
{
	struct peri_volt_poll *pvp = NULL;

	hisi_check_and_return(!curr_volt, -EINVAL, ERR, "nullptr!\n");
	pvp = peri_volt_poll_get(DEV_DSS_VOLTAGE_ID, NULL);
	hisi_check_and_return((pvp == NULL), -EINVAL, ERR, "pvp get failed!\n");
	*curr_volt = peri_get_volt(pvp);

	return 0;
}

int dss_set_peri_volt(int volt_to_set, int *curr_volt)
{
	int ret = 0;

	struct peri_volt_poll *pvp = NULL;
	bool is_lowtemp = false;

	hisi_check_and_return(!curr_volt, -EINVAL, ERR, "nullptr!\n");
	pvp = peri_volt_poll_get(DEV_DSS_VOLTAGE_ID, NULL);
	hisi_check_and_return((pvp == NULL), -EINVAL, ERR, "pvp get failed!\n");

	if (peri_get_temperature(pvp))
		is_lowtemp = true;

#ifdef LOW_TEMP_VOLTAGE_LIMIT
	if (!is_vote_needed_for_low_temp(is_lowtemp, volt_to_set)) {
		HISI_FB_DEBUG("is_lowtemp, return\n");
		return -EINVAL;
	}
#endif
	ret = peri_set_volt(pvp, volt_to_set);
	hisi_check_and_return((ret != 0), -EINVAL, ERR,
		"set votage_value=%d failed, ret=%d\n", volt_to_set, ret);

	HISI_FB_DEBUG("set votage_value=%d\n", volt_to_set);

	*curr_volt = peri_get_volt(pvp);

	return ret;
}
#else
int dss_get_peri_volt(int *curr_volt)
{
	(void)curr_volt;

	return 0;
}

int dss_set_peri_volt(int volt_to_set, int *curr_volt)
{
	(void)volt_to_set;
	(void)curr_volt;

	return 0;
}
#endif

int hisifb_get_other_fb_votelevel(struct hisi_fb_data_type *hisifd,
	uint32_t *max_vote_level)
{
	struct hisi_fb_data_type *targetfd1 = NULL;
	struct hisi_fb_data_type *targetfd2 = NULL;
	uint32_t target_dss_voltage_level;

	hisi_check_and_return((!hisifd || !max_vote_level), -EINVAL, ERR, "null ptr\n");

	if (hisifd->index == PRIMARY_PANEL_IDX) {
		targetfd1 = hisifd_list[EXTERNAL_PANEL_IDX];
		targetfd2 = hisifd_list[AUXILIARY_PANEL_IDX];
	} else if (hisifd->index == EXTERNAL_PANEL_IDX) {
		targetfd1 = hisifd_list[PRIMARY_PANEL_IDX];
		targetfd2 = hisifd_list[AUXILIARY_PANEL_IDX];
	} else {
		targetfd1 = hisifd_list[PRIMARY_PANEL_IDX];
		targetfd2 = hisifd_list[EXTERNAL_PANEL_IDX];
	}

	if (targetfd1 && targetfd2)
		target_dss_voltage_level = max(targetfd1->dss_vote_cmd.dss_voltage_level,
			targetfd2->dss_vote_cmd.dss_voltage_level);
	else if (targetfd1)
		target_dss_voltage_level = targetfd1->dss_vote_cmd.dss_voltage_level;
	else if (targetfd2)
		target_dss_voltage_level = targetfd2->dss_vote_cmd.dss_voltage_level;
	else
		target_dss_voltage_level = PERI_VOLTAGE_LEVEL0;

	*max_vote_level = target_dss_voltage_level;

	return 0;
}

int hisifb_set_dss_vote_voltage(struct hisi_fb_data_type *hisifd,
	uint32_t dss_voltage_level, int *curr_volt)
{
	int ret;
	int volt_to_set;
	uint32_t expected_voltage_level;
	struct hisi_fb_data_type *fb0 = hisifd_list[PRIMARY_PANEL_IDX];
	struct hisi_fb_data_type *fb1 = hisifd_list[EXTERNAL_PANEL_IDX];
	struct hisi_fb_data_type *fb2 = hisifd_list[AUXILIARY_PANEL_IDX];
	struct hisi_fb_data_type *fb3 = hisifd_list[MEDIACOMMON_PANEL_IDX];

	hisi_check_and_return((hisifd == NULL) || (curr_volt == NULL), -EINVAL,
		ERR, "null ptr\n");

	ret = hisifb_get_other_fb_votelevel(hisifd, &expected_voltage_level);
	hisi_check_and_return((ret != 0), -EINVAL, ERR,
		"set max votage_value=%d failed\n", ret);

	expected_voltage_level = max(expected_voltage_level, dss_voltage_level);

	volt_to_set = dpe_get_voltage_value(expected_voltage_level);
	hisi_check_and_return((volt_to_set < 0), -EINVAL, ERR, "get votage_value failed\n");

	hisi_check_and_return(dss_set_peri_volt(volt_to_set, curr_volt), -EINVAL, ERR,
		"dss_set_peri_volt %d failed\n", volt_to_set);

	HISI_FB_DEBUG("fb%d->level %d, [%d, %d, %d, %d], set %d, current %d\n",
		hisifd->index, dss_voltage_level,
		(fb0 != NULL) ? fb0->dss_vote_cmd.dss_voltage_level : 0,
		(fb1 != NULL) ? fb1->dss_vote_cmd.dss_voltage_level : 0,
		(fb2 != NULL) ? fb2->dss_vote_cmd.dss_voltage_level : 0,
		(fb3 != NULL) ? fb3->dss_vote_cmd.dss_voltage_level : 0,
		volt_to_set, *curr_volt);

	hisifd->dss_vote_cmd.dss_voltage_level = dss_voltage_level;

	return ret;
}

uint64_t dss_get_current_pri_clk_rate(struct hisi_fb_data_type *hisifd)
{
	uint64_t dss_pri_clk_rate;

	hisi_check_and_return((!hisifd), 0, ERR, "hisifd is NULL Pointer!\n");

	dss_pri_clk_rate = clk_get_rate(hisifd->dss_pri_clk);
	HISI_FB_INFO("fb%d get dss_pri_clk = %llu\n", hisifd->index, dss_pri_clk_rate);

	return dss_pri_clk_rate;
}
