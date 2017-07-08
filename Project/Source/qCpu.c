#include "qmRTOS.h"
#include "ARMCM3.h"


/******************************************************************************
 * 函数名称：SysTick定时器初始化函数
 * 函数功能：初始化SysTick定时器
 * 输入参数：uint32_t ms  定时时间
 * 输出参数：无 
 ******************************************************************************/
void qSetSysTickPeriod(uint32_t ms)
{
	SysTick->LOAD = ms * SystemCoreClock / 1000 - 1;
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	SysTick->VAL = 0;
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
					SysTick_CTRL_TICKINT_Msk |
					SysTick_CTRL_ENABLE_Msk;
}

/******************************************************************************
 * 函数名称：SysTick中断服务函数
 * 函数功能：SysTick中断服务
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void SysTick_Handler()
{
	qTaskSystemTickHandler();
}
