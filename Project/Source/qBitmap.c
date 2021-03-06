/*
******************************************************************************
* Copyright (c) 2017,学生开放实验室
* All rights reserved.
******************************************************************************
* 文件名称： qBitmap.c
* 文件标识： OS
* 摘 要： 位图函数定义
******************************************************************************
* 当前版本： 1.0
* 作 者： Quard
* 完成日期： 2017年7月18日
******************************************************************************
*/
#include "qLib.h"

/******************************************************************************
 * 函数名称：位图结构初始化函数
 * 函数功能：初始化位图
 * 输入参数：qBitmap * bitmap     位图结构指针
 * 输出参数：无 
 ******************************************************************************/
void qBitmapInit(qBitmap * bitmap)
{
	bitmap->bitmap = 0;
}

/******************************************************************************
 * 函数名称：位图长度函数
 * 函数功能：求位图长度
 * 输入参数：无
 * 输出参数：位图长度
 ******************************************************************************/
uint32_t qBitmapPosCount(void)
{
	return 32; 
}

/******************************************************************************
 * 函数名称：位图置为函数
 * 函数功能：对位图某一位置1
 * 输入参数：qBitmap * bitmap     位图结构指针
			uint32_t pos		 需要置1的位
 * 输出参数：无
 ******************************************************************************/
void qBitmapSet(qBitmap * bitmap, uint32_t pos)
{
	bitmap->bitmap |= 1 << pos;
}

/******************************************************************************
 * 函数名称：位图清零函数
 * 函数功能：对位图某一位清0
 * 输入参数：qBitmap * bitmap     位图结构指针
			uint32_t pos		 需要清0的位
 * 输出参数：无
 ******************************************************************************/
void qBitmapClear(qBitmap * bitmap, uint32_t pos)
{
	bitmap->bitmap &= ~(1 << pos);
}

/******************************************************************************
 * 函数名称：位图查找函数
 * 函数功能：查询位图中置1的最低一位
 * 输入参数：qBitmap * bitmap     位图结构指针
 * 输出参数：最低一个被置1的位序号
 ******************************************************************************/
uint32_t qBitmapGetFirstSet(qBitmap * bitmap)
{
	/*分组查询表以及查询算法*/
	static const uint8_t quickFindTable[] =     
	{
	    /* 00 */ 0xff, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 10 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 20 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 30 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 40 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 50 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 60 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 70 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 80 */ 7,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* 90 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* A0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* B0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* C0 */ 6,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* D0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* E0 */ 5,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	    /* F0 */ 4,    0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
	};
	
	if (bitmap->bitmap & 0xff)
    {
        return quickFindTable[bitmap->bitmap & 0xff];         
    }
    else if (bitmap->bitmap & 0xff00)
    {
        return quickFindTable[(bitmap->bitmap >> 8) & 0xff] + 8;        
    }
    else if (bitmap->bitmap & 0xff0000)
    {
        return quickFindTable[(bitmap->bitmap >> 16) & 0xff] + 16;        
    }
    else if (bitmap->bitmap & 0xFF000000)
    {
        return quickFindTable[(bitmap->bitmap >> 24) & 0xFF] + 24;
    }
    else
    {
        return qBitmapPosCount();
    }
}

/************** (C) COPYRIGHT 2014-2018 学生开放实验室 *****END OF FILE*********/
