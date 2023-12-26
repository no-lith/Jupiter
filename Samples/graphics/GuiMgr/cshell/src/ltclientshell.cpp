//------------------------------------------------------------------------------//
//
// MODULE   : ltclientshell.cpp
//
// PURPOSE  : CLTClientShell - Implementation
//
// CREATED  : 2002/03/02
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
//------------------------------------------------------------------------------//

#include "ltclientshell.h"

#include "camera.h"
#include "clientinterfaces.h"
#include "msgids.h"
#include "commandids.h"
#include "worldpropsclient.h"
#include "playerclnt.h"
#include "clienthelper.h"


// Engine Includes
#include <iltclient.h>
#include <iltphysics.h>
#include <iltsoundmgr.h>
#include <iltcommon.h>
#include <iltmodel.h>
#include <lteulerangles.h>
#include <ltobjectcreate.h>
#include <LTFontParams.h>
#include <ilttexturestring.h>

#include <stdio.h>
#include <time.h>




//-----------------------------------------------------------------------------
//	This macro defines certain DLL exports and variables
SETUP_CLIENTSHELL();

CLTClientShell* g_pCShell = NULL;



// Define the instantiation of the ClientShell interface.
define_interface(CLTClientShell, IClientShell);


//-----------------------------------------------------------------------------
// procedures and globals used to hook windows
HWND				g_hMainWnd = NULL;

#ifdef STRICT
	WNDPROC g_pfnMainWndProc = NULL;
#else
	FARPROC	g_pfnMainWndProc = NULL;
#endif

BOOL HookWindow();
void UnhookWindow();
BOOL SetWindowSize(uint32 nWidth, uint32 nHeight);
LRESULT CALLBACK HookedWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// we need this for window hooks crackers
CSampleScreenMgr *g_pScreenMgr;


//------------------------------------------------------------------------------
//	CLTClientShell::CLTClientShell()
//
//------------------------------------------------------------------------------
CLTClientShell::CLTClientShell()
	: m_hCamera(NULL),
	  m_bRender(true),
	  m_bFirstUpdate(true),
	  m_vPlayerStartPos(0.0f, 160.0f, 0.0f),
	  m_pWorldPropsClnt(NULL),
	  m_pPlayer(NULL)
{
	g_pCShell= this;

	g_pScreenMgr = &m_ScreenMgr;

	m_rPlayerStartRot.Init();
}



//------------------------------------------------------------------------------
//	CLTClientShell::~CLTClientShell()
//
//------------------------------------------------------------------------------
CLTClientShell::~CLTClientShell()
{
}



//------------------------------------------------------------------------------
//  CLTClientShell::OnEngineInitialized(RMode *pMode, LTGUID *pAppGuid)
//
//------------------------------------------------------------------------------
LTRESULT CLTClientShell::OnEngineInitialized(RMode *pMode, LTGUID *pAppGuid)
{

	if (!g_hMainWnd)
	{
		HookWindow();
		SetWindowSize(pMode->m_Width, pMode->m_Height);
	}


	LTRESULT result = LT_OK;

	result = VerifyClientInterfaces();
	if (ShutdownIfFailed(result, "Error in VerifyClientInterfaces(). Shutting down.\n"))
	{
		return result;
	}

	result = InitRenderer(pMode);
	if (ShutdownIfFailed(result, "Error in InitRenderer(). Shutting down.\n"))
	{
		return result;
	}

	result = InitSound();
	DebugPrintIfFailed(result, "Error: Couldn't intialize sound system.\n");

	result = InitGame();
	if (ShutdownIfFailed(result, "Error in InitGame(). Shutting down.\n"))
	{
		return result;
	}


	// Instantiate the camera class
	m_pCamera = new CCamera;

	if( !m_pCamera )
		return LT_ERROR;

	// Create the CAMERA
	result = m_pCamera->CreateCamera();
	if (ShutdownIfFailed(result, "Error in CreateCamera(). Shutting down.\n"))
	{
		return result;
	}

	m_hCamera = m_pCamera->GetCamera();

	this->CreateWorldPropObject();

	// Initialize the database for the screen etc
	m_LayoutMgr.Init();

	// Initialize the resource manager
	m_InterfaceResMgr.Init();

	// Create the screen mgr
	m_ScreenMgr.Init();

	// Create the cursor mgr
	m_CursorMgr.Init();

	return result;
}



//---------------------------------------------------------------------------
//  CLTClientShell::InitRenderer(RMode *pMode)
//
//---------------------------------------------------------------------------
LTRESULT CLTClientShell::InitRenderer(RMode *pMode)
{
	//	g_pLTClient->CPrint("CLTClientShell::InitRenderer");

	//	Attempt to set the render mode based on pMode.
	//		pMode contains command line parameters or
	//		console vars included in autoexec.cfg. Such as:
	//
	//		screenwidth 800
	//		screenheight 600
	//		bitdepth 32
	//
	//		Or on the command line:
	//
	//		lithtech.exe +screenwidth 800 +screenheight 600 +bitdepth 32

	if (g_pLTClient->SetRenderMode(pMode) != LT_OK)
	{
		g_pLTClient->DebugOut("Error: Couldn't set requested render mode!\n");

		// Try 640x480x16 as a fallback.

		RMode newRMode;
		memset(&newRMode, 0, sizeof(newRMode));

		newRMode.m_Width		= 640;
		newRMode.m_Height		= 480;
		newRMode.m_BitDepth		= 16;

		newRMode.m_bHWTnL   = pMode->m_bHWTnL;
		sprintf(newRMode.m_InternalName, "%s", pMode->m_InternalName);
		sprintf(newRMode.m_Description, "%s", pMode->m_Description);

		// Give it another try.
		g_pLTClient->DebugOut("Attempting to set render mode to 640x480x16...\n");

		if (g_pLTClient->SetRenderMode(&newRMode) != LT_OK)
		{
			// Still didn't work.
			g_pLTClient->DebugOut(
				"Error: Couldn't set to 640x480x16 mode. Shutting down.\n");
			g_pLTClient->ShutdownWithMessage(
				"Error: Couldn't set to 640x480x16 mode. Shutting down.\n");

			return LT_ERROR;
		}
	}

	return LT_OK;
}



//---------------------------------------------------------------------------
//  CLTClientShell::InitSound()
//
//---------------------------------------------------------------------------
LTRESULT CLTClientShell::InitSound()
{
	//	g_pLTClient->CPrint("CLTClientShell::InitSound");

	InitSoundInfo sndInfo;

	sndInfo.Init();

    // Initialize sound with defaults.
	return g_pLTCSoundMgr->InitSound(&sndInfo);
}



//---------------------------------------------------------------------------
//  CLTClientShell::InitGame()
//
//---------------------------------------------------------------------------
LTRESULT CLTClientShell::InitGame()
{
	LTRESULT result;
	const char* pLevelName = NULL;
	StartGameRequest request;
	memset(&request, 0, sizeof(StartGameRequest));

	request.m_Type = STARTGAME_NORMAL;

	HCONSOLEVAR hVar = g_pLTClient->GetConsoleVar("runworld");
	pLevelName = g_pLTClient->GetVarValueString(hVar);

	if (pLevelName)
	{
		// (length includes the NULL character)
		LTStrCpy(request.m_WorldName, pLevelName, strlen(pLevelName)+1);
	}
	else
	{
		strcpy(request.m_WorldName, "worlds/demoroom");
	}

	//request.m_HostInfo.m_dwMaxPlayers = 1;

	result = g_pLTClient->StartGame(&request);
	if (LT_OK != result)
	{
		DebugPrintIfFailed(result, "Start Game Request failed.\n");
		return result;
	}

	return result;
}


//---------------------------------------------------------------------------
//  CLTClientShell::OnEnterWorld()
//
//---------------------------------------------------------------------------
void CLTClientShell::OnEnterWorld()
{
	// Clear the input buffer for a clean start
	g_pLTClient->ClearInput();
}



//---------------------------------------------------------------------------
//  CLTClientShell::OnExitWorld()
//
//---------------------------------------------------------------------------
void CLTClientShell::OnExitWorld()
{
	if (m_pPlayer)
	{
		delete m_pPlayer;
		m_pPlayer = NULL;
	}
}



//---------------------------------------------------------------------------
//  CLTClientShell::OnMessage(ILTMessage_Read* pMessage)
//
//---------------------------------------------------------------------------
void CLTClientShell::OnMessage(ILTMessage_Read* pMessage)
{
	pMessage->SeekTo(0);
	uint8 messageID = pMessage->Readuint8();

	switch (messageID)
	{
	case MSG_STARTPOINT_POS_ROT:
		{
			m_vPlayerStartPos = pMessage->ReadLTVector();
			m_rPlayerStartRot = pMessage->ReadLTRotation();
		}
		break;

	case MSG_WORLD_PROPS:
		{
			m_pWorldPropsClnt->UnpackWorldProps(pMessage);
		}
		break;

	default:
		break;
	}

}



//---------------------------------------------------------------------------
//  CLTClientShell::Update()
//
//---------------------------------------------------------------------------
void CLTClientShell::Update()
{

	LTVector vDims;
	LTVector vPos;
	LTRotation rRot;

	if( m_bFirstUpdate )
	{
		// Make sure our rotation is setup correctly.
		// We convert from a Quat to Euler angles because
		// our input rotation scheme is setup using Euler angles
		EulerAngles euler = Eul_FromQuat(m_rPlayerStartRot, 0);
		m_fPitch = 0.0f;
		m_fYaw = euler.z; //all we care about is the yaw
		m_fRoll = 0.0f;
		g_pLTCCommon->SetupEuler(rRot, m_fPitch, m_fYaw, m_fRoll);

		m_hPlayer = g_pLTClient->GetClientObject();
		vDims.x = vDims.y = vDims.z = 10.0f;
		g_pLTCPhysics->SetObjectDims(m_hPlayer, &vDims, 0);
		g_pLTCPhysics->SetGlobalForce(LTVector(0.0f, -1000.0f, 0.0f));
		g_pLTCPhysics->SetMass(m_hPlayer, 30.0f);
		g_pLTCCommon->SetObjectFlags(m_hPlayer, OFT_Flags, 0, FLAG_GRAVITY);

		m_bFirstUpdate = false;

		// turn on the cursor
		m_CursorMgr.UseCursor(LTTRUE);

		// switch to the main screen
		m_ScreenMgr.SetCurrentScreen(SCREEN_ID_MAIN);
	}

	g_pLTClient->GetObjectPos(m_hPlayer, &vPos);
	g_pLTClient->GetObjectRotation(m_hPlayer, &rRot);
	g_pLTClient->SetObjectPosAndRotation(m_hCamera, &vPos, &rRot);

//	PollInput();	// Poll for user input

	if( m_bRender )
	{

		if( Render() != LT_OK )
		{
			g_pLTClient->DebugOut("Error in Render()\n");
			g_pLTClient->Shutdown();
		}
	}

	// Update the world properties class
	if( m_pWorldPropsClnt )
		m_pWorldPropsClnt->Update();
}



//---------------------------------------------------------------------------
//  CLTClientShell::Render()
//
//---------------------------------------------------------------------------
LTRESULT CLTClientShell::Render()
{
	LTRESULT result;

    float fFrameTime = g_pLTClient->GetFrameTime();

	// Clear the screen to prepare for the next draw
	result = g_pLTClient->ClearScreen(NULL, CLEARSCREEN_SCREEN | CLEARSCREEN_RENDER, NULL);
	
	if (LT_OK != result)
	{
		g_pLTClient->DebugOut("Error in ClearScreen()\n");
		g_pLTClient->Shutdown();
	}

	// Tell LithTech we are going to be starting our 3d render
	result = g_pLTClient->Start3D();
	if (LT_OK != result)
	{
		g_pLTClient->DebugOut("Error in Start3D()\n");
		g_pLTClient->Shutdown();
	}

	// Tell LithTech to render the scene using this camera
	result = g_pLTClient->RenderCamera(m_hCamera, fFrameTime);
	if (LT_OK != result)
	{
		g_pLTClient->DebugOut("Error in RenderCamera()\n");
		g_pLTClient->Shutdown();
	}

	g_pLTClient->StartOptimized2D();


	// Render screen mgr
	m_ScreenMgr.Render();

	// Render the cursor
	m_CursorMgr.Render();

	g_pLTClient->EndOptimized2D();

	//	Tell LithTech to finish rendering for this scene
	result = g_pLTClient->End3D(END3D_CANDRAWCONSOLE);
	if (LT_OK != result)
	{
		g_pLTClient->DebugOut("Error in End3D()\n");
		g_pLTClient->Shutdown();
	}

	//	Tell LithTech to flip the screen surface
	result = g_pLTClient->FlipScreen(NULL);
	if (LT_OK != result)
	{
		g_pLTClient->DebugOut("Error in FlipScreen()\n");
		g_pLTClient->Shutdown();
	}


	return LT_OK;
}



//---------------------------------------------------------------------------
//	CLTClientShell::OnEvent(uint32 dwEventID, uint32 dwParam)
//
//---------------------------------------------------------------------------
void CLTClientShell::OnEvent(uint32 dwEventID, uint32 dwParam)
{
	if (dwEventID == LTEVENT_LOSTFOCUS ||
		dwEventID == LTEVENT_RENDERTERM)
		m_bRender = false;
	if (dwEventID == LTEVENT_GAINEDFOCUS ||
		dwEventID == LTEVENT_RENDERINIT ||
		dwEventID == LTEVENT_RENDERALMOSTINITTED)
		m_bRender = true;
}



//---------------------------------------------------------------------------
//  CLTClientShell::OnCommandOn(int command)
//
//---------------------------------------------------------------------------
void CLTClientShell::OnCommandOn(int command)
{
//	switch (command)
//	{
//		case COMMAND_QUIT:
//			g_pLTClient->Shutdown();
//			break;
//		default:
//			break;
//	}
}



//---------------------------------------------------------------------------
//  CLTClientShell::OnCommandOff(int command)
//
//---------------------------------------------------------------------------
void CLTClientShell::OnCommandOff(int command)
{
}


/*
//---------------------------------------------------------------------------
//  CLTClientShell::PollInput()
//
//---------------------------------------------------------------------------
LTRESULT CLTClientShell::PollInput()
{
	float offsets[3];
	LTVector vPos, vUp, vRight, vForward, vTemp;
	LTRotation rRot, rTemp;
	float fFrameTime;
	bool bMoved = false;
	bool bRotated = false;


	fFrameTime = g_pLTClient->GetFrameTime();

	// Get player info
	g_pLTClient->GetObjectPos(m_hPlayer, &vPos);
	g_pLTClient->GetObjectRotation(m_hPlayer, &rRot);
	g_pLTCCommon->GetRotationVectors(rRot, vUp, vRight, vForward);

	g_pLTClient->GetAxisOffsets(offsets);

	if ((offsets[0] != 0.0f) ||
		(offsets[1] != 0.0f) ||
		(offsets[2] != 0.0f))
	{
		bRotated = true;
		m_fYaw += (offsets[0]);
		m_fPitch += (offsets[1]);
		m_fRoll += (offsets[2]);

	}

	if (g_pLTClient->IsCommandOn(COMMAND_MOVE_LEFT))
	{
		vTemp = vRight * fFrameTime * -5000.0f;
		vPos += vTemp;
		bMoved = true;
	}

	if (g_pLTClient->IsCommandOn(COMMAND_MOVE_RIGHT))
	{
		vTemp = vRight * fFrameTime * 5000.0f;
		vPos += vTemp;
		bMoved = true;
	}

	if ( g_pLTClient->IsCommandOn(COMMAND_MOVE_FORWARD))
	{
		vTemp = vForward * fFrameTime * 5000.0f;
		vPos += vTemp;
		bMoved = true;
	}

	if (g_pLTClient->IsCommandOn(COMMAND_MOVE_BACKWARD))
	{
		vTemp = vForward * fFrameTime * -5000.0f;
		vPos += vTemp;
		bMoved = true;
	}

	if (bMoved)
	{
		g_pLTCPhysics->MoveObject(m_hPlayer, &vPos, 0);
	}

	if (bRotated)
	{
		g_pLTCCommon->SetupEuler(rRot, m_fPitch, m_fYaw, m_fRoll);
		g_pLTClient->SetObjectRotation(m_hPlayer, &rRot);
		g_pLTClient->SetObjectRotation(m_hCamera, &rRot);
	}


	if (bMoved && bRotated)
		SendPosAndRot();
	else if (bMoved)
		SendPosition();
	else if (bRotated)
		SendRotation();

	return LT_OK;
}

*/

//-----------------------------------------------------------------------------
//  CLTClientShell::SendRotation()
//
//-----------------------------------------------------------------------------
void CLTClientShell::SendRotation()
{
	LTRotation rRot;
	g_pLTClient->GetObjectRotation(m_hPlayer, &rRot);

	ILTMessage_Write *pMessage;
	LTRESULT nResult = g_pLTCCommon->CreateMessage(pMessage);

	if( LT_OK == nResult)
	{
		pMessage->IncRef();
		pMessage->Writeuint8(MSG_CS_ROTATION);
		pMessage->WriteLTRotation(rRot);
		g_pLTClient->SendToServer(pMessage->Read(), 0);
		pMessage->DecRef();
	}
}



//-----------------------------------------------------------------------------
//  CLTClientShell::SendPosition()
//
//-----------------------------------------------------------------------------
void CLTClientShell::SendPosition()
{

	LTVector vPos;
	g_pLTClient->GetObjectPos(m_hPlayer, &vPos);

	ILTMessage_Write *pMessage;
	LTRESULT nResult = g_pLTCCommon->CreateMessage(pMessage);

	if( LT_OK == nResult)
	{
		pMessage->IncRef();
		pMessage->Writeuint8(MSG_CS_POSITION);
		pMessage->WriteLTVector(vPos);
		g_pLTClient->SendToServer(pMessage->Read(), 0);
		pMessage->DecRef();
	}

}



//-----------------------------------------------------------------------------
//  CLTClientShell::SendPosAndRot()
//
//-----------------------------------------------------------------------------
void CLTClientShell::SendPosAndRot()
{

	LTVector vPos;
	g_pLTClient->GetObjectPos(m_hPlayer, &vPos);

	LTRotation rRot;
	g_pLTClient->GetObjectRotation(m_hPlayer, &rRot);

	ILTMessage_Write *pMessage;
	LTRESULT nResult = g_pLTCCommon->CreateMessage(pMessage);

	if( LT_OK == nResult)
	{
		pMessage->IncRef();
		pMessage->Writeuint8(MSG_CS_POSANDROT);
		pMessage->WriteLTVector(vPos);
		pMessage->WriteLTRotation(rRot);
		g_pLTClient->SendToServer(pMessage->Read(), 0);
		pMessage->DecRef();
	}

}



//-----------------------------------------------------------------------------
//  CLTClientShell::GetPlayerStartInfo(LTVector &vStartPos,
//                                     LTRotation &rStartRot)
//
//-----------------------------------------------------------------------------
void CLTClientShell::GetPlayerStartInfo(LTVector &vStartPos, LTRotation &rStartRot)
{
	vStartPos = m_vPlayerStartPos;
	rStartRot = m_rPlayerStartRot;
}



//-----------------------------------------------------------------------------
//  CLTClientShell::CreateWorldPropObject()
//
//-----------------------------------------------------------------------------
void CLTClientShell::CreateWorldPropObject()
{
	m_pWorldPropsClnt = new CWorldPropsClnt;
}



//-----------------------------------------------------------------------------
//	CLTClientShell::DestroyWorldPropObject()
//
//-----------------------------------------------------------------------------
void CLTClientShell::DestroyWorldPropObject()
{
	if( m_pWorldPropsClnt )
	{
		delete m_pWorldPropsClnt;
		m_pWorldPropsClnt = NULL;
	}
}



//-----------------------------------------------------------------------------
//	CLTClientShell::OnLockRenderer()
//
//-----------------------------------------------------------------------------
void CLTClientShell::OnLockRenderer()
{
	//g_pLTClient->CPrint("OnLockRenderer()");
}



//-----------------------------------------------------------------------------
//  CLTClientShell::OnUnLockRenderer()
//
//-----------------------------------------------------------------------------
void CLTClientShell::OnUnLockRenderer()
{
	//g_pLTClient->CPrint("OnUnLockRenderer()");
}



//-----------------------------------------------------------------------------
//  CLTClientShell::OnKeyDown(int key, int rep)
//
//-----------------------------------------------------------------------------
void CLTClientShell::OnKeyDown(int key, int rep)
{
	g_pScreenMgr->HandleKeyDown(key,rep);

	//g_pLTClient->CPrint("OnKeyDown(%d,%d)", key, rep);
}



//-----------------------------------------------------------------------------
//  CLTClientShell::OnKeyUp(int key)
//
//-----------------------------------------------------------------------------
void CLTClientShell::OnKeyUp(int key)
{
	g_pScreenMgr->HandleKeyUp(key);
	//g_pLTClient->CPrint("OnKeyUp(%d)", key);
}



//-----------------------------------------------------------------------------
//  CLTClientShell::SpecialEffectNotify(HLOCALOBJ hObj, ILTMessage_Read *pMessage)
//
//-----------------------------------------------------------------------------
void CLTClientShell::SpecialEffectNotify(HLOCALOBJ hObj, ILTMessage_Read *pMessage)
{
	g_pLTClient->CPrint("SpecialEffectNotify(%p,%p)", hObj, pMessage);

	pMessage->SeekTo(0);
	uint8 messageID = pMessage->Readuint8();

	if (MSG_PLAYEROBJ == messageID)
	{
		m_pPlayer = new CPlayerClnt;
		m_pPlayer->SetHObject(hObj);
		m_pPlayer->SpecialEffectMessage(pMessage);
	}
}



//-----------------------------------------------------------------------------
//  CLTClientShell::OnTouchNotify(HOBJECT hMain, CollisionInfo *pInfo,
//					float forceMag)
//
//-----------------------------------------------------------------------------
LTRESULT CLTClientShell::OnTouchNotify(HOBJECT hMain, CollisionInfo *pInfo,
							float forceMag)
{
	//g_pLTClient->CPrint("OnTouchNotify(%p,%p,%g)", hMain, pInfo, forceMag);

	return LT_OK;
}



//-----------------------------------------------------------------------------
//  CLTClientShell::OnObjectRemove(HLOCALOBJ hObj)
//
//-----------------------------------------------------------------------------
void CLTClientShell::OnObjectRemove(HLOCALOBJ hObj)
{
	//g_pLTClient->CPrint("OnObjectRemove(%p)", hObj);
}



//-----------------------------------------------------------------------------
//  CLTClientShell::OnObjectMove(HLOCALOBJ hObj, bool bTeleport, LTVector *pNewPos)
//
//-----------------------------------------------------------------------------
LTRESULT CLTClientShell::OnObjectMove(HLOCALOBJ hObj, bool bTeleport, LTVector *pNewPos)
{
	//g_pLTClient->CPrint("OnObjectMove(%p,%b,%p)", hObj, bTeleport, pNewPos);

	return LT_OK;
}



//-----------------------------------------------------------------------------
//  CLTClientShell::OnObjectRotate(HLOCALOBJ hObj, bool bTeleport, LTRotation *pNewRot)
//
//-----------------------------------------------------------------------------
LTRESULT CLTClientShell::OnObjectRotate(HLOCALOBJ hObj, bool bTeleport, LTRotation *pNewRot)
{
	//g_pLTClient->CPrint("OnObjectRotate(%p,%d,%p)", hObj, bTeleport, pNewRot);

	return LT_OK;
}



//-----------------------------------------------------------------------------
//  CLTClientShell::OnPlaySound(PlaySoundInfo *pPlaySoundInfo)
//
//-----------------------------------------------------------------------------
void CLTClientShell::OnPlaySound(PlaySoundInfo *pPlaySoundInfo)
{
	//g_pLTClient->CPrint("OnPlaySound(%p)", pPlaySoundInfo);
}



//-----------------------------------------------------------------------------
//  CLTClientShell::OnConsolePrint(CConsolePrintData *pData)
//
//-----------------------------------------------------------------------------
void CLTClientShell::OnConsolePrint(CConsolePrintData *pData)
{
	//g_pLTClient->CPrint("OnConsolePrint(%p)",pData);
}



//-----------------------------------------------------------------------------
//  CLTClientShell::OnModelKey(HLOCALOBJ hObj, ArgList *pArgs)
//
//-----------------------------------------------------------------------------
void CLTClientShell::OnModelKey(HLOCALOBJ hObj, ArgList *pArgs)
{
	//g_pLTClient->CPrint("OnModelKey(%p, %p)", hObj, pArgs);
}



//-----------------------------------------------------------------------------
//  CLTClientShell::ProcessPacket(ILTMessage_Read *pMsg, uint8 senderAddr[4],
//								uint16 senderPort)
//
//-----------------------------------------------------------------------------
LTRESULT CLTClientShell::ProcessPacket(ILTMessage_Read *pMsg, uint8 senderAddr[4],
									uint16 senderPort)
{
	//g_pLTClient->CPrint("ProcessPacket(%p,%p,%d)", pMsg, senderAddr, senderPort);

	return LT_OK;
}



//-----------------------------------------------------------------------------
//  CLTClientShell::OnEngineTerm()
//
//-----------------------------------------------------------------------------
void CLTClientShell::OnEngineTerm()
{
	//g_pLTClient->CPrint("OnEngineTerm()");

    UnhookWindow();

	// Free memory
	if( m_hCamera )
	{
		g_pLTClient->RemoveObject(m_hCamera);
		delete m_pCamera;
		m_pCamera = NULL;
	}
	if( m_hPlayer )
	{
		g_pLTClient->RemoveObject(m_hPlayer);
	}

	this->DestroyWorldPropObject();


	m_CursorMgr.Term();

	m_ScreenMgr.Term();

	m_InterfaceResMgr.Term();

	m_LayoutMgr.Term();

}



//-----------------------------------------------------------------------------
//  CLTClientShell::PreUpdate()
//
//-----------------------------------------------------------------------------
void CLTClientShell::PreUpdate()
{
	//g_pLTClient->CPrint("PreUpdate()");
}



//-----------------------------------------------------------------------------
//  CLTClientShell::PostUpdate()
//
//-----------------------------------------------------------------------------
void CLTClientShell::PostUpdate()
{
	//g_pLTClient->CPrint("PostUpdate()");
}



//-----------------------------------------------------------------------------
//  CLTClientShell::PreLoadWorld(const char *pWorldName)
//
//-----------------------------------------------------------------------------
void CLTClientShell::PreLoadWorld(const char *pWorldName)
{
	//g_pLTClient->CPrint("PreLoadWorld(%p)", pWorldName);
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	HookedWindowProc
//
//	PURPOSE:	Hook it real good
//
// --------------------------------------------------------------------------- //

LRESULT CALLBACK HookedWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)
	{
		HANDLE_MSG(hWnd, WM_LBUTTONUP, CLTClientShell::OnLButtonUp);
		HANDLE_MSG(hWnd, WM_LBUTTONDOWN, CLTClientShell::OnLButtonDown);
		HANDLE_MSG(hWnd, WM_LBUTTONDBLCLK, CLTClientShell::OnLButtonDblClick);
		HANDLE_MSG(hWnd, WM_RBUTTONUP, CLTClientShell::OnRButtonUp);
		HANDLE_MSG(hWnd, WM_RBUTTONDOWN, CLTClientShell::OnRButtonDown);
		HANDLE_MSG(hWnd, WM_RBUTTONDBLCLK, CLTClientShell::OnRButtonDblClick);
		HANDLE_MSG(hWnd, WM_MOUSEMOVE, CLTClientShell::OnMouseMove);
		HANDLE_MSG(hWnd, WM_CHAR, CLTClientShell::OnChar);
		HANDLE_MSG(hWnd, WM_SETCURSOR, CLTClientShell::OnSetCursor);
	}
	assert(g_pfnMainWndProc);
	return(CallWindowProc(g_pfnMainWndProc,hWnd,uMsg,wParam,lParam));
}

void  CLTClientShell::OnChar(HWND hWnd, wchar_t c, int rep)
{
	g_pScreenMgr->HandleChar(c);
}


void  CLTClientShell::OnLButtonUp(HWND hWnd, int x, int y, unsigned int keyFlags)
{
	g_pScreenMgr->OnLButtonUp(x,y);
}

void  CLTClientShell::OnLButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, unsigned int keyFlags)
{
	/*if (!g_tmrDblClick.Stopped() &&
		(g_mouseMgr.GetClickPosX() == x) && (g_mouseMgr.GetClickPosY() == y))
	{
		g_tmrDblClick.Stop();
		OnLButtonDblClick(hwnd,fDoubleClick,x,y,keyFlags);
	}
	else
	{
		g_mouseMgr.SetClickPos(x,y);
		g_tmrDblClick.Start(.5);
	}*/

	g_pScreenMgr->OnLButtonDown(x,y);
}

void  CLTClientShell::OnLButtonDblClick(HWND hwnd, BOOL fDoubleClick, int x, int y, unsigned int keyFlags)
{
	g_pScreenMgr->OnLButtonDblClick(x,y);
}

void  CLTClientShell::OnRButtonUp(HWND hwnd, int x, int y, unsigned int keyFlags)
{
	g_pScreenMgr->OnRButtonUp(x,y);
}

void  CLTClientShell::OnRButtonDown(HWND hwnd, BOOL fDoubleClick, int x, int y, unsigned int keyFlags)
{
	g_pScreenMgr->OnRButtonDown(x,y);
}

void  CLTClientShell::OnRButtonDblClick(HWND hwnd, BOOL fDoubleClick, int x, int y, unsigned int keyFlags)
{
	g_pScreenMgr->OnRButtonDblClick(x,y);
}

void CLTClientShell::OnMouseMove(HWND hwnd, int x, int y, unsigned int keyFlags)
{
	g_pCursorMgr->SetCursorPos(x,y);

	g_pScreenMgr->OnMouseMove(x,y);
}

BOOL CLTClientShell::OnSetCursor(HWND hwnd, HWND hwndCursor, UINT codeHitTest, unsigned int msg)
{
	return TRUE;
}



// --------------------------------------------------------------------------- //
//
//	ROUTINE:	HookWindow
//
//	PURPOSE:	HOOK IT!
//
// --------------------------------------------------------------------------- //

BOOL HookWindow()
{
	// Hook the window
   if(g_pLTClient->GetEngineHook("HWND",(void **)&g_hMainWnd) != LT_OK)
	{
		return FALSE;
	}

	// Get the window procedure
#ifdef STRICT
	g_pfnMainWndProc = (WNDPROC)GetWindowLong(g_hMainWnd,GWL_WNDPROC);
#else
	g_pfnMainWndProc = (FARPROC)GetWindowLong(g_hMainWnd,GWL_WNDPROC);
#endif

	if(!g_pfnMainWndProc)
	{
		return FALSE;
	}

	// Replace it with ours
	if(!SetWindowLong(g_hMainWnd,GWL_WNDPROC,(LONG)HookedWindowProc))
	{
		return FALSE;
	}

	return TRUE;
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	UnhookWindow
//
//	PURPOSE:	Unhook the window
//
// --------------------------------------------------------------------------- //

void UnhookWindow()
{
	if(g_pfnMainWndProc && g_hMainWnd)
	{
		SetWindowLong(g_hMainWnd, GWL_WNDPROC, (LONG)g_pfnMainWndProc);
		g_hMainWnd = 0;
		g_pfnMainWndProc = NULL;
	}
}


// --------------------------------------------------------------------------- //
//
//	ROUTINE:	SetWindowSize
//
//	PURPOSE:	This will resize the window to the specified dimensions and clip the cursor if necessary
//
// --------------------------------------------------------------------------- //

BOOL SetWindowSize(uint32 nWidth, uint32 nHeight)
{
	// Clip the cursor if we're NOT in a window
   HCONSOLEVAR hVar = g_pLTClient->GetConsoleVar("Windowed");
	BOOL bClip = TRUE;

	if(hVar)
	{
      float fVal = g_pLTClient->GetVarValueFloat(hVar);
		if(fVal == 1.0f)
			bClip = FALSE;
	}

	RECT screenRect;
	GetWindowRect(GetDesktopWindow(), &screenRect);


	if(bClip)
	{
  		SetWindowLong(g_hMainWnd,GWL_STYLE,WS_VISIBLE);
		SetWindowPos(g_hMainWnd,HWND_TOPMOST,0,0,screenRect.right - screenRect.left,screenRect.bottom - screenRect.top,SWP_FRAMECHANGED);

		RECT wndRect;
		GetWindowRect(g_hMainWnd, &wndRect);
		ClipCursor(&wndRect);
	}
	else
	{
		SetWindowPos(g_hMainWnd,HWND_NOTOPMOST,
					((screenRect.right - screenRect.left) - nWidth) / 2,
					((screenRect.bottom - screenRect.top) - nHeight) / 2, 
					nWidth, nHeight,SWP_FRAMECHANGED);
		ShowWindow(g_hMainWnd, SW_NORMAL);
	}

	return TRUE;
}
