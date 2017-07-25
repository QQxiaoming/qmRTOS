/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： qConfig.h
* 文件标识： OS
* 摘 要： OS参数配置
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月18日
******************************************************************************
*/
#ifndef QCONFIG_H
#define QCONFIG_H

//OS相关配置
#define	QMRTOS_PRO_COUNT				32		
#define QMRTOS_SLTIC_MAX                10
#define QMRTOS_IDLETASK_STACK_SIZE      1024

#define QMRTOS_TIMERTASK_STACK_SIZE     1024
#define QMRTOS_TIMERTASK_PRTO           0

//启用调试开关
#define QMRTOS_KERNEL_DEBUG_OUT         1
#define QMRTOS_TEST_KEIL_SWSIMU         1

#endif /*QCONFIG_H*/

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
