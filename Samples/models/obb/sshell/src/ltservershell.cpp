//****************************************************************************
//
//  Filename:   ltservershell.cpp
//
//  Class:      CLTServerShell - Class implementation
//
//  Created:    2002/03/02
//
//****************************************************************************/

#include "ltservershell.h"
#include "serverinterfaces.h"


#include <iltcommon.h>
#include <ltobjectcreate.h>
#include <iltphysics.h>



SETUP_SERVERSHELL()

// Define our instantiation of the IServerShell interface.
define_interface(CLTServerShell, IServerShell);

CLTServerShell *	g_pSShell;



//-------------------------------------------------------------------------------------------------
// CLTServerShell
//-------------------------------------------------------------------------------------------------

CLTServerShell::CLTServerShell()
	: m_hClient(NULL),
	  m_pClientObj(NULL),
	  m_hClientHandle(NULL)
{
}



//-----------------------------------------------------------------------------
//  Name:       OnServerInitialized()
//
//  Purpose:	This function is called after the IServerShell implementation
//				class has been allocated in game code, and before any other
//				IServerShell function.
//
//  Returns:	LT_OK if no errors were encountered.
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

	return LT_OK;
}



//----------------------------------------------------------------------------
//  Name:       OnClientEnterWorld()
//
//  Purpose:	Called after the client has actually entered the world
//				ie. after OnAddClient(). An object to represent the client
//				must be created and returned from here.
//
//  Returns:	A BaseClass pointer to the newly created client object.
//
//----------------------------------------------------------------------------
LPBASECLASS CLTServerShell::OnClientEnterWorld(HCLIENT hClient)
{
  	ObjectCreateStruct objCreateStruct;
  	HCLASS hClass;
  	LTVector vPos, vStartPos;
  	LTRotation rStartRot;

	hClass = g_pLTServer->GetClass("BaseClass");

	if (!hClass)
	{
		assert(!"ERROR: Couldn't get BaseClass!!");
		return NULL;
	}

	objCreateStruct.Clear();
	objCreateStruct.m_ObjectType = OT_NORMAL;

	objCreateStruct.m_Flags = FLAG_SOLID | FLAG_FORCECLIENTUPDATE;

	//Setup starting position and rotation
	objCreateStruct.m_Pos.Init(0.0f, 0.0f, 0.0f);
	objCreateStruct.m_Rotation.Init();

	//Setup default position and rotation vars to send to client
	vStartPos = objCreateStruct.m_Pos;
	rStartRot = objCreateStruct.m_Rotation;

	ObjArray<HOBJECT, 1> objArray;
	uint32 nNumObjects;
	g_pLTServer->FindNamedObjects("GameStartPoint0", objArray ,&nNumObjects);
	if(nNumObjects)
	{
		HOBJECT hObj = objArray.GetObject(0);
		g_pLTServer->GetObjectPos(hObj, &objCreateStruct.m_Pos);
		g_pLTServer->GetObjectRotation(hObj, &objCreateStruct.m_Rotation);
		vStartPos = objCreateStruct.m_Pos;
		rStartRot = objCreateStruct.m_Rotation;
	}

	// Create our client object
	m_pClientObj = g_pLTServer->CreateObject(hClass, &objCreateStruct);

	// Store our client
	m_hClient = hClient;

	if (m_pClientObj)
	{
		g_pLTServer->SetClientUserData(hClient, m_pClientObj);
		m_hClientHandle = g_pLTServer->ObjectToHandle(m_pClientObj);
		vPos.x = vPos.y = vPos.z = 10.0f;
		g_pLTSPhysics->SetObjectDims(m_hClientHandle, &vPos, 0);
	}

	// Send over the position of the startpoint so the client can position the player
	ILTMessage_Write *pMsg;
	LTRESULT nResult = g_pLTSCommon->CreateMessage(pMsg);
	pMsg->IncRef();
	pMsg->Writeint8(MSG_STARTPOINT_POS_ROT);
	pMsg->WriteLTVector(vStartPos);
	pMsg->WriteLTRotation(rStartRot);
	g_pLTServer->SendToClient(pMsg->Read(), hClient,  MESSAGE_GUARANTEED);
	pMsg->DecRef();

	return m_pClientObj;
}



//----------------------------------------------------------------------------
//  Name:       OnClientExitWorld()
//
//  Purpose:	Called when client leaves the world (just before the engine
//				tells it to unload the world).
//
//  Returns:	void
//
//----------------------------------------------------------------------------
void CLTServerShell::OnClientExitWorld(HCLIENT hClient)
{
}



//----------------------------------------------------------------------------
//  CLTServerShell::OnMessage(HCLIENT hSender, ILTMessage_Read *pMessage)
//
//----------------------------------------------------------------------------
void CLTServerShell::OnMessage(HCLIENT hSender, ILTMessage_Read *pMessage)
{
	//g_pLTServer->CPrint("OnMessage(%p,%p)", hSender, pMessage);
}
