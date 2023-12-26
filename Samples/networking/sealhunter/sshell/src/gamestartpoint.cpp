//------------------------------------------------------------------------------//
//
// MODULE	: GameStartPoint.cpp
//
// PURPOSE	: GameStartPoint - Implementation
//
// CREATED	: 06/27/2002
//
// (c) 2002 LithTech, Inc.	All Rights Reserved
//
//------------------------------------------------------------------------------//

#include "gamestartpoint.h"

#include <ltobjectcreate.h>



//-----------------------------------------------------------------------------
BEGIN_CLASS(GameStartPoint)
END_CLASS_DEFAULT_FLAGS(GameStartPoint, BaseClass, LTNULL, LTNULL, CF_ALWAYSLOAD)



//-----------------------------------------------------------------------------
//	GameStartPoint::EngineMessageFn(uint32 messageID, void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 GameStartPoint::EngineMessageFn(uint32 messageID, void *pData, float fData)
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
//	GameStartPoint::PreCreate(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 GameStartPoint::PreCreate(void *pData, float fData)
{
	// Let parent class handle it first
	BaseClass::EngineMessageFn(MID_PRECREATE, pData, fData);

	// Cast pData to a ObjectCreateStruct* for convenience
	ObjectCreateStruct* pStruct = (ObjectCreateStruct*)pData;

	// Set the object type to OT_NORMAL
	pStruct->m_ObjectType = OT_NORMAL;

	// Check to see if this is coming from a world file
	if(fData == PRECREATE_WORLDFILE)
	{
		ReadProps(pStruct);
	}

	// Return default of 1
	return 1;
}



//-----------------------------------------------------------------------------
//	GameStartPoint::ReadProps(ObjectCreateStruct* pStruct)
//
//-----------------------------------------------------------------------------
void GameStartPoint::ReadProps(ObjectCreateStruct* pStruct)
{
	g_pLTServer->GetPropString("Name", pStruct->m_Filename, MAX_CS_FILENAME_LEN);
}
