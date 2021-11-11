/*
 * ws2812_led.c
 *
 *  Created on: 11 lis 2021
 *      Author: x300
 */


#include "ws2812_led.h"


//_Bool stringSendingFinished=1;

t_ws2812_STRIP WS2812_STRIP1={
		.ledsarrPtr=ws2812_LEDS1,
		.encodedarrPtr=ws2812_encoded_string1,
		.stringSendingFinished=1,
		.led_count=8,

		.dma=DMA1,
		.dma_stream=LL_DMA_STREAM_0,
		.tim=TIM4,
		.tim_channel=LL_TIM_CHANNEL_CH1,
};


/*
 *@param:led position in a row @param: color intensivity, @param: color intensivity, @param: color intensivity
 */
void ws2812_set_led_color(t_ws2812_LED *led, uint8_t green, uint8_t red,uint8_t blue)
{
	led->green=green;
	led->red=red;
	led->blue=blue;
}

void ws2812_animate_flow1(t_ws2812_LED *led){
	 uint8_t col_green=rand();
	 uint8_t col_red=rand();
	 uint8_t col_blue=rand();

	ws2812_set_led_color(led,col_green++,col_red++,col_blue++);
}
/*
 * encodes data for each led and inserts it into led strip array
 * @param: led instance, @param: led position in led string, @param: encoded led string data for dma ptr
 */
void ws2812_encode_led(t_ws2812_LED *led, uint16_t position,t_ws2812_STRIP *strip){
	uint32_t data = ((led->green)<<16) | ((led->red)<<8) | led->blue;

	for (int i=23;i>=0;i--){
		if((data & (uint32_t)(1<<i))) *(strip->encodedarrPtr+position*24+(uint16_t)i)=69;			//if bit is set to 1 then set ws2812 gigh bit oc compare val
		else *(strip->encodedarrPtr+position*24+(uint16_t)i)=34;							// when bit is low set bit low threshold
	}
}

/*
 * sets led brightness (common limit for all channels)
 * @param:led instance, @param: led brighness
 */
void ws2812_set_brightness(t_ws2812_LED *led, uint8_t max_brightness){
	led->brightness=max_brightness;
}

//pre configure dma and timers
//todo: struct for led strip that holds length, buffer for dma ,dma instances etc
void ws2812_init(t_ws2812_STRIP *strip)
{

	LL_DMA_DisableStream(strip->dma, strip->dma_stream);

	LL_DMA_ConfigAddresses(strip->dma, strip->dma_stream,
			(uint32_t)&strip->encodedarrPtr,
			(uint32_t)&(strip->tim->CCR1),
			LL_DMA_GetDataTransferDirection(strip->dma, strip->dma_stream));
	LL_DMA_SetDataLength(strip->dma, strip->dma_stream, strip->led_count*24+50);
	LL_DMA_EnableIT_TC(strip->dma, strip->dma_stream);
	LL_DMA_EnableIT_TE(strip->dma, strip->dma_stream);
	LL_DMA_EnableStream(strip->dma, strip->dma_stream);

	LL_TIM_OC_EnablePreload(strip->tim, strip->tim_channel);
	LL_TIM_EnableDMAReq_UPDATE(strip->tim);
	LL_TIM_EnableDMAReq_CC1(strip->tim);			//todo: is this necessary?
	LL_TIM_CC_EnableChannel(strip->tim, strip->tim_channel);
//	LL_TIM_EnableDMAReq_CC1(TIM4);
//	LL_TIM_EnableIT_UPDATE(TIM4);
//	LL_TIM_EnableAutomaticOutput(TIM4);
}
/*
 * prepares data sequence that ends the frame according to protocol: reset code for more than 50us
 */
void ws2812_fill_zerosEnd(t_ws2812_STRIP *strip, uint8_t zerosCount){
	for(uint8_t i=0;i<zerosCount;i++) *(strip->encodedarrPtr+strip->led_count*24+(uint16_t)i)=0;
}

//sends 1 frame
void ws2812_updateString(t_ws2812_STRIP *strip)
{
	if(strip->stringSendingFinished){
		strip->stringSendingFinished=0;
		for(uint16_t i=0;i<strip->led_count;i++)
		{
//			ws2812_set_brightness();//set brighness
//			ws2812_set_led_color(strip->ledsarrPtr+i,120,30,5);//set colors
			ws2812_animate_flow1(strip->ledsarrPtr+i);
			ws2812_encode_led(strip->ledsarrPtr+i, i, strip);//encode for proper positon
		}
		ws2812_fill_zerosEnd(strip,50);	//prepare end of frame sequence
		//the whole buffer for led strip was encoded so we can send data now



		LL_TIM_EnableCounter(strip->tim);
		LL_DMA_EnableStream(strip->dma, strip->dma_stream);
	}
}

void ws2812_set_led_count(t_ws2812_STRIP *strip, uint8_t count){
	if(count>120) strip->led_count=120;
	strip->led_count=count;
}
