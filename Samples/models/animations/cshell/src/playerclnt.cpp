//------------------------------------------------------------------------------//
// 
// MODULE   : playerclnt.cpp
// 
// PURPOSE  : CPlayerClnt - Implementation
// 
// CREATED  : 07/15/2002
// 
// (c) 2002 LithTech, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//

#include "playerclnt.h"

#include <iltclient.h>
#include <ltobjectcreate.h>
#include <iltmessage.h>
#include <iltphysics.h>
#include <iltcommon.h>

#include "clientinterfaces.h"
#include "commandids.h"
#include "msgids.h"

#define MOVEMENT_RATE 2000.0f

//----------------------------------------------------------------------------
// CPlayerClnt::CPlayerClnt()
//
//-----------------------------------------------------------------------------
CPlayerClnt::CPlayerClnt() :
m_dwInputFlags(0),
m_fYaw(0.0f),
m_fPitch(0.0f),
m_fRoll(0.0f),
m_hClientObject(NULL),
m_hObject(NULL),
m_hCurAnim(NULL)
{

}

CPlayerClnt::~CPlayerClnt()
{
}

//----------------------------------------------------------------------------
// void CPlayerClnt::SetHObject(HLOCALOBJ hObj)
//
//-----------------------------------------------------------------------------
void CPlayerClnt::SetClientObject(HLOCALOBJ hObj)
{
	m_hClientObject = hObj;
}

//----------------------------------------------------------------------------
// void CPlayerClnt::SpecialEffectMessage(ILTMessage_Read *pMessage)
//
//-----------------------------------------------------------------------------
void CPlayerClnt::SpecialEffectMessage(ILTMessage_Read *pMessage)
{
	// Nothing to read currently
}


//----------------------------------------------------------------------------
// void CPlayerClnt::CreatePlayer()
//
//-----------------------------------------------------------------------------
void CPlayerClnt::CreatePlayer()
{
    LTVector vPos;
    LTRotation rRot;

    g_pLTClient->GetObjectPos(m_hClientObject, &vPos);
    g_pLTClient->GetObjectRotation(m_hClientObject, &rRot);

	ObjectCreateStruct objCreateStruct;
	objCreateStruct.Clear();

    objCreateStruct.m_ObjectType = OT_MODEL;
    objCreateStruct.m_Flags = FLAG_SOLID | FLAG_GRAVITY | FLAG_STAIRSTEP | FLAG_VISIBLE;
	objCreateStruct.m_Flags2 = FLAG2_PLAYERCOLLIDE;

    objCreateStruct.m_Pos = vPos;
    objCreateStruct.m_Rotation = rRot;

    strcpy(objCreateStruct.m_Filename, "Models\\player_action.ltb");
    strcpy(objCreateStruct.m_SkinNames[0], "ModelTextures\\CateAction.dtx");
    strcpy(objCreateStruct.m_SkinNames[1], "ModelTextures\\CateActionHead.dtx");
    strcpy(objCreateStruct.m_SkinNames[2], "ModelTextures\\CateActionHair.dtx");
    strcpy(objCreateStruct.m_SkinNames[3], "ModelTextures\\CateActionLash.dtx");

    m_hObject = g_pLTClient->CreateObject(&objCreateStruct);

}


//----------------------------------------------------------------------------
// void CPlayerClnt::UpdateMouseMovement(float yaw, float pitch, float roll)
//
//-----------------------------------------------------------------------------
void CPlayerClnt::UpdateRotation(float yaw, float pitch, float roll)
{
    m_fYaw += yaw;
    m_fPitch += pitch;
    m_fRoll += roll;
}

//----------------------------------------------------------------------------
// void CPlayerClnt::Update()
//
//-----------------------------------------------------------------------------
void CPlayerClnt::Update()
{

    LTVector vZero(0.0f, 0.0f, 0.0f);
    g_pLTCPhysics->SetAcceleration(m_hObject, &vZero);


    // Calculate rotation
    LTVector vUp, vRight, vForward;
    LTRotation rRot;
	g_pLTCCommon->SetupEuler(rRot, m_fPitch, m_fYaw, m_fRoll);
	g_pLTClient->SetObjectRotation(m_hObject, &rRot);

    // Calulate movement
    LTVector vPos, vVel;
    //vVel.Init();

    UpdateMovement();
    
    g_pLTClient->GetObjectPos(m_hObject, &vPos);
    g_pLTCPhysics->GetVelocity(m_hObject, &vVel);

    // Send our information to the server
    g_pCShell->SendVelPosAndRot(vVel, vPos, rRot);

}


//----------------------------------------------------------------------------
// void CPlayerClnt::UpdateMovement()
//
//----------------------------------------------------------------------------
void CPlayerClnt::UpdateMovement()
{
    LTVector vPos, vVel;
    LTRotation rRot;
    g_pLTClient->GetObjectPos(m_hObject, &vPos);
    g_pLTCPhysics->GetVelocity(m_hObject, &vVel);
    g_pLTClient->GetObjectRotation(m_hObject, &rRot);
    
    float fFrameTime = g_pLTClient->GetFrameTime();

    // Look at our input flags
    if( m_dwInputFlags & MOVE_FORWARD)
    {
      vVel += rRot.Forward() * MOVEMENT_RATE * fFrameTime;

      PlayAnimation("LRF");

    }else
    if( m_dwInputFlags & MOVE_BACKWARD)
    {

      vVel += rRot.Forward() * -MOVEMENT_RATE * fFrameTime;
      PlayAnimation("LRB");

    }else
    if( m_dwInputFlags & MOVE_LEFT)
    {
      vVel += rRot.Right() * -MOVEMENT_RATE * fFrameTime;
      PlayAnimation("LRL");

    }else
    if( m_dwInputFlags & MOVE_RIGHT)
    {
      vVel += rRot.Right() * MOVEMENT_RATE * fFrameTime;
      PlayAnimation("LRR");
    }else
    {
      PlayAnimation("LSt");
      vVel.x = vVel.z = 0.0f;
    }

    g_pLTCPhysics->SetVelocity(m_hObject, &vVel);
    
    // Change the bounding box dims based on current animation
    LTVector vDims;
    g_pLTCCommon->GetModelAnimUserDims(m_hObject, &vDims, m_hCurAnim);
    g_pLTCPhysics->SetObjectDims(m_hObject, &vDims, 0);

    RecalculatePosition();

}


//----------------------------------------------------------------------------
// void CPlayerClnt::RecalculatePosition()
//
//-----------------------------------------------------------------------------
void CPlayerClnt::RecalculatePosition()
{
    
    // UpdateMovement() is only available to ILTCLientPhysics
    ILTClientPhysics* pCPhysics = (ILTClientPhysics*)g_pLTCPhysics;

    MoveInfo info;
    info.m_Offset.Init();
    info.m_hObject = m_hObject;
    info.m_dt = g_pLTClient->GetFrameTime();
    pCPhysics->UpdateMovement(&info);


    LTVector curPos, newPos;
    g_pLTClient->GetObjectPos(m_hObject, &curPos);
    newPos = curPos + info.m_Offset;
    pCPhysics->MoveObject(m_hObject, &newPos, 0);

}


//----------------------------------------------------------------------------
// void CPlayerClnt::PlayAnimation(const char* sAnimName)
//
//-----------------------------------------------------------------------------
void CPlayerClnt::PlayAnimation(const char* sAnimName)
{
		HMODELANIM hAnim = g_pLTClient->GetAnimIndex(m_hObject, (char *)sAnimName);
        
        if(hAnim != m_hCurAnim)
        {
            m_hCurAnim = hAnim;
            g_pLTClient->SetModelAnimation(m_hObject, hAnim);
            g_pLTClient->SetModelLooping(m_hObject, LTTRUE);
            
            // Send animation to server
            ILTMessage_Write *pMessage;
            LTRESULT nResult = g_pLTCCommon->CreateMessage(pMessage);
            
            if( LT_OK == nResult)
            {
                pMessage->IncRef();
                pMessage->Writeuint8(MSG_CS_ANIM);
                pMessage->WriteString(sAnimName);
                g_pLTClient->SendToServer(pMessage->Read(), 0);
                pMessage->DecRef();
            }
        }
}

