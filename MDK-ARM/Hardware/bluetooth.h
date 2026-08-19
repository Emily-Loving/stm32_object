#ifndef __bluetooth_H
#define __bluetooth_H

#include "usart.h"

/* 蓝牙命令应答表（按命令号索引，1~4 为方向，5=wait，6=error，7=帧尾分隔符） */
extern const char *CommendBlue[];

/* 初始化蓝牙接收：开启 USART1 中断接收，需在任务开始时调用一次 */
void v_Bluetooth_Init(void);

/* 串口接收完成回调（HAL 弱符号重写，勿改名，在中断上下文执行） */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

/* 蓝牙任务处理函数：从命令队列取命令并应答，应在任务循环中反复调用 */
void v_Bluetooth_Process(void);

/* 索引与命令号对应：1->"up" 2->"down" 3->"left" 4->"right" 5->"wait" 6->"error" */
static const char *CommendBlue[] =
{
    "up",    /* [0] 命令 1 */
    "down",  /* [1] 命令 2 */
    "left",  /* [2] 命令 3 */
    "right", /* [3] 命令 4 */
    "wait",  /* [4] 命令 5 */
    "error", /* [5] 命令 6 */
    "\r\n"    /* [6] 换行符（回车 + 换行） */
};

/// @NOTE 状态机状态
typedef enum
{
    Wait_Head = 0,   /* 等待 '[' */
    Wait_Num,      /* 等待数字 */
    Wait_Tail       /* 等待 ']' */
} FrameState_t;


#endif
