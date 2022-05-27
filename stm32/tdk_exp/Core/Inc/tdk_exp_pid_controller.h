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

	uint32_t step;
	int round;

	uint32_t last_Step;
	int last_Round;

	int velocity;

	int error_Data[10];

	int pwm_Current;
	int pwm_Min;
	int pwm_Max;
	int pwm_Rate;


}Wheel_Data_TypeDef;

Wheel_Data_TypeDef wheel_1;
Wheel_Data_TypeDef wheel_2;

TIM_HandleTypeDef *pid_htim2;
TIM_HandleTypeDef *pid_htim4;

int step_Per_Round;

int pid_Coefficient_Kp;
int pid_Coefficient_Ki;
int pid_Coefficient_Kd;

void velocity_Pid_Init();
void velocity_Pid_Set_Tim2(TIM_HandleTypeDef *htim);
void velocity_Pid_Set_Tim4(TIM_HandleTypeDef *htim);

int velocity_Pid_PWM_control(Wheel_Data_TypeDef *wheel);

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);


#endif
