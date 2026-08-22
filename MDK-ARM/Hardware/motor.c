#include "motor.h"

/// @NOTE 占空比(0.0~1.0) -> CCR 计数值。ARR 从定时器动态读取，
///       与 CubeMX 里配置的 Period 自动保持一致，避免写死。
static uint32_t Duty_To_CCR(float duty)
{
	uint32_t arr = __HAL_TIM_GET_AUTORELOAD(&htim2);   // = htim2.Instance->ARR
	if (duty < 0.0f) duty = 0.0f;
	if (duty > 1.0f) duty = 1.0f;
	// +1 是因为 PWM 周期实际为 (ARR+1) 个计数 
	return (uint32_t)(duty * (float)(arr + 1) + 0.5f);
}

void Motor_Init(void)
{
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
		uint32_t CCR = return_CCR(Num);

		CCR += MOTOR_STEP;

		/* 加速：升到上限就夹在上限 */
		if (CCR >= MOTOR_MAX_CCR)
		{
			CCR = MOTOR_MAX_CCR;
		}

		switch (Num)
			{
				case Motor_1:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, CCR);break;}
				case Motor_2:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, CCR);break;}
				case Motor_3:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, CCR);break;}
				case Motor_4:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, CCR);break;}
				default: break;
			}				
	}
	
	else if (State == Motor_State3)
	{
		uint32_t CCR = return_CCR(Num);

		/* 减速：无符号先防下溢，再夹在下限 */
		if (CCR <= MOTOR_MIN_CCR + MOTOR_STEP)
		{
			CCR = MOTOR_MIN_CCR;
		}
		else
		{
			CCR -= MOTOR_STEP;
		}

		switch (Num)
		{
			case Motor_1:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, CCR);break;}
			case Motor_2:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, CCR);break;}
			case Motor_3:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, CCR);break;}
			case Motor_4:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, CCR);break;}
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
				case Motor_State0:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,MOTOR_STOP_CCR);break;}			
				case Motor_State1:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1,MOTOR_MAX_CCR);break;}			
				case Motor_State2:{Hanlde_Motorapp(Motor_1,Motor_State2);break;}
				case Motor_State3:{Hanlde_Motorapp(Motor_1,Motor_State3);break;}
				default: break;
			}
			break;
		}
		case Motor_2:
		{
			switch (State)
			{
				case Motor_State0:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,MOTOR_STOP_CCR);break;}			
				case Motor_State1:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2,MOTOR_MAX_CCR);break;}			
				case Motor_State2:{Hanlde_Motorapp(Motor_2,Motor_State2);break;}
				case Motor_State3:{Hanlde_Motorapp(Motor_2,Motor_State3);break;}
				default: break;
			}
			break;
		}
		case Motor_3:
		{	
			switch (State)
			{
				case Motor_State0:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,MOTOR_STOP_CCR);break;}			
				case Motor_State1:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3,MOTOR_MAX_CCR);break;}			
				case Motor_State2:{Hanlde_Motorapp(Motor_3,Motor_State2);break;}
				case Motor_State3:{Hanlde_Motorapp(Motor_3,Motor_State3);break;}
				default: break;
			}
			break;
		}
		case Motor_4:
		{
			switch (State)
			{
				case Motor_State0:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4,MOTOR_STOP_CCR);break;}			
				case Motor_State1:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4,MOTOR_MAX_CCR);break;}			
				case Motor_State2:{Hanlde_Motorapp(Motor_4,Motor_State2);break;}
				case Motor_State3:{Hanlde_Motorapp(Motor_4,Motor_State3);break;}
				default: break;
			}
			break;
		}
		default: break;
	}
}

/// @NOTE 	起飞
void Motor_middle(void)
{
	Hanlde_Motor(Motor_1, Motor_State1);
	Hanlde_Motor(Motor_2, Motor_State1);
	Hanlde_Motor(Motor_3, Motor_State1);
	Hanlde_Motor(Motor_4, Motor_State1);
}

/// @NOTE    下降
void Motor_down(void)
{
	Hanlde_Motor(Motor_1, Motor_State3);
	Hanlde_Motor(Motor_2, Motor_State3);
	Hanlde_Motor(Motor_3, Motor_State3);
	Hanlde_Motor(Motor_4, Motor_State3);	
}

/// @NOTE	前进
void Motor_go(void)
{
	Hanlde_Motor(Motor_1, Motor_State1);
	Hanlde_Motor(Motor_2, Motor_State1);
	Hanlde_Motor(Motor_3, Motor_State1);
	Hanlde_Motor(Motor_4, Motor_State1);
	
	Hanlde_Motor(Motor_1, Motor_State3);
	Hanlde_Motor(Motor_2, Motor_State3);
	Hanlde_Motor(Motor_3, Motor_State1);
	Hanlde_Motor(Motor_4, Motor_State1);	
}

/// @NOTE	后退
void Motor_back(void)
{
	Hanlde_Motor(Motor_1, Motor_State1);
	Hanlde_Motor(Motor_2, Motor_State1);
	Hanlde_Motor(Motor_3, Motor_State1);
	Hanlde_Motor(Motor_4, Motor_State1);
	
	Hanlde_Motor(Motor_1, Motor_State1);
	Hanlde_Motor(Motor_2, Motor_State1);
	Hanlde_Motor(Motor_3, Motor_State3);
	Hanlde_Motor(Motor_4, Motor_State3);	
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

/// @NOTE 缓慢减速停机（单路）：学 Hanlde_Motorapp 的减速框架，
///       直接在当前 CCR 计数值上减 MOTOR_STEP，夹到 0，
///       非阻塞，依赖任务循环反复调用逐步减到 0。
void Motor_stopapp(Motor_Number_t Num)
{
	uint32_t CCR = return_CCR(Num);

	if (CCR <= MOTOR_STEP)
	{
		CCR = 0;                 
	}
	else
	{
		CCR -= MOTOR_STEP;   /* 递减一个步长 */
	}

	switch (Num)
	{
		case Motor_1:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, CCR);break;}
		case Motor_2:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, CCR);break;}
		case Motor_3:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, CCR);break;}
		case Motor_4:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, CCR);break;}
		default: break;
	}
}

/// @NOTE 缓慢减速停机：4 路各减一档，非阻塞逐步到 0
void Motor_stop(void)
{
	Motor_stopapp(Motor_1);
	Motor_stopapp(Motor_2);
	Motor_stopapp(Motor_3);
	Motor_stopapp(Motor_4);
}

void useMotor(uint8_t Num)
{
	switch (Num)
	{
		case 0: {Motor_middle(); break;}
		case 1: {Motor_down(); break;}
		case 2: {Motor_left(); break;}
		case 3: {Motor_right(); break;}
		case 4: {Motor_go(); break;}
		case 5: {Motor_back(); break;}
		default: {Motor_stop(); break;}   /* 非定义动作(wait/error/越界)一律停机 */
	}
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

