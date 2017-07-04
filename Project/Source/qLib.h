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

#endif /*QLIB_H*/
