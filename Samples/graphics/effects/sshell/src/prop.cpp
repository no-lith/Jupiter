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

#include "Prop.h"
#include "PlayerSrvr.h"
#include "msgids.h"

#include <iltcommon.h>
#include <iltsoundmgr.h>
#include "ServerInterfaces.h"


//-----------------------------------------------------------------------------
BEGIN_CLASS(Prop)
ADD_STRINGPROP_FLAG(Filename, "", PF_MODEL | PF_FILENAME)
ADD_STRINGPROP_FLAG(Texture0, "", PF_FILENAME)
ADD_STRINGPROP_FLAG(Texture1, "", PF_FILENAME)
ADD_STRINGPROP_FLAG(Texture2, "", PF_FILENAME)
ADD_STRINGPROP_FLAG(Texture3, "", PF_FILENAME)
ADD_STRINGPROP_FLAG(RenderStyle0, "", PF_FILENAME)
ADD_STRINGPROP_FLAG(RenderStyle1, "", PF_FILENAME)
ADD_STRINGPROP_FLAG(RenderStyle2, "", PF_FILENAME)
ADD_STRINGPROP_FLAG(RenderStyle3, "", PF_FILENAME)
ADD_VECTORPROP_VAL(Scale, 1.0f, 1.0f, 1.0f)
ADD_STRINGPROP(Animation, "")
ADD_BOOLPROP(Animate, false)
ADD_BOOLPROP(Shadow, false)
ADD_BOOLPROP(Translucent, false)
END_CLASS_DEFAULT_FLAGS(Prop, BaseClass, LTNULL, LTNULL, CF_ALWAYSLOAD)



//-----------------------------------------------------------------------------
//	Prop::Prop():
//
//-----------------------------------------------------------------------------  
Prop::Prop():
m_bAnimate(false)
{
	memset(m_szAnimation, 0, sizeof(m_szAnimation));
}


//-----------------------------------------------------------------------------
//	Prop::EngineMessageFn(uint32 messageID, void *pData, float fData)
//
//-----------------------------------------------------------------------------  
uint32 Prop::EngineMessageFn(uint32 messageID, void *pData, float fData)
{
    
    switch (messageID)
    {
    case MID_PRECREATE:
        return PreCreate(pData, fData);
    case MID_TOUCHNOTIFY:
        return TouchNotify(pData, fData);
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
uint32 Prop::PreCreate(void *pData, float fData)
{
    // Let parent class handle it first
    BaseClass::EngineMessageFn(MID_PRECREATE, pData, fData);
    
    // Cast pData to a ObjectCreateStruct* for convenience
    ObjectCreateStruct* pStruct = (ObjectCreateStruct*)pData;    

    // Setup flags
    pStruct->m_Flags = FLAG_VISIBLE;// | FLAG_NOLIGHT;
  
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

uint32 Prop::InitialUpdate()
{
	if(m_szAnimation[0] != '\0')
	{
		HMODELANIM hAnim = g_pLTServer->GetAnimIndex(m_hObject, m_szAnimation);
		g_pLTServer->SetModelAnimation(m_hObject, hAnim);
		g_pLTServer->ResetModelAnimation(m_hObject);
		g_pLTServer->SetModelPlaying(m_hObject, m_bAnimate);
		g_pLTServer->SetModelLooping(m_hObject, m_bAnimate);
	}
	else
	{
		g_pLTServer->SetModelPlaying(m_hObject, false);
		g_pLTServer->SetModelLooping(m_hObject, false);
	}

	g_pLTServer->SetNextUpdate(m_hObject, 0.0001f);

	return 1;
}

//-----------------------------------------------------------------------------
//	Prop::Update()
//
//-----------------------------------------------------------------------------
uint32 Prop::Update()
{

	/*
	LTRotation rRot;
	g_pLTServer->GetObjectRotation(m_hObject, &rRot);
	rRot.Rotate(rRot.Up(), MATH_DEGREES_TO_RADIANS(5.0));
	
	g_pLTServer->SetObjectRotation(m_hObject, &rRot);
	g_pLTServer->SetNextUpdate(m_hObject, 0.0001f);
	*/

    return 1;
}


//-----------------------------------------------------------------------------
//	Prop::TouchNotify(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Prop::TouchNotify(void *pData, float fData)
{
    HOBJECT hObj = (HOBJECT)pData;

    HCLASS hClass = g_pLTServer->GetClass("CPlayerSrvr");
    HCLASS hTarget = g_pLTServer->GetObjectClass(hObj);

    // Filter out non player objects
    if( g_pLTServer->IsKindOf(hClass, hTarget))
    {
        
        CPlayerSrvr *pPlayer = (CPlayerSrvr*)g_pLTServer->HandleToObject(hObj);
        
        if(pPlayer)
        {
          // Do something if needed

        }

    }

    return 1;
}

//-----------------------------------------------------------------------------
//	Prop::ReadProps(ObjectCreateStruct* pStruct)
//
//-----------------------------------------------------------------------------
void Prop::ReadProps(ObjectCreateStruct* pStruct)
{
    g_pLTServer->GetPropString("Filename", pStruct->m_Filename, MAX_CS_FILENAME_LEN);
    g_pLTServer->GetPropString("Texture0", pStruct->m_SkinNames[0], MAX_CS_FILENAME_LEN);
    g_pLTServer->GetPropString("Texture1", pStruct->m_SkinNames[1], MAX_CS_FILENAME_LEN);
	g_pLTServer->GetPropString("Texture2", pStruct->m_SkinNames[2], MAX_CS_FILENAME_LEN);
    g_pLTServer->GetPropString("Texture3", pStruct->m_SkinNames[3], MAX_CS_FILENAME_LEN);

    g_pLTServer->GetPropString("RenderStyle0", pStruct->m_RenderStyleNames[0], MAX_CS_FILENAME_LEN);
    g_pLTServer->GetPropString("RenderStyle1", pStruct->m_RenderStyleNames[1], MAX_CS_FILENAME_LEN);
    g_pLTServer->GetPropString("RenderStyle2", pStruct->m_RenderStyleNames[2], MAX_CS_FILENAME_LEN);
    g_pLTServer->GetPropString("RenderStyle3", pStruct->m_RenderStyleNames[3], MAX_CS_FILENAME_LEN);
	g_pLTServer->GetPropVector("Scale", &pStruct->m_Scale);

	g_pLTServer->GetPropString("Animation", m_szAnimation, 31);

	g_pLTServer->GetPropBool("Animate", &m_bAnimate);

	bool bValue = false;
	g_pLTServer->GetPropBool("Shadow", &bValue);
	if(bValue)
	{
		pStruct->m_Flags |= FLAG_SHADOW;
	}

	bValue = false;
	g_pLTServer->GetPropBool("Translucent", &bValue);
	if(bValue)
	{
		pStruct->m_Flags2 |= FLAG2_FORCETRANSLUCENT;
	}
}


//-----------------------------------------------------------------------------
//	Prop::PlaySound()
//
//-----------------------------------------------------------------------------
void Prop::PlaySound()
{
    PlaySoundInfo playSoundInfo;
    PLAYSOUNDINFO_INIT(playSoundInfo);
    HLTSOUND hSnd = LTNULL;
    
    playSoundInfo.m_dwFlags = PLAYSOUND_ATTACHED | PLAYSOUND_3D;
    playSoundInfo.m_hObject = m_hObject;
    playSoundInfo.m_fInnerRadius = 10.0f;
    playSoundInfo.m_fOuterRadius = 250.0f;
    playSoundInfo.m_nVolume = 100;
    
    SAFE_STRCPY(playSoundInfo.m_szSoundName, "Snd/pickup.wav");            
    
    LTRESULT hResult = g_pLTServer->SoundMgr()->PlaySound(&playSoundInfo, hSnd);
    
    if(hResult != LT_OK)
    {
        g_pLTServer->CPrint("(Prop) Error playing sound");
    }
}
