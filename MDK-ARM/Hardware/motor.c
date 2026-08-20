#include "motor.h"

void Motor_Init(void)
{
	// 同时开启4路PWM
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
	
	Hanlde_Motorapp(Motor_1, Motor_State0);
	Hanlde_Motorapp(Motor_2, Motor_State0);
	Hanlde_Motorapp(Motor_3, Motor_State0);
	Hanlde_Motorapp(Motor_4, Motor_State0);
}

/// @NOTE 控制单个电机
/// @param Num 电机编号
/// @param State 电机状态 2 单击加速 3单击减速
void Hanlde_Motorapp(Motor_Number_t Num, Motor_State_t State)
{
	
	if (State == Motor_State2)
	{
		uint32_t Servo_CCR = return_CCR(Num);

		Servo_CCR += (MOTOR_ADD *1000);
		
		if (Servo_CCR <= (MOTOR_ALL_MIN * 1000) || Servo_CCR >= (MOTOR_ALL_MAX * 1000))
		{
			Servo_CCR = (MOTOR_MIDDLE * 1000);
		}
		
		switch (Num)
			{
				case Motor_1:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, Servo_CCR);break;}
				case Motor_2:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, Servo_CCR);break;}
				case Motor_3:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, Servo_CCR);break;}
				case Motor_4:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, Servo_CCR);break;}
				default: break;
			}				
	}
	
	else if (State == Motor_State3)
	{
		uint32_t Servo_CCR = return_CCR(Num);

		Servo_CCR -= (MOTOR_ADD *1000);
		
		if (Servo_CCR <= (MOTOR_ALL_MIN * 1000) || Servo_CCR >= (MOTOR_ALL_MAX * 1000))
		{
			Servo_CCR = (MOTOR_MIDDLE * 1000);
		}
		
		switch (Num)
		{
			case Motor_1:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, Servo_CCR);break;}
			case Motor_2:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, Servo_CCR);break;}
			case Motor_3:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, Servo_CCR);break;}
			case Motor_4:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, Servo_CCR);break;}
			default: break;
		}	
	}
}

uint32_t return_CCR(Motor_Number_t ch)
{
	
	uint32_t Duty_ch1 = htim2.Instance->CCR1;
	uint32_t Duty_ch2 = htim2.Instance->CCR2;
	uint32_t Duty_ch3 = htim2.Instance->CCR3;
	uint32_t Duty_ch4 = htim2.Instance->CCR4;
	
	if (ch == Motor_1){return Duty_ch1;}
	else if (ch == Motor_2){return Duty_ch2;}
	else if (ch == Motor_3){return Duty_ch3;}
	else if (ch == Motor_4){return Duty_ch4;}
	else{return 0;}
}


/// @NOTE 控制单个电机
/// @param Num 电机编号
/// @param Stste 电机状态 0 停止 1加速到阈值 2 加速 3减速 
void Hanlde_Motor(Motor_Number_t Num, Motor_State_t State)
{
	switch (Num)
	{
		case Motor_1:
		{	
			switch (State)
			{
				case Motor_State0:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,MOTOR_STOP*1000);break;}			
				case Motor_State1:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,MOTOR_ALL_MAX*1000);break;}			
				case Motor_State2:{Hanlde_Motorapp(Motor_1,Motor_State2);break;}
				case Motor_State3:{Hanlde_Motorapp(Motor_1,Motor_State3);break;}
				default: break;
			}
		}
		case Motor_2:
		{
			switch (State)
			{
				case Motor_State0:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,MOTOR_STOP*1000);break;}			
				case Motor_State1:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,MOTOR_ALL_MAX*1000);break;}			
				case Motor_State2:{Hanlde_Motorapp(Motor_2,Motor_State2);break;}
				case Motor_State3:{Hanlde_Motorapp(Motor_2,Motor_State3);break;}
				default: break;
			}
		}
		case Motor_3:
		{	
			switch (State)
			{
				case Motor_State0:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,MOTOR_STOP*1000);break;}			
				case Motor_State1:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,MOTOR_ALL_MAX*1000);break;}			
				case Motor_State2:{Hanlde_Motorapp(Motor_3,Motor_State2);break;}
				case Motor_State3:{Hanlde_Motorapp(Motor_3,Motor_State3);break;}
				default: break;
			}
		}
		case Motor_4:
		{
			switch (State)
			{
				case Motor_State0:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,MOTOR_STOP*1000);break;}			
				case Motor_State1:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,MOTOR_ALL_MAX*1000);break;}			
				case Motor_State2:{Hanlde_Motorapp(Motor_4,Motor_State2);break;}
				case Motor_State3:{Hanlde_Motorapp(Motor_4,Motor_State3);break;}
				default: break;
			}
		}
	}
}

/// @NOTE 停止
void Motor_stop(void)
{
	Hanlde_Motor(Motor_1, Motor_State0);
	Hanlde_Motor(Motor_2, Motor_State0);
	Hanlde_Motor(Motor_3, Motor_State0);
	Hanlde_Motor(Motor_4, Motor_State0);
}

/// @NOTE 	起飞
void Motor_middle(void)
{
	Hanlde_Motor(Motor_1, Motor_State1);
	Hanlde_Motor(Motor_2, Motor_State1);
	Hanlde_Motor(Motor_3, Motor_State1);
	Hanlde_Motor(Motor_4, Motor_State1);
}

/// @NOTE 左转
void Motor_left(void)
{
	Hanlde_Motor(Motor_1, Motor_State1);
	Hanlde_Motor(Motor_2, Motor_State1);
	Hanlde_Motor(Motor_3, Motor_State1);
	Hanlde_Motor(Motor_4, Motor_State1);
	
	Hanlde_Motor(Motor_1, Motor_State3);
	Hanlde_Motor(Motor_2, Motor_State1);
	Hanlde_Motor(Motor_3, Motor_State1);
	Hanlde_Motor(Motor_4, Motor_State3);
}

/// @NOTE 右转
void Motor_right(void)
{
	Hanlde_Motor(Motor_1, Motor_State1);
	Hanlde_Motor(Motor_2, Motor_State1);
	Hanlde_Motor(Motor_3, Motor_State1);
	Hanlde_Motor(Motor_4, Motor_State1);
	
	Hanlde_Motor(Motor_1, Motor_State1);
	Hanlde_Motor(Motor_2, Motor_State3);
	Hanlde_Motor(Motor_3, Motor_State3);
	Hanlde_Motor(Motor_4, Motor_State1);
}


/// @NOTE 保护函数 处理mpu给的数据
/// @RETURN 0正常 1错误
uint8_t Analyse_Date_pid(MPU6050_HandleDataTDF *Date)
{
	uint8_t Flag = 1;
	if (Date -> fPitch >= FPITCH_ANGLE_MAX || Date -> fPitch <= FPITCH_ANGLE_MIN)
	{
		Motor_middle();
		Flag = 1;
		return Flag;		
	}
	else if (Date -> fRoll >= FPITCH_ANGLE_MAX || Date -> fRoll <= FPITCH_ANGLE_MIN)
	{
		Motor_middle();
		Flag = 1;
		return Flag;
	}
	else
	{
		return 0;
	}
}

