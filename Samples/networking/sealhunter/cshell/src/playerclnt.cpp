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
#include <iltmodel.h>

#include "clientinterfaces.h"
#include "commandids.h"
#include "msgids.h"
#include "animids.h"

#define MOVEMENT_RATE 2000.0f
#define JUMP_TIME 0.25f
#define JUMP_VELOCITY 800.0f
#define DEFAULT_LEASHTIME 2.0f



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
//m_hCurAnim(NULL),
m_bAttacking(false),
m_hClubObject(NULL),
m_bIsJumping(false),
m_fCurrentJumpRadians(0.0f),
m_fJumpTimeRemaining(0.0f),
m_DebugSphere(NULL),
m_bUseLeashing(true),
m_fLeashingDelay(0.0f)
{
}



//----------------------------------------------------------------------------
// CPlayerClnt::~CPlayerClnt()
//
//-----------------------------------------------------------------------------
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
    objCreateStruct.m_Flags = FLAG_SOLID | FLAG_GRAVITY | FLAG_STAIRSTEP | FLAG_VISIBLE | FLAG_SHADOW;
	objCreateStruct.m_Flags2 = FLAG2_PLAYERCOLLIDE;

    objCreateStruct.m_Pos = vPos;
    objCreateStruct.m_Rotation = rRot;

    strcpy(objCreateStruct.m_Filenames[0], "Models\\HARMGuard.ltb");
    strcpy(objCreateStruct.m_Filenames[1], "Models\\playerbase.ltb");
    strcpy(objCreateStruct.m_SkinNames[0], "ModelTextures\\HARMPurple.dtx");
    strcpy(objCreateStruct.m_SkinNames[1], "ModelTextures\\HARMHeadW1.dtx");

    m_hObject = g_pLTClient->CreateObject(&objCreateStruct);

    if(m_hObject)
    {
        // Set up animation trackers
        m_idUpperBodyTracker = ANIM_UPPER;
        g_pLTCModel->AddTracker(m_hObject, m_idUpperBodyTracker);

        m_idLowerBodyTracker = ANIM_LOWER;
        g_pLTCModel->AddTracker(m_hObject, m_idLowerBodyTracker);

        // Set up weight sets

        if ( LT_OK == g_pLTCModel->FindWeightSet(m_hObject, "Upper", m_hWeightUpper) )
        {
            g_pLTCModel->SetWeightSet(m_hObject, m_idUpperBodyTracker, m_hWeightUpper);
        }

        if ( LT_OK == g_pLTCModel->FindWeightSet(m_hObject, "Lower", m_hWeightLower) )
        {
            g_pLTCModel->SetWeightSet(m_hObject, m_idLowerBodyTracker, m_hWeightLower);
        }
    }
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

    UpdateMovement();

    if (m_bAttacking)
	{
    	UpdateAttacking(m_idUpperBodyTracker);
	}

    g_pLTClient->GetObjectPos(m_hObject, &vPos);
    g_pLTCPhysics->GetVelocity(m_hObject, &vVel);

    // Send our information to the server
    g_pCShell->SendVelPosAndRot(vVel, vPos, rRot);

    // update club position
    if(!m_hClubObject)
    {
        return;
    }

    HMODELSOCKET hSocket;
    LTRESULT SocketResult = g_pLTCModel->GetSocket(m_hObject, "RightHand", hSocket);
    if (LT_OK != SocketResult)
	{
        g_pLTClient->CPrint("GetSocket failed: " );
    }

    LTransform tSocketTransform;
    LTRESULT SocketTrResult = g_pLTCModel->GetSocketTransform(m_hObject, hSocket, tSocketTransform, LTTRUE);
    if (LT_OK != SocketTrResult)
    {
        g_pLTClient->CPrint("GetTrasform failed: ");
    }

    g_pLTClient->SetObjectPos(m_hClubObject, &tSocketTransform.m_Pos);
    g_pLTClient->SetObjectRotation(m_hClubObject, &tSocketTransform.m_Rot);

	m_dwInputFlags = 0;
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

		PlayMovementAnimation("LRF", m_idLowerBodyTracker);
    }
	else if( m_dwInputFlags & MOVE_BACKWARD)
    {
		vVel += rRot.Forward() * -MOVEMENT_RATE * fFrameTime;
		PlayMovementAnimation("LRB", m_idLowerBodyTracker);
    }
	else if( m_dwInputFlags & MOVE_LEFT)
    {
		vVel += rRot.Right() * -MOVEMENT_RATE * fFrameTime;
		PlayMovementAnimation("LRL", m_idLowerBodyTracker);
    }
	else if( m_dwInputFlags & MOVE_RIGHT)
    {
		vVel += rRot.Right() * MOVEMENT_RATE * fFrameTime;
		PlayMovementAnimation("LRR", m_idLowerBodyTracker);
    }
	else if(m_dwInputFlags & MOVE_JUMP)
	{
	}
	else
    {
		if( !m_bIsJumping )
		{
			PlayMovementAnimation("LSt", m_idLowerBodyTracker);
		}

		if(!m_bAttacking)
		{
      		PlayMovementAnimation("LSt", m_idUpperBodyTracker);
		}

		vVel.x = vVel.z = 0.0f;
    }

	if(m_dwInputFlags & MOVE_JUMP)
	{
		if( !m_bIsJumping )
		{
			CollisionInfo cInfo;
			g_pLTCPhysics->GetStandingOn(m_hObject, &cInfo);
			if(cInfo.m_hObject)
			{
				m_bIsJumping = true;
				m_fJumpTimeRemaining = JUMP_TIME;
				m_fCurrentJumpRadians = 0.0f;
				PlayMovementAnimation("LJT", m_idLowerBodyTracker);
			}
		}
	}

	// Clamp forward and right velocities..
	vVel.x = LTCLAMP(vVel.x, -435.0f, 435.0f);
	vVel.z = LTCLAMP(vVel.z, -435.0f, 435.0f);

    g_pLTCPhysics->SetVelocity(m_hObject, &vVel);

    // Change the bounding box dims based on current animation
    RecalculateBoundingBox();

	if( m_bIsJumping )
	{
		UpdateJump();
	}

    RecalculatePosition();
}



//----------------------------------------------------------------------------
// void CPlayerClnt::RecalculateBoundingBox()
//
//-----------------------------------------------------------------------------
void CPlayerClnt::RecalculateBoundingBox()
{
    LTVector vDims;
    HMODELANIM hCurAnim;
    g_pLTCModel->GetCurAnim(m_hObject, m_idLowerBodyTracker, hCurAnim);
    g_pLTCCommon->GetModelAnimUserDims(m_hObject, &vDims, hCurAnim);
    g_pLTCPhysics->SetObjectDims(m_hObject, &vDims, 0);
}



//----------------------------------------------------------------------------
// void CPlayerClnt::RecalculatePosition()
//
//-----------------------------------------------------------------------------
void CPlayerClnt::RecalculatePosition()
{
	LTVector curPos, newPos;

	ILTClientPhysics* pCPhysics = (ILTClientPhysics*)g_pLTCPhysics;

	MoveInfo info;
	info.m_Offset.Init();
	info.m_hObject = m_hObject;
	info.m_dt = g_pLTClient->GetFrameTime();
	pCPhysics->UpdateMovement(&info);

	g_pLTClient->GetObjectPos(m_hObject, &curPos);
	newPos = curPos + info.m_Offset;
	pCPhysics->MoveObject(m_hObject, &newPos, 0);

    if ( !m_hClientObject )
    {
        return;
    }

    // Leash our self if to far away.
    // -DLK - disable leashing for a short bit.

    if(m_bUseLeashing)
    {
        // Make sure we aren't too far away from our client.
        LTVector vClientPos;
        g_pLTClient->GetObjectPos(m_hClientObject, &vClientPos);

        float fDist = vClientPos.DistSqr(newPos);

        if( fDist > 1000.0f )
        {
            m_fLeashingDelay += g_pLTClient->GetFrameTime();
            //g_pLTClient->SetObjectPos(m_hObject, &vClientPos);
        }
        else
        {
            m_fLeashingDelay = 0.0f;
        }

        // If the player has been out of the bounds of the leash for DEFAULT_LEASHTIME
        // apply the leash.
        if(m_fLeashingDelay >= DEFAULT_LEASHTIME)
        {
            g_pLTClient->SetObjectPos(m_hObject, &vClientPos);
            m_fLeashingDelay = 0.0f;
        }


    }


}



//----------------------------------------------------------------------------
// void CPlayerClnt::UpdateJump()
//
//-----------------------------------------------------------------------------
void CPlayerClnt::UpdateJump()
{
	// Update our position from the velocity
	ILTClientPhysics* pCPhysics = (ILTClientPhysics*)g_pLTCPhysics;

	MoveInfo info;
	info.m_Offset.Init();
	info.m_hObject = m_hObject;
	info.m_dt = g_pLTClient->GetFrameTime();
	pCPhysics->UpdateMovement(&info);

	// Get current resultant spacial information
	LTVector vPos, vVel;
	LTRotation rRot;
    g_pLTClient->GetObjectPos(m_hObject, &vPos);
    g_pLTCPhysics->GetVelocity(m_hObject, &vVel);
    g_pLTClient->GetObjectRotation(m_hObject, &rRot);

	// Get current frame time
	LTFLOAT fFrameTime = g_pLTClient->GetFrameTime();

	if(m_fJumpTimeRemaining >= JUMP_TIME / 2.0f)
	{
		vVel.y = JUMP_VELOCITY;
	}

	m_fJumpTimeRemaining -= fFrameTime;

	if(m_fJumpTimeRemaining < 0.0f)
	{
		m_bIsJumping = false;
	}

    g_pLTCPhysics->SetVelocity(m_hObject, &vVel);
}



//----------------------------------------------------------------------------
// void CPlayerClnt::Attack()
//
//-----------------------------------------------------------------------------
bool CPlayerClnt::Attack()
{
    if(m_bAttacking)
    {
        return false;
    }

    // Play anims
    PlayAttackAnimation("UMFi", m_idUpperBodyTracker);

    m_bAttacking = true;
    return m_bAttacking;
}



//----------------------------------------------------------------------------
// void CPlayerClnt::UpdateAttacking()
//
//-----------------------------------------------------------------------------
void CPlayerClnt::UpdateAttacking(uint8 nTracker)
{
	HMODELANIM hCurAnim;
    g_pLTCModel->GetCurAnim(m_hObject, nTracker, hCurAnim);

	uint32 nAnimLen, nAnimTime;
    g_pLTCModel->GetCurAnimLength(m_hObject, nTracker, nAnimLen);
    g_pLTCModel->GetCurAnimTime(m_hObject, nTracker, nAnimTime);

    if(nAnimTime >= nAnimLen)
    {
        PlayMovementAnimation("LSt", m_idUpperBodyTracker);
        m_bAttacking = false;

		HideAttachClientFX();
	}
}



//----------------------------------------------------------------------------
// void CPlayerClnt::PlayAnimation(const char* sAnimName)
//
//-----------------------------------------------------------------------------
void CPlayerClnt::PlayMovementAnimation(const char* sAnimName, uint8 nTracker)
{
	HMODELANIM hAnim = g_pLTClient->GetAnimIndex(m_hObject, (char *)sAnimName);

    HMODELANIM hCurAnim;
    g_pLTCModel->GetCurAnim(m_hObject, nTracker, hCurAnim);
    if(hAnim != hCurAnim)
    {
        g_pLTCModel->SetCurAnim(m_hObject, nTracker, hAnim);
        g_pLTCModel->SetLooping(m_hObject, nTracker, LTTRUE);

        uint32 nAnimLen;
        g_pLTCModel->GetCurAnimLength(m_hObject, nTracker, nAnimLen);

        // Send animation to server
        ILTMessage_Write *pMessage;
        LTRESULT nResult = g_pLTCCommon->CreateMessage(pMessage);

        if( LT_OK == nResult)
        {
            pMessage->IncRef();
            pMessage->Writeuint8(MSG_CS_ANIM);
            pMessage->WriteString(sAnimName);
            pMessage->Writeuint8(nTracker);
            pMessage->Writebool(true);
            g_pLTClient->SendToServer(pMessage->Read(), 0);
            pMessage->DecRef();
        }
    }
}



//----------------------------------------------------------------------------
// void CPlayerClnt::PlayAttackAnimation(const char* sAnimName)
//
//-----------------------------------------------------------------------------
void CPlayerClnt::PlayAttackAnimation(const char* sAnimName, uint8 nTracker)
{
	HMODELANIM hAnim = g_pLTClient->GetAnimIndex(m_hObject, (char *)sAnimName);

    HMODELANIM hCurAnim;
    g_pLTCModel->GetCurAnim(m_hObject, nTracker, hCurAnim);
    if(hAnim != hCurAnim)
    {
        g_pLTCModel->SetCurAnim(m_hObject, nTracker, hAnim);
        g_pLTCModel->SetLooping(m_hObject, nTracker, LTFALSE);

        // Send animation to server
        ILTMessage_Write *pMessage;
        LTRESULT nResult = g_pLTCCommon->CreateMessage(pMessage);

        if( LT_OK == nResult)
        {
            pMessage->IncRef();
            pMessage->Writeuint8(MSG_CS_ANIM);
            pMessage->WriteString(sAnimName);
            pMessage->Writeuint8(nTracker);
            pMessage->Writebool(false);
            g_pLTClient->SendToServer(pMessage->Read(), 0);
            pMessage->DecRef();
        }
        else
        {
            g_pLTClient->CPrint("Failed to create message!");
        }

		ShowAttachClientFX();
	}
}



//----------------------------------------------------------------------------
// void CPlayerClnt::SetClub()
//
//-----------------------------------------------------------------------------
void CPlayerClnt::SetClub(HOBJECT hObj)
{
	m_hClubObject = hObj;

	CreateAttachClientFX();
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerClnt::ShowAttachClientFX
//
//	PURPOSE:	Show all the view attach client fx
//
// ----------------------------------------------------------------------- //

void CPlayerClnt::ShowAttachClientFX()
{
	for ( CLIENTFX_LINK_NODE* pCurr = m_AttachClientFX.m_pNext; pCurr; pCurr = pCurr->m_pNext )
	{
		if ( pCurr->m_Link.IsValid() )
		{
			pCurr->m_Link.GetInstance()->Show();
		}
		else
		{
			// when we hit 0, there are no more
			return;
		}
	}
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerClnt::HideAttachClientFX
//
//	PURPOSE:	Hide all the view attach client fx
//
// ----------------------------------------------------------------------- //

void CPlayerClnt::HideAttachClientFX()
{
	for ( CLIENTFX_LINK_NODE* pCurr = m_AttachClientFX.m_pNext; pCurr; pCurr = pCurr->m_pNext )
	{
		if ( pCurr->m_Link.IsValid() )
		{
			pCurr->m_Link.GetInstance()->Hide();
		}
		else
		{
			// when we hit 0, there are no more
			return;
		}
	}
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerClnt::CreateAttachClientFX
//
//	PURPOSE:	Create the player attach effect.
//
// ----------------------------------------------------------------------- //

void CPlayerClnt::CreateAttachClientFX()
{
	RemoveAttachClientFX();

	if (LTNULL != m_hClubObject)
	{
		const char *pClientFXName = "PlayerPolyTrail";
		CLIENTFX_CREATESTRUCT fxInit( pClientFXName, FXFLAG_LOOP, m_hClubObject );

		CLIENTFX_LINK_NODE* pNewNode = new CLIENTFX_LINK_NODE;

		if(pNewNode)
		{
			g_pClientFXMgr->CreateClientFX( &pNewNode->m_Link, fxInit, true );
			if ( pNewNode->m_Link.IsValid() )
			{
				// start out hidden
				pNewNode->m_Link.GetInstance()->Hide();

				// add the link to the list
				m_AttachClientFX.AddToEnd(pNewNode);
			}
		}
	}
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPlayerClnt::RemoveAttachClientFX
//
//	PURPOSE:	Destroys all the player attach client fx
//
// ----------------------------------------------------------------------- //

void CPlayerClnt::RemoveAttachClientFX()
{
	ASSERT( 0 != g_pClientFXMgr );

	for ( CLIENTFX_LINK_NODE* pCurr = m_AttachClientFX.m_pNext; pCurr; pCurr = pCurr->m_pNext )
	{
		if ( pCurr->m_Link.IsValid() )
		{
			g_pClientFXMgr->ShutdownClientFX( &pCurr->m_Link );
		}
	}
}
