#include "qmRTOS.h"
#include "ARMCM3.h"

qTask * currentTask;   //指示当前任务的指针
qTask * nextTask;      //指向下一个任务的指针
qTask * taskTable[2];  //任务数组大小

/******************************************************************************
 * 函数名称：任务初始化函数
 * 函数功能：任务初始化
 * 输入参数：qTask * task             任务结构指针
			void (*entry) (void *)   任务入口函数地址
			void *parm               传递给任务的参数地址
			qTaskStack * stack       任务堆栈地址
 * 输出参数：无 
 ******************************************************************************/
void qTaskInit (qTask * task , void (*entry) (void *), void *param , qTaskStack * stack )
{
	//堆栈内容初始化，传递末端的地址，内核按满递减方式增长。
	*(--stack) = (unsigned long)(1 << 24);              //XPSR, 设置了Thumb模式，恢复到Thumb状态而非ARM状态运行
	*(--stack) = (unsigned long)entry;                  // PC, 程序的入口地址
    *(--stack) = (unsigned long)0x14;                   // R14(LR), 任务不会通过return xxx结束自己，所以未用
    *(--stack) = (unsigned long)0x12;                   // R12, 未用
    *(--stack) = (unsigned long)0x3;                    // R3, 未用
    *(--stack) = (unsigned long)0x2;                    // R2, 未用
    *(--stack) = (unsigned long)0x1;                    // R1, 未用
    *(--stack) = (unsigned long)param;                  // R0 = param, 传给任务的入口函数
    *(--stack) = (unsigned long)0x11;                   // R11, 未用
    *(--stack) = (unsigned long)0x10;                   // R10, 未用
    *(--stack) = (unsigned long)0x9;                    // R9, 未用
    *(--stack) = (unsigned long)0x8;                    // R8, 未用
    *(--stack) = (unsigned long)0x7;                    // R7, 未用
    *(--stack) = (unsigned long)0x6;                    // R6, 未用
    *(--stack) = (unsigned long)0x5;                    // R5, 未用
    *(--stack) = (unsigned long)0x4;                    // R4, 未用
	
	task->stack = stack;                                // 保存最终的值
}

/******************************************************************************
 * 函数名称：任务调度函数
 * 函数功能：决定cpu在那些任务之间运行，如何分配
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void qTaskSched()
{
	if (currentTask == taskTable[0])      //判断当前任务是那一个，下一个任务跳转到另一个
	{
		nextTask = taskTable[1];
	}
	else 
	{
		nextTask = taskTable[0];
	}
	qTaskSwitch();                        //调用任务切换函数
}
	
/******************************************************************************
 * 函数名称：SysTick定时器初始化函数
 * 函数功能：初始化SysTick定时器
 * 输入参数：uint32_t ms  定时时间
 * 输出参数：无 
 ******************************************************************************/
void tSetSysTickPeriod (uint32_t ms)
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
void SysTick_Handler ()
{
	qTaskSched();      //调用调度函数，切换任务
}

/******************************************************************************
 * 函数名称：简单延时函数
 * 函数功能：延时
 * 输入参数：int count  延时时间
 * 输出参数：无 
 ******************************************************************************/
void delay(int count)
{
	while (--count > 0);
}

qTask qTask1;        //定义两个任务
qTask qTask2;

qTaskStack tasklEnv[1024];//定义任务堆栈空间
qTaskStack task2Env[1024];

int task1Flag;  //定义任务标志位
int task2Flag;

/******************************************************************************
 * 函数名称：任务1函数
 * 函数功能：任务1
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void task1Entry (void * param)
{
	tSetSysTickPeriod (10);   //初始化系统定时器为10ms
	for(;;)
	{
		task1Flag = 0;
		delay(100);
		task1Flag = 1;
		delay(100);
	}
}

/******************************************************************************
 * 函数名称：任务2函数
 * 函数功能：任务2
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void task2Entry (void * param)
{
	for(;;)
	{
		task2Flag = 0;
		delay(100);
		task2Flag = 1;
		delay(100);
	}
}

/******************************************************************************
 * 函数名称：主函数
 * 函数功能：
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
int main()
{
	qTaskInit(&qTask1, task1Entry, (void *)0x11111111, &tasklEnv[1024]);  //初始化任务
	qTaskInit(&qTask2, task2Entry, (void *)0x22222222, &task2Env[1024]);
	
	taskTable[0] = &qTask1;    //初始化任务数组
	taskTable[1] = &qTask2;
	
	nextTask = taskTable[0];   //初始运行任务
	
	qTaskRunFirst();           //运行OS，开始调度第一个任务
	
	return 0;
}	
