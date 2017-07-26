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
#define	QMRTOS_PRO_COUNT				32		                //优先级数
#define QMRTOS_SLTIC_MAX                10                      //时间片大小
#define QMRTOS_IDLETASK_STACK_SIZE      1024                    //空闲人任务堆栈大小

#define QMRTOS_TIMERTASK_STACK_SIZE     1024                    //定时器任务堆栈大小
#define QMRTOS_TIMERTASK_PRTO           0                       //定时器任务优先级

#define QMRTOS_SYSTICK_MS               10                      //时钟节拍的周期，以ms为单位

//启用调试开关
#define QMRTOS_KERNEL_DEBUG_OUT         1                       //是否开启调试信息
#define QMRTOS_TEST_KEIL_SWSIMU         1                       //是否采用软件调试
#define QMRTOS_ENABLE_INSPECT           0                       //是否进行功能巡检

// 内核功能裁剪部分
#define QMRTOS_ENABLE_SEM               1                       //是否使能信号量
#define QMRTOS_ENABLE_MUTEX             1                       //是否使能互斥锁
#define QMRTOS_ENABLE_FLAGGROUP         1                       //是否使能事件标志组
#define QMRTOS_ENABLE_MBOX              1                       //是否使能邮箱
#define QMRTOS_ENABLE_MEMBLOCK          1                       //是否使能存储块
#define QMRTOS_ENABLE_TIMER             1                       //是否使能定时器
#define QMRTOS_ENABLE_CPUUSAGE_STAT     1                       //是否使能CPU使用率统计
#define QMRTOS_ENABLE_HOOKS             1                       //是否启用钩子函数

#endif /*QCONFIG_H*/

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
