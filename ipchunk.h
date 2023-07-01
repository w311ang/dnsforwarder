#ifndef IPCHUNK_H_INCLUDED
#define IPCHUNK_H_INCLUDED

#include "bst.h"
#include "stablebuffer.h"
#include "common.h"

typedef struct _IpAddr {
    unsigned char   Addr[16]; /* Both v4 and v6. In big-endian order. */
    const char      *Zone;
} IpAddr;

typedef struct _IpPort {
    IpAddr      Ip;
    uint16_t    Port;
} IpPort;

typedef struct _IpSet {
    IpAddr  Ip;
    int     PrefixBits;
} IpSet;

int IpAddr_BitLength(const IpAddr *ipAddr);
int IpAddr_Is6(const IpAddr *ipAddr);
int IpAddr_HasZone(const IpAddr *ipAddr);
int IpAddr_IsValid(const IpAddr *ipAddr);
void IpAddr_From4(const unsigned char Addr[4], IpAddr *ipAddr);
void IpAddr_From6(const unsigned char Addr[16], IpAddr *ipAddr);
int IpAddr_Parse(const char *s, IpAddr *ipAddr);

BOOL IpSet_IsSingleIp(const IpSet *ipSet);
int IpSet_Parse(const char *s, const char *p, IpSet *ipSet);

typedef struct _IpElement {
    IpSet   IpSet;
    int     Type;
    void    *Data;
} IpElement;

typedef struct _IpChunk{
    Bst             AddrChunk;
    Bst             CidrChunk;
    StableBuffer    Datas;
    StableBuffer    Extra;
} IpChunk;

void IpChunk_Free(IpChunk *ic);

int IpChunk_Init(IpChunk *ic);

int IpChunk_Add(IpChunk *ic,
                const char *Ip,
                int Type,
                const void *Data,
                uint32_t DataLength
                );

BOOL IpChunk_Find(IpChunk *ic, unsigned char *Ip, int IpBytes, int *Type, const char **Data);

#endif /* IPCHUNK_H_INCLUDED */
