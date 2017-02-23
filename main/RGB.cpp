/*
 * RGB.cpp
 *
 *  Created on: 2017Äê1ÔÂ9ÈÕ
 *      Author: xieyi
 */

#include "RGB.h"
#include "esp_log.h"
#include <cstdlib>

//#define FADE_ON
#define FADE_TIME 200

RGB::RGB(gpio_num_t r, gpio_num_t g, gpio_num_t b) {
	ledc_timer_config_t ledc_timer = { LEDC_HIGH_SPEED_MODE, LEDC_TIMER_10_BIT, //0-1023
			LEDC_TIMER_0, 5000 };
	ledc_timer_config(&ledc_timer);
	ledc_timer.timer_num=LEDC_TIMER_1;
	ledc_timer_config(&ledc_timer);
	ledc_timer.timer_num=LEDC_TIMER_2;
	ledc_timer_config(&ledc_timer);
	ledc_channel_config_t ledc_channel =
			{ r, LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, LEDC_INTR_FADE_END, LEDC_TIMER_0, 0 };
	ledc_channel_config(&ledc_channel);
	ledc_channel.channel = LEDC_CHANNEL_1;
	ledc_channel.timer_sel=LEDC_TIMER_1;
	ledc_channel.gpio_num = g;
	ledc_channel_config(&ledc_channel);
	ledc_channel.channel = LEDC_CHANNEL_2;
	ledc_channel.timer_sel=LEDC_TIMER_2;
	ledc_channel.gpio_num = b;
	ledc_channel_config(&ledc_channel);
#ifdef FADE_ON
	ledc_fade_func_install(0);
#endif
}

void RGB::setcolor(uint8_t r, uint8_t g, uint8_t b) {
	rv = r;
	gv = g;
	bv = b;
	ESP_LOGI("rgb","r duty cycle:%d",(uint32_t)brightness * (uint32_t)rv * 1024 / 25600);
	ESP_LOGI("rgb","g duty cycle:%d",(uint32_t)brightness * (uint32_t)gv * 1024 / 25600);
	ESP_LOGI("rgb","b duty cycle:%d",(uint32_t)brightness * (uint32_t)bv * 1024 / 25600);
#ifdef FADE_ON
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, (uint32_t)brightness * (uint32_t)rv * 1024 / 25600, FADE_TIME);
	ledc_fade_start(LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, (uint32_t)brightness * (uint32_t)gv * 1024 / 25600, FADE_TIME);
	ledc_fade_start(LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, (uint32_t)brightness * (uint32_t)bv * 1024 / 25600, FADE_TIME);
	ledc_fade_start(LEDC_CHANNEL_2, LEDC_FADE_NO_WAIT);
#else
	ledc_set_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0,(uint32_t)brightness * (uint32_t)rv * 1024 / 25600);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0);
	ledc_set_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1,(uint32_t)brightness * (uint32_t)gv * 1024 / 25600);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1);
	ledc_set_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2,(uint32_t)brightness * (uint32_t)bv * 1024 / 25600);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2);
#endif
}

void RGB::turnon() {
	if (!ison) {
#ifdef FADE_ON
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, (uint32_t)brightness * (uint32_t)rv * 1024 / 25600, FADE_TIME);
	ledc_fade_start(LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, (uint32_t)brightness * (uint32_t)gv * 1024 / 25600, FADE_TIME);
	ledc_fade_start(LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, (uint32_t)brightness * (uint32_t)bv * 1024 / 25600, FADE_TIME);
	ledc_fade_start(LEDC_CHANNEL_2, LEDC_FADE_NO_WAIT);
#else
	ledc_set_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0,(uint32_t)brightness * (uint32_t)rv * 1024 / 25600);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0);
	ledc_set_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1,(uint32_t)brightness * (uint32_t)gv * 1024 / 25600);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1);
	ledc_set_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2,(uint32_t)brightness * (uint32_t)bv * 1024 / 25600);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2);
#endif
		ison = true;
	}
}

void RGB::turnoff() {
	if (ison) {
#ifdef FADE_ON
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, 0, FADE_TIME);
	ledc_fade_start(LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, 0, FADE_TIME);
	ledc_fade_start(LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, 0, FADE_TIME);
	ledc_fade_start(LEDC_CHANNEL_2, LEDC_FADE_NO_WAIT);
#else
	ledc_set_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0,0);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0);
	ledc_set_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1,0);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1);
	ledc_set_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2,0);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2);
#endif
		ison = false;
	}
}

uint8_t* RGB::getcolor() {
	uint8_t *temp=(uint8_t*)malloc(3);
	*temp = rv;
	*(temp+1) = gv;
	*(temp+2) = bv;
	return temp;
}

bool RGB::getstatus() {
	return ison;
}

void RGB::setbri(uint8_t bri) {
	brightness = bri;
#ifdef FADE_ON
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_0, (uint32_t)brightness * (uint32_t)rv * 1024 / 25600, FADE_TIME);
	ledc_fade_start(LEDC_CHANNEL_0, LEDC_FADE_NO_WAIT);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_1, (uint32_t)brightness * (uint32_t)gv * 1024 / 25600, FADE_TIME);
	ledc_fade_start(LEDC_CHANNEL_1, LEDC_FADE_NO_WAIT);
	ledc_set_fade_with_time(LEDC_HIGH_SPEED_MODE, LEDC_CHANNEL_2, (uint32_t)brightness * (uint32_t)bv * 1024 / 25600, FADE_TIME);
	ledc_fade_start(LEDC_CHANNEL_2, LEDC_FADE_NO_WAIT);
#else
	ledc_set_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0,(uint32_t)brightness * (uint32_t)rv * 1024 / 25600);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_0);
	ledc_set_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1,(uint32_t)brightness * (uint32_t)gv * 1024 / 25600);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_1);
	ledc_set_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2,(uint32_t)brightness * (uint32_t)bv * 1024 / 25600);
	ledc_update_duty(LEDC_HIGH_SPEED_MODE,LEDC_CHANNEL_2);
#endif
}

uint8_t RGB::getbri(){
	return brightness;
}

RGB::~RGB() {
}

