#include "sensor.h"

ADC_HandleTypeDef *temperature_Adc;
ADC_HandleTypeDef *co_Adc;

UART_HandleTypeDef *adc_Uart;

uint8_t tx_Data[10];

float voltage_Unit = 2.72/4096;
float co_Concentration;

float temp_Resistance[9][2] = {
		{0, 27900},
		{10, 18220},
		{20, 12120},
		{30, 8310},
		{40, 5800},
		{50, 4120},
		{60, 3000},
		{70, 2210},
		{80, 1660}
};

void set_Temp_Adc(ADC_HandleTypeDef *adc){
	temperature_Adc = adc;

	return;
}

void set_Co_Adc(ADC_HandleTypeDef *adc){
	co_Adc = adc;

	return;
}

void set_Adc_Uart(UART_HandleTypeDef *uart){
	adc_Uart = uart;

	return;
}

void adc_Init(){

	for(int i = 0; i < 10; i++){
		tx_Data[i] = 0;
	}

	tx_Data[0] = 255;
	tx_Data[9] = 254;

	return;
}

void adc_Capture(){
	float raw;

	HAL_ADC_Start(co_Adc);
	HAL_ADC_PollForConversion(co_Adc, HAL_MAX_DELAY);

	raw = HAL_ADC_GetValue(co_Adc);

	co_Concentration_Transfer(tx_Data, raw);
	HAL_UART_Transmit(adc_Uart, tx_Data, 10, 0xffff);

	for(int i = 1; i < 9; i++){
		tx_Data[i] = 0;
	}

	HAL_ADC_Start(temperature_Adc);
	HAL_ADC_PollForConversion(temperature_Adc, HAL_MAX_DELAY);

	raw = HAL_ADC_GetValue(temperature_Adc);

	temp_Transfer(tx_Data, raw);
	HAL_UART_Transmit(adc_Uart, tx_Data, 10, 0xffff);

	for(int i = 1; i < 9; i++){
		tx_Data[i] = 0;
	}

	return;
}

void co_Concentration_Transfer(uint8_t *msg, float raw){
	float voltage = raw*voltage_Unit;

	float current = voltage/1000;

	float Rs = (5 - voltage)/current;

	co_Concentration = ((((Rs/5041.237) - 1.8)/(0.8 - 1.8))*(1000 - 200)) + 200;

	int ppm = (int)co_Concentration;

	uint16_t data = (uint16_t)ppm;

	msg[1] = 10;
	msg[3] = (data & 0xff);
	msg[2] = ((data >> 8) & 0xff);

	return;
}

void temp_Transfer(uint8_t *msg, float raw){
	float sub_Resistor_Voltage = raw*voltage_Unit;

	float Rt = (4368/sub_Resistor_Voltage) - 910;

	float scanner[2][2];

	for(int i = 9; i > -1; i--){
		if(temp_Resistance[i][1] <= Rt){
			scanner[0][0] = temp_Resistance[i][0];
			scanner[0][1] = temp_Resistance[i][1];
		}

		else if(temp_Resistance[i][1] > Rt){
			scanner[1][0] = temp_Resistance[i][0];
			scanner[1][1] = temp_Resistance[i][1];
			break;
		}
	}

	msg[1] = 1;

	if(scanner[0][1] == Rt){
		int temp = (int)scanner[0][0];
		msg[2] = (uint8_t)temp;
	}
	else{
		int temp = (int)(((Rt - scanner[0][1])/(scanner[1][1] - scanner[0][1]))*(scanner[1][0] - scanner[0][0]) + scanner[0][0]);
		msg[2] = (uint8_t)temp;
	}

	return;
}
