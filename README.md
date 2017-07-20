# 自己学习写RTOS嵌入式操作系统

自己学习写RTOS嵌入式操作系统

* *系统内核源码全部在Project>Source目录下，适用于**CM3**内核*

> 创建工程  日期：2017年6月25日



- 本RTOS目前包含一下模块

*1.任务管理*

*2.双向链表*

*3.事件控制块*

*4.计数信号量*

*5.邮箱*

*6.内存存储块*

*7.事件标志组*

*8.互斥锁*

- 提供功能模块巡检测试任务，源码位于Project>Inspect目录下。

只需要在main.c包含巡检任务头文件

`include "api_inspect_entry.h"`

并调用开始巡检函数

`InspectTaskstart(); //进行功能巡检测试任务`

即可进行功能巡检，如需要调试信息输出，请在qConfig.h中打开调试开关

`//启用调试开关`

`#define QMRTOS_KERNEL_DEBUG_OUT         1`      

`#define QMRTOS_TEST_KEIL_SWSIMU         1`

