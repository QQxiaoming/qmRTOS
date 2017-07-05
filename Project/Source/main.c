#include "qmRTOS.h"
#include "ARMCM3.h"

qTask * currentTask;   //指示当前任务的指针
qTask * nextTask;      //指向下一个任务的指针
qTask * idleTask;      //指向空闲任务的指针

qBitmap taskPrioBitmap;// 任务优先级的标记位置结构

qList taskTable[QMRTOS_PRO_COUNT];  //任务列表

uint8_t schedLockCount;//调度锁计数器 

qList qTaskDelayedList; //延时队列

/******************************************************************************
 * 函数名称：任务初始化函数
 * 函数功能：任务初始化
 * 输入参数：qTask * task             任务结构指针
			void (*entry) (void *)   任务入口函数地址
			void *parm               传递给任务的参数地址
			uint32_t prio            任务优先级
			qTaskStack * stack       任务堆栈地址
 * 输出参数：无 
 ******************************************************************************/
void qTaskInit(qTask * task , void (*entry) (void *), void *param ,uint32_t prio, qTaskStack * stack )
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
	
	task->slice = QMRYOS_SLTIC_MAX;                     // 时间片初始化为最大值
	task->stack = stack;                                // 保存最终的值
	task->delayTicks = 0;								// 初始任务延时个数为0
	task->prio = prio;                                  // 设置任务的优先级
	task->state = QMRTOS_TASK_STATE_RDY;                // 设置任务为就绪状态
	
	qNodeInit(&(task->delayNode));                      // 初始化延时结点
	qNodeInit(&(task->linkNode));                       // 初始化链接结点
	qListAddFirst(&taskTable[prio], &task->linkNode);   // 将链接结点加入到优先级链表
	
    qBitmapSet(&taskPrioBitmap, prio);                  // 置位优先级位置中的相应位
}

/******************************************************************************
 * 函数名称：查询当前就绪的优先级最高任务函数
 * 函数功能：获取当前最高优先级且可运行的任务
 * 输入参数：无
 * 输出参数：优先级最高的且可运行的任务指针
 ******************************************************************************/
qTask * qTaskHighestReady (void) 
{
    uint32_t highestPrio = qBitmapGetFirstSet(&taskPrioBitmap);     //找到优先级最高的位
	qNode * node =qListFirst(&taskTable[highestPrio]);              //找到该优先级下第一个任务结点
    return qNodeParent(node, qTask, linkNode);                      //找到该任务结点下任务的指针并返回  
}

/******************************************************************************
 * 函数名称：内核初始化函数
 * 函数功能：初始化OS
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void qTaskSchedInit(void)
{
	int i;
	
	schedLockCount = 0;                                //初始化调度锁计数器为0
	qBitmapInit(&taskPrioBitmap);                      //初始化位图
	for(i = 0; i < QMRTOS_PRO_COUNT; i++)
	{
		qListInit(&taskTable[i]);                      //扫描任务就绪表并初始化所有链表
	}
}

/******************************************************************************
 * 函数名称：调度锁上锁函数
 * 函数功能：禁止任务调度
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void qTaskSchedDisable(void)
{
	uint32_t status = qTaskEnterCritical();             //临界区保护
	
	if(schedLockCount < 255)                            //调度锁计数器加1，即锁定任务调度
	{
		schedLockCount++;
	}
	
	qTaskExitCritical(status);
}

/******************************************************************************
 * 函数名称：调度锁解锁函数
 * 函数功能：允许任务调度
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void qTaskSchedEnable(void) 
{
    uint32_t status = qTaskEnterCritical();      //临界区保护

    if (schedLockCount > 0) 
    {
        if (--schedLockCount == 0) 
        {
            qTaskSched(); 
        }
    }

    qTaskExitCritical(status);
}

/******************************************************************************
 * 函数名称：就绪任务函数
 * 函数功能：添加任务到任务就绪表
 * 输入参数：qTask * task    任务指针
 * 输出参数：无
 ******************************************************************************/
void qTaskSchedRdy(qTask * task)
{
	qListAddFirst(&(taskTable[task->prio]), &(task->linkNode));  //向链表中插入任务结点
	qBitmapSet(&taskPrioBitmap, task->prio);   //置位位图中优先级位的相应位
}

/******************************************************************************
 * 函数名称：非就绪任务函数
 * 函数功能：删除任务就绪表中任务
 * 输入参数：qTask * task    任务指针
 * 输出参数：无 
 ******************************************************************************/
void qTaskSchedUnRdy(qTask * task)
{
	qListRemove(&taskTable[task->prio], &(task->linkNode));  //从链表中删除对应任务
	if(qListCount(&taskTable[task->prio]) == 0)               //判断链表里是否还有任务
	{
		qBitmapClear(&taskPrioBitmap, task->prio);   //如果没有任务则清除位图中优先级位的相应位
	}
}

/******************************************************************************
 * 函数名称：任务调度函数
 * 函数功能：决定cpu在那些任务之间运行，如何分配
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void qTaskSched(void)
{
	qTask * tempTask;
	
	uint32_t status = qTaskEnterCritical();          //对任务调度函数进行保护
		
    if (schedLockCount > 0)                          // 若调度器已经被上锁，则不进行调度，直接退出
    {
        qTaskExitCritical(status);
        return;
    }
    // 找到优先级最高的任务，如果其优先级比当前任务的还高，那么就切换到这个任务
    tempTask = qTaskHighestReady();
    if (tempTask != currentTask) 
    {
        nextTask = tempTask;
        qTaskSwitch();   
    }

    qTaskExitCritical(status);                       // 退出临界区
}
	
/******************************************************************************
 * 函数名称：延时队列初始化函数
 * 函数功能：调用链表初始化函数对延时队列初始化
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void qTaskDelayedInit(void)
{
	qListInit(&qTaskDelayedList);                     //初始化延时队列
}

/******************************************************************************
 * 函数名称：等待任务函数
 * 函数功能：将需要延时的任务的结点插入到延时队列
 * 输入参数：qTask * task     任务指针
			uint32_t ticks   需要延时的ticks数
 * 输出参数：无 
 ******************************************************************************/
void qTimeTaskWait(qTask * task, uint32_t ticks)
{
	task->delayTicks = ticks;                             //延时时间 
	qListAddLast(&qTaskDelayedList, &(task->delayNode));   //将对应任务插入到延时队列队尾
	task->state |= QMRTOS_TASK_STATE_DELAYED;             //将任务状态标志位置为延时状态 
}

/******************************************************************************
 * 函数名称：唤醒任务函数
 * 函数功能：将延时结束的任务的结点从延时队列移除
 * 输入参数：qTask * task     任务指针
 * 输出参数：无  
 ******************************************************************************/
void qTimeTaskWakeUp(qTask * task)
{
	qListRemove(&qTaskDelayedList, &(task->delayNode));   //将任务结点移除延时队列
	task->state &= ~QMRTOS_TASK_STATE_DELAYED;            //将任务状态标志位延时状态取消
}


/******************************************************************************
 * 函数名称：任务SysTick定时器中断函数
 * 函数功能：发生中断时被定时器中断函数调用，使delayTicks--，并调用一次调度函数
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void qTaskSystemTickHandler(void)
{
	qNode * node;
	
	uint32_t status = qTaskEnterCritical();          //对任务调度函数进行保护
	
	for(node = qTaskDelayedList.headNode.nextNode; node != &(qTaskDelayedList.headNode); node = node->nextNode)      //扫描任务延时队列
	{
		qTask * task = qNodeParent(node, qTask, delayNode);   //获取任务的地址
		if(--task->delayTicks == 0)
		{
			qTimeTaskWakeUp(task);                   //从延时队列唤醒任务
			qTaskSchedRdy(task);                     //就绪任务
		}
	}
	
	if(--currentTask->slice == 0)                    //判断时间片是否使用完毕
	{
		if(qListCount(&taskTable[currentTask->prio]) > 0) //如果使用完毕，判断当前优先级是否还存在任务
		{
			qListRemoveFirst(&taskTable[currentTask->prio]);  //如果存在任务，则移除当前任务结点
			qListAddLast(&taskTable[currentTask->prio], &(currentTask->linkNode));  //添加当前任务结点到最后一个
			
			currentTask->slice = QMRYOS_SLTIC_MAX;        //给当前任务重新配备时间片
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
	
	qTimeTaskWait(currentTask, delay);               //将任务进入等待
	qTaskSchedUnRdy(currentTask);                    //关闭任务就绪状态
	
	qTaskExitCritical(status);
	
	qTaskSched();                    //调用任务调度函数
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
int task3Flag;

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
		delay(0xff);
		task2Flag = 1;
		delay(0xff);
	}
}

/******************************************************************************
 * 函数名称：任务2函数
 * 函数功能：任务2
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void task3Entry(void * param)
{
	for(;;)
	{
		task3Flag = 0;
		delay(0xff);
		task3Flag = 1;
		delay(0xff);
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
qTask qTask3;

qTaskStack tasklEnv[1024];    //定义任务堆栈空间
qTaskStack task2Env[1024];
qTaskStack task3Env[1024];

/******************************************************************************
 * 函数名称：主函数
 * 函数功能：主函数
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
int main()
{
	qTaskSchedInit();          //初始化系统内核
	dprintf("TaskInit is Ok!");
	
	qTaskDelayedInit();        //延时列表初始化
	
	qTaskInit(&qTask1, task1Entry, (void *)0x11111111, 0, &tasklEnv[1024]);  //初始化任务
	qTaskInit(&qTask2, task2Entry, (void *)0x22222222, 1, &task2Env[1024]);
	qTaskInit(&qTask3, task3Entry, (void *)0x33333333, 1, &task3Env[1024]);
	
	qTaskInit(&qTaskIdle, idleTaskEntry, (void *)0, QMRTOS_PRO_COUNT - 1, &idleTaskEnv[1024]);  //初始化空闲任务
	idleTask = &qTaskIdle;

    nextTask = qTaskHighestReady();   //初始自动查找最高优先级的任务运行
	
	qTaskRunFirst();           //运行OS，开始调度第一个任务
	
	return 0;
}	
