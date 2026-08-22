#include "bluetooth.h"

#define BUFFER_SIZE 8

static uint8_t uc_ReceiveCom = 0;                // 中断接收缓冲（1 字节） 
static volatile uint8_t ucBuffer[BUFFER_SIZE];   // 命令队列 
static volatile uint8_t ucWrite = 0;             // 写索引（中断侧） 
static volatile uint8_t ucRead  = 0;             // 读索引（任务侧） 


static volatile FrameState_t eFrame = Wait_Head;

void v_Bluetooth_Init(void)
{
    eFrame     = Wait_Head;
    ucWrite = 0;
    ucRead  = 0;

    HAL_UART_Receive_IT(&huart1, &uc_ReceiveCom, 1);
}

/// @NOTE 把数据存入ucBuffer
static void v_CmdQueuePush(uint8_t ucCh)
{
    uint8_t ucNext = (uint8_t)((ucWrite + 1) % BUFFER_SIZE);

    if (ucNext != ucRead)         
    {
        ucBuffer[ucWrite] = ucCh;
        ucWrite = ucNext;
    }
}

/// @NOTE 队列空就返回数据
static uint8_t uc_CmdQueuePop(void)
{
    uint8_t ucCh;

    if (ucRead == ucWrite)      // 队列空 
    {
        return 0;
    }

    ucCh = ucBuffer[ucRead];
    ucRead = (uint8_t)((ucRead + 1) % BUFFER_SIZE);
    return ucCh;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint8_t ucCh;

    if (huart->Instance != USART1)
    {
        return;
    }

    ucCh = uc_ReceiveCom;

    switch (eFrame)
    {
        case Wait_Head:
            if (ucCh == '[')
            {
                eFrame = Wait_Num;
            }
            break;

        case Wait_Num:
            if ((ucCh >= '1') && (ucCh <= '6'))
            {
                v_CmdQueuePush((uint8_t)(ucCh - '0'));   // '1'->1 ... '6'->6 
                eFrame = Wait_Tail;
            }
            else if (ucCh == '[')
            {
                eFrame = Wait_Num;     // 重新开始一帧 
            }
            else
            {
                eFrame = Wait_Head;    // 非法字符，放弃本帧 
            }
            break;

        case Wait_Tail:
            if (ucCh == ']')
            {
                eFrame = Wait_Head;    
            }
            else if (ucCh == '[')
            {
                eFrame = Wait_Num;
            }
            else
            {
                eFrame = Wait_Head;
            }
            break;

        default:
            eFrame = Wait_Head;
            break;
    }
    HAL_UART_Receive_IT(&huart1, &uc_ReceiveCom, 1);
}

/// @NOTE 从命令队列取命令，带状态保持：收到新命令后持续返回该命令，
///       直到下一个新命令到来才切换（队列空时保持上次状态）。
/// @return 0=up 1=down 2=left 3=right 4=go 5=back 6=wait 7=error 见Variable.h的指令表
uint8_t v_Bluetooth_Process(void)
{
	static uint8_t s_LastCmd = 6;

    uint8_t ucCh = uc_CmdQueuePop();

    if (ucCh == 0)
    {     
        return s_LastCmd;					// 队列空 没有新命令 保持上一个latch状态 让电机持续转
    }
    else if ((ucCh >= 1) && (ucCh <= 6))	//见Variable.h的指令表
    {
        s_LastCmd = (uint8_t)(ucCh - 1);    //见Variable.h的指令表
    }
    else if (ucCh == 7)
    {
        s_LastCmd = 6;       				//见Variable.h的指令表               
    }
    else
    {
        //非法命令 返回 error，但不打断当前保持的动作 
        return 7;							//见Variable.h的指令表
    }

	return s_LastCmd;
}
