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

#include "playersrvr.h"

#include "projectile.h"

//-----------------------------------------------------------------------------
SETUP_SERVERSHELL()

// Define our instantiation of the IServerShell interface.
define_interface(CLTServerShell, IServerShell);

CLTServerShell *g_pSShell;

CLTServerShell::CLTServerShell()
{}


//-----------------------------------------------------------------------------
//  CLTServerShell::OnServerInitialized()
//
//-----------------------------------------------------------------------------
LTRESULT CLTServerShell::OnServerInitialized()
{
	g_pLTServer->CPrint("CLTServerShell::OnServerInitialized()");
	LTRESULT result;

	// Set the pointer to our server shell
	g_pSShell = this;

	// Verify that all server interfaces initialized correctly.
	result = VerifyServerInterfaces();
	if (LT_OK != result)
	{
		return result;
	}

    g_pLTSPhysics->SetStairHeight(16.0f);

	return LT_OK;
}


//-----------------------------------------------------------------------------
//  CLTServerShell::OnServerTerm()
//
//-----------------------------------------------------------------------------
void CLTServerShell::OnServerTerm()
{
	g_pLTServer->CPrint("CLTServerShell::OnServerTerm()");
}

//-----------------------------------------------------------------------------
//  CLTServerShell::SRand(unsigned int uiRand)
//
//-----------------------------------------------------------------------------
void CLTServerShell::SRand(unsigned int uiRand)
{
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
	g_pLTServer->CPrint("CLTServerShell::OnClientEnterWorld(HCLIENT: %p)", hClient);

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
    objCreateStruct.m_Flags = FLAG_SOLID | FLAG_FORCECLIENTUPDATE | FLAG_YROTATION | FLAG_VISIBLE;

    objCreateStruct.m_Flags2 |= FLAG2_PLAYERCOLLIDE;

    strcpy(objCreateStruct.m_Filename, "Models\\player_action.ltb");
    strcpy(objCreateStruct.m_SkinNames[0], "ModelTextures\\CateAction.dtx");
    strcpy(objCreateStruct.m_SkinNames[1], "ModelTextures\\CateActionHead.dtx");
    strcpy(objCreateStruct.m_SkinNames[2], "ModelTextures\\CateActionHair.dtx");
    strcpy(objCreateStruct.m_SkinNames[3], "ModelTextures\\CateActionLash.dtx");

	// Set client's pos and rot according to the "StartPoint", if one exists.
	this->FindStartPoint(vStartPos, rStartRot);
	objCreateStruct.m_Pos = vStartPos;
	objCreateStruct.m_Rotation = rStartRot;

	// Create our client object
	CPlayerSrvr *pClientObj = (CPlayerSrvr*)g_pLTServer->CreateObject(hClass, &objCreateStruct);

	if (pClientObj)
	{

        LTVector vTPos;
        g_pLTServer->GetObjectPos(pClientObj->m_hObject, &vTPos);
        g_pLTServer->CPrint("SERVER Player Pos: %f %f %f", vTPos.x, vTPos.y, vTPos.z);

		g_pLTServer->SetClientUserData(hClient, pClientObj);
		HOBJECT hClientHandle = g_pLTServer->ObjectToHandle(pClientObj);

        g_pLTServer->CPrint("ClientHandle: %p", hClientHandle);

        uint32 clientID = g_pLTServer->GetClientID(hClient);
        g_pLTServer->CPrint("ClientID %d", clientID);
        char name[16];
        memset(name, 0, 15);
        sprintf(name, "Player %d", clientID);
        pClientObj->SetPlayerName(name);

        LTVector vDims;
        g_pLTSCommon->GetModelAnimUserDims(hClientHandle, &vDims, g_pLTServer->GetModelAnimation(hClientHandle));
		g_pLTSPhysics->SetObjectDims(hClientHandle, &vDims, 0);

	}

	// Send the props to the client
	if( m_pWorldProperties )
		m_pWorldProperties->SendToClient(hClient);
	else
		g_pLTServer->CPrint("WARNING: Invalid WorldProps object, can't send world props");

	// Send over the position of the startpoint so the client can position the player
	ILTMessage_Write *pMsg;
	LTRESULT nResult = g_pLTSCommon->CreateMessage(pMsg);
	pMsg->IncRef();
	pMsg->Writeint8(MSG_STARTPOINT_POS_ROT);
	pMsg->WriteLTVector(vStartPos);
	pMsg->WriteLTRotation(rStartRot);
	g_pLTServer->SendToClient(pMsg->Read(), hClient,  MESSAGE_GUARANTEED);
	pMsg->DecRef();

	return pClientObj;
}


//----------------------------------------------------------------------------
//  CLTServerShell::OnClientExitWorld(HCLIENT hClient)
//
//----------------------------------------------------------------------------
void CLTServerShell::OnClientExitWorld(HCLIENT hClient)
{
	g_pLTServer->CPrint("CLTServerShell::OnClientExitWorld(%p)", hClient);
    BaseClass *pBaseClass = (BaseClass *)g_pLTServer->GetClientUserData(hClient);
	HOBJECT hPlayer = g_pLTServer->ObjectToHandle(pBaseClass);
    g_pLTServer->RemoveObject(hPlayer);
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
			LTVector vVel, vPos;
			vVel = pMessage->ReadLTVector();
			vPos = pMessage->ReadLTVector();
			g_pLTSPhysics->SetVelocity(hPlayer, &vVel);
			LTVector vZero(0.0f, 0.0f, 0.0f);
			g_pLTSPhysics->SetAcceleration(hPlayer, &vZero);
            g_pLTServer->MoveObject(hPlayer, &vPos);

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
			LTVector vVel, vPos;
			LTRotation rRot;
			vVel = pMessage->ReadLTVector();
			vPos = pMessage->ReadLTVector();
			rRot = pMessage->ReadLTRotation();
			g_pLTSPhysics->SetVelocity(hPlayer, &vVel);
			LTVector vZero(0.0f, 0.0f, 0.0f);
			g_pLTSPhysics->SetAcceleration(hPlayer, &vZero);

            g_pLTServer->SetObjectPos(hPlayer, &vPos);			
            //g_pLTServer->MoveObject(hPlayer, &vPos);			
            g_pLTServer->SetObjectRotation(hPlayer, &rRot);

			g_pLTServer->SetClientViewPos(hSender, &vPos);
		}
		break;
    case MSG_CS_SHOOT:
        {
            LTVector vPos;
            LTRotation rRot;
            
            g_pLTServer->GetObjectPos(hPlayer, &vPos);
            g_pLTServer->GetObjectRotation(hPlayer, &rRot);
            
            // Create Projectile
            HCLASS hClass = g_pLTServer->GetClass("Projectile");
            
            ObjectCreateStruct ocs;
            ocs.m_ObjectType = OT_MODEL;
            ocs.m_Flags      = FLAG_VISIBLE;
            ocs.m_Rotation   = rRot;
            vPos += rRot.Forward() * 25.f;
            ocs.m_Pos        = vPos;
            
            strncpy(ocs.m_Filename, "Models/GenCan.ltb", MAX_CS_FILENAME_LEN -1);
            strncpy(ocs.m_SkinName, "ModelTextures/GenCan1.dtx", MAX_CS_FILENAME_LEN -1);
            Projectile* pObject = (Projectile*)g_pLTServer->CreateObject(hClass, &ocs);
            
            if(pObject)
            {
                g_pLTServer->CPrint("Shoot!!!");
                pObject->SetOwner(hPlayer);
            }            
        }
        break;

    case MSG_CS_ANIM:
        {

            char sAnimName[128];
            pMessage->ReadString(sAnimName, 128);
            HMODELANIM hAnim = g_pLTServer->GetAnimIndex(hPlayer, (char *)sAnimName);
            g_pLTServer->SetModelAnimation(hPlayer, hAnim);
            g_pLTServer->SetModelLooping(hPlayer, LTTRUE);

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

