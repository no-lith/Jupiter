//------------------------------------------------------------------------------//
// 
// MODULE   : Prop.cpp
// 
// PURPOSE  : Prop - Implementation
// 
// CREATED  : 08/27/2002
// 
// (c) 2002 LithTech, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//

#include "GameLight.h"
#include "PlayerSrvr.h"
#include "msgids.h"

#include <iltcommon.h>
#include <iltsoundmgr.h>
#include "ServerInterfaces.h"


//-----------------------------------------------------------------------------
BEGIN_CLASS(GameLight)
ADD_STRINGPROP_FLAG(Filename, "", PF_MODEL | PF_FILENAME)
ADD_STRINGPROP_FLAG(Skin, "", PF_FILENAME)
ADD_STRINGPROP_FLAG(Target, "", PF_OBJECTLINK)
ADD_VECTORPROP_VAL(Scale, 25.0f, 25.0f, 25.0f)
ADD_REALPROP(DegreesPerSecond, 85.0f)
ADD_REALPROP(DistanceFromOrigin, 400.0f)
END_CLASS_DEFAULT_FLAGS(GameLight, BaseClass, LTNULL, LTNULL, CF_ALWAYSLOAD)



//-----------------------------------------------------------------------------
//	Prop::Prop():
//
//-----------------------------------------------------------------------------
GameLight::GameLight():
m_hTarget(NULL),
m_hLight(NULL),
m_fDegreesPerSecond(85.0f),
m_fDistanceFromOrigin(400.0f)
{
	m_rRot.Init();
}


//-----------------------------------------------------------------------------
//	Prop::EngineMessageFn(uint32 messageID, void *pData, float fData)
//
//-----------------------------------------------------------------------------  
uint32 GameLight::EngineMessageFn(uint32 messageID, void *pData, float fData)
{
    
    switch (messageID)
    {
    case MID_PRECREATE:
        return PreCreate(pData, fData);
	case MID_INITIALUPDATE:
		return InitialUpdate();
    case MID_UPDATE:
        return Update();
        
    default:
        break;
    }
    
    // Pass the message along to parent class.
    return BaseClass::EngineMessageFn(messageID, pData, fData);
}

//-----------------------------------------------------------------------------
//	Prop::PreCreate(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 GameLight::PreCreate(void *pData, float fData)
{
    // Let parent class handle it first
    BaseClass::EngineMessageFn(MID_PRECREATE, pData, fData);
    
    // Cast pData to a ObjectCreateStruct* for convenience
    ObjectCreateStruct* pStruct = (ObjectCreateStruct*)pData;    

    // Setup flags
    pStruct->m_Flags = FLAG_VISIBLE | FLAG_FORCECLIENTUPDATE | FLAG_NOLIGHT;
	//pStruct->m_Flags2 = FLAG2_FORCECLIENTUPDATE;
  
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

uint32 GameLight::InitialUpdate()
{

	g_pLTServer->SetNextUpdate(m_hObject, 0.0001f);

	ILTMessage_Write *pMsg;
	g_pLTSCommon->CreateMessage(pMsg);
	pMsg->IncRef();

	pMsg->Writeuint8(MSG_GAMELIGHT_INFO);
	g_pLTServer->SetObjectSFXMessage(m_hObject, pMsg->Read());

	pMsg->DecRef();

	//create OT_LIGHT object
	HCLASS hClass = g_pLTServer->GetClass("BaseClass");
	ObjectCreateStruct ocs;
	ocs.Clear();
	ocs.m_ObjectType = OT_LIGHT;
	ocs.m_Flags = FLAG_VISIBLE | FLAG_FORCECLIENTUPDATE | FLAG_DONTLIGHTBACKFACING;
	ocs.m_Flags2 = FLAG2_FORCEDYNAMICLIGHTWORLD;

	BaseClass* pObject = (BaseClass*)g_pLTServer->CreateObject(hClass, &ocs);
	if(pObject)
	{
		m_hLight = pObject->m_hObject;

		g_pLTServer->SetLightRadius(m_hLight, 650.0f);
		g_pLTServer->SetLightColor(m_hLight, 0.5f, 0.5f, 0.5f);
	}


	return 1;
}

//-----------------------------------------------------------------------------
//	Prop::Update()
//
//-----------------------------------------------------------------------------
uint32 GameLight::Update()
{

	float fTimeDelta = g_pLTServer->GetFrameTime();

	float fDeg =  m_fDegreesPerSecond * fTimeDelta;///1000.0f;
	m_rRot.Rotate(m_rRot.Up(), MATH_DEGREES_TO_RADIANS(fDeg));

	float fDist = m_fDistanceFromOrigin;
	LTVector newPos = LTVector(0.0,0.0,0.0) + m_rRot.Forward() * fDist;
	g_pLTServer->SetObjectPos(m_hObject, &newPos);

	if(m_hLight)
	{
		g_pLTServer->SetObjectPos(m_hLight, &newPos);
	}

	g_pLTServer->SetNextUpdate(m_hObject, 0.0001f);

    return 1;
}


//-----------------------------------------------------------------------------
//	Prop::ReadProps(ObjectCreateStruct* pStruct)
//
//-----------------------------------------------------------------------------
void GameLight::ReadProps(ObjectCreateStruct* pStruct)
{
	//char szTarget[]

	//g_pLTServer->GetPropString("Animation", m_szAnimation, 31);

	LTVector vScale;
	g_pLTServer->GetPropVector("Scale", &vScale);
	pStruct->m_Scale = vScale;

	g_pLTServer->GetPropReal("DegreesPerSecond", &m_fDegreesPerSecond);
	g_pLTServer->GetPropReal("DistanceFromOrigin", &m_fDistanceFromOrigin);

}
