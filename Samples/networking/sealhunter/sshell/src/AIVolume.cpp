//------------------------------------------------------------------------------//
//
// MODULE	: AIVolume.cpp
//
// PURPOSE	: AIVolume - Implementation
//
// CREATED	: 10/23/2002
//
// (c) 2002 LithTech, Inc.	All Rights Reserved
//
//------------------------------------------------------------------------------//

#include "AIVolume.h"

#include <ltobjectcreate.h>
#include <iltmessage.h>
#include <iltcommon.h>
#include <iltphysics.h>
#include "msgids.h"
#include "serverinterfaces.h"
#include "seal.h"
#include "statsmanager.h"



// Maximum number of seals on the screen at a given time
#define MAX_SEALS	10


//-----------------------------------------------------------------------------
BEGIN_CLASS(AIVolume)
ADD_STRINGPROP_FLAG(Target, "", PF_OBJECTLINK)
ADD_VECTORPROP_FLAG(Dims, PF_DIMS)
END_CLASS_DEFAULT_FLAGS(AIVolume, BaseClass, LTNULL, LTNULL, CF_ALWAYSLOAD)



//-----------------------------------------------------------------------------
//	AIVolume::EngineMessageFn(uint32 messageID, void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 AIVolume::EngineMessageFn(uint32 messageID, void *pData, float fData)
{
	switch (messageID)
	{
	case MID_PRECREATE:
		return PreCreate(pData, fData);
    //case MID_TOUCHNOTIFY:
    //   return TouchNotify(pData, fData);
    case MID_INITIALUPDATE:
        {
            g_pLTServer->SetNextUpdate(m_hObject, 5.0f);
        }
        break;
		
    case MID_UPDATE:
        {
            uint32 iNumPlayers = g_pStatsManager->GetNumPlayers();

			// if there isn't any players then have a least one seal spawned 
			if ( !iNumPlayers )
			{
				if ( !m_NumSeals )
				{
					Spawn();
				}
				
				g_pLTServer->SetNextUpdate(m_hObject, 10.0f);
				
			}
			else
			{
				if (m_NumSeals < MAX_SEALS)
				{
					//spawn a new one
            		Spawn();
				}
				
				g_pLTServer->SetNextUpdate(m_hObject, 10.0f/(static_cast<float>(iNumPlayers)));
            }
        }
        break;

	default:
		break;
	}

	// Pass the message along to parent class.
	return BaseClass::EngineMessageFn(messageID, pData, fData);
}



//-----------------------------------------------------------------------------
//	AIVolume::PreCreate(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 AIVolume::PreCreate(void *pData, float fData)
{
	// Let parent class handle it first
	BaseClass::EngineMessageFn(MID_PRECREATE, pData, fData);

	// Cast pData to a ObjectCreateStruct* for convenience
	ObjectCreateStruct* pStruct = (ObjectCreateStruct*)pData;

	// Set the object type to OT_NORMAL
	pStruct->m_ObjectType = OT_NORMAL;//OT_WORLDMODEL;

    pStruct->m_Flags = 0;//FLAG_VISIBLE | FLAG_SOLID | FLAG_BOXPHYSICS;//FLAG_TOUCH_NOTIFY;

	// Check to see if this is coming from a world file
	if(fData == PRECREATE_WORLDFILE)
	{
		ReadProps(pStruct);
	}

	// Return default of 1
	return 1;
}



//-----------------------------------------------------------------------------
//	AIVolume::TouchNotify(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 AIVolume::TouchNotify(void *pData, float fData)
{
    g_pLTServer->CPrint("(AIVolume) Touch!");
    if( m_hTarget == NULL)
    {
        /*
        //Find target object
        ObjArray<HOBJECT, 1> objArray; //The code below will crash if it returns more than 1 with the same name
        uint32 dwNumObjects = 0;
        g_pLTServer->FindNamedObjects(m_sTarget, objArray, &dwNumObjects);
        if( dwNumObjects )
        {
            m_hTarget = objArray.GetObject(0);
            PingTarget();
        }
        */
    }
    else
    {
        //PingTarget();
    }



    return 1;
}



//-----------------------------------------------------------------------------
//	AIVolume::PingTarget()
//
//-----------------------------------------------------------------------------
void AIVolume::PingTarget()
{
    /*
	ILTMessage_Write *pMsg;
	LTRESULT nResult = g_pLTSCommon->CreateMessage(pMsg);
	pMsg->IncRef();
	pMsg->Writeint8(MSG_OBJ_PINGOBJECT);
    g_pLTServer->SendToObject(pMsg->Read(), m_hObject, m_hTarget, 0);
	pMsg->DecRef();
    */

}



//-----------------------------------------------------------------------------
//	AIVolume::ReadProps(ObjectCreateStruct* pStruct)
//
//-----------------------------------------------------------------------------
void AIVolume::ReadProps(ObjectCreateStruct* pStruct)
{
	g_pLTServer->GetPropString("Name", pStruct->m_Filename, MAX_CS_FILENAME_LEN);
    g_pLTServer->GetPropVector("Dims", &m_vDims);
}



//-----------------------------------------------------------------------------
//	Spawn
//
//-----------------------------------------------------------------------------
void AIVolume::Spawn()
{
    LTVector vPos;
    LTRotation rRot;
    g_pLTServer->GetObjectPos(m_hObject, &vPos);
    g_pLTServer->GetObjectRotation(m_hObject, &rRot);

    ObjectCreateStruct ocs;
    ocs.m_ObjectType = OT_MODEL;
    strcpy(ocs.m_Filename, "Models/seal.ltb");
    strcpy(ocs.m_SkinName, "ModelTextures/seal.dtx");

    LTFLOAT x = g_pLTServer->Random(-m_vDims.x, m_vDims.x);
    LTFLOAT z = g_pLTServer->Random(-m_vDims.z, m_vDims.z);

    ocs.m_Pos.y = vPos.y;

    ocs.m_Pos.x = vPos.x + x;
    ocs.m_Pos.z = vPos.z + z;

    HCLASS hClass = g_pLTServer->GetClass("Seal");

    if (g_pLTServer->CreateObject(hClass, &ocs) != LTNULL)
	{
		++m_NumSeals;
	}
}
