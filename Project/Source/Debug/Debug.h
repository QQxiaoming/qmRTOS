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
