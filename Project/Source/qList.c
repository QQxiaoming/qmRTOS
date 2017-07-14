#include "qLib.h"

/******************************************************************************
 * 函数名称：结点初始化函数
 * 函数功能：初始化结点
 * 输入参数：qNode * node    结点结构指针
 * 输出参数：无
 ******************************************************************************/
void qNodeInit(qNode * node)
{
	node->nextNode = node;       //将前一结点和后一结点都指向自己
	node->preNode = node;
}
 
#define firstNode headNode.nextNode
#define lastNode headNode.preNode

/******************************************************************************
 * 函数名称：链表初始化函数
 * 函数功能：初始化链表
 * 输入参数：qList * list    链表结构指针
 * 输出参数：无
 ******************************************************************************/
void qListInit(qList * list)
{
	list->firstNode = &(list->headNode);     //将链表头结点指向自己并使计数值为0
	list->lastNode = &(list->headNode);
	list->nodeCount = 0;
}

/******************************************************************************
 * 函数名称：链表结点数计数函数
 * 函数功能：返回链表中的结点数
 * 输入参数：qList * list    链表结构指针
 * 输出参数：链表中的结点数
 ******************************************************************************/
uint32_t qListCount(qList * list)
{
	return list->nodeCount;
}

/******************************************************************************
 * 函数名称：返回首个结点函数
 * 函数功能：返回链表中首个结点
 * 输入参数：qList * list    链表结构指针
 * 输出参数：链表中首个结点
 ******************************************************************************/
qNode * qListFirst(qList * list)
{
	qNode * node = (qNode *)0;
	if(list->nodeCount != 0)
	{
		node = list->firstNode;
	}
	return node;
}

/******************************************************************************
 * 函数名称：返回末尾结点函数
 * 函数功能：返回链表中末尾结点
 * 输入参数：qList * list    链表结构指针
 * 输出参数：最后一个结点，若无结点，则返回0
 ******************************************************************************/
qNode * qListLast(qList * list)
{
	qNode * node = (qNode *)0;
	if(list->nodeCount != 0)
	{
		node = list->lastNode;
	}
	return node;
}

/******************************************************************************
 * 函数名称：返回链表中指定结点的前一结点函数
 * 函数功能：返回链表中指定结点的前一结点
 * 输入参数：qList * list    链表结构指针
			qNode * node    参考结点
 * 输出参数：链表中指定结点的前一结点指针
 ******************************************************************************/
qNode * qListPre(qList * list, qNode * node)
{
	if(node->preNode == node)
	{
		return (qNode *)0;
	}
	else
	{
		return node->preNode;
	}
}

/******************************************************************************
 * 函数名称：返回链表中指定结点的后一结点函数
 * 函数功能：返回链表中指定结点的后一结点
 * 输入参数：qList * list    链表结构指针
			qNode * node    参考结点
 * 输出参数：链表中指定结点的后一结点指针
 ******************************************************************************/
qNode * qListNext(qList * list, qNode * node)
{
	if(node->nextNode == node)
	{
		return (qNode *)0;
	}
	else 
	{
		return node->nextNode;
	}
}

/******************************************************************************
 * 函数名称：清空链表函数
 * 函数功能：移除链表中的所有结点
 * 输入参数：qList * list    链表结构指针
 * 输出参数：无
 ******************************************************************************/
void qListRemoveAll(qList * list)
{
	uint32_t count;
	qNode * nextNode;
	
	nextNode = list->firstNode;
	for(count = list->nodeCount; count != 0; count--)  //遍历所有结点
	{
		qNode * currentNode = nextNode;                //保持当前结点
		nextNode = nextNode->nextNode;                 //指向下一结点

		currentNode->nextNode = currentNode;           //重置当前结点的信息
		currentNode->preNode = currentNode;
	}
	
	list->firstNode = &(list->headNode);               //重置头结点
	list->lastNode = &(list->headNode);
	list->nodeCount = 0;
}

/******************************************************************************
 * 函数名称：添加指定结点到链表头部函数
 * 函数功能：将指定结点添加到链表的头部
 * 输入参数：qList * list    链表结构指针
			qNode * node    待插入结点
 * 输出参数：无
 ******************************************************************************/
void qListAddFirst(qList * list, qNode * node)
{
    node->preNode = list->firstNode->preNode;
    node->nextNode = list->firstNode;

    list->firstNode->preNode = node;
    list->firstNode = node;
    list->nodeCount++;
}


/******************************************************************************
 * 函数名称：添加指定结点到链表尾部函数
 * 函数功能：将指定结点添加到链表的尾部
 * 输入参数：qList * list    链表结构指针
			qNode * node    待插入结点
 * 输出参数：无
 ******************************************************************************/
void qListAddLast(qList * list, qNode * node)
{
	node->nextNode = &(list->headNode);
    node->preNode = list->lastNode;

    list->lastNode->nextNode = node;
    list->lastNode = node;
    list->nodeCount++;
}

/******************************************************************************
 * 函数名称：移除链表中的第1个结点函数
 * 函数功能：将链表中的第1个结点移除
 * 输入参数：qList * list    链表结构指针
 * 输出参数：如果链表为空，返回0，否则的话，返回第1个结点
 ******************************************************************************/
qNode * qListRemoveFirst(qList * list)
{
    qNode * node = (qNode *)0;

	if( list->nodeCount != 0 )
    {
        node = list->firstNode;

        node->nextNode->preNode = &(list->headNode);
        list->firstNode = node->nextNode;
        list->nodeCount--;
    }
    return  node;
}

/******************************************************************************
 * 函数名称：将指定的结点插入到某个结点后面函数
 * 函数功能：将指定的结点插入到某个结点后面
 * 输入参数：qList * list          链表结构指针
			qNode * nodeAfter	  参考结点指针
			qNode * nodeToInsert  待插入结点指针
 * 输出参数：无
 ******************************************************************************/
void qListInsertAfter(qList * list, qNode * nodeAfter, qNode * nodeToInsert)
{
    nodeToInsert->preNode = nodeAfter;
    nodeToInsert->nextNode = nodeAfter->nextNode;

    nodeAfter->nextNode->preNode = nodeToInsert;
    nodeAfter->nextNode = nodeToInsert;

    list->nodeCount++;
}

/******************************************************************************
 * 函数名称：移除链表中指定结点函数
 * 函数功能：将指定结点从链表中移除
 * 输入参数：qList * list      链表结构指针
			qNode * node	  结点指针
 * 输出参数：无
 ******************************************************************************/
void qListRemove(qList * list, qNode * node)
{
    node->preNode->nextNode = node->nextNode;
    node->nextNode->preNode = node->preNode;
    list->nodeCount--;
}
