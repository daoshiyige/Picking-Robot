#include "delay.h"

/**********************************************************
***	Emm_V5.0�����ջ���������
***	��д���ߣ�ZHANGDATOU
***	����֧�֣��Ŵ�ͷ�ջ��ŷ�
***	�Ա����̣�https://zhangdatou.taobao.com
***	CSDN���ͣ�http s://blog.csdn.net/zhangdatou666
***	qq����Ⱥ��262438510
**********************************************************/
/**
	*	@brief		���뼶��ʱ
	*	@param		int32_t u32Cnt
	*	@retval		��
	*/
void delay_ms(int32_t i32Cnt)
{
	__IO int32_t i32end = 0;

	SysTick->LOAD = 0xFFFFFF;
	SysTick->VAL  = 0;
	SysTick->CTRL = (SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_CLKSOURCE_Msk);

	while(i32Cnt > 0)
	{
		SysTick->VAL = 0;
		i32end = 0x1000000 - (SystemCoreClock / 1000);
		while(SysTick->VAL > i32end);
		--i32Cnt;
	}

	SysTick->CTRL = (SysTick->CTRL & (~SysTick_CTRL_ENABLE_Msk));
}

/**
	*	@brief		�����ʱ
	*	@param		int32_t u32Cnt
	*	@retval		��
	*/
void delay_cnt(int32_t i32Cnt)
{
	while(i32Cnt > 0) { i32Cnt--; }
}
