#include "tcpfrontend.h"
#include "socketpuller.h"
#include "addresslist.h"
#include "utils.h"
#include "mmgr.h"
#include "logs.h"

extern BOOL Ipv6_Enabled;

static SocketPuller Frontend;

static void TcpFrontend_Work(void *Unused)
{
    /* Buffer */
    #define BUF_LENGTH  2048
    char *ReceiveBuffer;
    IHeader *Header;

    #define LEFT_LENGTH  (BUF_LENGTH - sizeof(IHeader))
    char *Entity;

    ReceiveBuffer = SafeMalloc(BUF_LENGTH);
    if( ReceiveBuffer == NULL )
    {
        ERRORMSG("No enough memory, 26.\n");
        return;
    }

    Header = (IHeader *)ReceiveBuffer;
    Entity = ReceiveBuffer + sizeof(IHeader);

    /* Loop */
    while( TRUE )
    {
        /* Address */
        char AddressBuffer[sizeof(Address_Type)];
        struct sockaddr *IncomingAddress = (struct sockaddr *)AddressBuffer;

        SOCKET sock, sock_c;
        const sa_family_t *f;

        int RecvState;
        uint16_t TCPLength;

        socklen_t AddrLen;

        char Agent[sizeof(Header->Agent)];

        sock = Frontend.Select(&Frontend,
                               NULL,
                               (void **)&f,
                               TRUE,
                               FALSE
                               );
        if( sock == INVALID_SOCKET )
        {
            ERRORMSG("Fatal error 57.\n");
            break;
        }

        AddrLen = sizeof(Address_Type);

        sock_c = accept(sock, IncomingAddress, &AddrLen);
        if(sock_c == INVALID_SOCKET)
        {
            continue;
        }

        RecvState = recv(sock_c, (char *)&TCPLength, 2, 0);
        if( RecvState < 2 )
        {
            INFO("No valid data received from TCP client %s.\n", Agent);
            CLOSE_SOCKET(sock_c);
            continue;
        }

        TCPLength = ntohs(TCPLength);
        if( TCPLength > LEFT_LENGTH )
        {
            WARNING("TCP client %s segment is too large, discarded.\n", Agent);
            CLOSE_SOCKET(sock_c);
            continue;
        }

        RecvState = recv(sock_c, Entity, TCPLength, 0);

        if( *f == AF_INET )
        {
            IPv4AddressToAsc(&(((struct sockaddr_in *)IncomingAddress)->sin_addr), Agent);
        } else {
            IPv6AddressToAsc(&(((struct sockaddr_in6 *)IncomingAddress)->sin6_addr), Agent);
        }

        if( RecvState != TCPLength )
        {
            INFO("No valid data received from TCP client %s.\n", Agent);
            CLOSE_SOCKET(sock_c);
            continue;
        }

        IHeader_Fill(Header,
                     FALSE,
                     Entity,
                     RecvState,
                     NULL,
                     sock_c,
                     *f,
                     Agent
                     );

        MMgr_Send(Header, BUF_LENGTH);
    }
    SafeFree(ReceiveBuffer);
}

void TcpFrontend_StartWork(void)
{
    ThreadHandle t;

    CREATE_THREAD(TcpFrontend_Work, NULL, t);
    DETACH_THREAD(t);
}

static void TcpFrontend_Cleanup(void)
{
    Frontend.CloseAll(&Frontend, INVALID_SOCKET);
    Frontend.Free(&Frontend);
}

int TcpFrontend_Init(ConfigFileInfo *ConfigInfo, BOOL StartWork)
{
    StringList *TCPLocal;
    StringListIterator i;
    const char *One;

    int Count = 0;

    TCPLocal = ConfigGetStringList(ConfigInfo, "TCPLocal");
    if( TCPLocal == NULL )
    {
        WARNING("No TCP interface specified.\n");
        return -11;
    }

    if( StringListIterator_Init(&i, TCPLocal) != 0 )
    {
        return -20;
    }

    if( SocketPuller_Init(&Frontend) != 0 )
    {
        return -19;
    }

    while( (One = i.Next(&i)) != NULL )
    {
        Address_Type a;
        sa_family_t f;

        SOCKET sock;

        f = AddressList_ConvertFromString(&a, One, 53);
        if( f == AF_UNSPEC )
        {
            ERRORMSG("Invalid TCPLocal option: %s .\n", One);
            continue;
        }

        sock = socket(f, SOCK_STREAM, IPPROTO_TCP);
        if( sock == INVALID_SOCKET )
        {
            continue;
        }

        if( bind(sock,
                 (const struct sockaddr *)&(a.Addr),
                 GetAddressLength(f)
                 )
            != 0 )
        {
            char p[128];

            snprintf(p, sizeof(p), "Opening TCP interface %s failed", One);
            p[sizeof(p) - 1] = '\0';

            ShowSocketError(p, GET_LAST_ERROR());
            CLOSE_SOCKET(sock);
            continue;
        }

        if( listen(sock, 16) == SOCKET_ERROR )
        {
            ERRORMSG("Can't listen on interface: %s .\n", One);
            break;
        }

        if( f == AF_INET6 )
        {
            Ipv6_Enabled = TRUE;
        }

        Frontend.Add(&Frontend, sock, &f, sizeof(sa_family_t));
        INFO("TCP interface %s opened.\n", One);
        ++Count;
    }

    atexit(TcpFrontend_Cleanup);

    if( Count == 0 )
    {
        ERRORMSG("No TCP interface opened.\n");
        return -163;
    }

    if( StartWork )
    {
        TcpFrontend_StartWork();
    }

    return 0;
}
