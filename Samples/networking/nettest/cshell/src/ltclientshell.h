//------------------------------------------------------------------------------//
// 
// MODULE   : ltclientshell.h
// 
// PURPOSE  : CLTClientShell - Definition
// 
// CREATED  : 03/02/2002
// 
// (c) 2002 LithTech, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//


#ifndef __LT_CLIENT_SHELL_H__
#define __LT_CLIENT_SHELL_H__

// Engine includes

// Definition of IClientShell base class
#include <iclientshell.h>

#include "ClientFXMgr.h"
#include "gui.h"
#include "serverdefs.h"

class CWorldPropsClnt;
class CPlayerClnt;
class CCamera;
class CLTClientShell;
extern CLTClientShell *g_pCShell;

enum EGameType
{
	EGT_NORMAL,
	EGT_CONNECT,
    EGT_HOST
};

enum EGameMode
{
    LOCAL_GAMEMODE_NONE     = 0,
    LOCAL_GAMEMODE_NORMAL   = 1,
    LOCAL_GAMEMODE_HOST     = 2,
    LOCAL_GAMEMODE_CLIENT   = 3
};

//-----------------------------------------------------------------------------
class CLTClientShell : public IClientShellStub
{
public:
	
	CLTClientShell();
	~CLTClientShell();
	
	declare_interface(CLTClientShell);
	
	// Overridden functions, to receive callbacks from the engine
	//	These are all overridden here to make modification easier.
	//  Documentation exists in engine/sdk/inc/iclientshell.h in the
	//   IClientShell class
	LTRESULT	OnEngineInitialized(RMode *pMod, LTGUID *pAppGuid);
	void		OnEngineTerm();

	void		PreUpdate();
	void		Update();
	void		PostUpdate();

	void		PreLoadWorld(const char *pWorldName);
	void		OnEnterWorld();
	void		OnExitWorld();

	void		OnCommandOn(int command);
	void		OnCommandOff(int command);

	void		OnKeyDown(int key, int rep);
	void		OnKeyUp(int key);
	
	void		OnEvent(uint32 dwEventID, uint32 dwParam);
	void		OnMessage(ILTMessage_Read* pMessage);
	void		SpecialEffectNotify(HLOCALOBJ hObj, ILTMessage_Read *pMessage);

	LTRESULT	OnTouchNotify(HOBJECT hMain, CollisionInfo *pInfo, 
					float forceMag);

	void		OnObjectRemove(HLOCALOBJ hObj);
	LTRESULT	OnObjectMove(HLOCALOBJ hObj, bool bTeleport, LTVector *pNewPos);
	LTRESULT	OnObjectRotate(HLOCALOBJ hObj, bool bTeleport, LTRotation *pNewRot);
	
	void		OnPlaySound(PlaySoundInfo *pPlaySoundInfo);
	void		OnConsolePrint(CConsolePrintData *pData);
	void		OnModelKey(HLOCALOBJ hObj, ArgList *pArgs);
	
	LTRESULT	ProcessPacket(ILTMessage_Read *pMsg, uint8 senderAddr[4], 
					uint16 senderPort);
	
	void		OnLockRenderer();
	void		OnUnLockRenderer();
	
    //Multiplayer
    LTRESULT    StartMultiplayerServer();
    LTRESULT    JoinMultiplayerGame(char *ip);
    LTRESULT    StartNormalGame();
    bool        SetService();
	
    LTRESULT    CreateCamera();
    
	// Accessors
	void		GetPlayerStartInfo(LTVector &vStartPos, LTRotation &rStartRot);

	// Provides access to other client side class objects
	void		CreateWorldPropObject(); 
	void		DestroyWorldPropObject();

    //
	void		SendRotation(LTRotation &rRot);
	void		SendVelocityAndPos(LTVector &vVel, LTVector &vPos);
	void		SendVelPosAndRot(LTVector &vVel, LTVector &vPos, LTRotation &rRot);    
    uint        FindServers(SERVERINFO *pInfo);
	LTRESULT	Render();
private:

	LTRESULT	InitGame(EGameType eType, char* ip = NULL);    
	LTRESULT	PollInput();
	LTRESULT	InitRenderer(RMode *pMode);
	LTRESULT	InitSound();


    bool        IsInWorld(){return m_bInWorld;}
    void        SendPlayerName();

	//	Member variables
	CCamera*	m_pCamera;
	HLOCALOBJ	m_hCamera;
	bool		m_bRender;
	bool		m_bFirstUpdate;

	LTVector	m_vPlayerStartPos;
	LTRotation	m_rPlayerStartRot;

	float		m_fYaw;
	float		m_fPitch;
	float		m_fRoll;

    uint8       m_nGameMode;

    //used for movement
    float       m_fLastMoved;
    int         m_iTimesMoved;
	
	// Pointers to other client class objects
	CWorldPropsClnt *m_pWorldPropsClnt;
	CPlayerClnt		*m_pPlayer;

    bool            m_bInWorld;

    CClientFXMgr	m_ClientFXMgr;		// This handles the Client fx created with FxED
    CGui            m_Gui;
    
};


#endif // __LT_CLIENT_SHELL_H__
