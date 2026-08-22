#ifndef __motor_H
#define __motor_H

#include "tim.h"
#include "pid.h"
#include "Variable.h"

typedef enum
{
	Motor_1 = 1,
	Motor_2,
	Motor_3,
	Motor_4,
}Motor_Number_t;

//0 停止 1加速到阈值 2 加速 3减速 见Variable.h的指令表
typedef enum
{	
	Motor_State0 = 0,
	Motor_State1 = 1,
	Motor_State2 = 2,
	Motor_State3 = 3,
}Motor_State_t;

void Motor_Init(void);
void Hanlde_Motorapp(Motor_Number_t Num, Motor_State_t State);
uint32_t return_CCR(Motor_Number_t ch);
void Hanlde_Motor(Motor_Number_t Num, Motor_State_t State);
void Motor_middle(void);
void Motor_down(void);
void Motor_go(void);
void Motor_back(void);
void Motor_left(void);
void Motor_right(void);
void Motor_stopapp(Motor_Number_t Num);
void Motor_stop(void);
void useMotor(uint8_t Num);
uint8_t Analyse_Date_pid(MPU6050_HandleDataTDF *Date);

#endif
