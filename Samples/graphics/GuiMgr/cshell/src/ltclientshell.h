//------------------------------------------------------------------------------//
//
// MODULE   : ltclientshell.h
//
// PURPOSE  : CLTClientShell - Definition
//
// CREATED  : 03/02/2002
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
//------------------------------------------------------------------------------//


#ifndef __LT_CLIENT_SHELL_H__
#define __LT_CLIENT_SHELL_H__


#include <windows.h>
#include <windowsx.h>
// Engine includes

// Definition of IClientShell base class
#include <iclientshell.h>
#include "commandids.h"

// Manager includes
#include "SampleScreenMgr.h"
#include "LayoutMgr.h"
#include "ScreenMgr.h"
#include "InterfaceResMgr.h"
#include "CursorMgr.h"

class CWorldPropsClnt;
class CPlayerClnt;
class CCamera;

class CLTClientShell;
extern CLTClientShell *g_pCShell;

extern CSampleScreenMgr *g_pScreenMgr;


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
	LTRESULT			OnEngineInitialized(RMode *pMod, LTGUID *pAppGuid);
	void				OnEngineTerm();

	void				PreUpdate();
	void				Update();
	void				PostUpdate();

	void				PreLoadWorld(const char *pWorldName);
	void				OnEnterWorld();
	void				OnExitWorld();

	void				OnCommandOn(int command);
	void				OnCommandOff(int command);

	void				OnKeyDown(int key, int rep);
	void				OnKeyUp(int key);

	void				OnEvent(uint32 dwEventID, uint32 dwParam);
	void				OnMessage(ILTMessage_Read* pMessage);
	void				SpecialEffectNotify(HLOCALOBJ hObj, ILTMessage_Read *pMessage);

	LTRESULT			OnTouchNotify(HOBJECT hMain, CollisionInfo *pInfo, float forceMag);

	void				OnObjectRemove(HLOCALOBJ hObj);
	LTRESULT			OnObjectMove(HLOCALOBJ hObj, bool bTeleport, LTVector *pNewPos);
	LTRESULT			OnObjectRotate(HLOCALOBJ hObj, bool bTeleport, LTRotation *pNewRot);

	void				OnPlaySound(PlaySoundInfo *pPlaySoundInfo);
	void				OnConsolePrint(CConsolePrintData *pData);
	void				OnModelKey(HLOCALOBJ hObj, ArgList *pArgs);

	LTRESULT			ProcessPacket(ILTMessage_Read *pMsg, uint8 senderAddr[4], uint16 senderPort);

	void				OnLockRenderer();
	void				OnUnLockRenderer();

	// Accessors
	void				GetPlayerStartInfo(LTVector &vStartPos, LTRotation &rStartRot);

	// Provides access to other client side class objects
	void				CreateWorldPropObject();
	void				DestroyWorldPropObject();

	// Access to managers
	CScreenMgr*		GetScreenMgr() { return &m_ScreenMgr; }
	CLayoutMgr*		GetLayoutMgr() { return &m_LayoutMgr; }


   static void OnChar(HWND hWnd, wchar_t c, int rep);
	static void OnLButtonUp(HWND hwnd, int x, int y, unsigned int keyFlags);
	static void OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, unsigned int keyFlags);
	static void OnLButtonDblClick(HWND hwnd, BOOL fDoubleClick, int x, int y, unsigned int keyFlags);
	static void OnRButtonUp(HWND hwnd, int x, int y, unsigned int keyFlags);
	static void OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, unsigned int keyFlags);
	static void OnRButtonDblClick(HWND hwnd, BOOL fDoubleClick, int x, int y, unsigned int keyFlags);
	static void OnMouseMove(HWND hwnd, int x, int y, unsigned int keyFlags);
	static BOOL OnSetCursor(HWND hwnd, HWND hwndCursor, UINT codeHitTest, unsigned int msg);

private:


//	LTRESULT			PollInput();
	void				SendRotation();
	void				SendPosition();
	void				SendPosAndRot();
	LTRESULT			InitRenderer(RMode *pMode);
	LTRESULT			InitSound();
	LTRESULT			InitGame();
	LTRESULT			Render();


private:

	// Member variables
	HLOCALOBJ		m_hPlayer;
	CCamera*			m_pCamera;
	HLOCALOBJ		m_hCamera;
	bool				m_bRender;
	bool				m_bFirstUpdate;

	LTVector			m_vPlayerStartPos;
	LTRotation		m_rPlayerStartRot;

	float				m_fYaw;
	float				m_fPitch;
	float				m_fRoll;

	// Pointers to other client class objects
	CWorldPropsClnt*	m_pWorldPropsClnt;
	CPlayerClnt*		m_pPlayer;


	// Managers

	CSampleScreenMgr	m_ScreenMgr;					// Screen manager
	
	CLayoutMgr			m_LayoutMgr;					// Manages layouts for screen UI etc

	CInterfaceResMgr	m_InterfaceResMgr;			// Manages shared textures and resources

	CCursorMgr			m_CursorMgr;					// Manages the cursor
};


#endif // __LT_CLIENT_SHELL_H__
