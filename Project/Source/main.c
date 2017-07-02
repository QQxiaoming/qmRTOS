#include "qmRTOS.h"
#include "ARMCM3.h"

qTask * currentTask;   //指示当前任务的指针
qTask * nextTask;      //指向下一个任务的指针
qTask * idleTask;      //指向空闲任务的指针
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
void qTaskInit(qTask * task , void (*entry) (void *), void *param , qTaskStack * stack )
{
	//堆栈内容初始化，传递末端的地址，内核按满递减方式增长。
	*(--stack) = (unsigned long)(1 << 24);              // XPSR, 设置了Thumb模式，恢复到Thumb状态而非ARM状态运行
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
	task->delayTicks = 0;								// 初始任务延时个数为0
}

/******************************************************************************
 * 函数名称：任务调度函数
 * 函数功能：决定cpu在那些任务之间运行，如何分配
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void qTaskSched()
{
	uint32_t status = qTaskEnterCritical();          //对任务调度函数进行保护
	
	// 空闲任务只有在所有其它任务都不是延时状态时才执行
    // 所以，我们先检查下当前任务是否是空闲任务
    if (currentTask == idleTask) 
    {
        // 如果是的话，那么去执行task1或者task2中的任意一个
        // 当然，如果某个任务还在延时状态，那么就不应该切换到他。
        // 如果所有任务都在延时，那么就继续运行空闲任务，不进行任何切换了
        if (taskTable[0]->delayTicks == 0) 
        {
            nextTask = taskTable[0];
        }           
        else if (taskTable[1]->delayTicks == 0) 
        {
            nextTask = taskTable[1];
        } else 
        {
            return;
        }
    } 
    else 
    {
        // 如果是task1或者task2的话，检查下另外一个任务
        // 如果另外的任务不在延时中，就切换到该任务
        // 否则，判断下当前任务是否应该进入延时状态，如果是的话，就切换到空闲任务。否则就不进行任何切换
        if (currentTask == taskTable[0]) 
        {
            if (taskTable[1]->delayTicks == 0) 
            {
                nextTask = taskTable[1];
            }
            else if (currentTask->delayTicks != 0) 
            {
                nextTask = idleTask;
            } 
            else 
            {
                return;
            }
        }
        else if (currentTask == taskTable[1]) 
        {
            if (taskTable[0]->delayTicks == 0) 
            {
                nextTask = taskTable[0];
            }
            else if (currentTask->delayTicks != 0) 
            {
                nextTask = idleTask;
            }
            else 
            {
                return;
            }
        }
    }
    
	qTaskSwitch();                        //调用任务切换函数
	
	qTaskExitCritical(status);
}
	
/******************************************************************************
 * 函数名称：任务SysTick定时器中断函数
 * 函数功能：发生中断时被定时器中断函数调用，使delayTicks--，并调用一次调度函数
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void qTaskSystemTickHandler()
{
	int i;
	
	uint32_t status = qTaskEnterCritical();          //对任务调度函数进行保护
	
	for(i = 0; i < 2; i ++)          //扫描任务的delayTicks，使其递减1
	{
		if(taskTable[i]->delayTicks > 0)
		{
			taskTable[i]->delayTicks --;
		}
	}
	
	qTaskExitCritical(status);
	
	qTaskSched();                    //调用任务调度函数
}

/******************************************************************************
 * 函数名称：任务延时函数
 * 函数功能：实现任务中延时
 * 输入参数：uint32_t delay  延时中断的个数
 * 输出参数：无 
 ******************************************************************************/
void qTaskDelay(uint32_t delay)
{
	uint32_t status = qTaskEnterCritical();          //对任务调度函数进行保护
	
	currentTask->delayTicks = delay;
	qTaskSched();                    //调用任务调度函数
	
	qTaskExitCritical(status);
}

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


int task1Flag;  //定义任务标志位
int task2Flag;

/******************************************************************************
 * 函数名称：任务1函数
 * 函数功能：任务1
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void task1Entry(void * param)
{
	qSetSysTickPeriod (10);   //初始化系统定时器为10ms
	for(;;)
	{
		task1Flag = 0;
		qTaskDelay(1);
		task1Flag = 1;
		qTaskDelay(1);
	}
}

/******************************************************************************
 * 函数名称：任务2函数
 * 函数功能：任务2
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void task2Entry(void * param)
{
	for(;;)
	{
		task2Flag = 0;
		qTaskDelay(1);
		task2Flag = 1;
		qTaskDelay(1);
	}
}

qTask qTaskIdle;     //定义空闲任务
qTaskStack idleTaskEnv[1024]; //定义空任务堆栈空间  

/******************************************************************************
 * 函数名称：空闲任务函数
 * 函数功能：空闲任务
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void idleTaskEntry(void * param)
{
	for(;;)
	{
	}
}

qTask qTask1;        //定义两个任务
qTask qTask2;

qTaskStack tasklEnv[1024];    //定义任务堆栈空间
qTaskStack task2Env[1024];

/******************************************************************************
 * 函数名称：主函数
 * 函数功能：主函数
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
int main()
{
	qTaskInit(&qTask1, task1Entry, (void *)0x11111111, &tasklEnv[1024]);  //初始化任务
	qTaskInit(&qTask2, task2Entry, (void *)0x22222222, &task2Env[1024]);
	
	taskTable[0] = &qTask1;    //初始化任务数组
	taskTable[1] = &qTask2;
	
	qTaskInit(&qTaskIdle, idleTaskEntry, (void *)0, &idleTaskEnv[1024]);  //初始化空闲任务
	idleTask = &qTaskIdle;
	
	nextTask = taskTable[0];   //初始运行任务
	
	qTaskRunFirst();           //运行OS，开始调度第一个任务
	
	return 0;
}	
