#ifndef __bluetooth_H
#define __bluetooth_H

#include "usart.h"
#include <string.h>
#include "Variable.h"

// 索引与命令号对应：1->up 2->down 3->left 4->right 5->go 6->back 7->wait 8->error 见Variable.h的指令表
static const char *CommendBlue[14] =
{
    "up",       // [0] 命令 1 
    "down",     // [1] 命令 2 
    "left",     // [2] 命令 3 
    "right",    // [3] 命令 4 
    "go",       // [4] 命令 5（前进）
    "back",     // [5] 命令 6（后退）
    "wait",     // [6] 命令 7 / 无命令 
    "error",    // [7] 命令 8 / 非法 
    "\r\n",     // [8] 换行符（回车 + 换行） 
	";",		// [9]
	"Pitch:",	// [10]
	"roll:",	// [11]
	"Batter:",	// [12] 电池电量查询响应前缀
	"%"         // [13] 电量百分号后缀
};

/// @NOTE 状态机状态
typedef enum
{
	up = 0,
	down,
	left,
	right,
	go,
	back,
	wait,
	error,
} BlueFly_t;

/// @NOTE 状态机状态
typedef enum
{
    Wait_Head = 0,   // 等待 [ 
    Wait_Num,        // 等待数字 
    Wait_Tail        // 等待 ] 
} FrameState_t;

void v_Bluetooth_Init(void);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
uint8_t v_Bluetooth_Process(void);

#endif
