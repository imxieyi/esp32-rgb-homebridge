/*
 * RGB.h
 *
 *  Created on: 2017Äê1ÔÂ9ÈÕ
 *      Author: xieyi
 */

#ifndef MAIN_INCLUDE_RGB_H_
#define MAIN_INCLUDE_RGB_H_

#include "driver/gpio.h"
#include "driver/ledc.h"

#ifdef __cplusplus
extern "C"{
#endif

class RGB {
private:
	uint8_t rv=255,gv=255,bv=255,brightness=100;
	ledc_channel_config_t ledc_channel;
	bool ison=false;
public:
	RGB(gpio_num_t r,gpio_num_t g,gpio_num_t b);
	virtual ~RGB();
	void setcolor(uint8_t r,uint8_t g,uint8_t b);
	void turnon();
	void turnoff();
	uint8_t* getcolor();
	void setbri(uint8_t bri);
	uint8_t getbri();
	bool getstatus();
};

#ifdef __cplusplus
}
#endif

#endif /* MAIN_INCLUDE_RGB_H_ */
