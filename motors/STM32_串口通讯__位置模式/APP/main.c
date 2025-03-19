#include "board.h"
#include "delay.h"
#include "usart.h"
#include "Emm_V5.h"
#include "MyCAN.h"

/**********************************************************
***	Emm_V5.0步进闭环控制例程
***	编写作者：ZHANGDATOU
***	技术支持：张大头闭环伺服
***	淘宝店铺：https://zhangdatou.taobao.com
***	CSDN博客：http s://blog.csdn.net/zhangdatou666
***	qq交流群：262438510
**********************************************************/

/**
	*	@brief		MAIN函数
	*	@param		无
	*	@retval		无
	*/
	
CanTxMsg TxMsgArray[] = {
/*   StdId     ExtId         IDE             RTR        DLC         Data[8]          */
	{0x555, 0x12001A3C, CAN_Id_Extended, CAN_RTR_Data,     8, {0x70, 0x05, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00}},
	{0x000, 0x12001A3C, CAN_Id_Extended, CAN_RTR_Data,     8, {0x70, 0x17, 0x00, 0x00, 0x00, 0x00, 0x20, 0x41}},
	{0x666, 0x12001A3C, CAN_Id_Extended, CAN_RTR_Data, 	   8, {0x70, 0x16, 0x00, 0x00, 0x00, 0x00, 0x36, 0x42}},
	{0x000, 0x03001A3C, CAN_Id_Extended, CAN_RTR_Data,     8, {0x00, 0x00, 0x00, 0x00}},
};
void TurnToAngle(float angle);
uint8_t pTxMsgArray = 0;
float now_angle = 0;
uint16_t speed = 100;

int main(void)
{
/**********************************************************
***	初始化板载外设
**********************************************************/
	board_init();
	MyCAN_Init();
/**********************************************************
***	上电延时2秒等待Emm_V5.0闭环初始化完毕
**********************************************************/	
	delay_ms(2000);

/**********************************************************
***	位置模式：方向CW，速度1000RPM，加速度0（不使用加减速直接启动），脉冲数3200（16细分下发送3200个脉冲电机转一圈），相对运动
**********************************************************/	

/**********************************************************
***	等待返回命令，命令数据缓存在数组rxCmd上，长度为rxCount
**********************************************************/	
//while(rxFrameFlag == false); rxFrameFlag = false;

/**********************************************************
***	WHILE循环
**********************************************************/	
	while(1)
	{
		if (MyCAN_RxFlag == 1)
		{
			MyCAN_RxFlag = 0;
			if ((MyCAN_RxMsg.IDE == CAN_Id_Extended)&&(((uint8_t)(MyCAN_RxMsg.ExtId>>24)==0x12))&&((uint16_t)MyCAN_RxMsg.ExtId)==0x1A3C)
			{

				if((MyCAN_RxMsg.Data[0] == 0x70) && (MyCAN_RxMsg.Data[1] == 0x05))
				{
				}
				else	if((MyCAN_RxMsg.Data[0] == 0x70) && (MyCAN_RxMsg.Data[1] == 0x17)) //设置电机角速度
				{
					float * spd =(float*)&(MyCAN_RxMsg.Data[4]);
					speed = (*spd)*9.551;
					Emm_V5_Pos_Control(1, 0, speed, 0, 0, 0, 0);
				}
				else	if((MyCAN_RxMsg.Data[0] == 0x70) && (MyCAN_RxMsg.Data[1] == 0x16)) //设置电机移动到某个角度
				{
					float * target_angle = (float*)&(MyCAN_RxMsg.Data[4]);
					TurnToAngle(*target_angle);
				}
				else if((MyCAN_RxMsg.Data[0] == 0x70) && (MyCAN_RxMsg.Data[1] == 0x18)) //设置电机左转
				{
					float * target_angle = (float*)&(MyCAN_RxMsg.Data[4]);
					uint32_t clk;
					clk =((* target_angle*3200) / 360.0);
					now_angle -= *target_angle;
					if(now_angle < 0) now_angle += 360.f;
					Emm_V5_Pos_Control(1, 0, speed, 0, clk, 0, 0);
				}
				else if((MyCAN_RxMsg.Data[0] == 0x70) && (MyCAN_RxMsg.Data[1] == 0x19))	//设置电机右转
				{
					float * target_angle = (float*)&(MyCAN_RxMsg.Data[4]);
					uint32_t clk;
					clk =((* target_angle*3200) / 360.0);
					now_angle += *target_angle;
					if(now_angle > 360.f) now_angle -= 360.f;
					Emm_V5_Pos_Control(1, 1, speed, 0, clk, 0, 0);
				}
				else if((MyCAN_RxMsg.Data[0] == 0x70) && (MyCAN_RxMsg.Data[1] == 0x20))  //设置电机复位 
				{
					TurnToAngle(0.f);
				}
			}
			else if ((MyCAN_RxMsg.IDE == CAN_Id_Extended)&&(((uint8_t)(MyCAN_RxMsg.ExtId>>24)==0x03))&&((uint16_t)MyCAN_RxMsg.ExtId)==0x1A3C)
			{
				
			}
			
		}
		
		
//		Emm_V5_Pos_Control(1, 0, 100, 0, 800, 0, 0);
//		delay_ms(1000);
	}
}

void TurnToAngle(float angle)
{
	uint32_t clk;
	if(speed != 0)
	{
		if((now_angle - angle) > 0)
		{
			float angle_dif = now_angle - angle;
			clk = (uint32_t)((angle_dif*3200) / 360.0);
			Emm_V5_Pos_Control(1, 0, speed, 0, clk, 0, 0);
		}
		else if((now_angle - angle) < 0)
		{
			float angle_dif = angle - now_angle;
			clk = (uint32_t)((angle_dif*3200) / 360.0);
			Emm_V5_Pos_Control(1, 1, speed, 0, clk, 0, 0);
		}
		now_angle = angle;
	}
}
