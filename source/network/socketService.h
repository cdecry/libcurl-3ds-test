#pragma once

#ifndef SOCKETSERVICE_H
#define SOCKETSERVICE_H
#define SOC_ALIGN       0x1000
#define SOC_BUFFERSIZE  0x100000

#include <stdarg.h>
#include <stdlib.h>
#include <malloc.h>
#include "../utility/common.h"

class SocketService {
    private:
        u32 *SOC_buffer = NULL;
        s32 sock = -1, csock = -1;

    public: 
        void failExit(const char *fmt, ...);
        void initSocketService();
};

#endif
