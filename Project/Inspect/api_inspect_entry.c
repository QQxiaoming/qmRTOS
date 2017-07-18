/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： api_inspect_entry.c
* 文件标识： 巡检测试任务
* 摘 要： 定义系统各模块巡检任务函数
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月18日
******************************************************************************
*/
#include "qmRTOS.h"

#include "api_inspect_entry.h"

extern InspectStu Example_TaskEntry(void);
extern InspectStu Example_ListEntry(void);
extern InspectStu Example_EventEntry(void);

static qTask InspectTask;                                             //巡检任务
static qTaskStack InspectTaskEnv[1024];

static uint8_t gInspectErrCnt = 0;                                    //定义标记错误个数变量
	
static Inspect_def qInspect[INSPECT_NUM] = {                          //巡检表
	{INSPECT_TASK, INSPECT_STU_START, Example_TaskEntry, "TASK"},
	{INSPECT_LIST, INSPECT_STU_START, Example_ListEntry, "LIST"},
	{INSPECT_EVENT, INSPECT_STU_START, Example_EventEntry, "EVENT"},
};

/*****************************************************************************
 函数名  	: qInspectStatusSetByID
 描述  	    : 设置检查状态标志
 输入参数	: InspectID  ---  被检查的模块ID
              InspectStu ---  检查标志
 返回值   	: 是否成功
 *****************************************************************************/
InspectStu qInspectStatusSetByID(InspectID inspectid,InspectStu inspectstu)
{
	int index;
	if(inspectid >= INSPECT_NUM)
    {
        dprintf("\r\nInspectID = [%d] Err.\r\n",inspectid);
        return INSPECT_STU_ERROR;
    }
	
	for(index = 0;index < INSPECT_NUM;index++)
    {   
        if(inspectid == qInspect[index].inspectID)
        {
            qInspect[index].status = inspectstu;
        }
     
    }

    if(INSPECT_NUM == index)
    {
        return INSPECT_STU_ERROR;
    }
    else
    {
        return INSPECT_STU_SUCCESS;
    }

}
/*****************************************************************************
 函数名  	: qInspectByID
 描述  	    : 通过ID检查相关功能的函数
 输入参数	: inpectid  --- 被检查的模块ID
 返回值   	: 检查结果
 *****************************************************************************/
InspectStu qInspectByID(InspectID inspectid)
{
	int index;
	InspectStu INSPECT_STU;
	InspectStu enCurStatus = INSPECT_STU_START;
	
	if(inspectid >= INSPECT_NUM)
    {
        dprintf("\r\nInspectID = [%d] Err.\r\n",inspectid);
        return INSPECT_STU_ERROR;
    }
	
	for(index = 0;index < INSPECT_NUM;index++)           //扫描所有要检查的项目
    {
		if(inspectid == qInspect[index].inspectID)            //如果巡检项目在巡检表中
		{
			if(NULL == qInspect[index].Inspectfunc)          //如果巡检项目入口函数为空
			{
				dprintf("InspectID = [%d] Err,Inspectfunc is NULL.\r\n\r\n",inspectid);  //打印错误信息
				return INSPECT_STU_ERROR;
			}
			INSPECT_STU = qInspect[index].Inspectfunc();     //否则调用项目入口函数,返回结果
			do{
				if((INSPECT_STU_SUCCESS == qInspect[index].status) && (INSPECT_STU_SUCCESS == INSPECT_STU))
				{
					dprintf("Inspect %s success\r\n\r\n",qInspect[index].name);
					enCurStatus = INSPECT_STU_SUCCESS;
				}
				else if(INSPECT_STU_ERROR == qInspect[index].status)
				{
					enCurStatus = INSPECT_STU_ERROR;
                    printf("Inspect %s error,gInspectErrCnt = [%d]\r\n\r\n",qInspect[index].name,gInspectErrCnt);
				}
				
				qTaskDelay(10);
				
			}while((INSPECT_STU_START == qInspect[index].status) || (enCurStatus != qInspect[index].status));
			break;
		}
    }
	
	if(INSPECT_NUM == index)
    {
        return INSPECT_STU_ERROR;
    }
    else
    {
        return enCurStatus ? INSPECT_STU_ERROR:INSPECT_STU_SUCCESS;
    }
}

/*****************************************************************************
 函数名  	: InspectTaskEntry
 描述  	    : 功能巡检任务入口
 输入参数	: 无
 返回值   	: 无
 *****************************************************************************/
void InspectTaskEntry(void * parm)
{
	InspectStu INSPECT_STU;
	
	qSetSysTickPeriod (10);   //初始化系统定时器为10ms
	
	dprintf("\r\nInspect start.\r\n");
	
	for(int index = 0;index < INSPECT_NUM;index++)           //扫描所有要检查的项目
    {
        INSPECT_STU = qInspectByID((InspectID)index);        //调用检查函数检查
        if(INSPECT_STU_SUCCESS != INSPECT_STU)               //如果发生错误
        {
            gInspectErrCnt++;                                //错误计数值加一
        }
    }
     
    dprintf("Inspect completed,gInspectErrCnt = [%d]\r\n\r\n",gInspectErrCnt);     //打印巡检情况，错误个数
 
	while(1)
	{
		qTaskDelay(10);
	}
}

/*****************************************************************************
 函数名  	: InspectTaskstart
 描述  	    : 功能巡检任务初始化
 输入参数	: 无
 返回值   	: 无
 *****************************************************************************/
void InspectTaskstart(void)
{
	qTaskInit(&InspectTask, InspectTaskEntry, (void *)0xFFFFFFFF, 10, &InspectTaskEnv[1024]);  //初始化巡检任务
}

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
