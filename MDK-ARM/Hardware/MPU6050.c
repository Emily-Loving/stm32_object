#include "MPU6050.h"


#define MPU6050_RAW_SIZE    14   // ACCEL_XOUT_H 起连续 14 字节（含 2 字节温度）
#define MPU6050_TIMEOUT   100    // I2C 操作超时(ms)，防器件掉线卡死 

static uint8_t uc_Mpu_RawBuffer[MPU6050_RAW_SIZE];   // 原始数据接收缓冲 

/// @NOTE 写 MPU6050 单字节寄存器
/// @param uc_Mpu_Reg  寄存器地址
/// @param uc_Mpu_Data 要写入的数据
/// @return
void vMPU6050_WriteReg(uint8_t uc_Mpu_Reg, uint8_t uc_Mpu_Data)
{
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, uc_Mpu_Reg, I2C_MEMADD_SIZE_8BIT, &uc_Mpu_Data, 1, HAL_MAX_DELAY);
}

/// @NOTE 读 MPU6050 单字节寄存器
/// @param uc_Mpu_Reg  寄存器地址
/// @param uc_Mpu_pData 输出，读到的字节
/// @return HAL_OK 表示读到应答
static HAL_StatusTypeDef eMPU6050_ReadReg(uint8_t uc_Mpu_Reg, uint8_t *uc_Mpu_pData)
{
    return HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, uc_Mpu_Reg, I2C_MEMADD_SIZE_8BIT,
                            uc_Mpu_pData, 1, MPU6050_TIMEOUT);
}

/// @NOTE MPU6050 在线自检：读 WHO_AM_I，应为 0x68
/// @param void
/// @return 1=在线且身份正确 0=无应答或身份错误
uint8_t ucMPU6050_Probe(void)
{
    uint8_t uc_Mpu_Id = 0;
    if (eMPU6050_ReadReg(MPU6050_WHO_AM_I, &uc_Mpu_Id) != HAL_OK)
    {
        return 0;   // I2C 无应答：接线/地址/上拉/供电问题 
    }
	if(uc_Mpu_Id == 0x68)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/// @NOTE 扫描 I2C 总线上所有 7 位地址找出有应答的设备
/// @param void
/// @return 0xD0=在线地址正确(AD0=0)，0xD2=在线但 AD0 接高(需把 MPU6050_ADDR 改成 0xD2)，0x00=总线无任何设备应答
uint8_t ucI2C_Scan(void)
{
    for (uint8_t uc_Mpu_Addr7 = 1; uc_Mpu_Addr7 < 128; uc_Mpu_Addr7++)
    {
        // HAL 用 8 位地址(左移1位)，试 3 次，每次超时 10ms 
        if (HAL_I2C_IsDeviceReady(&hi2c1, (uint16_t)(uc_Mpu_Addr7 << 1), 3, 10) == HAL_OK)
        {
            return (uint8_t)(uc_Mpu_Addr7 << 1);   
		}
    }
    return 0;   // 全总线无应答 
}

/// @NOTE 初始化 MPU6050：唤醒 + 设量程 最简开启方式
/// @param void
/// @return
void vMPU6050_Init(void)
{
    // 先探测器件是否在线，便于定位硬件/接线问题 结果可打断点或串口观察
    volatile uint8_t uc_Mpu_Online = ucMPU6050_Probe();
    (void)uc_Mpu_Online;

    // 总线扫描：若 uc_Mpu_Online==0，看这个值能区分  无设备/地址不对 
    volatile uint8_t uc_Mpu_FoundAddr = ucI2C_Scan();
    (void)uc_Mpu_FoundAddr;

    vMPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x00);
    vMPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x07);
    vMPU6050_WriteReg(MPU6050_CONFIG, 0x00);
    vMPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
    vMPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x00);
}

/// @NOTE 读取 MPU6050 原始数据（阻塞方式，返回时数据已就绪）
/// @param p_Mpu_Data 输出，解析后的原始数据（角度字段暂未填充）
/// @return 1=成功 0=失败
uint8_t vMPU6050_Read(MPU6050_DataTDF *p_Mpu_Data)   
{

	if (HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, MPU6050_ACCEL_XOUT_H,
	                     I2C_MEMADD_SIZE_8BIT, uc_Mpu_RawBuffer,
	                     MPU6050_RAW_SIZE, MPU6050_TIMEOUT) != HAL_OK)
	{
	    return 0;   // I2C 无应答/超时：不解析，避免脏数据(全0)进入滤波器算出 NaN 
	}

    // 大端 -> 小端拼装，跳过温度字节 [6][7]
    p_Mpu_Data->s_Mpu_AccelX = (int16_t)((uc_Mpu_RawBuffer[0]  << 8) | uc_Mpu_RawBuffer[1]);
    p_Mpu_Data->s_Mpu_AccelY = (int16_t)((uc_Mpu_RawBuffer[2]  << 8) | uc_Mpu_RawBuffer[3]);
    p_Mpu_Data->s_Mpu_AccelZ = (int16_t)((uc_Mpu_RawBuffer[4]  << 8) | uc_Mpu_RawBuffer[5]);
    p_Mpu_Data->s_Mpu_GyroX  = (int16_t)((uc_Mpu_RawBuffer[8]  << 8) | uc_Mpu_RawBuffer[9]);
    p_Mpu_Data->s_Mpu_GyroY  = (int16_t)((uc_Mpu_RawBuffer[10] << 8) | uc_Mpu_RawBuffer[11]);
    p_Mpu_Data->s_Mpu_GyroZ  = (int16_t)((uc_Mpu_RawBuffer[12] << 8) | uc_Mpu_RawBuffer[13]);

    return 1;
}
