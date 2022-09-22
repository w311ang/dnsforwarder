#include <string.h>
#include "utils.h"
#include "ipchunk.h"

static const char *Z0 = NULL;
static const char *Z4 = "\x01";
static const char *Z6noz = "\x02";

/*
IPv6: https://datatracker.ietf.org/doc/html/rfc8200
Zone: https://datatracker.ietf.org/doc/html/rfc6874
CIDR: https://datatracker.ietf.org/doc/html/rfc4632
      https://datatracker.ietf.org/doc/html/rfc4291#section-2.3
*/

static int IpAddr_BitLength(IpAddr *ipAddr)
{
    if( ipAddr->Zone == Z0 )
    {
        return 0;
    } else if( ipAddr->Zone == Z4 )
    {
        return 32;
    }

    return 128;
}

static int IpAddr_Is6(IpAddr *ipAddr)
{
    return ipAddr->Zone != Z0 && ipAddr->Zone != Z4;
}

static int IpAddr_HasZone(IpAddr *ipAddr)
{
    return IpAddr_Is6(ipAddr) && ipAddr->Zone != Z6noz;
}

static int IpAddr_IsValid(IpAddr *ipAddr)
{
    return ipAddr->Zone != Z0;
}

static void IpAddr_SetPrefix4(unsigned char Addr[16])
{
    memset(Addr, 0, 10);
    *(uint16_t *)(Addr + 10) = 0xffff;
}

static void IpAddr_From4(unsigned char Addr[4], IpAddr *ipAddr)
{
    ipAddr->Zone = Z4;
    IpAddr_SetPrefix4(ipAddr->Addr);
    *(uint32_t *)(ipAddr->Addr + 12) = *(uint32_t *)Addr;
}

static void IpAddr_From6(unsigned char Addr[16], IpAddr *ipAddr)
{
    ipAddr->Zone = Z6noz;
    memcpy(ipAddr->Addr, Addr, 16);
}

static int IpAddr_Parse(const char *s, IpAddr *ipAddr)
{
    const char *p = s;

    for(; *p; ++p)
    {
        switch( *p )
        {
        case '.':
            ipAddr->Zone = Z4;
            IpAddr_SetPrefix4(ipAddr->Addr);
            return IPv4AddressToNum(s, ipAddr->Addr + 12) != 4;
        case ':':
            p = strchr(p, '%');
            if( p == NULL )
            {
                ipAddr->Zone = Z6noz;
            } else {
                ipAddr->Zone = p + 1;
            }
            return IPv6AddressToNum(s, ipAddr->Addr) != 16;
        case '%':
            break;
        }
    }

    ipAddr->Zone = Z0;
    memset(ipAddr->Addr, 0, 16);
    return -1;
}

static BOOL IpSet_IsSingleIp(IpSet *ipSet)
{
    return ipSet->PrefixBits != 0 && ipSet->PrefixBits == IpAddr_BitLength(&(ipSet->Ip));
}

static int IpSet_Parse(const char *s, const char *p, IpSet *ipSet)
{
    int n, L;
    IpAddr *ipAddr = &(ipSet->Ip);

    if( IpAddr_Parse(s, ipAddr) != 0 )
    {
        return -1;
    }

    L = IpAddr_BitLength(ipAddr);

    if( p != NULL && *p )
    {
        n = atoi(p);
        if( n < 0 )
        {
            n = -1;
        } else if( n > L ) {
            n = L;
        }

        if( IpAddr_Is6(ipAddr) )
        {
            ipSet->Ip.Zone = Z6noz;
        }
    } else {
        n = L;
    }

    ipSet->PrefixBits = n;

    return 0;
}


static int Contain(IpElement *New, IpElement *Elm)
{
    IpSet *ipSetNew = &(New->IpSet);
    IpSet *ipSetElm = &(Elm->IpSet);
    IpAddr *ipAddrNew = &(ipSetNew->Ip);
    IpAddr *ipAddrElm = &(ipSetElm->Ip);
    int BitsNew;
    int BitsElm;

    if( IpAddr_IsValid(ipAddrElm) == FALSE )
    {
        return -1;
    }

    BitsNew = IpAddr_BitLength(ipAddrNew);
    BitsElm = IpAddr_BitLength(ipAddrElm);

    /* 1st: type */
    if( BitsNew != BitsElm )
    {
        return  BitsNew - BitsElm;
    } else {
        unsigned char *bn = ipAddrNew->Addr;
        unsigned char *be = ipAddrElm->Addr;
        int prefixBitsNew = ipSetNew->PrefixBits;
        int prefixBitsElm = ipSetElm->PrefixBits;
        int ret = 0;

        /* 2nd: prefix bits */
        if( prefixBitsNew < prefixBitsElm )
        {
            return 1;
        }

        /* 3rd: prefix value */
        if( BitsElm == 32 )
        {
            bn += 12;
            be += 12;
        }

        for(; prefixBitsElm >= 32; prefixBitsElm -= 32)
        {
            ret = memcmp(bn, be, 4);

            if( ret != 0 )
            {
                return ret;
            }

            bn += 4;
            be += 4;
        }

        if( prefixBitsElm > 0 ) {
            uint32_t u32New, u32Elm, mask;

            mask = htonl(~(~0U >> prefixBitsElm));
            u32New = *(uint32_t *)bn & mask;
            u32Elm = *(uint32_t *)be & mask;
            ret = memcmp(&u32New, &u32Elm, 4);
        }

        /* 4th: zone */
        if( ret == 0 )
        {
            if( IpAddr_HasZone(ipAddrElm) )
            {
                if( IpAddr_HasZone(ipAddrNew) )
                {
                    return strcmp(ipAddrNew->Zone, ipAddrElm->Zone);
                } else {
                    return 1;
                }
            }
        }

        return ret;
    }
}


void IpChunk_Free(IpChunk *ic)
{
    ic->AddrChunk.Free(&(ic->AddrChunk));
    ic->CidrChunk.Free(&(ic->CidrChunk));
    ic->Datas.Free(&(ic->Datas));
    ic->Extra.Free(&(ic->Extra));
}

int IpChunk_Init(IpChunk *ic)
{
    if( Bst_Init(&(ic->AddrChunk), sizeof(IpElement), (CompareFunc)Contain) != 0 )
    {
        return -1;
    }

    if( Bst_Init(&(ic->CidrChunk), sizeof(IpElement), (CompareFunc)Contain) != 0 )
    {
        goto EXIT_1;
    }

    if( StableBuffer_Init(&(ic->Datas)) != 0 )
    {
        goto EXIT_2;
    }

    if( StableBuffer_Init(&(ic->Extra)) != 0 )
    {
        goto EXIT_3;
    }

    return 0;

EXIT_3:
    ic->Datas.Free(&(ic->Datas));
EXIT_2:
    ic->CidrChunk.Free(&(ic->CidrChunk));
EXIT_1:
    ic->AddrChunk.Free(&(ic->AddrChunk));
    return -1;
}

int IpChunk_Add(IpChunk *ic,
                const char *Ip,
                int Type,
                const void *Data,
                uint32_t DataLength
                )
{
    char *p;
    IpElement   New;
    const IpElement   *elm;

    char *ip = ic->Extra.Add(&(ic->Extra), Ip, strlen(Ip) + 1, FALSE);
    if( ip == NULL )
    {
        return -1;
    }

    p = ip;
    for(; *p; ++p)
    {
        if( *p == '/' )
        {
            *p = 0;
            p++;
            break;
        }
    }

    if( IpSet_Parse(ip, p, &(New.IpSet)) != 0 )
    {
        return -1;
    }

    New.Type = Type;
    New.Data = NULL;

    if( Data != NULL )
    {
        New.Data = ic->Datas.Add(&(ic->Datas), Data, DataLength, TRUE);
    }

    if( IpSet_IsSingleIp(&(New.IpSet)) )
    {
        elm = ic->AddrChunk.Add(&(ic->AddrChunk), &New);
    } else {
        elm = ic->CidrChunk.Add(&(ic->CidrChunk), &New);
    }

    return elm == NULL;
}

BOOL IpChunk_Find(IpChunk *ic, unsigned char *Ip, int IpBytes, int *Type, const char **Data)
{
    IpElement   Key;
    const IpElement *Result = NULL;

    if( ic == NULL )
    {
        return FALSE;
    }

    switch( IpBytes )
    {
    case 4:
        IpAddr_From4(Ip, &(Key.IpSet.Ip));
        Key.IpSet.PrefixBits = 32;
        break;
    case 16:
        IpAddr_From6(Ip, &(Key.IpSet.Ip));
        Key.IpSet.PrefixBits = 128;
        break;
    default:
        return FALSE;
    }

    Key.Type = 0;
    Key.Data = NULL;

    Result = ic->AddrChunk.Search(&(ic->AddrChunk), &Key, NULL);
    if( Result == NULL )
    {
        Result = ic->CidrChunk.Search(&(ic->CidrChunk), &Key, NULL);
    }

    if( Result == NULL )
    {
        return FALSE;
    } else {
        if( Type != NULL )
        {
            *Type = Result->Type;
        }

        if( Data != NULL )
        {
            *Data = Result->Data;
        }

        return TRUE;
    }
}
