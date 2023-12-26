//------------------------------------------------------------------------------//
//
// MODULE	: StatsManager.cpp
//
// PURPOSE	: StatsManager - Implementation
//
// CREATED	: 07/17/2003
//
// (c) 2003 Touchdown Entertainment / LithTech, Inc.	All Rights Reserved
//
//------------------------------------------------------------------------------//

#include "statsmanager.h"
#include "playersrvr.h"
#include "scoredefs.h"

StatsManager::StatsManager():
m_iNumPlayers(0)
{
    ResetPlayerList();
}


StatsManager::~StatsManager()
{
    ResetPlayerList();
}


void StatsManager::RegisterPlayer(CPlayerSrvr *pPlayer)
{
    m_pPlayers.Append(pPlayer);
}

void StatsManager::RemovePlayer(CPlayerSrvr *_pPlayer)
{
    // Find the LinkedMember which contains the player that we want to remove.

    LinkedMember<CPlayerSrvr*> *pMember = m_pPlayers.First();

    for(int i = 0; pMember; i++)
    {
        // Get handle to the server player object data in the LinkedMember
        CPlayerSrvr *pPlayer = pMember->data;

        if(_pPlayer == pPlayer)
        {

            m_pPlayers.Remove(pMember);
            return;
        }

        // Next element
        pMember = pMember->next;
    }

}

void StatsManager::ResetPlayerList()
{
    // Zero out the player list.
    m_pPlayers.FreeStack();
}

uint32 StatsManager::GetNumPlayers()
{
    return m_pPlayers.GetSize();
}

LTRESULT StatsManager::GetPlayerScores(SCORESTRUCT *scores)
{

    LinkedMember<CPlayerSrvr*> *pMember = m_pPlayers.First();

    uint32 iSize = GetNumPlayers();
    for(int i = 0; iSize; i++)
    {
        if(!pMember)
        {
            break;
        }

        // Get handle to the server player object data in the LinkedMember
        CPlayerSrvr *pPlayer = pMember->data;

        // Set the struct info
        scores[i].iClientID = pPlayer->GetClientID();
        sprintf(scores[i].sPlayerName, "%s", pPlayer->GetPlayerName());
        scores[i].iScore    = pPlayer->GetScore();
        scores[i].fMoney    = pPlayer->GetMoney();

        // Next element
        pMember = pMember->next;
    }

    return LT_OK;
}
