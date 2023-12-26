//------------------------------------------------------------------------------//
// 
// MODULE   : ltclientshell.cpp
// 
// PURPOSE  : CLTClientShell - Implementation
// 
// CREATED  : 2002/03/02
// 
// (c) 2002 LithTech, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//

#include "ltclientshell.h"

#include "camera.h"
#include "clientinterfaces.h"
#include "msgids.h"
#include "commandids.h"

#include "worldpropsclient.h"
#include "playerclnt.h"

// Client-side helper functions
#include "clienthelper.h"

// Engine Includes
#include <iltclient.h>
#include <iltphysics.h>

#include <iltsoundmgr.h>

#include <iltcommon.h>

#include <stdio.h>

#define DEFAULT_GRAVITY     -2000.0f

//-----------------------------------------------------------------------------
//	This macro defines certain DLL exports and variables
SETUP_CLIENTSHELL();

CLTClientShell* g_pCShell = NULL;

// Define the instantiation of the ClientShell interface.
define_interface(CLTClientShell, IClientShell);


void ServerFn(int argc, char **argv)
{
    g_pLTClient->CPrint("Starting Multiplayer Server...");
    g_pLTClient->CPrint("Result: %d", g_pCShell->StartMultiplayerServer() );
}

void NormalFn(int argc, char **argv)
{
    g_pLTClient->CPrint("NormalFn");
    g_pLTClient->CPrint("Result: %d", g_pCShell->StartNormalGame() );
}

void JoinFn(int argc, char **argv)
{
    g_pLTClient->CPrint("JoinFn");
    if(argc == 1)
    {
		g_pLTClient->CPrint("Connecting to Multiplayer Server... %s", argv[0]);
		g_pLTClient->CPrint("Result: %d", g_pCShell->JoinMultiplayerGame(argv[0]) );
    }
}

//------------------------------------------------------------------------------
//	CLTClientShell::CLTClientShell()
//
//------------------------------------------------------------------------------
CLTClientShell::CLTClientShell()
:
m_hCamera(NULL),
m_pCamera(NULL),
m_bRender(true),
m_bFirstUpdate(true),
m_vPlayerStartPos(0.0f, 160.0f, 0.0f),
m_pWorldPropsClnt(NULL),
m_pPlayer(NULL),
m_nGameMode(LOCAL_GAMEMODE_NONE),
m_bInWorld(false)
{
	g_pCShell= this;
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
	
	/*
	result = InitGame();
	if (ShutdownIfFailed(result, "Error in InitGame(). Shutting down.\n"))
	{
		return result;
	}
	*/
	
	this->CreateWorldPropObject();

    g_pLTClient->CPrint("You must type server, normal or join <ip.ip.ip.ip>");
    
    result = this->CreateCamera();
	
    //Register console programs
	g_pLTClient->RegisterConsoleProgram("Server", ServerFn);
	g_pLTClient->RegisterConsoleProgram("Normal", NormalFn);
	g_pLTClient->RegisterConsoleProgram("Join", JoinFn);

	return result;
}

//---------------------------------------------------------------------------
//  CLTClientShell::CreateCamera()
//
//---------------------------------------------------------------------------
LTRESULT CLTClientShell::CreateCamera()
{
	
    LTRESULT result = LT_OK;
	
    if( m_hCamera )
        return LT_OK;
    
    if( !m_pCamera )
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
		
		newRMode.m_bHWTnL	= pMode->m_bHWTnL;
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
LTRESULT CLTClientShell::InitGame(EGameType eType, char* ip)
{
	LTRESULT result;
	const char* pLevelName = NULL;
	StartGameRequest request;
	memset(&request, 0, sizeof(StartGameRequest));
	
    switch(eType)
    {
	case EGT_HOST:
		{
			request.m_Type = STARTGAME_HOST;	
			request.m_HostInfo.m_dwMaxConnections = 8;
			g_pLTClient->InitNetworking(NULL, 0);
			SetService();
		}
		break;
	case EGT_CONNECT:
		{
			request.m_Type = STARTGAME_CLIENTTCP;
			strncpy( request.m_TCPAddress, ip, MAX_SGR_STRINGLEN);
			g_pLTClient->InitNetworking(NULL, 0);
			SetService();
		}break;
	case EGT_NORMAL:
	default:
		{
			request.m_Type = STARTGAME_NORMAL;
			request.m_HostInfo.m_dwMaxConnections = 1;
		}
		break;
    }
    
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
	
	result = g_pLTClient->StartGame(&request);
	if (LT_OK != result)
	{
		DebugPrintIfFailed(result, "Start Game Request failed.\n");
		return result;
	}
	
	return result;
}

//---------------------------------------------------------------------------
//  CLTClientShell::SetService()
//
//---------------------------------------------------------------------------
bool CLTClientShell::SetService()
{
	NetService *pCur, *pListHead;
	HNETSERVICE hNetService;
	
	pCur      = NULL;
	pListHead = NULL;
	hNetService = NULL;
	
    if( g_pLTClient->GetServiceList( pListHead ) != LT_OK || !pListHead )
        return false;
	
	// Find the service specified.
	pCur = pListHead;
	while( pCur )
	{
		if( pCur->m_dwFlags & NETSERVICE_TCPIP )
		{
			hNetService = pCur->m_handle;
			break;
		}
		
		pCur = pCur->m_pNext;
	}
	
	// Free the service list.
    g_pLTClient->FreeServiceList( pListHead );
	
	// Check if tcp not found.
	if( !hNetService )
        return false;
	
	// Select it.
    if( g_pLTClient->SelectService( hNetService ) != LT_OK )
        return false;
	
    return true;
}

//---------------------------------------------------------------------------
//  CLTClientShell::OnEnterWorld()
//
//---------------------------------------------------------------------------
void CLTClientShell::OnEnterWorld()
{
    g_pLTClient->CPrint("CLTClientShell::OnEnterWorld()");

    // Clear the input buffer for a clean start
    g_pLTClient->ClearInput();
   
    if(!m_pPlayer)
    {
        m_pPlayer = new CPlayerClnt();
    }

    m_bInWorld = true;
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

    m_bInWorld = false;
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
            g_pLTClient->CPrint("CLIENT startpoint: %f %f %f", m_vPlayerStartPos.x, 
                                                               m_vPlayerStartPos.y, 
                                                               m_vPlayerStartPos.z);
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

    // If the game mode is other than NONE, proceed
    if(m_nGameMode)
    {   

        if(IsInWorld())
        {
          

            if( m_bFirstUpdate )
            {
                //Check to see if the client object is available, if not return
                HLOCALOBJ hObject = g_pLTClient->GetClientObject();
                if(!hObject)return;

                //Set our client object non solid
                g_pLTCCommon->SetObjectFlags(hObject, OFT_Flags, FLAG_CLIENTNONSOLID, FLAG_CLIENTNONSOLID);              
                m_pPlayer->SetClientObject(hObject);
                m_pPlayer->CreatePlayer();
                HLOCALOBJ hPlayer = m_pPlayer->GetPlayerObject(); 
                
                if (NULL != hPlayer)
                {                        

                    LTVector vNewGlobalForce(0, DEFAULT_GRAVITY, 0);
                    g_pLTCPhysics->SetGlobalForce(vNewGlobalForce);
                    g_pLTCPhysics->SetStairHeight(16.0f);                  
                    m_bFirstUpdate = false;
                    
                }

                //Set the actual client object invisible.
                g_pLTCCommon->SetObjectFlags(hObject, OFT_Flags, 0, FLAG_VISIBLE);

            }
            
            // Poll for user input   
            PollInput();
            
            //Update our player object
            m_pPlayer->Update();
            
            // Update our camera
            m_pCamera->UpdatePosition(m_pPlayer->GetPlayerObject());

        }

    }
	
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
	
	//	Clear the screen to prepare for the next draw
	result = g_pLTClient->ClearScreen(NULL, CLEARSCREEN_SCREEN | CLEARSCREEN_RENDER, 0 );
	if (LT_OK != result)
	{
		g_pLTClient->DebugOut("Error in ClearScreen()\n");
		g_pLTClient->Shutdown();
	}
	
	//	Tell LithTech we are going to be starting our 3d render
	result = g_pLTClient->Start3D();
	if (LT_OK != result)
	{
		g_pLTClient->DebugOut("Error in Start3D()\n");
		g_pLTClient->Shutdown();
	}
	
	//	Tell LithTech to render the scene using this camera
	result = g_pLTClient->RenderCamera(m_hCamera, fFrameTime);
	if (LT_OK != result)
	{
		g_pLTClient->DebugOut("Error in RenderCamera()\n");
		g_pLTClient->Shutdown();
	}
	
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
	switch (command)
	{
	case COMMAND_QUIT:
		g_pLTClient->Shutdown();
		break;
	default:
		break;
	}
}


//---------------------------------------------------------------------------
//  CLTClientShell::OnCommandOff(int command)
//
//---------------------------------------------------------------------------
void CLTClientShell::OnCommandOff(int command)
{
}


//---------------------------------------------------------------------------
//  CLTClientShell::PollInput()
//
//---------------------------------------------------------------------------
LTRESULT CLTClientShell::PollInput()
{
	
	float offsets[3];
	LTVector vVel, vUp, vRight, vForward, vTemp;
	LTRotation rRot, rTemp;
	float fFrameTime;
	bool bMoved = false;
	bool bRotated = false;
	
	
	fFrameTime = g_pLTClient->GetFrameTime();

    HLOCALOBJ hPlayer = m_pPlayer->GetPlayerObject();
	
    g_pLTCPhysics->GetVelocity(hPlayer, &vVel);

	g_pLTClient->GetObjectRotation(hPlayer, &rRot);
	g_pLTCCommon->GetRotationVectors(rRot, vUp, vRight, vForward);
	
	g_pLTClient->GetAxisOffsets(offsets);
	
	if ((offsets[0] != 0.0f) || 
		(offsets[1] != 0.0f) || 
		(offsets[2] != 0.0f))
	{
		bRotated = true;
        m_pCamera->UpdatePitch(offsets[1]);
        m_pPlayer->UpdateRotation(offsets[0], 0.0f, offsets[2]);  
	}



    uint8  dwMoveFlags = 0;

	if (g_pLTClient->IsCommandOn(COMMAND_MOVE_LEFT))
	{
        dwMoveFlags |= MOVE_LEFT;
	}
	
	if (g_pLTClient->IsCommandOn(COMMAND_MOVE_RIGHT))	
	{
        dwMoveFlags |= MOVE_RIGHT;
    }
    
    if ( g_pLTClient->IsCommandOn(COMMAND_MOVE_FORWARD))
    {
        dwMoveFlags |= MOVE_FORWARD;
    }
    
    if (g_pLTClient->IsCommandOn(COMMAND_MOVE_BACKWARD))
    {
        dwMoveFlags |= MOVE_BACKWARD;
    }
    
    if ( g_pLTClient->IsCommandOn(COMMAND_MOVE_UP))
    {
        dwMoveFlags |= MOVE_UP;
    }
    
    // Debug command
    if( g_pLTClient->IsCommandOn(COMMAND_DEBUG_1))
    {
        HLOCALOBJ hObject = g_pLTClient->GetClientObject();
        LTVector vPos;
        g_pLTClient->GetObjectPos(hObject, &vPos);
        g_pLTClient->CPrint("***DBG*** CLIENTObj pos: %f %f %f", VEC_EXPAND(vPos));    
    }
	
    m_pPlayer->UpdateMoveFlags(dwMoveFlags);

	
	return LT_OK;
}


//-----------------------------------------------------------------------------
//  CLTClientShell::SendRotation()
//
//-----------------------------------------------------------------------------
void CLTClientShell::SendRotation(LTRotation &rRot)
{
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
//  CLTClientShell::SendVelocity()
//
//-----------------------------------------------------------------------------
void CLTClientShell::SendVelocityAndPos(LTVector &vVel, LTVector &vPos)
{
	ILTMessage_Write *pMessage;
	LTRESULT nResult = g_pLTCCommon->CreateMessage(pMessage);
	
	if( LT_OK == nResult)
	{
		pMessage->IncRef();
		pMessage->Writeuint8(MSG_CS_VELOCITY);
		pMessage->WriteLTVector(vVel);
		pMessage->WriteLTVector(vPos);
		g_pLTClient->SendToServer(pMessage->Read(), 0);
		pMessage->DecRef();
	}
	
}


//-----------------------------------------------------------------------------
//  CLTClientShell::SendVelAndRot()
//
//-----------------------------------------------------------------------------
void CLTClientShell::SendVelPosAndRot(LTVector &vVel, LTVector &vPos, LTRotation &rRot)
{
	ILTMessage_Write *pMessage;
	LTRESULT nResult = g_pLTCCommon->CreateMessage(pMessage);
	
	if( LT_OK == nResult)
	{
		pMessage->IncRef();
		pMessage->Writeuint8(MSG_CS_VELANDROT);
		pMessage->WriteLTVector(vVel);
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
//  CLTClientShell::StartMultiplayerServer()
//
//-----------------------------------------------------------------------------
LTRESULT CLTClientShell::StartMultiplayerServer()
{
    LTRESULT result = this->InitGame(EGT_HOST);
    
    if(LT_OK == result)
    {
        m_nGameMode = LOCAL_GAMEMODE_HOST;
    }

    return result;
}


//-----------------------------------------------------------------------------
//  CLTClientShell::JoinMultiplayerGame()
//
//-----------------------------------------------------------------------------
LTRESULT CLTClientShell::JoinMultiplayerGame(char* ip)
{
    
    LTRESULT result = this->InitGame(EGT_CONNECT, ip);

    if(LT_OK == result)
    {
        m_nGameMode = LOCAL_GAMEMODE_CLIENT;
    }

    return result;

}


//-----------------------------------------------------------------------------
//  CLTClientShell::StartNormalGame()
//
//-----------------------------------------------------------------------------
LTRESULT CLTClientShell::StartNormalGame()
{
    LTRESULT result = this->InitGame(EGT_NORMAL);

    if(LT_OK == result)
    {
        m_nGameMode = LOCAL_GAMEMODE_NORMAL;
    }
    
    return result;
    
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
	//g_pLTClient->CPrint("OnKeyDown(%d,%d)", key, rep);
}

//-----------------------------------------------------------------------------
//  CLTClientShell::OnKeyUp(int key)
//
//-----------------------------------------------------------------------------
void CLTClientShell::OnKeyUp(int key)
{
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
    
    switch(messageID)
    {
    case 0:
        break;
    default:
        break;
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
	g_pLTClient->CPrint("CLTClientShell::OnEngineTerm()");

	// Free memory
	if( m_hCamera )
	{	
		g_pLTClient->RemoveObject(m_hCamera);
		delete m_pCamera;
		m_pCamera = NULL;
	}
	
	this->DestroyWorldPropObject();
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
//  Note: this will only get called on a remote client
//
//-----------------------------------------------------------------------------
void CLTClientShell::PreLoadWorld(const char *pWorldName)
{
	g_pLTClient->CPrint("PreLoadWorld(%s)", pWorldName);
}

