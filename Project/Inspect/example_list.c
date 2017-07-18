/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： example_list.c
* 文件标识： 巡检测试任务
* 摘 要： 定义双向链表模块测试任务入口
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月18日
******************************************************************************
*/
#include "qmRTOS.h"
#include "api_inspect_entry.h"

qList list;
qNode node[8];

InspectStu Example_ListEntry(void)
{
	int i;
	dprintf("initial......\n");
	qListInit(&list);  //初始化链表
	
	for (i = 0; i < 8; i++)          //在链表内插入结点
    {
        qNodeInit(&node[i]);
        qListAddFirst(&list, &node[i]);
		if(qListCount(&list) != (i + 1))
		{
			dprintf("add node error\n");
			qInspectStatusSetByID(INSPECT_LIST,INSPECT_STU_ERROR);
			return INSPECT_STU_ERROR;
		}
    }

    for (i = 0; i < 8; i++)          //在链表内删除结点
    {
        qListRemoveFirst(&list);
		if(qListCount(&list) != (8 - (i + 1)))
		{
			dprintf("delete node error\n");
			qInspectStatusSetByID(INSPECT_LIST,INSPECT_STU_ERROR);
			return INSPECT_STU_ERROR;
		}
    }
	
	dprintf("List module is success\r\n");
	qInspectStatusSetByID(INSPECT_LIST, INSPECT_STU_SUCCESS);
	return INSPECT_STU_SUCCESS;
}

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
