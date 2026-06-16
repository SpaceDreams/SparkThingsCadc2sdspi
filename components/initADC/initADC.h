/*
 * SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_adc/adc_continuous.h"
#include <math.h>

#define init_ADC_UNIT                    ADC_UNIT_1
#define init_ADC_CONV_MODE               ADC_CONV_SINGLE_UNIT_1
#define init_ADC_ATTEN                   ADC_ATTEN_DB_0
#define init_ADC_BIT_WIDTH               SOC_ADC_DIGI_MAX_BITWIDTH
#define init_ADC_SAMPLING_FREQ           CONFIG_INIT_SAMPLE_RATE

#define init_READ_LEN                    256


static adc_channel_t channel[1] = {ADC_CHANNEL_7};

static TaskHandle_t s_task_handle;
static const char *ADCTAG = "init_ADC";

static bool IRAM_ATTR s_conv_done_cb(adc_continuous_handle_t handle, const adc_continuous_evt_data_t *edata, void *user_data)

static void continuous_adc_init(adc_channel_t *channel, uint8_t channel_num, adc_continuous_handle_t *out_handle)