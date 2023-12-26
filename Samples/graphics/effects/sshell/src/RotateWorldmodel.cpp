//------------------------------------------------------------------------------//
//
// MODULE	  : TransWorldmodel.cpp
//
// PURPOSE	: TransWorldmodel - Implementation
//
// CREATED	: 07/24/2003
//
// (c) 2002 LithTech, Inc.	All Rights Reserved
//
//------------------------------------------------------------------------------//

#include "RotateWorldmodel.h"

#include <ltobjectcreate.h>
#include <iltmessage.h>
#include <iltcommon.h>
#include "msgids.h"
#include "serverinterfaces.h"



//-----------------------------------------------------------------------------
BEGIN_CLASS(RotateWorldmodel)
ADD_BOOLPROP(RotateX, false)
ADD_BOOLPROP(RotateY, false)
ADD_BOOLPROP(RotateZ, false)
ADD_REALPROP(RotateXBy, 0.0f)
ADD_REALPROP(RotateYBy, 0.0f)
ADD_REALPROP(RotateZBy, 0.0f)
END_CLASS_DEFAULT_FLAGS(RotateWorldmodel, BaseClass, LTNULL, LTNULL, CF_ALWAYSLOAD| CF_WORLDMODEL)



//-----------------------------------------------------------------------------
//	TransWorldmodel::EngineMessageFn(uint32 messageID, void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 RotateWorldmodel::EngineMessageFn(uint32 messageID, void *pData, float fData)
{
	switch (messageID)
	{
	case MID_PRECREATE:
		return PreCreate(pData, fData);
	case MID_INITIALUPDATE:
		{
			g_pLTServer->SetNextUpdate(m_hObject, 0.1f);
		}break;
	case MID_UPDATE:
		{
			//rotate!
			for(int i = 0; i < 3; ++i)
			{
				if(m_bRotate[i])
				{
					switch(i)
					{
					case 0: //X
						{
							LTRotation rRot;
							g_pLTServer->GetObjectRotation(m_hObject, &rRot);
							rRot.Rotate(rRot.Right(), MATH_DEGREES_TO_RADIANS(m_fRotateBy[0]) * g_pLTServer->GetFrameTime());
							g_pLTServer->SetObjectRotation(m_hObject, &rRot);
						}
						break;
					case 1: //Y
						{
							LTRotation rRot;
							g_pLTServer->GetObjectRotation(m_hObject, &rRot);
							rRot.Rotate(rRot.Up(), MATH_DEGREES_TO_RADIANS(m_fRotateBy[1]) * g_pLTServer->GetFrameTime());
							g_pLTServer->SetObjectRotation(m_hObject, &rRot);
						}
						break;
					case 2: //Z
						{
							LTRotation rRot;
							g_pLTServer->GetObjectRotation(m_hObject, &rRot);
							rRot.Rotate(rRot.Forward(), MATH_DEGREES_TO_RADIANS(m_fRotateBy[2]) * g_pLTServer->GetFrameTime());
							g_pLTServer->SetObjectRotation(m_hObject, &rRot);
						}
						break;
					default:
						break;
					}
				}
			}

			g_pLTServer->SetNextUpdate(m_hObject, 0.0001f);
		}break;
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
uint32 RotateWorldmodel::PreCreate(void *pData, float fData)
{
	// Let parent class handle it first
	BaseClass::EngineMessageFn(MID_PRECREATE, pData, fData);

	// Cast pData to a ObjectCreateStruct* for convenience
	ObjectCreateStruct* pStruct = (ObjectCreateStruct*)pData;

	// Set the object type to OT_NORMAL
	pStruct->m_ObjectType = OT_WORLDMODEL;

    pStruct->m_Flags = FLAG_VISIBLE | FLAG_SOLID | FLAG_FORCECLIENTUPDATE;
    //pStruct->m_Flags2 |= ;

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
void RotateWorldmodel::ReadProps(ObjectCreateStruct* pStruct)
{
	g_pLTServer->GetPropString("Name", pStruct->m_Filename, MAX_CS_FILENAME_LEN);

	bool bProp = false;

	g_pLTServer->GetPropBool("RotateX", &m_bRotate[0]);
	g_pLTServer->GetPropBool("RotateY", &m_bRotate[1]);
	g_pLTServer->GetPropBool("RotateZ", &m_bRotate[2]);

	g_pLTServer->GetPropReal("RotateXBy", &m_fRotateBy[0]);
	g_pLTServer->GetPropReal("RotateYBy", &m_fRotateBy[1]);
	g_pLTServer->GetPropReal("RotateZBy", &m_fRotateBy[2]);
}
