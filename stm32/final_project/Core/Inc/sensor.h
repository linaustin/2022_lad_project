#ifndef __SENSOR_H__
#define __SENSOR_H__

#include "stm32f4xx_hal.h"

extern ADC_HandleTypeDef *temperature_Adc;
extern ADC_HandleTypeDef *co_Adc;

extern UART_HandleTypeDef *adc_Uart;

extern uint8_t tx_Data[10];

extern float voltage_Unit;
extern float co_Concentration;

extern float temp_Resistance[9][2];

void set_Temp_Adc(ADC_HandleTypeDef *adc);
void set_Co_Adc(ADC_HandleTypeDef *adc);
void set_Adc_Uart(UART_HandleTypeDef *uart);

void adc_Init();
void adc_Capture();

void co_Concentration_Transfer(uint8_t *msg, float raw);
void temp_Transfer(uint8_t *msg, float raw);

#endif
