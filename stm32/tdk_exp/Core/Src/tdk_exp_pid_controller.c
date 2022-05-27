#include "tdk_exp_pid_controller.h"

Wheel_Data_TypeDef wheel_1 = {
		.out1_Gpio = GPIOE,
		.out1_Pin  = GPIO_PIN_4,
		.out2_Gpio = GPIOE,
		.out2_Pin = GPIO_PIN_5,

		.encoder_Ch1_Gpio = GPIOB,
		.encoder_Ch1_Pin = GPIO_PIN_12,
		.encoder_Ch2_Gpio = GPIOB,
		.encoder_Ch2_Pin = GPIO_PIN_13
};

Wheel_Data_TypeDef wheel_2 = {
		.out1_Gpio = GPIOE,
		.out1_Pin  = GPIO_PIN_6,
		.out2_Gpio = GPIOE,
		.out2_Pin = GPIO_PIN_7,

		.encoder_Ch1_Gpio = GPIOB,
		.encoder_Ch1_Pin = GPIO_PIN_14,
		.encoder_Ch2_Gpio = GPIOB,
		.encoder_Ch2_Pin = GPIO_PIN_15
};

void velocity_Pid_Init(){
	//clear error_Data
	for(int i = 0; i < 10; i++){
			wheel_1.error_Data[i] = 0;
			wheel_2.error_Data[i] = 0;
	}

	//clear wheel_1 counting data
	wheel_1.step = 0;
	wheel_1.round = 0;
	wheel_1.last_Step = 0;
	wheel_1.last_Round = 0;

	//clear wheel_2 counting data
	wheel_2.step = 0;
	wheel_2.round = 0;
	wheel_2.last_Step = 0;
	wheel_2.last_Round = 0;

	//setting pid controller coefficient
	pid_Coefficient_Kp = 1;
	pid_Coefficient_Ki = 1;
	pid_Coefficient_Kd = 1;

	//setting step per round
	step_Per_Round = 1000;

	return;
}

void velocity_Pid_Set_Tim2(TIM_HandleTypeDef *htim){
	pid_htim2 = htim;
	return;
}

void velocity_Pid_Set_Tim4(TIM_HandleTypeDef *htim){
	//setting htim4 struct pointer
	pid_htim4 = htim;
	return;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){





	return;
}
