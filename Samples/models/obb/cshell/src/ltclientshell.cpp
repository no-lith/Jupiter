//	-----------------------------------------------------------------------	//
//
//	MODULE  : ltclientshell.cpp
//
//	PURPOSE : LT Client Shell - Implementation
//
//	CREATED : 5/20/2002
//
//	(c) 2002	LithTech, Inc.	 All Rights	Reserved
//
//	-----------------------------------------------------------------------	//
#include	"ltclientshell.h"
#include	"clientinterfaces.h"
#include	"clienthelper.h"
#include	"ModelMgr.h"

#include	<iltdrawprim.h>
#include	<lteulerangles.h>
#include	<iltsoundmgr.h>
#include	<ltobjectcreate.h>
#include	<iltmessage.h>
#include	<iltphysics.h>
#include	<iltcommon.h>
#include	<iltmodel.h>


//	This macro defines certain	DLL exports	and variables
SETUP_CLIENTSHELL();

CLTClientShell* g_pCShell = NULL;

//	Define the instantiation of the ClientShell interface.
define_interface(CLTClientShell,	IClientShell);



//-------------------------------------------------------------------------------------------------
//	CLTClientShell
//-------------------------------------------------------------------------------------------------

CLTClientShell::CLTClientShell()
: m_bRender(true),
m_bFirstUpdate(true),
m_hCamera(NULL),
m_hPlayer(NULL),
m_vPlayerStartPos(0.0f, -100.0f, 0.0f),
m_fYaw(0.0f),
m_fPitch(0.0f),
m_fRoll(0.0f)
{
	g_pCShell= this;
	m_rPlayerStartRot.Init();
}



CLTClientShell::~CLTClientShell()
{
}



//------------------------------------------------------------------------------
//	 Name:		 OnEngineInitialized(...)
//
//	 Purpose:	Must be overridden in the clientshell.	Called after the
//				engine has been fully initialized. The	engine doesn't	initialize
//				the renderer itself,	but passes in the	default render	mode it
//				would	use to initialize	the renderer (from the "RenderDLL",
//				"ScreenWidth",	and "ScreenHeight" console	variables).
//				SetRenderMode() needs to be called to initialize the renderer
//				in	this function.	If	this function returns something other than
//				LT_OK, the engine	will shut down	immediately.
//				Additionally, this function can process command-line arguments,
//				and set the	initial game mode	(go to a	menu,	run a	world, etc).
//
//	 Returns:	The LTRESULT of the StartGame() request.
//
//------------------------------------------------------------------------------
LTRESULT	CLTClientShell::OnEngineInitialized(RMode	*pMode, LTGUID	*pAppGuid)
{
	LTRESULT	result =	LT_OK;

	//	Verify the interfaces.
	result =	VerifyClientInterfaces();
	if	(ShutdownIfFailed(result, "Error	in	VerifyClientInterfaces(). Shutting down.\n"))
	{
		return result;
	}

	//	Initialize the	renderer.
	result =	InitRenderer(pMode);
	if	(ShutdownIfFailed(result, "Error	in	InitRenderer(). Shutting down.\n"))
	{
		return result;
	}

	//	Initialize the	game.
	result =	InitGame();
	DebugPrintIfFailed(result,	"Error in InitGame().\n");

	//	Initialize the	sound.
	result =	InitSound();
	DebugPrintIfFailed(result,	"Error in InitSound().\n");

	//	Initialize the	camera.
	result =	InitCamera();
	DebugPrintIfFailed(result,	"Error in InitCamera().\n");

	//	Initialize the	gui manager.
	result =	m_GuiMgr.Init();

	//	Initialize the	model	manager.
	result =	g_ModelMgr.Init();

	//	Initialize the	crosshair display
	m_Crosshair.Init("Tex/Crosshair2_I.dtx");
	m_Crosshair.Center();

	return result;
}



LTRESULT	CLTClientShell::InitSound()
{
	//	Initialize sound
	InitSoundInfo sndinfo;
	sndinfo.Init();
	sndinfo.m_nNum3DVoices = 64;
	strcpy(sndinfo.m_sz3DProvider, "DirectSound Hardware");

	g_pLTCSoundMgr->InitSound(&sndinfo);

	return LT_OK;
}



LTRESULT	CLTClientShell::InitCamera()
{
	//Create	Camera
	uint32 nWidth	= 640;
	uint32 nHeight	= 480;
	ObjectCreateStruct objCreate;

	//	Get our screen	dimensions,	for the camera	rectangle
	g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);

	//	Initialize our	object creation structure
	objCreate.Clear();
	objCreate.m_ObjectType = OT_CAMERA;

	m_hCamera = g_pLTClient->CreateObject(&objCreate);

	if	(m_hCamera == NULL)
	{
		return LT_ERROR;
	}

	g_pLTClient->SetCameraRect(m_hCamera,	false, 0, 0, nWidth,	nHeight);

	//	This is an fov	of	90	degrees
	float	fFovX	= MATH_PI/2.0f;
	float fFovY =	(fFovX *	nHeight)	/ nWidth;
	g_pLTClient->SetCameraFOV(m_hCamera, fFovX,	fFovY);

	LTVector	vZero(0.0f,	0.0f,	0.0f);
	g_pLTCPhysics->SetObjectDims(m_hCamera, &vZero,	0);

	return LT_OK;
}



//---------------------------------------------------------------------------
//	 Name:			InitRenderer(...)
//
//	 Purpose:	  Set	the render mode for this client
//
//	 Returns:	  The	LTRESULT	of	the SetRenderMode() call.
//
//---------------------------------------------------------------------------
LTRESULT	CLTClientShell::InitRenderer(RMode *pMode)
{
	//	g_pLTClient->CPrint("CLTClientShell::InitRenderer");

	//	Attempt to set	the render mode based on pMode.
	//		pMode	contains	command line parameters	or
	//		console vars included in autoexec.cfg.	Such as:
	//
	//		screenwidth	800
	//		screenheight 600
	//		bitdepth	32
	//
	//		Or	on	the command	line:
	//
	//		lithtech.exe +screenwidth 800	+screenheight 600	+bitdepth 32

	if	(g_pLTClient->SetRenderMode(pMode) != LT_OK)
	{
		g_pLTClient->DebugOut("Error:	Couldn't	set requested render	mode!\n");

		//	Try 640x480x16	as	a fallback.

		RMode	newRMode;
		memset(&newRMode,	0,	sizeof(newRMode));

		newRMode.m_Width		= 640;
		newRMode.m_Height		= 480;
		newRMode.m_BitDepth		= 16;

		newRMode.m_bHWTnL	= pMode->m_bHWTnL;
		sprintf(newRMode.m_InternalName,	"%s",	pMode->m_InternalName);
		sprintf(newRMode.m_Description, "%s", pMode->m_Description);

		//	Give it another try.
		g_pLTClient->DebugOut("Attempting to set render	mode to 640x480x16...\n");

		if	(g_pLTClient->SetRenderMode(&newRMode)	!=	LT_OK)
		{
			//	Still	didn't work.
			g_pLTClient->DebugOut("Error:	Couldn't	set to 640x480x16	mode.	Shutting	down.\n");
			g_pLTClient->ShutdownWithMessage("Error: Couldn't set	to	640x480x16 mode. Shutting down.\n");

			return LT_ERROR;
		}
	}

	return LT_OK;
}



//---------------------------------------------------------------------------
//	 Name:			InitGame(...)
//
//	 Purpose:	  After the	engine has been initialized, requests the	LTClient
//				  to start up the	world/game.
//
//	 Returns:	  The	LTRESULT	of	the StartGame() request.
//
//---------------------------------------------------------------------------
LTRESULT	CLTClientShell::InitGame()
{
	LTRESULT	result;

	StartGameRequest request;
	memset(&request, 0, sizeof(StartGameRequest));

	request.m_Type	= STARTGAME_NORMAL; //start local loop-back server

	//	If	"+runworld <world>" is specified	on	the command-line,	use it.
	HCONSOLEVAR	hVar = g_pLTClient->GetConsoleVar("runworld");
	const	char *pLevelName = g_pLTClient->GetVarValueString(hVar);

	if	(pLevelName)
	{
		//	(length includes the	NULL)
		LTStrCpy(request.m_WorldName,	pLevelName,	strlen(pLevelName)+1);
	}
	else
	{
		//Default level
		strcpy(request.m_WorldName, "d:\\resource\\worlds\\demoroom");
	}

	request.m_HostInfo.m_dwMaxConnections = 1;

	result =	g_pLTClient->StartGame(&request);
	if	(LT_OK != result)
	{
		g_pLTClient->DebugOut("Start Game Request	failed.\n");
	}

	//	Start	with a model.
	g_ModelMgr.AddRow();

	return result;
}



//---------------------------------------------------------------------------
//	 Name:			OnEnterWorld(...)
//
//	 Purpose:	  Called	by	the engine when the client	enters a	world	(after
//				  the	world	has been	loaded, all	objects have been	loaded, etc).
//
//	 Returns:	  void
//
//---------------------------------------------------------------------------
void CLTClientShell::OnEnterWorld()
{
	g_pLTClient->ClearInput();
}



//---------------------------------------------------------------------------
//	 Name:			OnExitWorld()
//
//	 Purpose:	  Called	by	the engine when the client	exits	a world (after
//				  disconnecting/unloading the	server and all	objects have been
//				  deleted).	All server objects are lost at this	point.
//
//	 Returns:	  void
//
//---------------------------------------------------------------------------
void CLTClientShell::OnExitWorld()
{
}



//---------------------------------------------------------------------------
//	 CLTClientShell::OnMessage(ILTMessage_Read* pMessage)
//
//---------------------------------------------------------------------------
void CLTClientShell::OnMessage(ILTMessage_Read*	pMessage)
{
	pMessage->SeekTo(0);
	uint8 messageID = pMessage->Readuint8();

	switch (messageID)
	{
	case MSG_STARTPOINT_POS_ROT:
		{

			m_vPlayerStartPos	= pMessage->ReadLTVector();
			m_rPlayerStartRot	= pMessage->ReadLTRotation();
			g_pLTClient->CPrint("CLIENT startpoint: %f %f %f", m_vPlayerStartPos.x,
				m_vPlayerStartPos.y,
				m_vPlayerStartPos.z);
			break;
		}
	default:
		break;
	}
}


//---------------------------------------------------------------------------
//	 Name:			Update()
//
//	 Purpose:	  Clientshell callback which gets called every update	cycle.
//
//	 Parameters:	None
//
//	 Returns:		void
//---------------------------------------------------------------------------
void CLTClientShell::Update()
{
	LTVector	vDims;
	LTVector	vPos;
	LTRotation rRot;

	//	If	this is our	first	update
	if (m_bFirstUpdate)
	{
		//	Get our client	object handle and	set our dimensions
		m_hPlayer =	g_pLTClient->GetClientObject();
		vDims.x = vDims.y	= vDims.z =	10.0f;
		g_pLTCPhysics->SetObjectDims(m_hPlayer, &vDims,	0);

		//	Set ourselves at the	starting	point
		g_pLTClient->SetObjectPos(m_hPlayer, &m_vPlayerStartPos);

		//	Make sure our rotation is setup correctly.
		//	We	convert from a	Quat to Euler angles	because
		//	our input rotation scheme is setup using Euler angles
		EulerAngles	euler	= Eul_FromQuat(m_rPlayerStartRot, 0);
		m_fPitch	= 0.0f;
		m_fYaw =	euler.z;	//all	we	care about is the	yaw
		m_fRoll = 0.0f;
		g_pLTCCommon->SetupEuler(rRot, m_fPitch, m_fYaw, m_fRoll);
		g_pLTClient->SetObjectRotation(m_hPlayer,	&rRot);

		//	Print	out or current	Euler	angles.
		g_pLTClient->CPrint("Eulers: %f %f %f", m_fPitch, m_fYaw, m_fRoll);

		//	Set our flag that	we	have infact	gone through our first update
		m_bFirstUpdate	= false;
	}

	//	Poll for	user input
	PollInput();

	//	Attune camera to the	client object
	g_pLTClient->GetObjectPos(m_hPlayer, &vPos);
	g_pLTClient->GetObjectRotation(m_hPlayer,	&rRot);
	g_pLTClient->SetObjectPosAndRotation(m_hCamera,	&vPos, &rRot);

	//	Check	for ray intersection	on	Model	OBBs.
	m_HitText[0] =	'\0';
	g_ModelMgr.CheckModelObbs(vPos, rRot, m_HitText);
	m_GuiMgr.UpdateHitGui(m_HitText);

	//	Render
	if	(m_bRender)
	{
		if	(Render() != LT_OK)
		{
			g_pLTClient->DebugOut("Error in Render()\n");
			g_pLTClient->Shutdown();
		}
	}
}


//---------------------------------------------------------------------------
//	 Name:			Render()
//
//	 Purpose:	  Instruct the	engine to render the	scene	through a given camera
//
//	 Returns:		LT_OK	if	all rendering calls return	successfully
//---------------------------------------------------------------------------
LTRESULT	CLTClientShell::Render()
{
	LTRESULT	result;

	float fFrameTime	= g_pLTClient->GetFrameTime();

	//	Clear	the screen to prepare for the	next draw
	result =	g_pLTClient->ClearScreen(NULL, CLEARSCREEN_SCREEN | CLEARSCREEN_RENDER,	m_BackgroundMgr.GetColor());
	if	(LT_OK != result)
	{
		g_pLTClient->DebugOut("Error in ClearScreen()\n");
		g_pLTClient->Shutdown();
	}

	//	Tell LithTech we are	going	to	be	starting	our 3d render
	result =	g_pLTClient->Start3D();
	if	(LT_OK != result)
	{
		g_pLTClient->DebugOut("Error in Start3D()\n");
		g_pLTClient->Shutdown();
	}

	//	Tell LithTech to render	the scene using this	camera
	result =	g_pLTClient->RenderCamera(m_hCamera, fFrameTime);
	if	(LT_OK != result)
	{
		g_pLTClient->DebugOut("Error in RenderCamera()\n");
		g_pLTClient->Shutdown();
	}

	//	Draw Crosshair
	m_Crosshair.Render();

	//	Render the gui	stuff.
	m_GuiMgr.Render(fFrameTime);

	//	Tell LithTech to finish	rendering for this scene
	result =	g_pLTClient->End3D(END3D_CANDRAWCONSOLE);
	if	(LT_OK != result)
	{
		g_pLTClient->DebugOut("Error in End3D()\n");
		g_pLTClient->Shutdown();
	}

	//	Tell LithTech to flip the screen	surface
	result =	g_pLTClient->FlipScreen(NULL);
	if	(LT_OK != result)
	{
		g_pLTClient->DebugOut("Error in FlipScreen()\n");
		g_pLTClient->Shutdown();
	}

	return LT_OK;
}



//---------------------------------------------------------------------------
//	 Name:		 OnEvent()
//
//	 Purpose:	Clientshell	callback: Called when a	particular engine	event
//				happens e.g. the game lost	focus	due to the player	pressing
//				ALT-TAB,	or	the renderer was initialized,	etc.
//
//	 Returns:	 void
//---------------------------------------------------------------------------
void CLTClientShell::OnEvent(uint32	dwEventID, uint32	dwParam)
{
	if	(dwEventID == LTEVENT_LOSTFOCUS ||
		dwEventID == LTEVENT_RENDERTERM)
	{
		m_bRender =	false;
	}

	if	(dwEventID == LTEVENT_GAINEDFOCUS ||
		dwEventID == LTEVENT_RENDERINIT ||
		dwEventID == LTEVENT_RENDERALMOSTINITTED)
	{
		m_bRender =	true;
	}
}



//---------------------------------------------------------------------------
//	 CLTClientShell::OnCommandOn()
//
//---------------------------------------------------------------------------
void CLTClientShell::OnCommandOn( int ActionID ) 
{
	//	quit
	if	(ActionID == COMMAND_QUIT)
	{
		g_pLTClient->Shutdown();
	}

	switch (ActionID)
	{
	case COMMAND_ADD_ROW:
		{
			g_ModelMgr.AddRow();
			m_GuiMgr.UpdateModelGui();

			break;
		}

	case COMMAND_REMOVE_ROW:
		{
			g_ModelMgr.SubtractRow();
			m_GuiMgr.UpdateModelGui();

			break;
		}

	case COMMAND_ADD_COL:
		{
			g_ModelMgr.AddCol();
			m_GuiMgr.UpdateModelGui();

			break;
		}

	case COMMAND_REMOVE_COL:
		{
			g_ModelMgr.SubtractCol();
			m_GuiMgr.UpdateModelGui();

			break;
		}

	case COMMAND_REMOVE_ALL:
		{
			g_ModelMgr.RemoveAllModels();
			m_GuiMgr.UpdateModelGui();

			break;
		}

	case COMMAND_PREV_MODEL:
		{
			g_ModelMgr.PrevModel();
			m_GuiMgr.UpdateModelGui();

			break;
		}

	case COMMAND_NEXT_MODEL:
		{
			g_ModelMgr.NextModel();
			m_GuiMgr.UpdateModelGui();

			break;
		}

	case COMMAND_TOGGLE_COMMANDGUI:
		{
			m_GuiMgr.ToggleControlsGui();
			break;
		}

	case COMMAND_NEXT_BKGROUND_COLOR:
		{
			m_BackgroundMgr.IncColor();
			break;
		}

	case COMMAND_TOGGLE_BOXES:
		{
			g_ModelMgr.ToggleDebugBoxes();
			break;
		}

	default:
		break;
	}
}


//---------------------------------------------------------------------------
//	 CLTClientShell::OnCommandOff()
//
//---------------------------------------------------------------------------
void CLTClientShell::OnCommandOff( int	ActionID)
{
}






//---------------------------------------------------------------------------
//	 Name:		 PollInput()
//
//	 Purpose:	Every	update, polls for	user input by checking command states.
//
//	 Returns:	 An LTRESULT
//---------------------------------------------------------------------------
LTRESULT	CLTClientShell::PollInput()
{

	bool bMoved	= false;
	bool bRotated = false;

	float	fFrameTime = g_pLTClient->GetFrameTime();

	//	Get player position.
	LTVector	vPos;
	float offsets[3];


	g_pLTClient->GetObjectPos(m_hPlayer, &vPos);

	//	Get player rotation.
	LTRotation rRot;
	g_pLTClient->GetObjectRotation(m_hPlayer,	&rRot);

	//	Get player rotation vectors.
	LTVector	vUp, vRight, vForward;
	g_pLTCCommon->GetRotationVectors(rRot,	vUp, vRight, vForward);

	// Get our new look offset 
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


	//	move forward
	if	(g_pLTClient->IsCommandOn(COMMAND_MOVE_FORWARD))
	{
		LTVector	vTemp	= vForward * fFrameTime	* 500.0f;
		vPos += vTemp;
		bMoved =	true;
	}

	//	move backward
	if	(g_pLTClient->IsCommandOn(COMMAND_MOVE_BACKWARD))
	{
		LTVector	vTemp	= vForward * fFrameTime	* -500.0f;
		vPos += vTemp;
		bMoved =	true;
	}

	//	move right
	if	(g_pLTClient->IsCommandOn(COMMAND_MOVE_RIGHT))
	{
		LTVector	vTemp	= vRight	* fFrameTime *	500.0f;
		vPos += vTemp;
		bMoved =	true;
	}

	//	move left
	if	(g_pLTClient->IsCommandOn(COMMAND_MOVE_LEFT))
	{
		LTVector	vTemp	= vRight	* fFrameTime *	-500.0f;
		vPos += vTemp;
		bMoved =	true;
	}

	//	move up
	if	(g_pLTClient->IsCommandOn(COMMAND_MOVE_UP))
	{
		LTVector	vTemp	= vUp	* fFrameTime *	500.0f;
		vPos += vTemp;
		bMoved =	true;
	}

	//	move down
	if	(g_pLTClient->IsCommandOn(COMMAND_MOVE_DOWN))
	{
		LTVector	vTemp	= vUp	* fFrameTime *	-500.0f;
		vPos += vTemp;
		bMoved =	true;
	}

	//	turn right
	if	(g_pLTClient->IsCommandOn(COMMAND_TURN_RIGHT))
	{
		m_fYaw += fFrameTime	* 1.5f;
		bRotated	= true;
	}


	//	turn left
	if	(g_pLTClient->IsCommandOn(COMMAND_TURN_LEFT))
	{
		m_fYaw -= fFrameTime	* 1.5f;
		bRotated	= true;
	}

	//	look up
	if	(g_pLTClient->IsCommandOn(COMMAND_LOOK_UP))
	{
		m_fPitch	+=	fFrameTime * 1.5f;
		bRotated	= true;
	}

	//	look down
	if	(g_pLTClient->IsCommandOn(COMMAND_LOOK_DOWN))
	{
		m_fPitch	+=	fFrameTime * -1.5f;
		bRotated	= true;
	}


	//	re-center the vertical view
	if	(g_pLTClient->IsCommandOn(COMMAND_CENTER_VIEW))
	{
		m_fPitch	= 0.0f;
		bRotated	= true;
	}

	//	set the player	position	back to the	start	position
	if	(g_pLTClient->IsCommandOn(COMMAND_RESET_POS))
	{
		m_fYaw =	0.0f;
		m_fPitch	= 0.0f;
		m_fRoll = 0.0f;
		bRotated	= true;

		vPos = m_vPlayerStartPos;
		bMoved =	true;
	}

	if	(bMoved)
	{
		//Move the client
		g_pLTClient->SetObjectPos(m_hPlayer, &vPos);
	}

	if	(bRotated)
	{
		//Set	the orientation of the client	object and camera	object
		g_pLTCCommon->SetupEuler(rRot, m_fPitch, m_fYaw, m_fRoll);
		g_pLTClient->SetObjectRotation(m_hPlayer,	&rRot);
		g_pLTClient->SetObjectRotation(m_hCamera,	&rRot);
	}

	return LT_OK;
}


//-----------------------------------------------------------------------------
//	 CLTClientShell::SpecialEffectNotify(HLOCALOBJ hObj, ILTMessage_Read	*pMessage)
//
//-----------------------------------------------------------------------------
void CLTClientShell::SpecialEffectNotify(HLOCALOBJ	hObj,	ILTMessage_Read *pMessage)
{
	g_pLTClient->CPrint("SpecialEffectNotify(%p,%p)", hObj, pMessage);
	pMessage->SeekTo(0);
	uint8 messageID = pMessage->Readuint8();

	switch(messageID)
	{
	case	0:
		break;
	default:
		break;
	}

}


//-----------------------------------------------------------------------------
//	 CLTClientShell::OnEngineTerm()
//
//-----------------------------------------------------------------------------
void CLTClientShell::OnEngineTerm()
{
	m_GuiMgr.Term();
	g_ModelMgr.Term();
	m_Crosshair.Term();
}
