//------------------------------------------------------------------------------//
//
// MODULE	: StatsManager.h
//
// PURPOSE	: StatsManager - Definition
//
// CREATED	: 07/17/2003
//
// (c) 2003 Touchdown Entertainment / LithTech, Inc.	All Rights Reserved
//
//------------------------------------------------------------------------------//

#ifndef _STATSMANAGER_H_
#define _STATSMANAGER_H_

#include <ltbasedefs.h>
#include "linkedlist.h"
#include "scoredefs.h"

//struct SCORESTRUCT;
class CPlayerSrvr;

class StatsManager
{
public:
    StatsManager();
    ~StatsManager();

    void            RegisterPlayer(CPlayerSrvr *pPlayer);
    void            RemovePlayer(CPlayerSrvr *_pPlayer);
    uint32          GetNumPlayers();
    LTRESULT        GetPlayerScores(SCORESTRUCT *scores);



private:

    void            ResetPlayerList();

    LinkedList<CPlayerSrvr*> m_pPlayers;
    //CPlayerSrvr*     m_pPlayers[16];
    uint32          m_iNumPlayers;
};

extern StatsManager* g_pStatsManager;

#endif // _STATSMANAGER_H_
