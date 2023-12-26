//------------------------------------------------------------------------------//
// 
// MODULE	: Point.cpp
// 
// PURPOSE	: Point - Implementation
// 
// CREATED	: 10/23/2002
// 
// (c) 2002 LithTech, Inc.	All Rights Reserved
// 
//------------------------------------------------------------------------------//

#include "Point.h"

#include <ltobjectcreate.h>

//-----------------------------------------------------------------------------
BEGIN_CLASS(Point)
END_CLASS_DEFAULT_FLAGS(Point, BaseClass, LTNULL, LTNULL, CF_ALWAYSLOAD)


//-----------------------------------------------------------------------------
//	Point::EngineMessageFn(uint32 messageID, void *pData, float fData)
//
//-----------------------------------------------------------------------------  
uint32 Point::EngineMessageFn(uint32 messageID, void *pData, float fData)
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
//	Point::PreCreate(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Point::PreCreate(void *pData, float fData)
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
//	Point::ReadProps(ObjectCreateStruct* pStruct)
//
//-----------------------------------------------------------------------------
void Point::ReadProps(ObjectCreateStruct* pStruct)
{
	g_pLTServer->GetPropString("Name", pStruct->m_Filename, MAX_CS_FILENAME_LEN);
}
