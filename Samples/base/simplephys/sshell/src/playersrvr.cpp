//------------------------------------------------------------------------------//
// 
// MODULE	: playersrvr.cpp
// 
// PURPOSE	: PlayerSrvr - Implementation
// 
// CREATED	: 07/15/2002
// 
// (c) 2002 LithTech, Inc.	All Rights Reserved
// 
//------------------------------------------------------------------------------//

#include "playersrvr.h"

#include <ltobjectcreate.h>

//-----------------------------------------------------------------------------
BEGIN_CLASS(CPlayerSrvr)
END_CLASS_DEFAULT_FLAGS(CPlayerSrvr, BaseClass, LTNULL, LTNULL, CF_HIDDEN)


//-----------------------------------------------------------------------------
//	CPlayerSrvr::EngineMessageFn(uint32 messageID, void *pData, float fData)
//
//-----------------------------------------------------------------------------  
uint32 CPlayerSrvr::EngineMessageFn(uint32 messageID, void *pData, float fData)
{
	
	switch (messageID)
	{
	case MID_PRECREATE:
		return PreCreate(pData, fData);
		
	default:
		break;
	}
	
	// Pass the message along to parent class.
	return BaseClass::EngineMessageFn(messageID, pData, fData);
}

//-----------------------------------------------------------------------------
//	CPlayerSrvr::PreCreate(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 CPlayerSrvr::PreCreate(void *pData, float fData)
{
	// Let parent class handle it first
	BaseClass::EngineMessageFn(MID_PRECREATE, pData, fData);
	
	// Cast pData to a ObjectCreateStruct* for convenience
	ObjectCreateStruct* pStruct = (ObjectCreateStruct*)pData;
	
	// Set the object type to OT_MODEL
	pStruct->m_ObjectType = OT_MODEL;
	
	// Check to see if this is coming from a world file
	if(fData == PRECREATE_WORLDFILE)	  
	{
		ReadProps(pStruct);
	}
	
	// Return default of 1
	return 1;
}


//-----------------------------------------------------------------------------
//	CPlayerSrvr::ReadProps(ObjectCreateStruct* pStruct)
//
//-----------------------------------------------------------------------------
void CPlayerSrvr::ReadProps(ObjectCreateStruct* pStruct)
{
	g_pLTServer->GetPropString("Name", pStruct->m_Filename, MAX_CS_FILENAME_LEN);
}
