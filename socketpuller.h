#ifndef SOCKETPULLER_H_INCLUDED
#define SOCKETPULLER_H_INCLUDED
/** Non-thread-safe socket puller */

#include "socketpool.h"
#include "common.h"
#include "oo.h"

typedef struct _SocketPuller SocketPuller;

struct _SocketPuller{
    PRIMEMB SocketPool  p;
    PRIMEMB fd_set  s;
    PRIMEMB SOCKET  Max;

    PUBMEMB int (*Add)(SocketPuller *p,
                       SOCKET s,
                       const void *Data,
                       int DataLength
                       );

    int (*Del)(SocketPuller *p, SOCKET s);

    PUBMEMB SOCKET (*Select)(SocketPuller *p,
                             struct timeval *tv,
                             void **Data,
                             BOOL Reading,
                             BOOL Writing,
                             int *err
                             );

    PUBMEMB void (*CloseAll)(SocketPuller *p, SOCKET ExceptFor);
    PUBMEMB void (*Free)(SocketPuller *p);
    PUBMEMB void (*FreeWithoutClose)(SocketPuller *p);
};

int SocketPuller_Init(SocketPuller *p, int DataLength);

SocketPuller **SocketPullers_Init(int Count, int DataLength);

void SocketPullers_CloseAll(SocketPuller **Pullers);

void SocketPullers_FreeWithoutClose(SocketPuller **Pullers);

void SocketPullers_Free(SocketPuller **Pullers);

#endif // SOCKETPULLER_H_INCLUDED
