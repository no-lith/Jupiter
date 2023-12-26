//------------------------------------------------------------------------------//
//
// MODULE   : playersrvr.h
//
// PURPOSE  : CPlayerSrvr - Definition
//
// CREATED  : 07/15/2002
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#ifndef __PLAYERSRVR_H__
#define __PLAYERSRVR_H__


// Engine includes
#include <ltbasedefs.h>
#include <ltengineobjects.h>



//-----------------------------------------------------------------------------
class CPlayerSrvr : public BaseClass
{
public:

    CPlayerSrvr()
		: m_hClub(NULL),
    	  m_hClient(NULL),
    	  m_iScore(0),
    	  m_fMoney(0.0f),
		  m_DebugSphere(NULL),
          m_bSendStats(false),
          m_iSendStatsCounter(0)
    {
    }

	~CPlayerSrvr()
    {
        if(m_hClub)
        {
			g_pLTServer->RemoveObject(m_hClub);
			m_hClub = NULL;
        }
    }

	// EngineMessageFn handlers
	uint32				EngineMessageFn(uint32 messageID, void *pData, float fData);
    uint32  			ObjectMessageFn(HOBJECT hSender, ILTMessage_Read *pMsg);

    //
    void    			SetPlayerName(const char* name);
    char*   			GetPlayerName();

    void    			PlayAnimation(const char* sAnimName, uint8 nTracker, bool bLooping);
    void    			SetClient(HCLIENT hClient){ m_hClient = hClient; }
    void    			SetClubID();
    void                SetClientID(uint32 id){ m_iClientID = id; }
    uint32              GetClientID(){ return m_iClientID; }
    uint32              GetScore(){ return m_iScore; }
    void                SetSendStatsFlag(bool bSend)
                                        { 
                                            m_bSendStats = bSend; 
                                            m_iSendStatsCounter = 0;
                                        }
    float               GetMoney(){ return m_fMoney; }
    char*               GetName(){ return m_sName; }

private:

	uint32				PreCreate(void *pData, float fData);
	uint32				InitialUpdate(void *pData, float fData);
	void				ReadProps(ObjectCreateStruct* pStruct);
    void        		CreateProjectile();
    void        		CreateAttachment(HATTACHMENT &hAttachment, HOBJECT hChildObject, const char* sSocket,
                    	             	 LTVector &vRotOffset, LTVector &vPosOffset);
    void 				CheckForHit();
    void 				PlaySound(int i);

private:

    char                m_sName[32];

    ANIMTRACKERID       m_idUpperBodyTracker;
    ANIMTRACKERID       m_idLowerBodyTracker;

    HMODELWEIGHTSET     m_hWeightUpper;
    HMODELWEIGHTSET     m_hWeightLower;

    HMODELSOCKET        hRightHandSocket;

    HATTACHMENT         m_hClubAttach;
	HOBJECT             m_hClub;
	ModelOBB			m_WeaponOBB;

    HCLIENT             m_hClient;

    // Stats
    uint32              m_iScore;
    float               m_fMoney;
    uint32              m_iClientID;

    // Stat send toggle and delay counter
    bool                m_bSendStats;
    uint8               m_iSendStatsCounter;

	HOBJECT				m_DebugSphere;
};


#endif // __PLAYERSRVR_H__
