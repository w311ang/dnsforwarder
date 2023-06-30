#ifndef TCPM_C_INCLUDED
#define TCPM_C_INCLUDED

#include "mcontext.h"
#include "addresslist.h"
#include "socketpuller.h"

typedef struct _TcpM TcpM;

struct _TcpM {
    /* private */
    SOCKET          Incoming;
    Address_Type    IncomingAddr;
    SocketPuller    Puller;

    ModuleContext   Context;

    ThreadHandle    WorkThread;

    int IsServer;

    const char      *ServiceName;
    AddressList     ServiceList;
    struct sockaddr **Services;
    sa_family_t     *ServiceFamilies;
    SocketPuller    QueryPuller;
    SocketPuller    **Agents;

    const char      *ProxyName;
    AddressList     SocksProxyList;
    struct sockaddr **SocksProxies;
    sa_family_t     *SocksProxyFamilies;
    SocketPuller    ProxyPuller;
    SocketPuller    **Proxies;

    /* TCP 3-way handshake is heavier. Connect all, and then choose. */
    BOOL            Parallel;

    /* public */
    int (*Send)(TcpM *m,
                const char *Buffer,
                int BufferLength
                );
};

int TcpM_Init(TcpM *m, const char *Services, BOOL Parallel, const char *SocksProxies);

#endif /* TCPM_C_INCLUDED */
