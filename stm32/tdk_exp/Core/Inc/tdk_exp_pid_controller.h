#ifndef __TDK_EXP_PID_CONTROLLER_H__
#define __TDK_EXP_PID_CONTROLLER_H__

#include "main.h"

typedef struct wheel_Data {
	GPIO_TypeDef* out1_Gpio;
	uint16_t out1_Pin;
	GPIO_TypeDef* out2_Gpio;
	uint16_t out2_Pin;

	GPIO_TypeDef* encoder_Ch1_Gpio;
	uint16_t encoder_Ch1_Pin;
	GPIO_TypeDef* encoder_Ch2_Gpio;
	uint16_t encoder_Ch2_Pin;

	uint8_t encoder_Ch1_State;
	uint8_t encoder_Ch2_State;

	uint32_t count;
	int round;

	uint32_t last_Count;
	int last_Round;

	int velocity;
}Wheel_Data_TypeDef;

Wheel_Data_TypeDef wheel_1;
Wheel_Data_TypeDef wheel_2;

int pid_Coefficient_Kp;
int pid_Coefficient_Ki;
int pid_Coefficient_Kd;

int error_Data[2][10];

int pid_Init();
void pid_PWM_Cal(Wheel_Data_TypeDef *wheel);
void pid_Output();

#endif
