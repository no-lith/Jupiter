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

#include "Trigger.h"

#include <ltobjectcreate.h>
#include <iltmessage.h>
#include <iltcommon.h>
#include "msgids.h"
#include "serverinterfaces.h"

//-----------------------------------------------------------------------------
BEGIN_CLASS(Trigger)
ADD_STRINGPROP_FLAG(Target, "", PF_OBJECTLINK)
ADD_STRINGPROP_FLAG(TargetEnd0, "", PF_OBJECTLINK)
ADD_STRINGPROP_FLAG(TargetEnd1, "", PF_OBJECTLINK)
ADD_STRINGPROP_FLAG(TargetEnd2, "", PF_OBJECTLINK)
ADD_LONGINTPROP(Intensity, 0)
END_CLASS_DEFAULT_FLAGS(Trigger, BaseClass, LTNULL, LTNULL, CF_ALWAYSLOAD| CF_WORLDMODEL)

#define NUM_TRG_MUSIC	4

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
    case MID_TOUCHNOTIFY:
        return TouchNotify(pData, fData);
		
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
	

	pStruct->m_ObjectType = OT_WORLDMODEL;

    pStruct->m_Flags |= FLAG_TOUCH_NOTIFY;
	
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
	
	// Send the a message to the client to play the "triggered" intensity (set in DEdit)
	ILTMessage_Write *pMsg;
	LTRESULT nResult = g_pLTSCommon->CreateMessage(pMsg);
	pMsg->IncRef();
	pMsg->Writeuint8(MSG_TRG_PLAYMUSIC);
	pMsg->Writeint32(m_nIntensity);
	g_pLTServer->SendToClient(pMsg->Read(), LTNULL,  MESSAGE_GUARANTEED);
	pMsg->DecRef();


	// Enable touch notify flags for all triggers (named Trigger#)
	ObjArray <HOBJECT, 1> pStartPt;
	char szTrgName[32] = {0};
	for(int i = 0; i < NUM_TRG_MUSIC; i++)
	{
		pStartPt.Reset();
		sprintf(szTrgName, "Trigger%d", i);
		g_pLTServer->FindNamedObjects(szTrgName, pStartPt);
		
		if(!pStartPt.NumObjects())
		{
			continue;
		}
		// Set the touch notify flag
		g_pLTSCommon->SetObjectFlags(pStartPt.GetObject(0), OFT_Flags, FLAG_TOUCH_NOTIFY, FLAG_TOUCH_NOTIFY);
	}

	// Disable this trigger object's touch notify flag (so that repeated intensity changes are avoided)
	g_pLTSCommon->SetObjectFlags(m_hObject, OFT_Flags, 0,  FLAG_TOUCH_NOTIFY);
	
	pStartPt.Reset();

	// Find the trigger's target FX to be created
	g_pLTServer->FindNamedObjects(m_sTarget, pStartPt);
	if(pStartPt.NumObjects())
	{
		//Send a message to the ServerSpecialFX to be activated (Turn On)
		ILTMessage_Write *pMsg2;
		nResult = g_pLTSCommon->CreateMessage(pMsg2);
		pMsg2->IncRef();
		pMsg2->Writeuint8(MSG_OBJ_STARTFX);
		g_pLTServer->SendToObject(pMsg2->Read(), m_hObject, pStartPt.GetObject(0),  MESSAGE_GUARANTEED);
		pMsg->DecRef();
	}
	
	// Turn off the FX that are not supposed to be playing
	for(int n = 0; n < 3; n++)
	{
		pStartPt.Reset();
		g_pLTServer->FindNamedObjects(m_sTargetEnd[n], pStartPt);
		if(pStartPt.NumObjects())
		{
			//Send a message to the ServerSpecialFX to removed (Turn Off)
			ILTMessage_Write *pMsg;
			nResult = g_pLTSCommon->CreateMessage(pMsg);
			pMsg->IncRef();
			pMsg->Writeuint8(MSG_OBJ_STOPFX);
			g_pLTServer->SendToObject(pMsg->Read(), m_hObject, pStartPt.GetObject(0),  MESSAGE_GUARANTEED);
			pMsg->DecRef();

			//Send a message to the ClientFXMgr to remove the FX
			ILTMessage_Write *pMsg2;
			nResult = g_pLTSCommon->CreateMessage(pMsg2);
			pMsg2->IncRef();
			pMsg2->Writeuint8(SFX_CLIENTFXSTOPFX);
			pMsg2->WriteObject(pStartPt.GetObject(0));
			LTVector pos;
			g_pLTServer->SendSFXMessage(pMsg2->Read(), pos, 0);
			pMsg->DecRef();
		}	
	}
    return 1;
}

//-----------------------------------------------------------------------------
//	Trigger::PingTarget()
//
//-----------------------------------------------------------------------------
void Trigger::PingTarget()
{
	ILTMessage_Write *pMsg;
	LTRESULT nResult = g_pLTSCommon->CreateMessage(pMsg);
	pMsg->IncRef(); 
	pMsg->Writeint8(MSG_OBJ_PINGOBJECT);
    g_pLTServer->SendToObject(pMsg->Read(), m_hObject, m_hTarget, 0);
	pMsg->DecRef();    
    
}



//-----------------------------------------------------------------------------
//	Trigger::ReadProps(ObjectCreateStruct* pStruct)
//
//-----------------------------------------------------------------------------
void Trigger::ReadProps(ObjectCreateStruct* pStruct)
{
	g_pLTServer->GetPropString("Name", pStruct->m_Filename, MAX_CS_FILENAME_LEN);
	g_pLTServer->GetPropString("Target", m_sTarget, 32);
	g_pLTServer->GetPropString("TargetEnd0", m_sTargetEnd[0], 32);
	g_pLTServer->GetPropString("TargetEnd1", m_sTargetEnd[1], 32);
	g_pLTServer->GetPropString("TargetEnd2", m_sTargetEnd[2], 32);
	g_pLTServer->GetPropLongInt("Intensity", &m_nIntensity);
}
