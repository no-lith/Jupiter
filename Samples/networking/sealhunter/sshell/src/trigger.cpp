//------------------------------------------------------------------------------//
//
// MODULE	: Trigger.cpp
//
// PURPOSE	: Trigger - Implementation
//
// CREATED	: 10/23/2002
//
// (c) 2002 LithTech, Inc.	All Rights Reserved
//
//------------------------------------------------------------------------------//

#include "trigger.h"

#include <ltobjectcreate.h>
#include <iltmessage.h>
#include <iltcommon.h>
#include "msgids.h"
#include "serverinterfaces.h"



//-----------------------------------------------------------------------------
BEGIN_CLASS(Trigger)
ADD_STRINGPROP_FLAG(Target, "", PF_OBJECTLINK)
END_CLASS_DEFAULT_FLAGS(Trigger, BaseClass, LTNULL, LTNULL, CF_ALWAYSLOAD| CF_WORLDMODEL)



//-----------------------------------------------------------------------------
//	Trigger::EngineMessageFn(uint32 messageID, void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Trigger::EngineMessageFn(uint32 messageID, void *pData, float fData)
{
	switch (messageID)
	{
	case MID_PRECREATE:
		return PreCreate(pData, fData);
    //case MID_TOUCHNOTIFY:
    //   return TouchNotify(pData, fData);

	default:
		break;
	}

	// Pass the message along to parent class.
	return BaseClass::EngineMessageFn(messageID, pData, fData);
}



//-----------------------------------------------------------------------------
//	Trigger::PreCreate(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Trigger::PreCreate(void *pData, float fData)
{
	// Let parent class handle it first
	BaseClass::EngineMessageFn(MID_PRECREATE, pData, fData);

	// Cast pData to a ObjectCreateStruct* for convenience
	ObjectCreateStruct* pStruct = (ObjectCreateStruct*)pData;

	// Set the object type to OT_NORMAL
	pStruct->m_ObjectType = OT_WORLDMODEL;

    pStruct->m_Flags |= FLAG_VISIBLE | FLAG_SOLID | FLAG_BOXPHYSICS;//FLAG_TOUCH_NOTIFY;

	// Check to see if this is coming from a world file
	if(fData == PRECREATE_WORLDFILE)
	{
		ReadProps(pStruct);
	}

	// Return default of 1
	return 1;
}



//-----------------------------------------------------------------------------
//	Trigger::TouchNotify(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Trigger::TouchNotify(void *pData, float fData)
{
    g_pLTServer->CPrint("(Trigger) Touch!");
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
//	Trigger::PingTarget()
//
//-----------------------------------------------------------------------------
void Trigger::PingTarget()
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
//	Trigger::ReadProps(ObjectCreateStruct* pStruct)
//
//-----------------------------------------------------------------------------
void Trigger::ReadProps(ObjectCreateStruct* pStruct)
{
	g_pLTServer->GetPropString("Name", pStruct->m_Filename, MAX_CS_FILENAME_LEN);
	//g_pLTServer->GetPropString("Target", m_sTarget, 32);
}
