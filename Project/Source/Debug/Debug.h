/*
******************************************************************************
* Copyright (c) 2017,ѧ������ʵ����
* All rights reserved.
******************************************************************************
* �ļ����ƣ� Debug.h
* �ļ���ʶ�� ������Ϣ
* ժ Ҫ�� ������Ϣ�����������
******************************************************************************
* ��ǰ�汾�� 1.0
* �� �ߣ� Quard
* ������ڣ� 2017��7��18��
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

/************** (C) COPYRIGHT 2014-2018 ѧ������ʵ���� *****END OF FILE*********/
