#ifndef __PAF_CONFIG_H__
#define __PAF_CONFIG_H__

#define PAF_DEF_LED_GPIO (2)
#define PAF_DEF_LED_DC (4000)
#define PAF_DEF_LED_FREQ (5000)
#define PAF_DEF_LED_PWM_FADE_TIME (3000)

#define PAF_DEF_WIFI_SSID "PAF"
#define PAF_DEF_WIFI_PASSWORD "paf"
#define PAF_DEF_AP_IP "192.168.1.1"
#define PAF_DEF_GATEWAY_IP PAF_DEF_AP_IP
#define PAF_DEF_NETMASK "255.255.255.0"
#define PAF_DEF_WIFI_TIMEOUT (10000)
#define PAF_DEF_WIFI_CHANNEL 0
#define PAF_DEF_WIFI_AP_MAX_CON 5
#define PAF_DEF_LED_MODE PAF_LED_MODE_PWM

#define PAF_AP_STACK 4096
#define PAF_AP_PRORITY 3
#define PAF_AP_CORE tskNO_AFFINITY

#define PAF_WEBSERVER_STACK 4096
#define PAF_WEBSERVER_PRIORITY 2
#define PAF_WEBSERVER_CORE tskNO_AFFINITY

#define PAF_CONSOLE_STACK 4096
#define PAF_CONSOLE_PRIORITY 2
#define PAF_CONSOLE_CORE tskNO_AFFINITY

#define PAF_DEF_OLED_SDA_PIN (21)
#define PAF_DEF_OLED_SCL_PIN (22)
#define PAF_DEF_I2C_NUM I2C_NUM_0

#endif // __PAF_CONFIG_H__
