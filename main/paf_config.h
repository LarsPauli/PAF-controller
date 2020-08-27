#ifndef __PAF_CONFIG_H__
#define __PAF_CONFIG_H__

#define PAF_DEF_LED_GPIO (18)
#define PAF_DEF_LED_DC (4000)
#define PAF_DEF_LED_FREQ (5000)

#define PAF_DEF_WIFI_SSID "PAF"
#define PAF_DEF_WIFI_PASSWORD "paf"
#define PAF_DEF_WIFI_TIMEOUT (10000)
#define PAF_DEF_WIFI_CHANNEL 0
#define PAF_DEF_WIFI_AP_MAX_CON 5

#define PAF_WEBSERVER_STACK 4096
#define PAF_WEBSERVER_PRIORITY 3
#define PAF_WEBSERVER_CORE tskNO_AFFINITY

#define PAF_CONSOLE_STACK 4096
#define PAF_CONSOLE_PRIORITY 3
#define PAF_CONSOLE_CORE tskNO_AFFINITY

#endif // __PAF_CONFIG_H__
