#include <tdk_pid_controller.h>
//setting wheel_1_information
Wheel_Data_TypeDef wheel_1 = {
		.out1_Gpio = GPIOE,
		.out1_Pin  = GPIO_PIN_5,
		.out2_Gpio = GPIOE,
		.out2_Pin = GPIO_PIN_4,

		.encoder_Ch1_Gpio = GPIOB,
		.encoder_Ch1_Pin = GPIO_PIN_12,
		.encoder_Ch2_Gpio = GPIOB,
		.encoder_Ch2_Pin = GPIO_PIN_13,

		.pwm_Data = {
				{150, 0},
				{200, 1},
				{250, 1.91},
				{300, 2.58},
				{400, 3.5},
				{500, 4.03},
				{600, 4.4},
				{700, 4.66},
				{800, 4.87},
				{900, 5},
				{950, 5},
				{980, 5.1},
				{1000, 5.3},
		}
};
//setting wheel_2 information
Wheel_Data_TypeDef wheel_2 = {
		.out1_Gpio = GPIOE,
		.out1_Pin  = GPIO_PIN_6,
		.out2_Gpio = GPIOE,
		.out2_Pin = GPIO_PIN_7,

		.encoder_Ch1_Gpio = GPIOD,
		.encoder_Ch1_Pin = GPIO_PIN_11,
		.encoder_Ch2_Gpio = GPIOD,
		.encoder_Ch2_Pin = GPIO_PIN_12,

		.pwm_Data = {
				{150, 0},
				{200, 0},
				{250, 1},
				{300, 1.85},
				{400, 2.97},
				{500, 3.73},
				{600, 4.16},
				{700, 4.48},
				{800, 4.75},
				{900, 4.9},
				{950, 4.9},
				{980, 5},
				{1000, 5.2},
		}
};

TIM_HandleTypeDef *pwm_TIM;
TIM_HandleTypeDef *pid_TIM;

UART_HandleTypeDef *pid_Uart;

//setting step per round
int step_Per_Round = 780;

int pid_Stop_Count;

//setting position pid controller coefficient
float position_Pid_Kp = 5;

//setting velocity pid controller coefficient
float velocity_Pid_Kp = 1.5;
float velocity_Pid_Ki = 0;
float velocity_Pid_Kd = 0;

uint8_t rx_Data[10];

void set_Pwm_TIM(TIM_HandleTypeDef *htim){
	//setting htim2 struct pointer
	pwm_TIM = htim;
	return;
}

void set_Pid_TIM(TIM_HandleTypeDef *htim){
	//setting htim4 struct pointer
	pid_TIM = htim;
	return;
}

void set_Pid_Uart(UART_HandleTypeDef *huart){
	//setting uart2 struct pointer
	pid_Uart = huart;
	return;
}

void pid_Init(){
	//clear error_Data
	for(int i = 0; i < 10; i++){
			wheel_1.velocity_Error_Data[i] = 0;
			wheel_2.velocity_Error_Data[i] = 0;
	}

	//clear wheel_1 counting data
	wheel_1.step = 0;
	wheel_1.round = 0;
	wheel_1.position = 0;
	wheel_1.last_Position = 0;
	wheel_1.desire_Position = 0;
	wheel_1.velocity = 0;
	wheel_1.pwm = 0;

	//clear wheel_2 counting data
	wheel_2.step = 0;
	wheel_2.round = 0;
	wheel_2.position = 0;
	wheel_2.last_Position = 0;
	wheel_2.desire_Position = 0;
	wheel_2.velocity = 0;
	wheel_2.pwm = 0;

	for(int i = 0; i < 10; i++){
		rx_Data[i] = 0;
	}

	return;
}

void pid_Start(uint8_t type, uint8_t value){
	if(type == 11){
		wheel_1.desire_Position += value;
		wheel_2.desire_Position += value;
	}
	else if(type == 12){
		wheel_1.desire_Position -= value;
		wheel_2.desire_Position -= value;
	}
	else if(type == 13){
		wheel_1.desire_Position -= value;
		wheel_2.desire_Position += value;
	}
	else if(type == 14){
		wheel_1.desire_Position += value;
		wheel_2.desire_Position -= value;
	}

	//clear pid stop count
	pid_Stop_Count = 0;

	//start pwm channel;
	HAL_TIM_PWM_Start(pwm_TIM, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(pwm_TIM, TIM_CHANNEL_2);

	//start pid controller
	HAL_TIM_Base_Start_IT(pid_TIM);

	return;
}

void pid_Stop(){
	HAL_GPIO_WritePin(wheel_1.out1_Gpio, wheel_1.out1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(wheel_1.out2_Gpio, wheel_1.out2_Pin, GPIO_PIN_RESET);

	HAL_GPIO_WritePin(wheel_2.out1_Gpio, wheel_2.out1_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(wheel_2.out2_Gpio, wheel_2.out2_Pin, GPIO_PIN_RESET);

	//stop pwm channel;
	HAL_TIM_PWM_Stop(pwm_TIM, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(pwm_TIM, TIM_CHANNEL_2);

	//stop pid controller
	HAL_TIM_Base_Stop_IT(pid_TIM);

	//adc capture and uart transmit
	adc_Capture();

	//recieve next command
	HAL_UART_Receive_IT(pid_Uart, rx_Data, 10);

	return;
}

void position_Pid_Control(){
	float pos_Error;
	float desire_Velocity;

	//wheel_1 calculate
	pos_Error = wheel_1.desire_Position - wheel_1.position;

	//set dead zone
	if(pos_Error < -0.1 || pos_Error > 0.1){
		desire_Velocity = pos_Error*position_Pid_Kp;

		//set speed limit
		if(desire_Velocity > 3){
			desire_Velocity = 3;
		}
		else if(desire_Velocity < -3){
			desire_Velocity = -3;
		}

		wheel_1.desire_Velocity = desire_Velocity;
	}
	else{
		//at dead zone no velocity;
		wheel_1.desire_Velocity = 0;
	}

	//wheel_2 calculate
	pos_Error = wheel_2.desire_Position - wheel_2.position;

	//set dead zone
	if(pos_Error < -0.1 || pos_Error > 0.1){
		desire_Velocity = pos_Error*position_Pid_Kp;

		//set speed limit
		if(desire_Velocity > 3){
			desire_Velocity = 3;
		}
		else if(desire_Velocity < -3){
			desire_Velocity = -3;
		}

		wheel_2.desire_Velocity = desire_Velocity;
	}
	else{
		//at dead zone no velocity;
		wheel_2.desire_Velocity = 0;
	}

	return;
}

void velocity_Pid_Control(){
	velocity_Pid_Calculate(&wheel_1);
	velocity_Pid_Calculate(&wheel_2);
	pwm_Pid_Matching(&wheel_1);
	pwm_Pid_Matching(&wheel_2);

	return;
}

void motor_Output(){
	if(pid_Stop_Count > 2){
		pid_Stop();
		return;
	}

	if(wheel_1.desire_Velocity == 0 && wheel_2.desire_Velocity == 0){
		pid_Stop_Count++;
	}
	else{
		pid_Stop_Count = 0;
	}

	//output pwm
	pwm_TIM->Instance->CCR1 = wheel_1.pwm;

	//forward
	if(wheel_1.setting_Velocity > 0){
		HAL_GPIO_WritePin(wheel_1.out1_Gpio, wheel_1.out1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(wheel_1.out2_Gpio, wheel_1.out2_Pin, GPIO_PIN_RESET);
	}
	//backward
	else if(wheel_1.setting_Velocity < 0){
		HAL_GPIO_WritePin(wheel_1.out1_Gpio, wheel_1.out1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(wheel_1.out2_Gpio, wheel_1.out2_Pin, GPIO_PIN_SET);
	}
	//stoop
	else{
		HAL_GPIO_WritePin(wheel_1.out1_Gpio, wheel_1.out1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(wheel_1.out2_Gpio, wheel_1.out2_Pin, GPIO_PIN_RESET);
	}

	//output pwm
	pwm_TIM->Instance->CCR2 = wheel_2.pwm;

	//forward
	if(wheel_2.setting_Velocity > 0){
		HAL_GPIO_WritePin(wheel_2.out1_Gpio, wheel_2.out1_Pin, GPIO_PIN_SET);
		HAL_GPIO_WritePin(wheel_2.out2_Gpio, wheel_2.out2_Pin, GPIO_PIN_RESET);
	}
	//backward
	else if(wheel_2.setting_Velocity < 0){
		HAL_GPIO_WritePin(wheel_2.out1_Gpio, wheel_2.out1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(wheel_2.out2_Gpio, wheel_2.out2_Pin, GPIO_PIN_SET);
	}
	//stoop
	else{
		HAL_GPIO_WritePin(wheel_2.out1_Gpio, wheel_2.out1_Pin, GPIO_PIN_RESET);
		HAL_GPIO_WritePin(wheel_2.out2_Gpio, wheel_2.out2_Pin, GPIO_PIN_RESET);
	}

	return;
}

void wheel_Information_Update(Wheel_Data_TypeDef *wheel){
	//update position
	wheel->position = wheel->round + ((float)wheel->step/step_Per_Round);

	//update velocity
	wheel->velocity = (wheel->position - wheel->last_Position)/0.1;

	//update velocity error
	for(int i = 9; i > -1; i--){
		if(i == 0){
			wheel->velocity_Error_Data[i] = wheel->desire_Velocity - wheel->velocity;
		}
		else{
			wheel->velocity_Error_Data[i] = wheel->velocity_Error_Data[i-1];
		}
	}

	//update last position
	wheel->last_Position = wheel->position;

	return;
}

void velocity_Pid_Calculate(Wheel_Data_TypeDef *wheel){
	float velocity_Cal = wheel->desire_Velocity;

	if(velocity_Cal != 0){

		velocity_Cal = velocity_Cal + wheel->velocity_Error_Data[0]*velocity_Pid_Kp;

		for(int i = 0; i < 10; i++){
			velocity_Cal = velocity_Cal + wheel->velocity_Error_Data[i]*velocity_Pid_Ki;
		}

		velocity_Cal = velocity_Cal - ((wheel->velocity_Error_Data[0] - wheel->velocity_Error_Data[1])/0.1)*velocity_Pid_Kd;

		if(velocity_Cal > 5){
			velocity_Cal = 5;
		}
		else if(velocity_Cal < -5){
			velocity_Cal = -5;
		}

		wheel->setting_Velocity = velocity_Cal;
	}
	else{
		wheel->setting_Velocity = 0;
	}

	return;
}

void pwm_Pid_Matching(Wheel_Data_TypeDef *wheel){
	float scanner[2][2];
	float ratio;
	float setting_Velocity;

	//wheel matching
	setting_Velocity = wheel->setting_Velocity;

	if(setting_Velocity < 0){
		setting_Velocity = (-1)*setting_Velocity;
	}
	//scanner scan
	for(int i = 0; i < 13; i++){
		if(wheel->pwm_Data[i][1] <= setting_Velocity){
			scanner[0][0] = wheel->pwm_Data[i][0];
			scanner[0][1] = wheel->pwm_Data[i][1];
		}
		else if(wheel->pwm_Data[i][1] > setting_Velocity){
			scanner[1][0] = wheel->pwm_Data[i][0];
			scanner[1][1] = wheel->pwm_Data[i][1];
			break;
		}
	}

	//calculate wheel ratio;
	ratio = (setting_Velocity - scanner[0][1])/(scanner[1][1] - scanner[0][1]);

	//setting wheel_1 pwm
	if(ratio == 0){
		wheel->pwm = scanner[0][0];
	}
	else{
		wheel->pwm = (ratio*(scanner[1][0] - scanner[0][0])) + scanner[0][0];
	}

	return;
}

//define tim callback function
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim->Instance == pid_TIM->Instance){
		wheel_Information_Update(&wheel_1);
		wheel_Information_Update(&wheel_2);
		position_Pid_Control();
		velocity_Pid_Control();
		motor_Output();
	}

	return;
}

//define exti callback function
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	//check exti source
	if(GPIO_Pin == wheel_1.encoder_Ch1_Pin){
			//ch1 rising edge
			if(HAL_GPIO_ReadPin(wheel_1.encoder_Ch1_Gpio, wheel_1.encoder_Ch1_Pin)){
				//ch2 HIGH and ch1 rising edge
				if(HAL_GPIO_ReadPin(wheel_1.encoder_Ch2_Gpio, wheel_1.encoder_Ch2_Pin)){
					wheel_1.step--;
				}
				//ch2 LOW and ch11 rising edge
				else{
					wheel_1.step++;
				}
			}
			//ch1 falling edge
			else{
				//ch2 HIGH and ch1 falling edge
				if(HAL_GPIO_ReadPin(wheel_1.encoder_Ch2_Gpio, wheel_1.encoder_Ch2_Pin)){
					wheel_1.step++;
				}
				//ch2 LOW and ch1 falling edge
				else{
					wheel_1.step--;
				}
			}
			//round step transfer

			//case 1 positive round
			if(wheel_1.round > 0){
				if(wheel_1.step > step_Per_Round){
					wheel_1.step = 0;
					wheel_1.round++;
				}
				else if(wheel_1.step < 0){
					wheel_1.step = step_Per_Round;
					wheel_1.round--;
				}

				return;
			}
			//case 2 zero round
			else if(wheel_1.round == 0){
				if(wheel_1.step > step_Per_Round){
					wheel_1.step = 0;
					wheel_1.round++;
				}
				else if(wheel_1.step < -1*step_Per_Round){
					wheel_1.step = 0;
					wheel_1.round--;
				}

				return;
			}
			//case 3 negative round
			else if(wheel_1.round < 0){
				if(wheel_1.step < -1*step_Per_Round){
					wheel_1.step = 0;
					wheel_1.round--;
				}
				else if(wheel_1.step > 0){
					wheel_1.step = -1*step_Per_Round;
					wheel_1.round++;
				}

				return;
			}
		}
		//check exti source
		else if(GPIO_Pin == wheel_2.encoder_Ch1_Pin){
			//ch1 rising edge
			if(HAL_GPIO_ReadPin(wheel_2.encoder_Ch1_Gpio, wheel_2.encoder_Ch1_Pin)){
				//ch2 HIGH and ch1 rising edge
				if(HAL_GPIO_ReadPin(wheel_2.encoder_Ch2_Gpio, wheel_2.encoder_Ch2_Pin)){
					wheel_2.step--;
				}
				//ch2 LOW and ch1 rising edge
				else{
					wheel_2.step++;
				}
			}
			//ch1 falling edge
			else{
				//ch2 HIGH and ch1 falling edge
				if(HAL_GPIO_ReadPin(wheel_2.encoder_Ch2_Gpio, wheel_2.encoder_Ch2_Pin)){
					wheel_2.step++;
				}
				//ch2 LOW and ch1 falling edge
				else{
					wheel_2.step--;
				}
			}
			//round step transfer

			//case 1 positive round
			if(wheel_2.round > 0){
				if(wheel_2.step > step_Per_Round){
					wheel_2.step = 0;
					wheel_2.round++;
				}
				else if(wheel_2.step < 0){
					wheel_2.step = step_Per_Round;
					wheel_2.round--;
				}

				return;
			}
			//case 2 zero round
			else if(wheel_2.round == 0){
				if(wheel_2.step > step_Per_Round){
					wheel_2.step = 0;
					wheel_2.round++;
				}
				else if(wheel_2.step < -1*step_Per_Round){
					wheel_2.step = 0;
					wheel_2.round--;
				}

				return;
			}
			//case 3 negative round
			else if(wheel_2.round < 0){
				if(wheel_2.step < -1*step_Per_Round){
					wheel_2.step = 0;
					wheel_2.round--;
				}
				else if(wheel_2.step > 0){
					wheel_2.step = -1*step_Per_Round;
					wheel_2.round++;
				}

				return;
			}
		}

	return;
}
