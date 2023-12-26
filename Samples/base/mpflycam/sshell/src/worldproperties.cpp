//------------------------------------------------------------------------------//
// 
// MODULE   : worldproperties.cpp
// 
// PURPOSE  : WorldProperties - Implementation
// 
// CREATED  : 01/14/2002
// 
// (c) 2002 LithTech, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//


// Definition of this class
#include "WorldProperties.h"

// The sample defined ServerShell
#include "ltservershell.h"

// Global pointers to server interfaces
#include "serverinterfaces.h"

// Message IDs
#include "msgids.h"

// Engine includes
// Object Create Struct definition
#include <ltobjectcreate.h>

// ILTCommon definition
#include <iltcommon.h>

extern CLTServerShell *g_pSShell;


//-----------------------------------------------------------------------------
BEGIN_CLASS(WorldProperties)
	ADD_LONGINTPROP_FLAG(FarZ, 10000, PF_DISTANCE)
	ADD_COLORPROP_FLAG(BackgroundColor, 0.0f, 0.0f, 0.0f, NULL)
	PROP_DEFINEGROUP(FogProps, PF_GROUP(1))
		ADD_BOOLPROP_FLAG(FogEnable, false, PF_GROUP(1))
		ADD_COLORPROP_FLAG(FogColor, 1.0f, 1.0f, 1.0f, PF_GROUP(1))
		ADD_LONGINTPROP_FLAG(FogNearZ, 0, PF_DISTANCE | PF_GROUP(1))
		ADD_LONGINTPROP_FLAG(FogFarZ, 2000, PF_DISTANCE | PF_GROUP(1))
		ADD_BOOLPROP_FLAG(SkyFogEnable, false, PF_GROUP(1))
		ADD_LONGINTPROP_FLAG(SkyFogNearZ, 0, PF_DISTANCE | PF_GROUP(1))
		ADD_LONGINTPROP_FLAG(SkyFogFarZ, 2000, PF_DISTANCE | PF_GROUP(1))
	PROP_DEFINEGROUP(SkyPanProps, PF_GROUP(2))
		ADD_BOOLPROP_FLAG(SkyPanEnable, false, PF_GROUP(2))
		ADD_STRINGPROP_FLAG(SkyPanTexture, "", PF_FILENAME | PF_GROUP(2))
		ADD_LONGINTPROP_FLAG(SkyPanAutoPanX, 0, PF_DISTANCE | PF_GROUP(2))
		ADD_LONGINTPROP_FLAG(SkyPanAutoPanZ, 0, PF_DISTANCE | PF_GROUP(2))
		ADD_REALPROP_FLAG(SkyPanScaleX, 1.0, PF_GROUP(2))
		ADD_REALPROP_FLAG(SkyPanScaleZ, 1.0, PF_GROUP(2))
		ADD_REALPROP_FLAG(SkyScale, 1.0, NULL)
END_CLASS_DEFAULT_FLAGS(WorldProperties, BaseClass, NULL, NULL, CF_ALWAYSLOAD)


//-----------------------------------------------------------------------------
//	WorldProperties::WorldProperties()
//
//-----------------------------------------------------------------------------
WorldProperties::WorldProperties() : BaseClass(),
m_bFirstUpdate(true),
m_nFarZ(10000),
m_vBackgroundColor(0.0f,0.0f,0.0f),
m_bFogEnable(false),
m_vFogColor(1.0f,1.0f,1.0f),
m_nFogNearZ(0),
m_nFogFarZ(2000),
m_bSkyFogEnable(false),
m_nSkyFogNearZ(0),
m_nSkyFogFarZ(2000),
m_bSkyPanEnable(false),
//m_szSkyPanTexture(NULL),
m_nSkyPanAutoPanX(0),
m_nSkyPanAutoPanZ(0),
m_fSkyPanScaleX(1.0f),
m_fSkyPanScaleZ(1.0f),
m_fSkyScale(1.0f)
{}


//-----------------------------------------------------------------------------
//	WorldProperties::~WorldProperties()
//
//-----------------------------------------------------------------------------
WorldProperties::~WorldProperties()		
{
    g_pSShell->UnRegisterObject(this);
}


//-----------------------------------------------------------------------------
//	WorldProperties::EngineMessageFn(uint32 messageID,
//					  void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 WorldProperties::EngineMessageFn(uint32 messageID, void *pData, float fData)
{
	//	g_pLTServer->CPrint("WorldProperties::EngineMessageFn(%s, ..., ...)", 
	//		ENGINEMESSAGEID_TO_STRING(messageID));
	
	switch (messageID)
	{
	case MID_PRECREATE:
		return PreCreate(pData, fData);
		
	case MID_OBJECTCREATED:
		ObjectCreated();
		break;
		
		// Handle general game updates
	case MID_UPDATE:
		Update();
		break;
		
	default:
		break;
	}
	
	// Pass the message along to parent class.
	return BaseClass::EngineMessageFn(messageID, pData, fData);
}


//-----------------------------------------------------------------------------
//	WorldProperties::PreCreate(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 WorldProperties::PreCreate(void *pData, float fData)
{
	// Let parent class handle it first
	BaseClass::EngineMessageFn(MID_PRECREATE, pData, fData);
	
	// Cast pData to a ObjectCreateStruct* for convenience
	ObjectCreateStruct* pStruct = (ObjectCreateStruct*)pData;
	
	// Set the object type to OT_NORMAL
	pStruct->m_ObjectType = OT_NORMAL;
	
	// Force this to update on all clients
	pStruct->m_Flags = FLAG_FORCECLIENTUPDATE;
	
	// Check to see if this is coming from a world file
	if(fData == PRECREATE_WORLDFILE)	  
	{
		ReadProps(pStruct); 	// Read in props
		PostReadProps();		// Process props (console strings, etc)
	}
	
	// Return default of 1
	return 1;
}


//-----------------------------------------------------------------------------
//	WorldProperties::ReadProps(ObjectCreateStruct* pStruct)
//
//-----------------------------------------------------------------------------
void WorldProperties::ReadProps(ObjectCreateStruct* pStruct)
{
	
	g_pLTServer->GetPropString("Name", pStruct->m_Filename, MAX_CS_FILENAME_LEN);
	
	g_pLTServer->GetPropLongInt("FarZ", (int32*)&m_nFarZ);
	g_pLTServer->GetPropColor("BackgroundColor", &m_vBackgroundColor);
	
	g_pLTServer->GetPropBool("FogEnable", &m_bFogEnable);
	g_pLTServer->GetPropColor("FogColor", &m_vFogColor);
	g_pLTServer->GetPropLongInt("FogNearZ", (int32*)&m_nFogNearZ);
	g_pLTServer->GetPropLongInt("FogFarZ", (int32*)&m_nFogFarZ);
	
	g_pLTServer->GetPropBool("SkyFogEnable", &m_bSkyFogEnable);
	g_pLTServer->GetPropLongInt("SkyFogNearZ", (int32*)&m_nSkyFogNearZ);
	g_pLTServer->GetPropLongInt("SkyFogFarZ", (int32*)&m_nSkyFogFarZ);
	
	g_pLTServer->GetPropBool("SkyPanEnable", &m_bSkyPanEnable);
	/*
	//	if (m_szSkyPanTexture)
	g_pLTServer->GetPropString("SkyPanTexture", m_szSkyPanTexture, 255);
	
	g_pLTServer->GetPropLongInt("SkyPanAutoPanX", (int32*)&m_nSkyPanAutoPanX);
	g_pLTServer->GetPropLongInt("SkyPanAutoPanZ", (int32*)&m_nSkyPanAutoPanZ);
	g_pLTServer->GetPropReal("SkyPanScaleX", &m_fSkyPanScaleX);
	g_pLTServer->GetPropReal("SkyPanScaleZ", &m_fSkyPanScaleZ);
	*/
	g_pLTServer->GetPropReal("SkyScale", &m_fSkyScale);
	
}


//-----------------------------------------------------------------------------
//	WorldProperties::PostReadProps()
//
//-----------------------------------------------------------------------------
void WorldProperties::PostReadProps()
{
    
}


//-----------------------------------------------------------------------------
//	WorldProperties::SendToClient()
//
//-----------------------------------------------------------------------------
void WorldProperties::SendToClient()
{
	
	ILTMessage_Write *pMsg;
	LTRESULT nResult = g_pLTSCommon->CreateMessage(pMsg);
	
	if( LT_OK == nResult )
	{
		pMsg->IncRef();
		pMsg->Writeuint8(MSG_WORLD_PROPS);
		pMsg->Writeuint32(m_nFarZ);
		pMsg->WriteLTVector(m_vBackgroundColor);
		pMsg->Writebool(m_bFogEnable);
		pMsg->WriteLTVector(m_vFogColor);
		pMsg->Writeuint32(m_nFogNearZ);
		pMsg->Writeuint32(m_nFogFarZ);
		pMsg->Writebool(m_bSkyPanEnable);
		pMsg->Writeuint32(m_nSkyFogNearZ);
		pMsg->Writeuint32(m_nSkyFogFarZ);
		/*
		pMsg->Writebool(m_bSkyPanEnable);
		pMsg->Writeuint32(m_nSkyPanAutoPanX);
		pMsg->Writeuint32(m_nSkyPanAutoPanZ);
		pMsg->Writefloat(m_fSkyPanScaleX);
		pMsg->Writefloat(m_fSkyPanScaleZ);
		*/
		pMsg->Writefloat(m_fSkyScale);
		//pMsg->WriteString(m_szSkyPanTexture);
		g_pLTServer->SendToClient(pMsg->Read(), LTNULL, MESSAGE_GUARANTEED);
		
		pMsg->DecRef();
	}
	
}


//-----------------------------------------------------------------------------
//	WorldProperties::ObjectCreated()
//
//-----------------------------------------------------------------------------
void WorldProperties::ObjectCreated()
{
	g_pSShell->RegisterObject(this);	
	g_pLTServer->SetNextUpdate(m_hObject,0.01f);
}


//-----------------------------------------------------------------------------
//	WorldProperties::Update()
//
//-----------------------------------------------------------------------------
void WorldProperties::Update()
{
  
	g_pLTServer->SetNextUpdate(m_hObject, 0.01f);
	
	if(m_bFirstUpdate)
	{
		m_bFirstUpdate = false;
	}

}


