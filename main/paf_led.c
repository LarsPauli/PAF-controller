
/**
 * @file paf_led.c
 * @author Alex Hoffman
 * @date 25 August 2020
 * @brief LED control
 *
 * @verbatim
   ----------------------------------------------------------------------
    Copyright (C) Alexander Hoffman, 2020
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    any later version.
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
   ----------------------------------------------------------------------
@endverbatim
 */

#include "esp_log.h"

#include "driver/gpio.h"
#include "driver/ledc.h"

#include "paf_config.h"
#include "paf_led.h"

#define PAF_LED_TIMER LEDC_TIMER_0
#define PAF_LED_MODE LEDC_HIGH_SPEED_MODE
#define PAF_LED_GPIO_PIN PAF_DEF_LED_GPIO
#define PAF_LED_CHANNEL LEDC_CHANNEL_0

ledc_timer_config_t ledc_timer = {
    .duty_resolution = LEDC_TIMER_13_BIT,
    .freq_hz = PAF_DEF_LED_FREQ,
    .speed_mode = PAF_LED_MODE,
    .timer_num = PAF_LED_TIMER,
    .clk_cfg = LEDC_AUTO_CLK,
};

ledc_channel_config_t ledc_channel = {
    .channel = PAF_LED_CHANNEL,
    .duty = 0,
    .gpio_num = PAF_DEF_LED_GPIO,
    .speed_mode = PAF_LED_MODE,
    .hpoint = 0,
    .timer_sel = PAF_LED_TIMER,
};

struct led_config {
    char led_status;
    char ledc_initd;
    unsigned int ledc_dc;
    unsigned int ledc_freq;

} static ledc_cfg = { .ledc_dc = PAF_DEF_LED_DC,
                      .ledc_freq = PAF_DEF_LED_FREQ
                    };

static paf_led_mode_t led_mode = PAF_LED_MODE_NOTSET;

char paf_led_get_led(void)
{
    return ledc_cfg.led_status;
}

esp_err_t paf_led_init(paf_led_mode_t mode)
{
    led_mode = mode;
    switch (mode) {
        case PAF_LED_MODE_GPIO:
            gpio_pad_select_gpio(PAF_DEF_LED_GPIO);
            gpio_set_direction(PAF_LED_GPIO_PIN, GPIO_MODE_OUTPUT);
            break;
        case PAF_LED_MODE_PWM:
            if (!ledc_cfg.ledc_initd) {
                ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));
                ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
                ESP_ERROR_CHECK(ledc_fade_func_install(0));
                ledc_cfg.ledc_initd = 1;
            }
            break;
        case PAF_LED_MODE_CONSOLE:
            break;
        default:
            return ESP_FAIL;
    }

    return ESP_OK;
}

enum led_pwd_update_mode {
    FROM_CONFIG,
    MANUAL_BOTH,
    MANUAL_FREQ,
    MANUAL_DC,
};

static esp_err_t paf_led_update_freq(unsigned int freq)
{
    esp_err_t err;
    err = ledc_set_freq(ledc_timer.speed_mode, ledc_timer.timer_num,
                        ledc_cfg.ledc_freq);
    if (err != ESP_OK)
        ESP_LOGI(__func__, "Couldn't set PWM freq\n-> %s",
                 esp_err_to_name(err));

    return err;
}

static esp_err_t paf_led_update_dc(unsigned int freq)
{
    esp_err_t err;
    err = ledc_set_duty(ledc_timer.speed_mode, ledc_timer.timer_num,
                        ledc_cfg.ledc_dc);
    if (err != ESP_OK)
        ESP_LOGI(__func__, "Couldn't set PWM dc\n-> %s",
                 esp_err_to_name(err));

    return err;
}

static esp_err_t paf_led_update_pwm(enum led_pwd_update_mode mode,
                                    unsigned int freq, unsigned int dc)
{
    switch (mode) {
        case FROM_CONFIG:
            paf_led_update_freq(ledc_cfg.ledc_freq);
            paf_led_update_dc(ledc_cfg.ledc_dc);
            ESP_ERROR_CHECK(ledc_update_duty(ledc_channel.speed_mode,
                                             ledc_channel.channel));
            break;
        case MANUAL_BOTH:
            paf_led_update_freq(freq);
            paf_led_update_dc(dc);
            ESP_ERROR_CHECK(ledc_update_duty(ledc_channel.speed_mode,
                                             ledc_channel.channel));
            break;
        case MANUAL_FREQ:
            paf_led_update_freq(freq);
            paf_led_update_dc(ledc_cfg.ledc_dc);
            ESP_ERROR_CHECK(ledc_update_duty(ledc_channel.speed_mode,
                                             ledc_channel.channel));
            break;
        case MANUAL_DC:
            paf_led_update_freq(ledc_cfg.ledc_freq);
            paf_led_update_dc(dc);
            ESP_ERROR_CHECK(ledc_update_duty(ledc_channel.speed_mode,
                                             ledc_channel.channel));
            break;
        default:
            return ESP_FAIL;
    }

    return ESP_OK;
}

esp_err_t paf_led_set_on(void)
{
    esp_err_t ret = ESP_OK;
    switch (led_mode) {
        case PAF_LED_MODE_GPIO:
            ESP_ERROR_CHECK(gpio_set_level(PAF_DEF_LED_GPIO, 1));
            break;
        case PAF_LED_MODE_PWM:
            if (!ledc_cfg.ledc_initd) {
                ESP_ERROR_CHECK(paf_led_init(PAF_LED_MODE_PWM));
            }

            ESP_ERROR_CHECK(paf_led_update_pwm(FROM_CONFIG, 0, 0));
            break;
        case PAF_LED_MODE_CONSOLE:
            ESP_LOGI(__func__, "Setting LED on");
            break;
        default:
            ret = ESP_FAIL;
            break;
    }
    ledc_cfg.led_status = 1;
    return ret;
}

esp_err_t paf_led_set_off(void)
{
    esp_err_t ret = ESP_OK;
    switch (led_mode) {
        case PAF_LED_MODE_GPIO:
            ESP_ERROR_CHECK(gpio_set_level(PAF_DEF_LED_GPIO, 0));
            break;
        case PAF_LED_MODE_PWM:
            if (ledc_cfg.ledc_initd) {
                ESP_ERROR_CHECK(paf_led_update_pwm(MANUAL_DC, 0, 0));
            }
            break;
        case PAF_LED_MODE_CONSOLE:
            ESP_LOGI(__func__, "Setting LED off");
            break;
        default:
            ret = ESP_FAIL;
            break;
    }
    ledc_cfg.led_status = 0;
    return ret;
}

esp_err_t paf_led_set_toggle(void)
{
    ESP_LOGI(__func__, "Toggling LED %d -> %d", ledc_cfg.led_status,
             !ledc_cfg.led_status);

    if (ledc_cfg.led_status) {
        return paf_led_set_off();
    }

    return paf_led_set_on();
}

esp_err_t paf_led_set_dc(unsigned int duty_cycle)
{
    if ((led_mode != PAF_LED_MODE_PWM) || (!ledc_cfg.ledc_initd)) {
        return -1;
    }

    if (paf_led_update_dc(duty_cycle) == ESP_OK) {
        ledc_cfg.ledc_dc = duty_cycle;
        if (ledc_cfg.led_status) {
            paf_led_update_pwm(FROM_CONFIG, 0, 0);
        }
    }

    ESP_LOGI(__func__, "DC set to %d", duty_cycle);

    return ESP_OK;
}

unsigned int paf_led_get_dc(void)
{
    if ((led_mode != PAF_LED_MODE_PWM) || (!ledc_cfg.ledc_initd)) {
        return -1;
    }

    return ledc_cfg.ledc_dc;
}

esp_err_t paf_led_set_freq(unsigned int freq)
{
    if ((led_mode != PAF_LED_MODE_PWM) || (!ledc_cfg.ledc_initd)) {
        return -1;
    }

    if (paf_led_update_freq(freq) == ESP_OK) { //Check freq is valid
        ledc_cfg.ledc_freq = freq;
        if (ledc_cfg.led_status) {
            ESP_ERROR_CHECK(paf_led_update_pwm(FROM_CONFIG, 0, 0));
        }
    }

    ESP_LOGI(__func__, "Freq set to %d", freq);

    return 0;
}

unsigned int paf_led_get_freq(void)
{
    if ((led_mode != PAF_LED_MODE_PWM) || (!ledc_cfg.ledc_initd)) {
        return -1;
    }

    return ledc_cfg.ledc_freq;
}
