/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： qHooks.h
* 文件标识： OS
* 摘 要： 钩子函数声明
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月26日
******************************************************************************
*/
#ifndef QHOOKS_H
#define QHOOKS_H

#include "qTask.h"

void qHooksCpuIdle(void);
void qHooksSysTick(void);
void qHooksTaskSwitch(qTask * from, qTask * to);
void qHooksTaskInit(qTask * task);

#endif /*QHOOKS_H*/
/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
