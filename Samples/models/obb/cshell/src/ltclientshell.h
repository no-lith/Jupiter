// ----------------------------------------------------------------------- //
//
// MODULE  : ltclientshell.h
//
// PURPOSE : LT Client Shell - Definition
//
// CREATED : 5/20/2002
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //


#ifndef __LT_CLIENT_SHELL_H__
#define __LT_CLIENT_SHELL_H__

#include <iclientshell.h>
#include <ltbasetypes.h>
#include <ltbasedefs.h>

#include "MsgIDs.h"
#include "CommandIDs.h"
#include "huditem.h"
#include "GuiMgr.h"
#include "BackgroundMgr.h"


// forwards:
class CLTClientShell;
class CCamera;


// Global pointer to the ClientShell
extern CLTClientShell*		g_pCShell;



class CLTClientShell : public IClientShellStub
{
public:

	CLTClientShell();
	~CLTClientShell();

	declare_interface(CLTClientShell);

	//Overridden LithTech Engine Calls
	virtual void			Update();
	virtual LTRESULT		OnEngineInitialized(RMode *pMod, LTGUID *pAppGuid);
	virtual void			OnEngineTerm();

	virtual void			OnEvent(uint32 dwEventID, uint32 dwParam);
	virtual void			OnMessage(ILTMessage_Read* pMessage);
	virtual void			SpecialEffectNotify(HLOCALOBJ hObj, ILTMessage_Read *pMessage);

	virtual void			OnEnterWorld();
	virtual void			OnExitWorld();

	virtual void			OnCommandOn(int command);
	virtual void			OnCommandOff(int command);



protected:

	LTRESULT   			PollInput();

	LTRESULT				InitRenderer(RMode *pMode);
	LTRESULT				InitGame();
	LTRESULT				Render();

	// sound
	LTRESULT 				InitSound();

	// camera
	LTRESULT				InitCamera();

protected:

	bool					m_bRender;
	bool					m_bFirstUpdate;

	// camera
	HLOCALOBJ				m_hCamera;

	// player
	HLOCALOBJ   			m_hPlayer;		// Player object from GetClientObject()
	LTVector				m_vPlayerStartPos;
	LTRotation  			m_rPlayerStartRot;
	float					m_fYaw;
	float					m_fPitch;
	float					m_fRoll;

	char					m_HitText[255];
	HudItem					m_Crosshair;

	// managers
	GuiMgr					m_GuiMgr;
	BackgroundMgr			m_BackgroundMgr;
};



#endif // __LT_CLIENT_SHELL_H__
