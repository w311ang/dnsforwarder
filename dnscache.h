#ifndef _DNS_CACHE_
#define _DNS_CACHE_

#include "dnsrelated.h"
#include "readconfig.h"
#include "iheader.h"

int DNSCache_Init(ConfigFileInfo *ConfigInfo);

BOOL Cache_IsInited(void);

int DNSCache_AddItemsToCache(MsgContext *MsgCtx, BOOL IsFirst);

int DNSCache_FetchFromCache(MsgContext *MsgCtx, int BufferLength);

#endif /* _DNS_CACHE_ */
