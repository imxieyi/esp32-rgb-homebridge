/*
 * main.cpp
 *
 *  Created on: 2016Äê12ÔÂ24ÈÕ
 *      Author: xieyi
 */
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "RGB.h"
#include "httpserver-netconn.h"
#include "string.h"
#include "ctype.h"

#define TAG "main"
#define R GPIO_NUM_22
#define G GPIO_NUM_17
#define B GPIO_NUM_16

const char* wifi_ssid="ForIOT";
const char* wifi_password="Hehehehe_Hahahaha";
const int CONNECTED_BIT = BIT0;
static tcpip_adapter_ip_info_t sta_ip;

RGB rgb=RGB(R,G,B);

static EventGroupHandle_t wifi_event_group;

char* getval(char* raw){
	char* st=strstr(raw,":");
	st++;
	return st;
}

uint8_t hex2dec(char c)
{
    if(c >= '0' && c <= '9')
        return c - '0';
    else if(c >= 'A' && c <= 'F')
        return (c - 'A' + 10);
    else if(c >= 'a' && c <= 'f')
        return (c - 'a' + 10);
    return 0;
}

char* dec2hex(uint8_t d){
	char* temp=(char*)malloc(3);
	memset(temp,0,3);
	if(d<16){
		sprintf(temp,"0%x",d);
	}else{
		sprintf(temp,"%x",d);
	}
	temp[0]=tolower(temp[0]);
	temp[1]=tolower(temp[1]);
	return temp;
}

char* cmd_handle(char* cmd) {
	char* temp=NULL;
	ESP_LOGI("main", "cmd received:%s", cmd);
	ESP_LOGI("main", "cmd length:%d", strlen(cmd));
	if(strncmp(cmd,"status",6)==0){
		temp=(char*)malloc(2);
		memset(temp,0,2);
		if(rgb.getstatus()){
			sprintf(temp,"1");
		}else{
			sprintf(temp,"0");
		}
		goto finish;
	}
	if(strncmp(cmd,"on",2)==0){
		rgb.turnon();
		temp="";
		goto finish;
	}
	if(strncmp(cmd,"off",2)==0){
		rgb.turnoff();
		temp="";
		goto finish;
	}
	if(strncmp(cmd,"brightness:",11)==0){
		char* val=getval(cmd);
		ESP_LOGI(TAG,"got new brightness:%s",val);
		rgb.setbri(atoi(val));
		temp="";
		goto finish;
	}
	if(strncmp(cmd,"brightness",10)==0){
		temp=(char*)malloc(4);
		memset(temp,0,4);
		sprintf(temp,"%d",rgb.getbri());
		goto finish;
	}
	if(strncmp(cmd,"color:",6)==0){
		char color[6];
		strncpy(color,getval(cmd),6);
		uint8_t r=hex2dec(color[0])*16+hex2dec(color[1]);
		uint8_t g=hex2dec(color[2])*16+hex2dec(color[3]);
		uint8_t b=hex2dec(color[4])*16+hex2dec(color[5]);
		ESP_LOGI(TAG,"got new color:%d %d %d",r,g,b);
		rgb.setcolor(r, g, b);
		temp="";
		goto finish;
	}
	if(strncmp(cmd,"color",5)==0){
		uint8_t* color=rgb.getcolor();
		temp=(char*)malloc(7);
		memset(temp,0,7);
		sprintf(temp,"%s%s%s",dec2hex(color[0]),dec2hex(color[1]),dec2hex(color[2]));
		goto finish;
	}
	finish:
	free(cmd);
	return temp;
}

static esp_err_t event_handler(void *ctx, system_event_t *event) {
	switch (event->event_id) {
	case SYSTEM_EVENT_STA_START:
		esp_wifi_connect();
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
		ESP_LOGI(TAG, "wifi connected");
		tcpip_adapter_get_ip_info(TCPIP_ADAPTER_IF_STA, &sta_ip);
		http_server_netconn_init(cmd_handle);
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		ESP_LOGI(TAG, "wifi disconnected");
		xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
		esp_wifi_connect();
		break;
	default:
		break;
	}
	return ESP_OK;
}

#ifdef __cplusplus
extern "C" {
#endif
void app_main() {
	tcpip_adapter_init();
	wifi_event_group = xEventGroupCreate();
	ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL));
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
	wifi_config_t sta_config;
	strcpy((char*)sta_config.sta.ssid, wifi_ssid);
	strcpy((char*)sta_config.sta.password, wifi_password);
	ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &sta_config));
	ESP_LOGI(TAG, "Setting WiFi configuration SSID %s...",
			sta_config.sta.ssid);
	ESP_ERROR_CHECK(esp_wifi_start());
	ESP_ERROR_CHECK(esp_wifi_connect());
}
#ifdef __cplusplus
}
#endif
