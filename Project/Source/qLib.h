#ifndef QLIB_H
#define QLIB_H

#include <stdint.h>     // 标准头文件，里面包含了常用的类型定义，如uint32_t

typedef struct         // 位图类型
{
	uint32_t bitmap;
}qBitmap;

void qBitmapInit(qBitmap * bitmap);
uint32_t qBitmapPosCount(void);
void qBitmapSet(qBitmap * bitmap, uint32_t pos);
void qBitmapClear(qBitmap * bitmap, uint32_t pos);
uint32_t qBitmapGetFirstSet(qBitmap * bitmap);

typedef struct _qNode           //链表的结点类型
{
    struct _qNode * preNode; 	// 该结点的前一个结点
    struct _qNode * nextNode;   // 该结点的后一个结点
}qNode;

void qNodeInit(qNode * node);

typedef struct _qList           //链表结构类型
{
	qNode headNode;             //链表的头结点
	uint32_t nodeCount;         //链表中结点数量
}qList;

#define qNodeParent(node, parent, name) (parent *)((uint32_t)node - (uint32_t)&((parent *)0)->name)
													//获取结点所在的父struct结构首地址

void qListInit(qList * list);
uint32_t qListCount(qList * list);
qNode * qListFirst(qList * list);
qNode * qListLast(qList * list);
qNode * qListPre(qList * list, qNode * node);
qNode * qListNext(qList * list, qNode * node);
void qListRemoveAll(qList * list);
void qListAddFirst(qList * list, qNode * node);
void qListAddLast(qList * list, qNode * node);
qNode * qListRemoveFirst(qList * list);
void qListInsertAfter(qList * list, qNode * nodeAfter, qNode * nodeToInsert);
void qListRemove(qList * list, qNode * node);

#endif /*QLIB_H*/
