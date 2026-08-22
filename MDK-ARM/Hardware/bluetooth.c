#include "bluetooth.h"

#define BUFFER_SIZE 8

static uint8_t uc_Blue_ReceiveCom = 0;                // 中断接收缓冲（1 字节） 
static volatile uint8_t uc_Blue_Buffer[BUFFER_SIZE];  // 命令队列 
static volatile uint8_t uc_Blue_Write = 0;            // 写索引（中断侧） 
static volatile uint8_t uc_Blue_Read  = 0;            // 读索引（任务侧） 


static volatile FrameState_t e_Blue_Frame = Wait_Head;

/// @NOTE 蓝牙初始化：复位帧状态机与读写索引，开启串口接收中断
/// @param void
/// @return
void v_Bluetooth_Init(void)
{
    e_Blue_Frame = Wait_Head;
    uc_Blue_Write = 0;
    uc_Blue_Read  = 0;

    HAL_UART_Receive_IT(&huart1, &uc_Blue_ReceiveCom, 1);
}

/// @NOTE 把数据存入命令队列（队列满则丢弃）
/// @param uc_Blue_Ch 待入队的命令字节
/// @return
static void v_CmdQueuePush(uint8_t uc_Blue_Ch)
{
    uint8_t uc_Blue_Next = (uint8_t)((uc_Blue_Write + 1) % BUFFER_SIZE);

    if (uc_Blue_Next != uc_Blue_Read)         
    {
        uc_Blue_Buffer[uc_Blue_Write] = uc_Blue_Ch;
        uc_Blue_Write = uc_Blue_Next;
    }
}

/// @NOTE 从命令队列取一个字节
/// @param void
/// @return 取出的命令字节，队列空返回 0
static uint8_t uc_CmdQueuePop(void)
{
    uint8_t uc_Blue_Ch;

    if (uc_Blue_Read == uc_Blue_Write)      // 队列空 
    {
        return 0;
    }

    uc_Blue_Ch = uc_Blue_Buffer[uc_Blue_Read];
    uc_Blue_Read = (uint8_t)((uc_Blue_Read + 1) % BUFFER_SIZE);
    return uc_Blue_Ch;
}

/// @NOTE 串口接收完成回调：按 [数字] 帧格式的状态机解析并把命令入队
/// @param huart 触发回调的串口句柄
/// @return
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    uint8_t uc_Blue_Ch;

    if (huart->Instance != USART1)
    {
        return;
    }

    uc_Blue_Ch = uc_Blue_ReceiveCom;

    switch (e_Blue_Frame)
    {
        case Wait_Head:
            if (uc_Blue_Ch == '[')
            {
                e_Blue_Frame = Wait_Num;
            }
            break;

        case Wait_Num:
            if ((uc_Blue_Ch >= '1') && (uc_Blue_Ch <= '6'))
            {
                v_CmdQueuePush((uint8_t)(uc_Blue_Ch - '0'));   // '1'->1 ... '6'->6 
                e_Blue_Frame = Wait_Tail;
            }
            else if (uc_Blue_Ch == '[')
            {
                e_Blue_Frame = Wait_Num;     // 重新开始一帧 
            }
            else
            {
                e_Blue_Frame = Wait_Head;    // 非法字符，放弃本帧 
            }
            break;

        case Wait_Tail:
            if (uc_Blue_Ch == ']')
            {
                e_Blue_Frame = Wait_Head;    
            }
            else if (uc_Blue_Ch == '[')
            {
                e_Blue_Frame = Wait_Num;
            }
            else
            {
                e_Blue_Frame = Wait_Head;
            }
            break;

        default:
            e_Blue_Frame = Wait_Head;
            break;
    }
    HAL_UART_Receive_IT(&huart1, &uc_Blue_ReceiveCom, 1);
}

/// @NOTE 从命令队列取命令，带状态保持：收到新命令后持续返回该命令，
///       直到下一个新命令到来才切换（队列空时保持上次状态）。
/// @param void
/// @return 0=up 1=down 2=left 3=right 4=go 5=back 6=wait 7=error 见 Variable.h 的指令表
uint8_t v_Bluetooth_Process(void)
{
	static uint8_t uc_Blue_LastCmd = 6;

    uint8_t uc_Blue_Ch = uc_CmdQueuePop();

    if (uc_Blue_Ch == 0)
    {     
        return uc_Blue_LastCmd;							// 队列空 没有新命令 保持上一个latch状态 让电机持续转
    }
    else if ((uc_Blue_Ch >= 1) && (uc_Blue_Ch <= 6))	//见Variable.h的指令表
    {
        uc_Blue_LastCmd = (uint8_t)(uc_Blue_Ch - 1);    //见Variable.h的指令表
    }
    else if (uc_Blue_Ch == 7)
    {
        uc_Blue_LastCmd = 6;       						//见Variable.h的指令表               
    }
    else
    {
        //非法命令 返回 error，但不打断当前保持的动作 
        return 7;										//见Variable.h的指令表
    }

	return uc_Blue_LastCmd;
}
