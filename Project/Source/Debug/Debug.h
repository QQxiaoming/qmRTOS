/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： Debug.h
* 文件标识： 调试信息
* 摘 要： 调试信息输出函数声明
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月18日
******************************************************************************
*/
#ifndef DEBUG_H
#define DEBUG_H

#include <string.h>
#include <stdio.h>

#include "qConfig.h"

int dprintf_1(const char *format,...);

#if QMRTOS_KERNEL_DEBUG_OUT
    #define dprintf printf
#else
    #define dprintf dprintf_1
#endif

#endif /*DEBUG_H*/

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
