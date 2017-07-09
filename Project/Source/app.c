#include "qmRTOS.h"

qTask qTask1;        //定义两个任务
qTask qTask2;
qTask qTask3;
qTask qTask4;

qTaskStack tasklEnv[1024];    //定义任务堆栈空间
qTaskStack task2Env[1024];
qTaskStack task3Env[1024];
qTaskStack task4Env[1024];

int task1Flag;  //定义任务标志位
int task2Flag;
int task3Flag;
int task4Flag;

/******************************************************************************
 * 函数名称：任务1函数
 * 函数功能：任务1
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void task1Entry(void * param)
{
	qSetSysTickPeriod (10);   //初始化系统定时器为10ms
	dprintf("this is task1\n");
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
	dprintf("this is task2\n");
	for(;;)
	{
		task2Flag = 0;
		qTaskDelay(1);
		task2Flag = 1;
		qTaskDelay(1);

	}
}

/******************************************************************************
 * 函数名称：任务3函数
 * 函数功能：任务3
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void task3Entry(void * param)
{
	dprintf("this is task3\n");
	for(;;)
	{
		task3Flag = 0;
		qTaskDelay(1);
		task3Flag = 1;
		qTaskDelay(1);
	}
}

/******************************************************************************
 * 函数名称：任务4函数
 * 函数功能：任务4
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void task4Entry(void * param)
{
	dprintf("this is task4\n");
	for(;;)
	{
		task4Flag = 0;
		qTaskDelay(1);
		task4Flag = 1;
		qTaskDelay(1);
	}
}

/******************************************************************************
 * 函数名称：任务初始化函数
 * 函数功能：初始化任务
 * 输入参数：无
 * 输出参数：无 
 ******************************************************************************/
void qInitApp(void)
{
	qTaskInit(&qTask1, task1Entry, (void *)0x11111111, 0, &tasklEnv[1024]);  //初始化任务
	qTaskInit(&qTask2, task2Entry, (void *)0x22222222, 1, &task2Env[1024]);
	qTaskInit(&qTask3, task3Entry, (void *)0x33333333, 0, &task3Env[1024]);
	qTaskInit(&qTask4, task4Entry, (void *)0x44444444, 1, &task4Env[1024]);

}


///******************************************************************************
// * 函数名称：简单延时函数
// * 函数功能：延时
// * 输入参数：int count  延时时间
// * 输出参数：无 
// ******************************************************************************/
//void delay(int count)
//{
//	while (--count > 0);
//}
