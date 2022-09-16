#ifndef EXCLUDEDLIST_H_INCLUDED
#define EXCLUDEDLIST_H_INCLUDED

#include "stringlist.h"
#include "stringchunk.h"
#include "readconfig.h"
#include "iheader.h"

int Filter_Init(ConfigFileInfo *ConfigInfo);

int Filter_Update(void);

BOOL Filter_Out(MsgContext *MsgCtx);

#endif // EXCLUDEDLIST_H_INCLUDED
