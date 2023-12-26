#ifndef _SCOREDEFS_H_
#define _SCOREDEFS_H_

#include <ltbasedefs.h>

typedef struct ScoreStruct
{
    uint32 iClientID;
    char   sPlayerName[32];
    uint32 iScore;
    float  fMoney;
}SCORESTRUCT;


#endif //_SCOREDEFS_H_
