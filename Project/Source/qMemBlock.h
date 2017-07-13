#ifndef QMEMBLOCK_H
#define QMEMBLOCK_H

#include "qEvent.h"

typedef struct _qMemBlock
{
	qEvent event;               //事件控制块
	void * memStart;            //存储块起始地址
	uint32_t blockSize;         //存储块大小
	uint32_t maxCount;          //存储块最大数量
	qList blockList;            //存储块链表
}qMemBlock;

typedef struct _qMemBlockInfo
{
	uint32_t count;             //当前存储块数量
	uint32_t maxCount;          //存储块最大数量
	uint32_t blockSize;         //存储块大小
	uint32_t taskCount;         //等待的任务数量
}qMemBlockInfo;

void qMemBlockInit(qMemBlock * memBlock, uint8_t * memStart, uint32_t blockSize, uint32_t blockCnt);
uint32_t qMemBlockWait(qMemBlock * memBlock, uint8_t **mem, uint32_t waitTicks);
uint32_t qMemBlockNoWaitGet(qMemBlock * memBlock, void **mem);
uint32_t qMemBlockNotify(qMemBlock * memBlock, uint8_t *mem);
uint32_t qMemBlockDestroy(qMemBlock * memBlock);
void qMemBlockGetInfo(qMemBlock * memBlock, qMemBlockInfo * info);

#endif /*QMEMBLOCK_H*/
