/*
本项目代码以及原理图均由  Emy  制作  最近一次修改的时间为 2026.8.22  version 1.0.0 (≧∇≦)

							感谢各位观看！！！！！！

项目简介:基于 stm32f103c8t6 使用 hal 库和 rtos 的四轴飞控，通过手机蓝牙发送指令控制飞控


硬件: (主控)stm32f103c8t6  (陀螺仪)mpu6050  (蓝牙)HC-08  (电机)716空心杯   	
      以及n多个电阻，电容升压，驱动模块 (详情见原理图)
	
	
Emy 的 Email   1472464885@qq.com
		       e94449431@gmail.com
*/
/***********************************************************************************************/
/*	无人机示意图  1~4均为motor
 *     1		 2
 *	    \		/
 *		 \	   /
 *		  \   /
 *		   \ /
 *			|
 *		   / \
 *	  	  /	  \
 *		 /	   \
 *		/		\
 *	   4	     3
 */
/***********************************************************************************************/
/*					  蓝牙指令  Bluetooth_Commend	 
 *
 *		Bluetooth_in	 	    Bluetooth_out		Phenomenon
 *
 *	 	[1]						up					Motor_middle
 *		[2]						down				Motor_down
 *	 	[3]						left				Motor_left
 *	 	[4]						right				Motor_right
 *	 	[5]						go					Motor_go
 *	 	[6]						back				Motor_back
 *      [B]             		Batter:XX      	    Batter_Power
 *
 */
/***********************************************************************************************/
//看这里！！！！！  look_here！！！！！  看这里！！！！！  look_here！！！！！  看这里！！！！！ 

/*需要自定义的参数全部在下面，我已经调好参数了，可以根据需求自行修改，其他文件的参数不建议修改*/ 
/*All the variable that need to custom are below , but I have adjusted it. You can custom it for youself need,
  I not recommend custom to variable for other files*/ 

//看这里！！！！！  look_here！！！！！  看这里！！！！！  look_here！！！！！  看这里！！！！！ 
/***********************************************************************************************/

/// @blue  
#define BLUE_SEND_TIME	    5	  // 间[1,10] 蓝牙发送数据速度1最快 10最慢

/// @butter
#define SEND_BUTTER_NUM		5     // 蓝牙发送几次电量信息

/// @motor
#define MOTOR_STEP      	200   // 区间[0,1000] 单次加/减速的 CCR 步长（≈0.2 占空比）
#define MOTOR_MIN_CCR       200   // 区间[0,1000] 电机最小 CCR（≈0.2）
#define MOTOR_MAX_CCR       950   // 区间[0,1000] 电机最大 CCR（≈0.95）
#define MOTOR_MIDDLE_CCR    500   // 区间[0,1000] 中值 CCR（≈0.5）
#define MOTOR_STOP     		0.0
#define MOTOR_STOP_CCR      0     // 停机 CCR 计数（0% 占空比）
#define FPITCH_ANGLE_MAX	60	  // 区间[0,90] 飞行时的最大倾斜角度 
#define FPITCH_ANGLE_MIN	-60	  // 区间[-90,0] 飞行时的最小倾斜角度
