#include <string.h>
#include "stm32f1xx.h"
#define TIMEOUT 		10
#define BUFFER_SIZE 	4096
#define ADC_CHANNELS 	4

UART_HandleTypeDef uart;
DMA_HandleTypeDef dma;
ADC_HandleTypeDef adc;

uint8_t	src_buffer[BUFFER_SIZE];
uint8_t dst_buffer[BUFFER_SIZE];
uint16_t adc_values[ADC_CHANNELS];

void send_char(char c) {
	HAL_UART_Transmit(&uart, (uint8_t*)&c, 1, TIMEOUT);
}

int __io_putchar(int ch) {
	send_char(ch);
	return ch;
}

int main(void) {
	uint8_t duty = 0;
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
	adc.Init.ContinuousConvMode = ENABLE;
	adc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	adc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	adc.Init.ScanConvMode = ADC_SCAN_ENABLE;
	adc.Init.NbrOfConversion = ADC_CHANNELS;
	adc.Init.DiscontinuousConvMode = DISABLE;
	adc.Init.NbrOfDiscConversion= 1;
	HAL_ADC_Init(&adc);
/*--------------------------adc config end---------------------------*/
/*------------------------adc ch config start-------------------------*/
	ADC_ChannelConfTypeDef adc_ch;
	adc_ch.Channel = ADC_CHANNEL_0;
	adc_ch.Rank = ADC_REGULAR_RANK_1;
	adc_ch.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
	HAL_ADC_ConfigChannel(&adc, &adc_ch);

	adc_ch.Channel = ADC_CHANNEL_1;
	adc_ch.Rank = ADC_REGULAR_RANK_2;
	HAL_ADC_ConfigChannel(&adc, &adc_ch);

	adc_ch.Channel = ADC_CHANNEL_4;
	adc_ch.Rank = ADC_REGULAR_RANK_3;
	HAL_ADC_ConfigChannel(&adc, &adc_ch);

	adc_ch.Channel = ADC_CHANNEL_8;
	adc_ch.Rank = ADC_REGULAR_RANK_4;
	HAL_ADC_ConfigChannel(&adc, &adc_ch);
/*-------------------------adc ch config end--------------------------*/
	HAL_ADCEx_Calibration_Start(&adc);
/*-------------------------dma config start--------------------------*/
	dma.Instance = DMA1_Channel1;
	dma.Init.Direction = DMA_PERIPH_TO_MEMORY;
	dma.Init.PeriphInc= DMA_PINC_DISABLE;
	dma.Init.MemInc = DMA_MINC_ENABLE;
	dma.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	dma.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	dma.Init.Mode = DMA_CIRCULAR;
	dma.Init.Priority = DMA_PRIORITY_HIGH;
	HAL_DMA_Init(&dma);
	__HAL_LINKDMA(&adc, DMA_Handle, dma);
/*--------------------------dma config end---------------------------*/

	HAL_ADC_Start_DMA(&adc, (uint32_t*)adc_values, ADC_CHANNELS);

	while (1) {
		for(int i = 0; i < ADC_CHANNELS; i++) {
			printf("ADC Ch%d = %4d  ", i, adc_values[i]);
		}
		printf("\n");
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		HAL_Delay(400);
	}

}
