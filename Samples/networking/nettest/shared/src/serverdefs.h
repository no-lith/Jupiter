#ifndef _SERVERDEFS_H_
#define _SERVERDEFS_H_

#include <ltbasedefs.h>

typedef struct ServerInfo
{
    char Name[256];
    char IpAddress[MAX_HOSTIP_LEN];
}SERVERINFO;

#endif