#ifndef TCPFRONTEND_H_INCLUDED
#define TCPFRONTEND_H_INCLUDED

#include "readconfig.h"

void TcpFrontend_StartWork(void);

int TcpFrontend_Init(ConfigFileInfo *ConfigInfo, BOOL StartWork);

#endif /* TCPFRONTEND_H_INCLUDED */
