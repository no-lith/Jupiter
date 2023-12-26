//------------------------------------------------------------------------------//
// 
// MODULE   : Propmodel.cpp
// 
// PURPOSE  : Propmodel - Implementation
// 
// CREATED  : 08/27/2002
// 
// (c) 2002 LithTech, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//

#include "Propmodel.h"
#include "PlayerSrvr.h"
#include "msgids.h"
#include "helper.h"

#include <iltcommon.h>
#include <iltmodel.h>
#include <ilttransform.h>
#include <iltphysics.h>
#include "ServerInterfaces.h"


//-----------------------------------------------------------------------------
BEGIN_CLASS(Propmodel)
ADD_STRINGPROP_FLAG(PreviewModel, "", PF_MODEL | PF_FILENAME)
END_CLASS_DEFAULT_FLAGS(Propmodel, BaseClass, LTNULL, LTNULL, CF_ALWAYSLOAD)



//-----------------------------------------------------------------------------
//	Propmodel::Propmodel():
//
//-----------------------------------------------------------------------------  
Propmodel::Propmodel():
m_nAnimStep(0)
{

}


//-----------------------------------------------------------------------------
//	Propmodel::EngineMessageFn(uint32 messageID, void *pData, float fData)
//
//-----------------------------------------------------------------------------  
uint32 Propmodel::EngineMessageFn(uint32 messageID, void *pData, float fData)
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
//	Propmodel::PreCreate(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Propmodel::PreCreate(void *pData, float fData)
{
    // Let parent class handle it first
    BaseClass::EngineMessageFn(MID_PRECREATE, pData, fData);
    
    // Cast pData to a ObjectCreateStruct* for convenience
    ObjectCreateStruct* pStruct = (ObjectCreateStruct*)pData;    

    // Setup flags
    pStruct->m_Flags = FLAG_VISIBLE | FLAG_SOLID | FLAG_GRAVITY;
    pStruct->m_Flags2 |= FLAG2_PLAYERCOLLIDE;
  
    // Set the object type to OT_MODEL
    pStruct->m_ObjectType = OT_MODEL;
    
    strcpy(pStruct->m_Filename, "Models\\player_action.ltb");
    strcpy(pStruct->m_SkinNames[0], "ModelTextures\\CateAction.dtx");
    strcpy(pStruct->m_SkinNames[1], "ModelTextures\\CateActionHead.dtx");
    strcpy(pStruct->m_SkinNames[2], "ModelTextures\\CateActionHair.dtx");
    strcpy(pStruct->m_SkinNames[3], "ModelTextures\\CateActionLash.dtx");

    // Check to see if this is coming from a world file
    if(fData == PRECREATE_WORLDFILE)	  
    {
        ReadProps(pStruct);
    }
    
    // Return default of 1
    return 1;
}


//-----------------------------------------------------------------------------
//	Propmodel::InitialUpdate()
//
//-----------------------------------------------------------------------------
uint32 Propmodel::InitialUpdate()
{

    //Set Default Animation
	PlayAnimation("LSt");
    RecalculateBoundingBox();

    CreateAttachmentObject(m_hCrossbow, "Models\\crossbow.ltb", "ModelTextures\\crossbow.dtx");    
    // Create Attachment in right hand
    LTVector vRotOffset(25.0f, 25.0f, 0.0f);
    LTVector vPosOffset(0.0f, 0.0f, 0.0f);
    CreateAttachment(m_hAttachment, m_hCrossbow, "RightHand", vRotOffset, vPosOffset);

    LTVector vVel(0.0f, -2000.0f, 0.0f);
    g_pLTSPhysics->SetVelocity(m_hObject, &vVel);

    g_pLTServer->SetNextUpdate(m_hObject, 2.0f);
    return 1;
}


//-----------------------------------------------------------------------------
//	Propmodel::Update()
//
//-----------------------------------------------------------------------------
uint32 Propmodel::Update()
{

	// Animation sequence
    switch(m_nAnimStep)
    {
    case 0:
        PlayAnimation("LStR");
        ++m_nAnimStep;
        break;
        
    case 1:
        PlayAnimation("LStL");
        ++m_nAnimStep;
        break;

    case 2:
        PlayAnimation("LRF");
        ++m_nAnimStep;
        // Let's run for 3 seconds instead of 2
        g_pLTServer->SetNextUpdate(m_hObject, 3.0f);
        return 1;
        break;

    case 3:
        PlayAnimation("LSt");
        ++m_nAnimStep;
        break;       
		
    case 4:
		{
			PlayAnimation("LSt");
			g_pLTServer->RemoveAttachment(m_hAttachment);
			g_pLTServer->RemoveObject(m_hCrossbow);
			CreateAttachmentObject(m_hBillyclub, "Models\\billyclub.ltb", "ModelTextures\\club.dtx");
			LTVector vRotOffset(25.0f, 25.0f, 0.0f);
			LTVector vPosOffset(0.0f, 0.0f, 0.0f);
			CreateAttachment(m_hAttachment, m_hBillyclub, "RightHand", vRotOffset, vPosOffset);
			++m_nAnimStep;
		}
        break;   
		
    case 5:
		{
			
			PlayAnimation("LC");
			g_pLTServer->SetNextUpdate(m_hObject, 2.0f);
			++m_nAnimStep;
			return 1;
		}
        break; 

    default:
		{
			m_nAnimStep = 0;
			PlayAnimation("LSt");
			g_pLTServer->RemoveAttachment(m_hAttachment);
			g_pLTServer->RemoveObject(m_hBillyclub);
			CreateAttachmentObject(m_hCrossbow, "Models\\crossbow.ltb", "ModelTextures\\crossbow.dtx");
			LTVector vRotOffset(25.0f, 25.0f, 0.0f);
			LTVector vPosOffset(0.0f, 0.0f, 0.0f);
			CreateAttachment(m_hAttachment, m_hCrossbow, "RightHand", vRotOffset, vPosOffset);
		}
        break;
    }

    g_pLTServer->SetNextUpdate(m_hObject, 2.0f);
    return 1;
}


//-----------------------------------------------------------------------------
//	Propmodel::ReadProps(ObjectCreateStruct* pStruct)
//
//-----------------------------------------------------------------------------
void Propmodel::ReadProps(ObjectCreateStruct* pStruct)
{
}


//-----------------------------------------------------------------------------
//	Propmodel::CreateAttachmentObject()
//
//-----------------------------------------------------------------------------
void Propmodel::CreateAttachmentObject(HOBJECT &hObj, const char* sFilename, const char* sTexture)
{

    ObjectCreateStruct objCreateStruct;
    HCLASS hClass = g_pLTServer->GetClass("BaseClass");
    
    if (!hClass)
    {
        assert(!"ERROR: Couldn't get BaseClass!!");
        return;
    }
    
    objCreateStruct.Clear();
    objCreateStruct.m_ObjectType = OT_MODEL;	
    objCreateStruct.m_Flags = FLAG_VISIBLE;
    
    strncpy(objCreateStruct.m_Filename, sFilename, 127);
    strncpy(objCreateStruct.m_SkinNames[0], sTexture, 127);

    BaseClass* m_pObj = (BaseClass*)g_pLTServer->CreateObject(hClass, &objCreateStruct);
    
    if (m_pObj)
    {
        hObj = m_pObj->m_hObject;
        g_pLTServer->CPrint("Attachment Object Created!");
    }

}


//-----------------------------------------------------------------------------
//	Propmodel::CreateAttachment()
//
//-----------------------------------------------------------------------------
void Propmodel::CreateAttachment(HATTACHMENT &hAttachment, 
                                 HOBJECT hChildObject, 
                                 const char* sSocket, 
                                 LTVector &vRotOffset, 
                                 LTVector &vPosOffset)
{

    LTVector vOffset;
    LTRotation rOffset;
    
    vOffset.Init();
    rOffset.Init();
    hAttachment = NULL;
    
    HMODELSOCKET hSocket;
    LTRESULT SocketResult = g_pLTSModel->GetSocket(m_hObject, sSocket, hSocket);
    if(LT_OK == SocketResult)
    {
        g_pLTServer->CPrint("Got the socket");
    }else
    {
        g_pLTServer->CPrint("GetSocket failed: %s", LTRESULT_TO_STRING(SocketResult));    
    }
    
    LTransform tSocketTransform;
    LTRESULT SocketTrResult = g_pLTSModel->GetSocketTransform(m_hObject, hSocket, tSocketTransform, LTTRUE);
    if(LT_OK == SocketTrResult)
    {
        g_pLTServer->CPrint("Got the transform");
    }else
    {
        g_pLTServer->CPrint("GetTrasform failed: %s", LTRESULT_TO_STRING(SocketTrResult));    
    }

    // We need to adjust the rotation of our weapon before we make the attachment
    rOffset.Rotate(rOffset.Right(), MATH_DEGREES_TO_RADIANS(vRotOffset.x));
    rOffset.Rotate(rOffset.Up(), MATH_DEGREES_TO_RADIANS(vRotOffset.y));
    rOffset.Rotate(rOffset.Forward(), MATH_DEGREES_TO_RADIANS(vRotOffset.z));


    LTRESULT resultAttachment = g_pLTServer->CreateAttachment(m_hObject,
        hChildObject,
        sSocket,
        &vOffset,
        &rOffset,
        &hAttachment);
    
    if(LT_OK != resultAttachment)
    {
        g_pLTServer->CPrint("Error creating attachment!!");
    }

}


//-----------------------------------------------------------------------------
// Propmodel::PlayAnimation(const char* sAnimName)
//
//-----------------------------------------------------------------------------
void Propmodel::PlayAnimation(const char* sAnimName)
{
	HMODELANIM hAnim = g_pLTServer->GetAnimIndex(m_hObject, sAnimName);
    g_pLTServer->SetModelAnimation(m_hObject, hAnim);
    g_pLTServer->SetModelLooping(m_hObject, LTTRUE);
    RecalculateBoundingBox();
}


//----------------------------------------------------------------------------
// void Propmodel::RecalculateBoundingBox()
//
//-----------------------------------------------------------------------------
void Propmodel::RecalculateBoundingBox()
{
    LTVector vDims;
    HMODELANIM hCurAnim;
    g_pLTSModel->GetCurAnim(m_hObject, MAIN_TRACKER, hCurAnim);  
    g_pLTSCommon->GetModelAnimUserDims(m_hObject, &vDims, hCurAnim);
    g_pLTSPhysics->SetObjectDims(m_hObject, &vDims, 0);
}

