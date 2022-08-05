#ifndef __TDK_PID_CONTROLLER_H__
#define __TDK_PID_CONTROLLER_H__

#include "stm32f4xx_hal.h"
#include "sensor.h"

typedef struct wheel_Data {
	GPIO_TypeDef* out1_Gpio;
	uint16_t out1_Pin;
	GPIO_TypeDef* out2_Gpio;
	uint16_t out2_Pin;

	GPIO_TypeDef* encoder_Ch1_Gpio;
	uint16_t encoder_Ch1_Pin;
	GPIO_TypeDef* encoder_Ch2_Gpio;
	uint16_t encoder_Ch2_Pin;

	int step;
	int round;

	float desire_Position;
	float position;
	float last_Position;

	float desire_Velocity;
	float setting_Velocity;

	float velocity;
	float velocity_Error_Data[10];

	int pwm;
	float pwm_Data[13][2];

}Wheel_Data_TypeDef;

extern Wheel_Data_TypeDef wheel_1;
extern Wheel_Data_TypeDef wheel_2;

extern TIM_HandleTypeDef *pwm_TIM;
extern TIM_HandleTypeDef *pid_TIM;
extern UART_HandleTypeDef *pid_Uart;

extern int step_Per_Round;
extern int pid_Stop_Count;

extern float position_Pid_Kp;

extern float velocity_Pid_Kp;
extern float velocity_Pid_Ki;
extern float velocity_Pid_Kd;

extern uint8_t rx_Data[10];

void set_Pwm_TIM(TIM_HandleTypeDef *htim);
void set_Pid_TIM(TIM_HandleTypeDef *htim);
void set_Pid_Uart(UART_HandleTypeDef *huart);

void pid_Init();

void pid_Start(uint8_t type, uint8_t value);
void pid_Stop();

void position_Pid_Control();
void velocity_Pid_Control();
void motor_Output();

void wheel_Information_Update(Wheel_Data_TypeDef *wheel);
void velocity_Pid_Calculate(Wheel_Data_TypeDef *wheel);
void pwm_Pid_Matching(Wheel_Data_TypeDef *wheel);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);


#endif
