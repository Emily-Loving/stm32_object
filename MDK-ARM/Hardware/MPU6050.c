#include "MPU6050.h"


#define MPU6050_RAW_SIZE    14   // ACCEL_XOUT_H 起连续 14 字节（含 2 字节温度）
#define MPU6050_TIMEOUT   100    // I2C 操作超时(ms)，防器件掉线卡死 

static uint8_t MPURawBuffer[MPU6050_RAW_SIZE];   // 原始数据接收缓冲 

/// @brief  写 MPU6050 单字节寄存器
///
/// @param  ucReg  : 寄存器地址
/// @param  ucData : 要写入的数据
void vMPU6050_WriteReg(uint8_t ucReg, uint8_t ucData)
{
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ucReg, I2C_MEMADD_SIZE_8BIT, &ucData, 1, HAL_MAX_DELAY);
}

/// @brief  读 MPU6050 单字节寄存器
/// @return HAL_OK 表示读到应答
static HAL_StatusTypeDef eMPU6050_ReadReg(uint8_t ucReg, uint8_t *pucData)
{
    return HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, ucReg, I2C_MEMADD_SIZE_8BIT,
                            pucData, 1, MPU6050_TIMEOUT);
}

/// @brief  MPU6050 在线自检：读 WHO_AM_I，应为 0x68
/// @return 1=在线且身份正确 0=无应答或身份错误
uint8_t ucMPU6050_Probe(void)
{
    uint8_t ucId = 0;
    if (eMPU6050_ReadReg(MPU6050_WHO_AM_I, &ucId) != HAL_OK)
    {
        return 0;   // I2C 无应答：接线/地址/上拉/供电问题 
    }
	if(ucId == 0x68)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/// @brief  扫描 I2C 总线上所有 7 位地址，找出有应答的设备。
///         把返回值(8位地址)存 volatile 变量打断点看，或改用串口打印。
///         - 返回 0xD0：MPU 在线，地址正确(AD0=0)
///         - 返回 0xD2：MPU 在线，但 AD0 接高 -> 需把 MPU6050_ADDR 改成 0xD2
///         - 返回 0x00：总线上无任何设备应答 -> 上拉/供电/接线问题
/// @return 第一个应答设备的 8 位地址(左移后)，无设备返回 0
uint8_t ucI2C_Scan(void)
{
    for (uint8_t addr7 = 1; addr7 < 128; addr7++)
    {
        // HAL 用 8 位地址(左移1位)，试 3 次，每次超时 10ms 
        if (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(addr7 << 1), 3, 10) == HAL_OK)
        {
            return (uint8_t)(addr7 << 1);   
		}
    }
    return 0;   // 全总线无应答 
}

/// @brief  初始化 MPU6050：唤醒 + 设量程 最简开启方式
void vMPU6050_Init(void)
{
    // 先探测器件是否在线，便于定位硬件/接线问题 结果可打断点或串口观察
    volatile uint8_t ucOnline = ucMPU6050_Probe();
    (void)ucOnline;

    // 总线扫描：若 ucOnline==0，看这个值能区分  无设备/地址不对 
    volatile uint8_t ucFoundAddr = ucI2C_Scan();
    (void)ucFoundAddr;

    vMPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x00);
    vMPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x07);
    vMPU6050_WriteReg(MPU6050_CONFIG, 0x00);
    vMPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
    vMPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x00);
}

/// @brief  读取 MPU6050 原始数据（阻塞方式，返回时数据已就绪）
/// @param  Data : 输出，解析后的原始数据（角度字段暂未填充）
/// @return 1=成功 0=失败
uint8_t vMPU6050_Read(MPU6050_DataTDF *Data)   
{

	if (HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, MPU6050_ACCEL_XOUT_H,
	                     I2C_MEMADD_SIZE_8BIT, MPURawBuffer,
	                     MPU6050_RAW_SIZE, MPU6050_TIMEOUT) != HAL_OK)
	{
	    return 0;   // I2C 无应答/超时：不解析，避免脏数据(全0)进入滤波器算出 NaN 
	}

    // 大端 -> 小端拼装，跳过温度字节 [6][7]
    Data->sAccelX = (int16_t)((MPURawBuffer[0]  << 8) | MPURawBuffer[1]);
    Data->sAccelY = (int16_t)((MPURawBuffer[2]  << 8) | MPURawBuffer[3]);
    Data->sAccelZ = (int16_t)((MPURawBuffer[4]  << 8) | MPURawBuffer[5]);
    Data->sGyroX  = (int16_t)((MPURawBuffer[8]  << 8) | MPURawBuffer[9]);
    Data->sGyroY  = (int16_t)((MPURawBuffer[10] << 8) | MPURawBuffer[11]);
    Data->sGyroZ  = (int16_t)((MPURawBuffer[12] << 8) | MPURawBuffer[13]);

    return 1;
}
