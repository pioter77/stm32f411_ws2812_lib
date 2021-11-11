/*
 * ws2812_led.h
 *
 *  Created on: 11 lis 2021
 *      Author: x300
 */

#ifndef INC_WS2812_LED_H_
#define INC_WS2812_LED_H_

#include "main.h"
#include "stdlib.h"

typedef struct T_ws2812_LED{
	uint8_t brightness;															//max brightness level (sets limit on every diode channel)
	uint8_t green;																//channel color intensity value
	uint8_t red;																//channel color intensity value
	uint8_t blue;																//channel color intensity value
//	uint16_t data_encoded[24];													//encoded data for this led ready to be send
}t_ws2812_LED;

typedef struct T_ws2812_STRIP{
	t_ws2812_LED *ledsarrPtr;														//pointer to led struct array for that strip
	uint16_t *encodedarrPtr;													//pointer to encoded data array for dma
	volatile _Bool stringSendingFinished;										//flag indicating frame transfer end
	uint8_t led_count;															//led count in a strip (120 led limit)
	//hardware:
	DMA_TypeDef *dma;															//dma instance
	uint32_t dma_stream;														//dma stream instance
	TIM_TypeDef *tim;															//timer instance
	uint32_t tim_channel;														//timer channel instance

}t_ws2812_STRIP;


t_ws2812_LED ws2812_LEDS1[120];
uint16_t ws2812_encoded_string1[120*24+50]; 						//24 for each led, 50 for reset code (zeros for >=50us)


void ws2812_init(t_ws2812_STRIP *strip);
void ws2812_set_brightness(t_ws2812_LED *led, uint8_t max_brightness);
void ws2812_set_led_color(t_ws2812_LED *led, uint8_t green, uint8_t red,uint8_t blue);
void ws2812_encode_led(t_ws2812_LED *led, uint16_t position,t_ws2812_STRIP *strip);
void ws2812_fill_zerosEnd(t_ws2812_STRIP *strip, uint8_t zerosCount);
void ws2812_updateString(t_ws2812_STRIP *strip);
void ws2812_set_led_count(t_ws2812_STRIP *strip, uint8_t count);
void ws2812_animate_flow1(t_ws2812_LED *led);			//simple animation type 1



extern t_ws2812_STRIP WS2812_STRIP1;
#endif /* INC_WS2812_LED_H_ */
