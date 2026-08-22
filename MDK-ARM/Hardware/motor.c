#include "motor.h"

/// @NOTE 初始化四路电机 PWM 并全部置为停机状态
/// @param void
/// @return
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

/// @NOTE 控制单个电机做一步加/减速（在当前 CCR 上按步长增减并夹到上下限）
/// @param Num 电机编号
/// @param State 电机状态 2 单击加速 3 单击减速
/// @return
void Hanlde_Motorapp(Motor_Number_t Num, Motor_State_t State)
{
	
	if (State == Motor_State2)
	{
		uint32_t u32_Motor_Ccr = return_CCR(Num);
		u32_Motor_Ccr += MOTOR_STEP;

		if (u32_Motor_Ccr >= MOTOR_MAX_CCR)
		{
			u32_Motor_Ccr = MOTOR_MAX_CCR;
		}

		switch (Num)
			{
				case Motor_1:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, u32_Motor_Ccr);break;}
				case Motor_2:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, u32_Motor_Ccr);break;}
				case Motor_3:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, u32_Motor_Ccr);break;}
				case Motor_4:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, u32_Motor_Ccr);break;}
				default: break;
			}				
	}
	
	else if (State == Motor_State3)
	{
		uint32_t u32_Motor_Ccr = return_CCR(Num);

		/* 减速：无符号先防下溢，再夹在下限 */
		if (u32_Motor_Ccr <= MOTOR_MIN_CCR + MOTOR_STEP)
		{
			u32_Motor_Ccr = MOTOR_MIN_CCR;
		}
		else
		{
			u32_Motor_Ccr -= MOTOR_STEP;
		}

		switch (Num)
		{
			case Motor_1:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, u32_Motor_Ccr);break;}
			case Motor_2:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, u32_Motor_Ccr);break;}
			case Motor_3:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, u32_Motor_Ccr);break;}
			case Motor_4:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, u32_Motor_Ccr);break;}
			default: break;
		}	
	}
}

/// @NOTE 读取指定通道当前的 CCR 计数值
/// @param ch 电机编号
/// @return 该通道当前的 CCR 计数值，非法编号返回 0
uint32_t return_CCR(Motor_Number_t ch)
{
	
	uint32_t u32_Motor_DutyCh1 = htim2.Instance->CCR1;
	uint32_t u32_Motor_DutyCh2 = htim2.Instance->CCR2;
	uint32_t u32_Motor_DutyCh3 = htim2.Instance->CCR3;
	uint32_t u32_Motor_DutyCh4 = htim2.Instance->CCR4;
	
	if (ch == Motor_1){return u32_Motor_DutyCh1;}
	else if (ch == Motor_2){return u32_Motor_DutyCh2;}
	else if (ch == Motor_3){return u32_Motor_DutyCh3;}
	else if (ch == Motor_4){return u32_Motor_DutyCh4;}
	else{return 0;}
}


/// @NOTE 控制单个电机
/// @param Num 电机编号
/// @param State 电机状态 0 停止 1 加速到阈值 2 加速 3 减速
/// @return
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

/// @NOTE 起飞：四路电机同时加速到阈值
/// @param void
/// @return
void Motor_middle(void)
{
	Hanlde_Motor(Motor_1, Motor_State1);
	Hanlde_Motor(Motor_2, Motor_State1);
	Hanlde_Motor(Motor_3, Motor_State1);
	Hanlde_Motor(Motor_4, Motor_State1);
}

/// @NOTE 下降：四路电机同时减速
/// @param void
/// @return
void Motor_down(void)
{
	Hanlde_Motor(Motor_1, Motor_State3);
	Hanlde_Motor(Motor_2, Motor_State3);
	Hanlde_Motor(Motor_3, Motor_State3);
	Hanlde_Motor(Motor_4, Motor_State3);	
}

/// @NOTE 前进：调整前后两路电机转速差实现俯冲前进
/// @param void
/// @return
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

/// @NOTE 后退：调整前后两路电机转速差实现后退
/// @param void
/// @return
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

/// @NOTE 左转：调整左右两路电机转速差实现左转
/// @param void
/// @return
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

/// @NOTE 右转：调整左右两路电机转速差实现右转
/// @param void
/// @return
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

/// @NOTE 缓慢减速停机（单路）：在当前 CCR 上减 MOTOR_STEP 并夹到 0，非阻塞，需任务循环反复调用
/// @param Num 电机编号
/// @return
void Motor_stopapp(Motor_Number_t Num)
{
	uint32_t u32_Motor_Ccr = return_CCR(Num);

	if (u32_Motor_Ccr <= MOTOR_STEP)
	{
		u32_Motor_Ccr = 0;                 
	}
	else
	{
		u32_Motor_Ccr -= MOTOR_STEP;   
	}

	switch (Num)
	{
		case Motor_1:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, u32_Motor_Ccr);break;}
		case Motor_2:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, u32_Motor_Ccr);break;}
		case Motor_3:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, u32_Motor_Ccr);break;}
		case Motor_4:{__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, u32_Motor_Ccr);break;}
		default: break;
	}
}

/// @NOTE 缓慢减速停机：4 路各减一档，非阻塞逐步到 0
/// @param void
/// @return
void Motor_stop(void)
{
	Motor_stopapp(Motor_1);
	Motor_stopapp(Motor_2);
	Motor_stopapp(Motor_3);
	Motor_stopapp(Motor_4);
}

/// @NOTE 按蓝牙命令号分发对应飞行动作，未定义动作(wait/error/越界)一律停机
/// @param Num 命令号 0 起飞 1 下降 2 左 3 右 4 前进 5 后退
/// @return
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

/// @NOTE 保护函数 处理 mpu 给的数据：角度超限则起飞回正
/// @param Date MPU6050 解算后的角度数据
/// @return 0 正常 1 错误
uint8_t Analyse_Date_pid(MPU6050_HandleDataTDF *Date)
{
	uint8_t uc_Motor_Flag = 1;
	if (Date -> f_Mpu_Pitch >= FPITCH_ANGLE_MAX || Date -> f_Mpu_Pitch <= FPITCH_ANGLE_MIN)
	{
		Motor_middle();
		uc_Motor_Flag = 1;
		return uc_Motor_Flag;		
	}
	else if (Date -> f_Mpu_Roll >= FPITCH_ANGLE_MAX || Date -> f_Mpu_Roll <= FPITCH_ANGLE_MIN)
	{
		Motor_middle();
		uc_Motor_Flag = 1;
		return uc_Motor_Flag;
	}
	else
	{
		return 0;
	}
}
