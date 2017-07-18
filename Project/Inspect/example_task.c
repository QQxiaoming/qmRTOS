/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： example_task.c
* 文件标识： 巡检测试任务
* 摘 要： 定义任务模块测试任务入口
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月18日
******************************************************************************
*/
#include "qmRTOS.h"
#include "api_inspect_entry.h"

qTask taskHi;                 //定义两个任务
qTask taskLo;

qTaskStack taskHiEnv[1024];    //定义任务堆栈空间
qTaskStack taskLoEnv[1024]; 

void taskHiEntry(void * param)
{
	dprintf("Enter TaskHi Handler.\r\n");
	qTaskDelay(2);                                   //任务延时
	dprintf("TaskHi LOS_TaskDelay Done.\r\n");
	qTaskSuspend(currentTask);                       //挂起高优先级任务
	dprintf("TaskHi LOS_TaskResume Success.\r\n");
	qInspectStatusSetByID(INSPECT_TASK, INSPECT_STU_SUCCESS);     //设置巡检成功
	dprintf("Task module is success\r\n");
	qTaskDeleteSelf();                               //删除高优先级任务
}

void taskLoEntry(void * param)
{
	dprintf("Enter TaskLo Handler.\r\n");
	qTaskDelay(2);                                   //任务延时
	dprintf("TaskHi LOS_TaskSuspend Success.\r\n");
	qTaskWakeUp(&taskHi);                            //唤醒高优先级任务
	qTaskDeleteSelf();                               //删除低优先级任务
}

/*任务测试入口函数，在里面创建优先级不一样的两个任务*/
InspectStu Example_TaskEntry(void)
{
	qTaskSchedDisable();     //锁任务调度
	
	qTaskInit(&taskHi, taskHiEntry, (void *)0x11111111, 4, &taskHiEnv[1024]);  //初始化任务
	qTaskInit(&taskLo, taskLoEntry, (void *)0x22222222, 5, &taskLoEnv[1024]);
	
	dprintf("qTaskInit() Success!\r\n");
	
	qTaskSchedEnable();      //解锁任务调度
	
	return INSPECT_STU_SUCCESS;
}

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
