#include <Debug/Debug.h>

/*重定向fputc函数，以便使用printf输出调试信息，QMRTOS_KERNEL_DEBUG_OUT = 1时开启调试*/
/*QMRTOS_TEST_KEIL_SWSIMU = 1时，从keil软件调试的Debug (printf) Viewer窗口可以看到调试信息*/
/*QMRTOS_TEST_KEIL_SWSIMU = 0时，可以进行硬件调试（需添加硬件调试相关代码）*/
#if QMRTOS_TEST_KEIL_SWSIMU

#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

struct __FILE 
{ 
	int handle; /* Add whatever needed */ 
};

FILE __stdout;
FILE __stdin;

int fputc(int ch, FILE *f) {
  if (DEMCR & TRCENA) {
    while (ITM_Port32(0) == 0);
    ITM_Port8(0) = ch;
  }
  return(ch);
}

#else

int fputc(int ch, FILE *f)
{
	/*在这里添加硬件调试输出*/
	
    return (ch);
}

#endif

/*QMRTOS_KERNEL_DEBUG_OUT = 0时，关闭调试dprintf直接返回*/
int dprintf_1(const char *format,...)
{
    return 0;
}
