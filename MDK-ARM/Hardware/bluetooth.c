#include "bluetooth.h"


/* ======================= 接收与命令队列 ======================= */
#define BUFFER_SIZE 8

static uint8_t uc_ReceiveCom = 0;                        /* 中断接收缓冲（1 字节） */
static volatile uint8_t ucBuffer[BUFFER_SIZE]; /* 命令队列 */
static volatile uint8_t ucWrite = 0;             /* 写索引（中断侧） */
static volatile uint8_t ucRead  = 0;             /* 读索引（任务侧） */


static volatile FrameState_t eFrame = Wait_Head;

/* ======================= 初始化 ======================= */
void v_Bluetooth_Init(void)
{
    eFrame     = Wait_Head;
    ucWrite = 0;
    ucRead  = 0;

    HAL_UART_Receive_IT(&huart1, &uc_ReceiveCom, 1);
}

/* ======================= 命令入队（仅中断调用） ======================= */
/// @NOTE 把数据存入ucBuffer
static void v_CmdQueuePush(uint8_t ucCh)
{
    uint8_t ucNext = (uint8_t)((ucWrite + 1) % BUFFER_SIZE);

    if (ucNext != ucRead)          /* 队列未满才入队 */
    {
        ucBuffer[ucWrite] = ucCh;
        ucWrite = ucNext;
    }
    /* 队列满则丢弃当前命令，避免覆盖未处理命令 */
}

/* ======================= 命令出队（仅任务调用） ======================= */
/// @NOTE 队列空就返回数据
static uint8_t uc_CmdQueuePop(void)
{
    uint8_t ucCh;

    if (ucRead == ucWrite)      /* 队列空 */
    {
        return 0;
    }

    ucCh = ucBuffer[ucRead];
    ucRead = (uint8_t)((ucRead + 1) % BUFFER_SIZE);
    return ucCh;
}

/* ======================= 串口接收完成回调（中断上下文） ======================= */
/* 注意：此函数在中断中执行，只做帧解析与入队，严禁耗时操作 */
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
                v_CmdQueuePush((uint8_t)(ucCh - '0'));   /* '1'->1 ... '6'->6 */
                eFrame = Wait_Tail;
            }
            else if (ucCh == '[')
            {
                eFrame = Wait_Num;   /* 重新开始一帧 */
            }
            else
            {
                eFrame = Wait_Head;    /* 非法字符，放弃本帧 */
            }
            break;

        case Wait_Tail:
            if (ucCh == ']')
            {
                eFrame = Wait_Head;    /* 一帧完整结束 */
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

    /* 必须重新开启接收，否则后续数据无法触发中断 */
    HAL_UART_Receive_IT(&huart1, &uc_ReceiveCom, 1);
}

/* ======================= 蓝牙任务处理（任务上下文） ======================= */
void v_Bluetooth_Process(void)
{
    const char *Servo;
    uint8_t ucCh = uc_CmdQueuePop();

    if (ucCh == 0)
    {
        return;   /* 无待处理命令 */
    }

    if ((ucCh >= 1) && (ucCh <= 4))
    {
        Servo = CommendBlue[ucCh - 1];   /* up/down/left/right */
    }
    else if (ucCh == 5)
    {
        Servo = CommendBlue[4];           /* wait */
    }
    else
    {
        Servo = CommendBlue[5];           /* error */
    }

    HAL_UART_Transmit(&huart1, (uint8_t *)Servo, (uint16_t)strlen(Servo), 100);
    HAL_UART_Transmit(&huart1, (uint8_t *)CommendBlue[6], (uint16_t)strlen(CommendBlue[6]), 100);
}
