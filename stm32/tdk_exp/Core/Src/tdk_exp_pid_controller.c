#include "tdk_exp_pid_controller.h"

Wheel_Data_TypeDef wheel_1 = {
		.encoder_Ch1_Gpio = GPIOB,
		.encoder_Ch1_Pin = GPIO_PIN_12,
		.encoder_Ch2_Gpio = GPIOB,
		.encoder_Ch2_Pin = GPIO_PIN_13
};

Wheel_Data_TypeDef wheel_2 = {
		.encoder_Ch1_Gpio = GPIOB,
		.encoder_Ch1_Pin = GPIO_PIN_14,
		.encoder_Ch2_Gpio = GPIOB,
		.encoder_Ch2_Pin = GPIO_PIN_15
};

void pid_Init(){
	//clear err_Data
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 10; j++){
			err_Data[i][j] = 0;
		}
	}

	//clear wheel_1 counting data
	wheel_1.count = 0;
	wheel_1.round = 0;
	wheel_1.last_Count = 0;
	wheel_1.last_Round = 0;

	//clear wheel_2 counting data
	wheel_2.count = 0;
	wheel_2.round = 0;
	wheel_2.last_Count = 0;
	wheel_2.last_Round = 0;

	//setting pid controller coefficient
	pid_Coefficient_Kp = 1;
	pid_Coefficient_Ki = 1;
	pid_Coefficient_Kd = 1;

	HAL_TIM_Base_Start_IT(&htim4);

	return;
}
