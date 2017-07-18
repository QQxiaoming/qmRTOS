/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： qTime.c
* 文件标识： OS
* 摘 要： 时间相关模块函数定义
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月18日
******************************************************************************
*/
#include "qmRTOS.h"

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

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
