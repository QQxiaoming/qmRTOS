/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： example_event.c
* 文件标识： 巡检测试任务
* 摘 要： 定义事件控制块模块测试任务入口
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月18日
******************************************************************************
*/
#include "qmRTOS.h"
#include "api_inspect_entry.h"

qEvent eventWaitTimeout;

qTask eventTask;
qTaskStack eventTaskEnv[1024]; 

void eventTaskEntry(void * param)
{
	dprintf("eventTaskEntry write event.\n");    //任务进入等待
	qEventWait(&eventWaitTimeout, currentTask, (void *)0, 0, 5);
	qTaskSched();
	dprintf("eventTaskEntry,read event success\n");

	if(qEventRemoveAll(&eventWaitTimeout, (void *)0, 0) != 0)
	{
		dprintf("Event remove error\r\n");
		qInspectStatusSetByID(INSPECT_EVENT, INSPECT_STU_ERROR);
		qTaskDeleteSelf();                           //删除任务
		return;
	}
	
	qInspectStatusSetByID(INSPECT_EVENT, INSPECT_STU_SUCCESS);
	dprintf("Event module is success\r\n");
	qTaskDeleteSelf();                               //删除任务
}
	
InspectStu Example_EventEntry(void)
{
	int exampleCnt = 0;
	
	qEventInit(&eventWaitTimeout, qEventTypeUnknow);    //初始化事件
	
	qTaskInit(&eventTask, eventTaskEntry, 0x0, 5, &eventTaskEnv[1024]);  //初始化示例任务
	
	qTaskDelay(7);                                   //任务延时
	
	qTask * rdyTask = qEventWakeUp(&eventWaitTimeout, (void *)0, 0);
	if (rdyTask == (qTask *)0)
    {
		exampleCnt++;
		if(exampleCnt == 0)
		{
		dprintf("eventTaskEntry,read event error\n");
		qInspectStatusSetByID(INSPECT_EVENT,INSPECT_STU_ERROR);
		}
	}
	else
	{
		qTaskSched();
	}

	return INSPECT_STU_SUCCESS;
}

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
