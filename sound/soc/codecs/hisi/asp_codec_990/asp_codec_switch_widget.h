/*
 * asp_codec_switch_widget.h -- asp codec switch widget driver
 *
 * Copyright (c) Huawei Technologies Co., Ltd. 2018-2019. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 */

#ifndef __ASP_CODEC_SWITCH_WIDGET_H__
#define __ASP_CODEC_SWITCH_WIDGET_H__
#include <sound/core.h>
#include <sound/soc.h>
#include <sound/soc-dapm.h>

int asp_codec_add_switch_widgets(struct snd_soc_dapm_context *dapm);
#endif

