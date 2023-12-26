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
#include <windows.h>

#include "ltclientshell.h"
#include "camera.h"
#include "clientinterfaces.h"
#include "msgids.h"
#include "commandids.h"
#include "worldpropsclient.h"
#include "playerclnt.h"
#include "statsgui.h"
#include "chatgui.h"
// Client-side helper functions
#include "clienthelper.h"

// ClientFX
#include "ClientFXDB.h"

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



//-------------------------------------------------------------------------------------------------
// Helper functions
//-------------------------------------------------------------------------------------------------


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



void FindServersFn(int argc, char **argv)
{
    g_pLTClient->CPrint("This cvar is disabled");
//    g_pLTClient->CPrint("Finding Servers...");
//    g_pCShell->FindServers();
}



//-------------------------------------------------------------------------------------------------
// CLTClientShell
//-------------------------------------------------------------------------------------------------


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
m_bInWorld(false),
m_bShowStats(false)
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

	CreateWorldPropObject();

    g_pLTClient->CPrint("You must type server, normal or join <ip.ip.ip.ip>");

    result = this->CreateCamera();

    // Initialize Our ClientFX Database
    if(!CClientFXDB::GetSingleton().Init(g_pLTClient))
    {
        g_pLTClient->ShutdownWithMessage( "Could not init ClientFXDB!" );
        return LT_ERROR;
    }

	// Init the ClientFX mgr... (this must become before most other classes)
	if( !m_ClientFXMgr.Init( g_pLTClient ) )
	{
		// Make sure ClientFX.fxd is built and in the game dir
		g_pLTClient->ShutdownWithMessage( "Could not init ClientFXMgr!" );
		return LT_ERROR;
	}

    // We need to make sure to setup the camera for the FX Mgr
    m_ClientFXMgr.SetCamera(m_hCamera);

    //Register console programs
	g_pLTClient->RegisterConsoleProgram("Server", ServerFn);
	g_pLTClient->RegisterConsoleProgram("Normal", NormalFn);
	g_pLTClient->RegisterConsoleProgram("Join", JoinFn);
	g_pLTClient->RegisterConsoleProgram("FindServers", FindServersFn);

	m_Gui.Init(15, 18);

    // Create and Init the stats gui
    m_pStatsGui = new CStatsGui;
    assert(m_pStatsGui);
    m_pStatsGui->Init();

    // Create and Init the chat gui
    m_pChatGui = new CChatGui;
    assert(m_pChatGui);
    m_pChatGui->Init();

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
	{
        return LT_OK;
	}

    if( !m_pCamera )
	{
		m_pCamera = new CCamera;
	}

    if( !m_pCamera )
	{
		return LT_ERROR;
	}

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
			request.m_HostInfo.m_dwMaxConnections = 12;
            strncpy(request.m_HostInfo.m_sName, "Seal Hunter Server", MAX_NETSESSIONNAME_LEN);
            request.m_HostInfo.m_Port = 27889;
			g_pLTClient->InitNetworking(NULL, 0);
			SetService();
		}
		break;
	case EGT_CONNECT:
		{
			request.m_Type = STARTGAME_CLIENTTCP;
			strncpy( request.m_TCPAddress, ip, MAX_SGR_STRINGLEN);
            strncat( request.m_TCPAddress, ":27889", MAX_SGR_STRINGLEN);
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

    // Stop all the client FX
	m_ClientFXMgr.ShutdownAllFX();
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
            g_pLTClient->CPrint("CLIENT startpoint: %f %f %f", m_vPlayerStartPos.x, m_vPlayerStartPos.y, m_vPlayerStartPos.z);
		}
		break;
    case MSG_CS_MY_CLUB:
        {
            m_pPlayer->SetClub(pMessage->ReadObject());
        }
		break;
    case MSG_CS_SCORE:
        {
            uint32 iScore = pMessage->Readuint32();
            float  fMoney = pMessage->Readfloat();
            m_pPlayer->SetScore(iScore);
            m_pPlayer->SetMoney(fMoney);

            m_Gui.SetStats(iScore, fMoney);
        }
        break;
	case MSG_WORLD_PROPS:
		{
			m_pWorldPropsClnt->UnpackWorldProps(pMessage);
		}
		break;
    case MSG_SERVER_SCORES:
        {
            assert(m_pStatsGui);
            m_pStatsGui->HandleMessage(pMessage);

        }
        break;
    case MSG_SC_CHAT:
        {
            assert(m_pChatGui);
            m_pChatGui->HandleMessage(pMessage);
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
                if(!hObject)
				{
					return;
				}

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

                    //Ask for CLUB ID
                	ILTMessage_Write *pMessage;
	                LTRESULT nResult = g_pLTCCommon->CreateMessage(pMessage);

	                if( LT_OK == nResult)
	                {
		                pMessage->IncRef();
		                pMessage->Writeuint8(MSG_CS_MY_CLUB);
		                g_pLTClient->SendToServer(pMessage->Read(), MESSAGE_GUARANTEED);
		                pMessage->DecRef();
	                }

                    m_bFirstUpdate = false;
                }

                //Set the actual client object invisible.
                g_pLTCCommon->SetObjectFlags(hObject, OFT_Flags, 0, FLAG_VISIBLE);

                // Send our name to the server
                SendPlayerName();
            }


            assert(m_pChatGui);
            m_pChatGui->Update();

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
	{
		m_pWorldPropsClnt->Update();
	}
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
	result = g_pLTClient->ClearScreen(NULL, CLEARSCREEN_SCREEN | CLEARSCREEN_RENDER, NULL);
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

	//update all the effects status as well as any that might effect the camera
	m_ClientFXMgr.UpdateAllActiveFX( m_bRender );

	// Render the effects
	m_ClientFXMgr.RenderAllActiveFX( m_bRender );

	// Render the gui.
	m_Gui.Render();

    if(IsInWorld())
    {
        if(m_bShowStats)
        {
            assert(m_pStatsGui);
            m_pStatsGui->Render();
        }

        assert(m_pChatGui);
        m_pChatGui->Render();
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
    switch(dwEventID)
    {
    case LTEVENT_LOSTFOCUS:
        m_bRender = false;
        break;

    case LTEVENT_RENDERTERM:
        {
            m_bRender = false;

            // Let the ClientFx mgr know the renderer is shutting down
            m_ClientFXMgr.OnRendererShutdown();
        }
        break;

    case LTEVENT_GAINEDFOCUS:
        m_bRender = true;
        break;

    case LTEVENT_RENDERINIT:
        m_bRender = true;
        break;

    case LTEVENT_RENDERALMOSTINITTED:
        m_bRender = true;
        break;

    default:
        break;
    }
}



//---------------------------------------------------------------------------
//	CLTClientShell::OnCommandOn()
//
//---------------------------------------------------------------------------
void CLTClientShell::OnCommandOn(int command)
{
	if (m_nGameMode == LOCAL_GAMEMODE_NONE)
	{
		m_Gui.HandleInput(command);
	}


    // Are we currently typing text?
    if(!m_pChatGui->IsChatInputActive())
    {
	    // quit
	    if (command == COMMAND_QUIT)
	    {
            // This is done in OnKeyDown() now
            // g_pLTClient->Shutdown();
        }else
        if(m_bInWorld)
        {
            if(command == COMMAND_CHAT)
            {
                m_pChatGui->SetActive(true);
            }
            else
            if (command == COMMAND_SHOWSTATS)
            {
                //g_pLTClient->CPrint("Client: showstats ON");
                //Send stats show message
	            ILTMessage_Write *pMessage;
	            LTRESULT nResult = g_pLTCCommon->CreateMessage(pMessage);

	            if( LT_OK == nResult)
	            {
		            pMessage->IncRef();
		            pMessage->Writeuint8(MSG_CS_SCORE);
		            pMessage->Writebool(true);
		            g_pLTClient->SendToServer(pMessage->Read(), MESSAGE_GUARANTEED);
		            pMessage->DecRef();
	            }

                m_bShowStats = true;
            }
        }
    }
    else        
    {
        // Don't show stats if we're typing in chat.
        m_bShowStats = false;
    }
}



//---------------------------------------------------------------------------
//  CLTClientShell::OnCommandOff()
//
//---------------------------------------------------------------------------
void CLTClientShell::OnCommandOff(int command)
{
    //
    if (command == COMMAND_SHOWSTATS)
    {
        //g_pLTClient->CPrint("Client: showstats OFF");
        //Send stats kill message
	    ILTMessage_Write *pMessage;
	    LTRESULT nResult = g_pLTCCommon->CreateMessage(pMessage);

	    if( LT_OK == nResult)
	    {
		    pMessage->IncRef();
		    pMessage->Writeuint8(MSG_CS_SCORE);
		    pMessage->Writebool(false);
		    g_pLTClient->SendToServer(pMessage->Read(), MESSAGE_GUARANTEED);
		    pMessage->DecRef();
	    }

        m_bShowStats = false;
    }
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

    if(m_pChatGui->IsChatInputActive())
    {
        return LT_OK;
    }

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
        m_pPlayer->UpdateRotation(offsets[0], 0.0f, 0.0f);
        m_pCamera->UpdateZoom(offsets[2]);
	}

    uint8 dwMoveFlags = 0;

	// left
	if (g_pLTClient->IsCommandOn(COMMAND_MOVE_LEFT))
	{
        dwMoveFlags |= MOVE_LEFT;
	}

	// right
	if (g_pLTClient->IsCommandOn(COMMAND_MOVE_RIGHT))
	{
        dwMoveFlags |= MOVE_RIGHT;
    }

	// move forward
	if ( g_pLTClient->IsCommandOn(COMMAND_MOVE_FORWARD))
    {
        dwMoveFlags |= MOVE_FORWARD;
    }

	// move back
	if (g_pLTClient->IsCommandOn(COMMAND_MOVE_BACKWARD))
    {
        dwMoveFlags |= MOVE_BACKWARD;
    }

	// shoot
	if (g_pLTClient->IsCommandOn(COMMAND_SHOOT))
    {
		m_pPlayer->Attack();
    }

	// jump
	if (g_pLTClient->IsCommandOn(COMMAND_JUMP))
	{
		dwMoveFlags |= MOVE_JUMP;
	}

	// Debug command
    if ( g_pLTClient->IsCommandOn(COMMAND_DEBUG_1))
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
//  CLTClientShell::SendPlayerName()
//
//-----------------------------------------------------------------------------
void CLTClientShell::SendPlayerName()
{
	ILTMessage_Write *pMessage;
	LTRESULT nResult = g_pLTCCommon->CreateMessage(pMessage);

	if( LT_OK == nResult)
	{
        char szName[32];
        szName[31] = '\0';
        unsigned long nSize = 16;
        GetComputerName(szName, &nSize);
        g_pLTClient->CPrint("Computer Name: %s", szName);

		//sprintf(szName, "Player %p", this);

		pMessage->IncRef();
		pMessage->Writeuint8(MSG_CS_PLAYERNAME);
        pMessage->WriteString(szName);
		g_pLTClient->SendToServer(pMessage->Read(), MESSAGE_GUARANTEED);
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
    if(m_bInWorld)
    {
        if( VK_F12 == key )
        {
            g_pLTClient->CPrint("Creating Cubic Environment Map...");
            g_pLTClient->MakeCubicEnvMap(m_pCamera->GetCamera(), 256, "CubicEnvMap");
        }

        // If chat input is active, then let's give the chat gui a chance to use it
        if(m_pChatGui->IsChatInputActive())
        {
            m_pChatGui->HandleInputDown(key, rep);
        }
        else
        {
           if(VK_ESCAPE == key)
           {
               g_pLTClient->Shutdown();
           }
        }
    }
    else
    if(VK_ESCAPE == key)
    {
       g_pLTClient->Shutdown();
    }
}



//-----------------------------------------------------------------------------
//  CLTClientShell::OnKeyUp(int key)
//
//-----------------------------------------------------------------------------
void CLTClientShell::OnKeyUp(int key)
{
	//g_pLTClient->CPrint("OnKeyUp(%d)", key);
        // If chat input is active, then let's give 
    if(m_pChatGui->IsChatInputActive())
    {
        m_pChatGui->HandleInputUp(key);
    }
    else
    {
       // if(  )
    }
}



//-----------------------------------------------------------------------------
//  CLTClientShell::SpecialEffectNotify(HLOCALOBJ hObj, ILTMessage_Read *pMessage)
//
//-----------------------------------------------------------------------------
void CLTClientShell::SpecialEffectNotify(HLOCALOBJ hObj, ILTMessage_Read *pMessage)
{
	//g_pLTClient->CPrint("SpecialEffectNotify(%p,%p)", hObj, pMessage);
	pMessage->SeekTo(0);
    uint8 messageID = pMessage->Readuint8();

    switch(messageID)
    {
    case MSG_CS_PLAYERNAME:
        {
            char szName[16];
            szName[15] = '\0';
            pMessage->ReadString(szName, 16);
            g_pLTClient->CPrint("%s entered the game.", szName);
        }
        break;
    default:
        break;
    }

    // ClientFX: handle this!
	pMessage->SeekTo(0);
	m_ClientFXMgr.OnSpecialEffectNotify( hObj, pMessage );

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
	if(!hObj)
	{
		return;
	}

	m_ClientFXMgr.OnObjectRemove( hObj );
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

    // Remove the camera handle from the ClientFXMgr
    m_ClientFXMgr.SetCamera(NULL);

	m_Gui.Term();

    // Dispose of the Stats gui
    assert(m_pStatsGui);
    m_pStatsGui->Term();
    delete m_pStatsGui;
    m_pStatsGui = NULL;

    // Dispose of the Chat gui
    assert(m_pChatGui);
    m_pChatGui->Term();
    delete m_pChatGui;
    m_pChatGui = NULL;

    //LTRESULT res = g_pLTClient->RemoveInternetDriver();
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



//-----------------------------------------------------------------------------
//  CLTClientShell::FindServers()
//
//-----------------------------------------------------------------------------
uint CLTClientShell::FindServers(SERVERINFO *pInfo)
{
    uint nServerCount = 0;
    NetSession *pList, *pSession;

    // Clear old values
    for(int j = 0; j < 4; j++)
    {
        memset(pInfo[j].Name, 0, 255);
        memset(pInfo[j].IpAddress, 0, MAX_HOSTIP_LEN - 1);
    }

	
	LTRESULT res = g_pLTClient->AddInternetDriver();
	if(res == LT_ERROR)
    {
        g_pLTClient->CPrint("ERROR: AddInternetDriver()");
        return 0;
    }
	

    SetService();

    // Find the sessions
	if((g_pLTClient->GetSessionList( pList, "*:27889" ) == LT_OK) && pList)
	{
		pSession = pList;
		for(int i = 0; pSession; i++)
		{
            strncpy(pInfo[i].Name, pSession->m_sName, 256);
            strncpy(pInfo[i].IpAddress , pSession->m_HostIP, MAX_HOSTIP_LEN);
            g_pLTClient->CPrint("(%d) %s -> %s:%d", i, pSession->m_sName, pSession->m_HostIP, pSession->m_HostPort);
            g_pLTClient->CPrint("(%d) Ping: %d", i, pSession->m_Ping);
            g_pLTClient->CPrint("(%d) Players: %d/%d", i, pSession->m_dwCurConnections, pSession->m_dwMaxConnections);
            pSession = pSession->m_pNext;
            ++nServerCount;

            // 4 is our hardcoded Max for this sample
            if(nServerCount == 4)
                break;
        }
    }

	res = g_pLTClient->RemoveInternetDriver();

    return nServerCount;

}
