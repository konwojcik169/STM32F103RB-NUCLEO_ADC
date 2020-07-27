/**
 ******************************************************************************
 * @file    main.c
 * @author  Ac6
 * @version V1.0
 * @date    01-December-2013
 * @brief   Default main function.
 ******************************************************************************
 */

#include <string.h>
#include <stm32f1xx.h>
#define TIMEOUT 		10
#define BUFFER_SIZE 	4096

UART_HandleTypeDef uart;
ADC_HandleTypeDef adc;

void send_char(char c) {
	HAL_UART_Transmit(&uart, (uint8_t*)&c, 1, TIMEOUT);
}

int __io_putchar(int ch) {
	send_char(ch);
	return ch;
}

int adc_read(uint32_t channel) {
	ADC_ChannelConfTypeDef adc_ch;
	adc_ch.Channel = channel;
	adc_ch.Rank = ADC_REGULAR_RANK_1;
	adc_ch.SamplingTime = ADC_SAMPLETIME_13CYCLES_5;
	HAL_ADC_ConfigChannel(&adc, &adc_ch);

	HAL_ADC_Start(&adc);
	HAL_ADC_PollForConversion(&adc, 1000);

	return HAL_ADC_GetValue(&adc);
}

int main(void) {
	int i = 0;

	SystemCoreClock = 8000000;				// clock 8MHz
	HAL_Init();

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_DMA1_CLK_ENABLE();
	__HAL_RCC_ADC1_CLK_ENABLE();

	GPIO_InitTypeDef gpio;					// GPIO object
/*-----------------------------LD2 start-----------------------------*/
	gpio.Pin = GPIO_PIN_5;
	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &gpio);
/*------------------------------LD2 end------------------------------*/
/*---------------------config analog inputs start--------------------*/
	gpio.Mode = GPIO_MODE_ANALOG;
	gpio.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	HAL_GPIO_Init(GPIOA, &gpio);
/*----------------------config analog inputs end---------------------*/

	gpio.Pin = GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9;
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &gpio);
/*--------------------------usart2 RX start--------------------------*/
	gpio.Mode = GPIO_MODE_AF_PP;
	gpio.Pin = GPIO_PIN_2;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &gpio);
/*---------------------------usart2 RX end---------------------------*/
/*--------------------------usart2 TX start--------------------------*/
	gpio.Mode = GPIO_MODE_AF_INPUT;
	gpio.Pin = GPIO_PIN_3;
	HAL_GPIO_Init(GPIOA, &gpio);
/*---------------------------usart2 TX end---------------------------*/
/*------------------------usart2 config start------------------------*/
	uart.Instance = USART2;
	uart.Init.BaudRate = 115200;
	uart.Init.WordLength = UART_WORDLENGTH_8B;
	uart.Init.Parity = UART_PARITY_NONE;
	uart.Init.StopBits = UART_STOPBITS_1;
	uart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart.Init.OverSampling = UART_OVERSAMPLING_16;
	uart.Init.Mode = UART_MODE_TX_RX;
	HAL_UART_Init(&uart);
/*-------------------------usart2 config end-------------------------*/
/*----------------------adc clock config start-----------------------*/
	RCC_PeriphCLKInitTypeDef adc_clk;
	adc_clk.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	adc_clk.AdcClockSelection = RCC_ADCPCLK2_DIV2;
	HAL_RCCEx_PeriphCLKConfig(&adc_clk);
/*-----------------------adc clock config end------------------------*/
/*-------------------------adc config start--------------------------*/
	adc.Instance = ADC1;
	adc.Init.ContinuousConvMode = DISABLE;
	adc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc.Init.ScanConvMode = ADC_SCAN_DISABLE;
	adc.Init.NbrOfConversion = 1;
	adc.Init.DiscontinuousConvMode = DISABLE;
	adc.Init.NbrOfDiscConversion= 1;
	HAL_ADC_Init(&adc);
/*--------------------------adc config end---------------------------*/

	HAL_ADCEx_Calibration_Start(&adc);
	HAL_ADC_Start(&adc);

	while (1) {
		uint16_t value = adc_read(ADC_CHANNEL_0);
		float v = (float)value * 3.3f / 4096.0f;
		printf("ADC0 = %d (%.3fV)    ", value, v);

		value = adc_read(ADC_CHANNEL_1);
		v = (float)value * 3.3f / 4096.0f;
		printf("ADC1 = %d (%.3fV)\n", value, v);
		HAL_Delay(500);
	}

}
