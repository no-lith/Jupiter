//------------------------------------------------------------------------------//
//
// MODULE   : ltservershell.cpp
//
// PURPOSE  : CLTServerShell - Implementation
//
// CREATED  : 03/02/2002
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

// Definition of class
#include "ltservershell.h"

// Global pointers to server interfaces
#include "serverinterfaces.h"

// Msg Ids
#include "msgids.h"

//--Engine includes--

// Object Create Struct definition
#include <ltobjectcreate.h>

// ILTPhysics definition
#include <iltphysics.h>

// ILTCommon definition
#include <iltcommon.h>

//-----------------------------------------------------------------------------
SETUP_SERVERSHELL()

// Define our instantiation of the IServerShell interface.
define_interface(CLTServerShell, IServerShell);

CLTServerShell *g_pSShell;

CLTServerShell::CLTServerShell()
:
m_hClient(NULL),
m_pClientObj(NULL),
m_hClientHandle(NULL)
{}


//-----------------------------------------------------------------------------
//  CLTServerShell::OnServerInitialized()
//
//-----------------------------------------------------------------------------
LTRESULT CLTServerShell::OnServerInitialized()
{
	//g_pLTServer->CPrint("OnServerInitialized()");
	LTRESULT result;

	// Set the pointer to our server shell
	g_pSShell = this;

	// Verify that all server interfaces initialized correctly.
	result = VerifyServerInterfaces();
	if (LT_OK != result)
	{
		return result;
	}

	return LT_OK;
}


//-----------------------------------------------------------------------------
//  CLTServerShell::OnServerTerm()
//
//-----------------------------------------------------------------------------
void CLTServerShell::OnServerTerm()
{
	//g_pLTServer->CPrint("OnServerTerm()");
}

//-----------------------------------------------------------------------------
//  CLTServerShell::SRand(unsigned int uiRand)
//
//-----------------------------------------------------------------------------
void CLTServerShell::SRand(unsigned int uiRand)
{
	//g_pLTServer->CPrint("CLTServerShell::SRand");
	srand(uiRand);
}

//----------------------------------------------------------------------------
//  CLTServerShell::RegisterObject(CWorldProperties *pWorldProps)
//
//----------------------------------------------------------------------------
void CLTServerShell::RegisterObject(WorldProperties *pWorldProps)
{
	m_pWorldProperties = pWorldProps;
}


//----------------------------------------------------------------------------
//  CLTServerShell::UnRegisterObject(CWorldProperties *pWorldProps)
//
//----------------------------------------------------------------------------
void CLTServerShell::UnRegisterObject(WorldProperties *pWorldProps)
{
	m_pWorldProperties = NULL;
}


//----------------------------------------------------------------------------
// CLTServerShell::OnClientEnterWorld(HCLIENT hClient)
//
//----------------------------------------------------------------------------
LPBASECLASS CLTServerShell::OnClientEnterWorld(HCLIENT hClient)
{
	ObjectCreateStruct objCreateStruct;
	HCLASS hClass;
	LTVector vPos, vStartPos;
	LTRotation rStartRot;

	hClass = g_pLTServer->GetClass("CPlayerSrvr");

	if (!hClass)
	{
		assert(!"ERROR: Couldn't get BaseClass!!");
		return NULL;
	}

	objCreateStruct.Clear();
	objCreateStruct.m_ObjectType = OT_MODEL;
	objCreateStruct.m_Flags = FLAG_SOLID | FLAG_VISIBLE;

    strcpy(objCreateStruct.m_Filename, "Models\\GenPigeon.ltb");
    strcpy(objCreateStruct.m_SkinName, "ModelTextures\\GenPigeon.dtx");

	// Set client's pos and rot according to the "StartPoint", if one exists.
	this->FindStartPoint(vStartPos, rStartRot);
	objCreateStruct.m_Pos = vStartPos;
	objCreateStruct.m_Rotation = rStartRot;

	// Create our client object
	m_pClientObj = g_pLTServer->CreateObject(hClass, &objCreateStruct);

	// Store our client
	m_hClient = hClient;

	if (m_pClientObj)
	{
		g_pLTServer->SetClientUserData(hClient, m_pClientObj);
		m_hClientHandle = g_pLTServer->ObjectToHandle(m_pClientObj);
		vPos.x = vPos.y = vPos.z = 15.0f;
		g_pLTSPhysics->SetObjectDims(m_hClientHandle, &vPos, 0);

		HMODELANIM hAnim = g_pLTServer->GetAnimIndex( m_hClientHandle, "fly" );
		g_pLTServer->SetModelAnimation( m_hClientHandle, hAnim );
		g_pLTServer->SetModelLooping( m_hClientHandle, LTTRUE );

		// Create a Special FX message and attach it to the object
		ILTMessage_Write *pMsg;
		LTRESULT nResult = g_pLTSCommon->CreateMessage(pMsg);
		if (LT_OK == nResult)
		{
			pMsg->IncRef();
			pMsg->Writeint8(MSG_PLAYEROBJ);
			g_pLTServer->SetObjectSFXMessage(m_hClientHandle, pMsg->Read());
			pMsg->DecRef();
		}
	}

	// Send the props to the client
	if( m_pWorldProperties )
		m_pWorldProperties->SendToClient();
	else
		g_pLTServer->CPrint("WARNING: Invalid WorldProps object, can't send world props");

	// Send over the position of the startpoint so the client can position the player
	ILTMessage_Write *pMsg;
	LTRESULT nResult = g_pLTSCommon->CreateMessage(pMsg);
	pMsg->IncRef();
	pMsg->Writeint8(MSG_STARTPOINT_POS_ROT);
	pMsg->WriteLTVector(vStartPos);
	pMsg->WriteLTRotation(rStartRot);
	g_pLTServer->SendToClient(pMsg->Read(), LTNULL,  MESSAGE_GUARANTEED);
	pMsg->DecRef();

	return m_pClientObj;
}


//----------------------------------------------------------------------------
//  CLTServerShell::OnClientExitWorld(HCLIENT hClient)
//
//----------------------------------------------------------------------------
void CLTServerShell::OnClientExitWorld(HCLIENT hClient)
{
	//g_pLTServer->CPrint("OnClientExitWorld(%p)", hClient);
}


//----------------------------------------------------------------------------
//  CLTServerShell::OnMessage(HCLIENT hSender, ILTMessage_Read *pMessage)
//
//----------------------------------------------------------------------------
void CLTServerShell::OnMessage(HCLIENT hSender, ILTMessage_Read *pMessage)
{
	//g_pLTServer->CPrint("OnMessage(%p,%p)", hSender, pMessage);
	
	BaseClass *pBaseClass = (BaseClass *)g_pLTServer->GetClientUserData(hSender);
	HOBJECT hPlayer = g_pLTServer->ObjectToHandle(pBaseClass);
	
	pMessage->SeekTo(0);
	uint8 messageID = pMessage->Readuint8();
	
	switch(messageID)
	{
	case MSG_CS_VELOCITY:
		{
			//g_pLTServer->CPrint("MSG_CS_VELOCITY");
			LTVector vVel;
			vVel = pMessage->ReadLTVector();
			g_pLTSPhysics->SetVelocity(hPlayer, &vVel);
			LTVector vZero(0.0f, 0.0f, 0.0f);
			g_pLTSPhysics->SetAcceleration(hPlayer, &vZero);

			LTVector vPos;
			g_pLTServer->GetObjectPos(hPlayer, &vPos);
			g_pLTServer->SetClientViewPos(hSender, &vPos);
		}
		break;
	case MSG_CS_ROTATION:
		{
			//g_pLTServer->CPrint("MSG_CS_ROTATION");
			LTRotation rRot;
			rRot = pMessage->ReadLTRotation();
			g_pLTServer->SetObjectRotation(hPlayer, &rRot);
		}
		break;
	case MSG_CS_VELANDROT:
		{
			//g_pLTServer->CPrint("MSG_CS_VELANDROT");
			LTVector vVel;
			LTRotation rRot;
			vVel = pMessage->ReadLTVector();
			rRot = pMessage->ReadLTRotation();
			g_pLTSPhysics->SetVelocity(hPlayer, &vVel);
			LTVector vZero(0.0f, 0.0f, 0.0f);
			g_pLTSPhysics->SetAcceleration(hPlayer, &vZero);
			g_pLTServer->SetObjectRotation(hPlayer, &rRot);

			LTVector vPos;
			g_pLTServer->GetObjectPos(hPlayer, &vPos);
			g_pLTServer->SetClientViewPos(hSender, &vPos);
		}
		break;
		
		
	default:
		g_pLTServer->CPrint("Server: Received Unknown message");
		break;
	}
	
}


//----------------------------------------------------------------------------
// CLTServerShell::FindStartPoint(LTVector &vStartPtPos, 
//                                LTRotation &rStartPtRot)
//
//----------------------------------------------------------------------------
LTRESULT CLTServerShell::FindStartPoint(LTVector &vStartPtPos, 
										LTRotation &rStartPtRot)
{
	
	// Get a list of the startpoint objects with a particular name (should be 1)
	ObjArray <HOBJECT, 1> pStartPt;
	
	g_pLTServer->FindNamedObjects("GameStartPoint0", pStartPt);
	
	if (pStartPt.NumObjects() == 0)
	{
		// Fall back: Check for objects named "StartPoint".
		g_pLTServer->FindNamedObjects("StartPoint", pStartPt);
		if (pStartPt.NumObjects() == 0)
		{
			vStartPtPos.Init(0.0f, 0.0f, 0.0f);
			rStartPtRot.Init();
			g_pLTServer->CPrint("Can't find StartPoint. Using (%f, %f, %f) instead.", 
				vStartPtPos.x,
				vStartPtPos.y,
				vStartPtPos.z);
			return LT_NOTFOUND;
		}
		else
		{
			// Get startpoint's position and rotation
			g_pLTServer->GetObjectPos(pStartPt.GetObject(0), &vStartPtPos);
			g_pLTServer->GetObjectRotation(pStartPt.GetObject(0), &rStartPtRot);			
		}
	}
	else
	{
		// Get startpoint's position and rotation
		g_pLTServer->GetObjectPos(pStartPt.GetObject(0), &vStartPtPos);
		g_pLTServer->GetObjectRotation(pStartPt.GetObject(0), &rStartPtRot);					
	}
	
	return LT_OK;
	
}

//-----------------------------------------------------------------------------
//  CLTServerShell::OnAddClient(HCLIENT hClient)
//
//-----------------------------------------------------------------------------
void CLTServerShell::OnAddClient(HCLIENT hClient)
{
	//g_pLTServer->CPrint("OnAddClient(%p)", hClient);
}

//-----------------------------------------------------------------------------
//  CLTServerShell::OnRemoveClient(HCLIENT hClient)
//
//-----------------------------------------------------------------------------
void CLTServerShell::OnRemoveClient(HCLIENT hClient)
{
	//g_pLTServer->CPrint("OnRemoveClient(%p)", hClient);
}

//-----------------------------------------------------------------------------
//  CLTServerShell::PreStartWorld(bool bSwitchingWorlds)
//
//-----------------------------------------------------------------------------
void CLTServerShell::PreStartWorld(bool bSwitchingWorlds)
{
	//g_pLTServer->CPrint("PreStartWorld(%d)", bSwitchingWorlds);
}

//-----------------------------------------------------------------------------
//  CLTServerShell::PostStartWorld()
//
//-----------------------------------------------------------------------------
void CLTServerShell::PostStartWorld()
{
	//g_pLTServer->CPrint("PostStartWorld()");
}

//-----------------------------------------------------------------------------
//  CLTServerShell::Update(LTFLOAT timeElapsed)
//
//-----------------------------------------------------------------------------
void CLTServerShell::Update(LTFLOAT timeElapsed)
{
	//g_pLTServer->CPrint("Update(%g)", timeElapsed);
}

//-----------------------------------------------------------------------------
//  CLTServerShell::CacheFiles()
//
//-----------------------------------------------------------------------------
void CLTServerShell::CacheFiles()
{
	//g_pLTServer->CPrint("CacheFiles()");
}

//-----------------------------------------------------------------------------
//  CLTServerShell::FileLoadNotify(const char *pFilename, LTRESULT status)
//
//-----------------------------------------------------------------------------
LTRESULT CLTServerShell::FileLoadNotify(const char *pFilename, LTRESULT status)
{
	//g_pLTServer->CPrint("FileLoadNotify(%s, %d)", pFilename, status);

	return LT_OK;
}

//-----------------------------------------------------------------------------
//  CLTServerShell::OnPlaybackFinish()
//
//-----------------------------------------------------------------------------
void CLTServerShell::OnPlaybackFinish()
{
	//g_pLTServer->CPrint("OnPlaybackFinish()");
}

//-----------------------------------------------------------------------------
//  CLTServerShell::ProcessPacket(ILTMessage_Read *pMsg, uint8 senderAddr[4], 
//		uint16 senderPort)
//
//-----------------------------------------------------------------------------
LTRESULT CLTServerShell::ProcessPacket(ILTMessage_Read *pMsg, uint8 senderAddr[4],
										uint16 senderPort)
{
	//g_pLTServer->CPrint("ProcessPacket(%p, %p, %d)",pMsg, senderAddr, senderPort);
	
	return LT_OK;
}

