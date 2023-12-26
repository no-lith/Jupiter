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
	
	// Construction / destruction
	CPlayerSrvr()  {}
	~CPlayerSrvr() {}
	
	// EngineMessageFn handlers
	uint32	EngineMessageFn(uint32 messageID, void *pData, float fData);
    uint32  ObjectMessageFn(HOBJECT hSender, ILTMessage_Read *pMsg);

    //
    void    SetPlayerName(const char* name);
    char*   GetPlayerName();

    void    PlayAnimation(const char* sAnimName, uint8 nTracker, bool bLooping);
	
private:
	
	uint32		PreCreate(void *pData, float fData);
	uint32		InitialUpdate(void *pData, float fData);
	void		ReadProps(ObjectCreateStruct* pStruct);
    void        CreateProjectile();

    char       m_sName[32];

    ANIMTRACKERID m_idUpperBodyTracker;
    ANIMTRACKERID m_idLowerBodyTracker;

    HMODELWEIGHTSET m_hWeightUpper;
    HMODELWEIGHTSET m_hWeightLower;

    HMODELSOCKET hRightHandSocket;
    
};


#endif // __PLAYERSRVR_H__
