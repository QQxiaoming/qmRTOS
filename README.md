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

*9.软件定时器*

*10.Hooks扩展*

- 提供功能模块巡检测试任务，源码位于Project>Inspect目录下。

请在qConfig.h中打开功能巡检开关

`#define QMRTOS_ENABLE_INSPECT          1`  

即可进行功能巡检。

如需要调试信息输出，请在qConfig.h中打开调试开关

`#define QMRTOS_KERNEL_DEBUG_OUT         1`      

`#define QMRTOS_TEST_KEIL_SWSIMU         1`

如需要内核裁剪，请在qConfig.h中打开内核裁剪开关

` #define QMRTOS_ENABLE_SEM               1`                       

`#define QMRTOS_ENABLE_MUTEX             1`

`#define QMRTOS_ENABLE_FLAGGROUP         1`      

`#define QMRTOS_ENABLE_MBOX              1`          

`#define QMRTOS_ENABLE_MEMBLOCK          1`    

`#define QMRTOS_ENABLE_TIMER             1`        

`#define QMRTOS_ENABLE_CPUUSAGE_STAT     1`        

`#define QMRTOS_ENABLE_HOOKS             1`        