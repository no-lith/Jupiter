//------------------------------------------------------------------------------//
//
// MODULE	  : Worldmodel.cpp
//
// PURPOSE	: Worldmodel - Implementation
//
// CREATED	: 07/24/2003
//
// (c) 2002 LithTech, Inc.	All Rights Reserved
//
//------------------------------------------------------------------------------//

#include "Worldmodel.h"

#include <ltobjectcreate.h>
#include <iltmessage.h>
#include <iltcommon.h>
#include "msgids.h"
#include "serverinterfaces.h"



//-----------------------------------------------------------------------------
BEGIN_CLASS(Worldmodel)
ADD_BOOLPROP(Solid, true)
ADD_BOOLPROP(Translucent, false)
END_CLASS_DEFAULT_FLAGS(Worldmodel, BaseClass, LTNULL, LTNULL, CF_ALWAYSLOAD| CF_WORLDMODEL)



//-----------------------------------------------------------------------------
//	TransWorldmodel::EngineMessageFn(uint32 messageID, void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Worldmodel::EngineMessageFn(uint32 messageID, void *pData, float fData)
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
//	TransWorldmodel::PreCreate(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Worldmodel::PreCreate(void *pData, float fData)
{
	// Let parent class handle it first
	BaseClass::EngineMessageFn(MID_PRECREATE, pData, fData);

	// Cast pData to a ObjectCreateStruct* for convenience
	ObjectCreateStruct* pStruct = (ObjectCreateStruct*)pData;

	// Set the object type to OT_NORMAL
	pStruct->m_ObjectType = OT_WORLDMODEL;

    pStruct->m_Flags = FLAG_VISIBLE;// | FLAG_SOLID;
    //pStruct->m_Flags2 |= FLAG2_ADDITIVE | FLAG2_FORCETRANSLUCENT;

	// Check to see if this is coming from a world file
	if(fData == PRECREATE_WORLDFILE)
	{
		ReadProps(pStruct);
	}

	// Return default of 1
	return 1;
}


//-----------------------------------------------------------------------------
//	TransWorldmodel::ReadProps(ObjectCreateStruct* pStruct)
//
//-----------------------------------------------------------------------------
void Worldmodel::ReadProps(ObjectCreateStruct* pStruct)
{
	g_pLTServer->GetPropString("Name", pStruct->m_Filename, MAX_CS_FILENAME_LEN);

	bool bSolid = false;
	g_pLTServer->GetPropBool("Solid", &bSolid);

	if(bSolid)
	{
		pStruct->m_Flags |= FLAG_SOLID;
	}

	bool bTranslucent = false;
	g_pLTServer->GetPropBool("Translucent", &bTranslucent);

	if(bTranslucent)
	{
		pStruct->m_Flags2 |= FLAG2_FORCETRANSLUCENT;
	}
}
