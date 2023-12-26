//------------------------------------------------------------------------------//
//
// MODULE   : Seal.cpp
//
// PURPOSE  : Seal - Implementation
//
// CREATED  : 08/27/2002
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#include "seal.h"
#include "playersrvr.h"
#include "msgids.h"

#include <iltcommon.h>
#include <iltsoundmgr.h>
#include <iltphysics.h>
#include <iltmodel.h>
#include "serverinterfaces.h"
#include "serverutilities.h"
#include "AIVolume.h"

#define ANIM_IDLE "idleAnim", true, false
#define ANIM_HIT  "hitAnim", false, true
#define ANIM_RUN  "runAnim", true, true
#define ANIM_DIE  "deathAnim", false, false
#define ANIM_DIE2 "death2Anim", false, false
#define ANIM_BITE "biteAnim", false, true


namespace
{
	// seal speeds
	const float g_Speed_Flee 				= 200.0f;
	const float g_Speed_Wander 				= 50.0f;
	const float g_Speed_Warping 			= 10.0f;

	// seal AI counters
	const unsigned g_Counter_Flee 			= 360;
	const unsigned g_Counter_Wander 		= 60;
	const unsigned g_Counter_Wander_Init 	= 200;

    // seal Raycast distance
	const float g_RayCastToGroundDist		= 2000.0f;
	const float g_MaxDistBeforeGroundCheck 	= 50.0f;

	// seal life
	const uint8 g_MaxSealLife 				= 20;

	// seal worth
	const float g_SealPeltValue 			= 9.95f;
}


//-----------------------------------------------------------------------------
BEGIN_CLASS(Seal)
ADD_STRINGPROP_FLAG(Filename, "", PF_MODEL | PF_FILENAME)
ADD_STRINGPROP_FLAG(Texture, "", PF_FILENAME)
ADD_STRINGPROP_FLAG(RenderStyle, "", PF_FILENAME)
ADD_REALPROP(SealValue, 0.0f)
END_CLASS_DEFAULT_FLAGS(Seal, BaseClass, LTNULL, LTNULL, CF_ALWAYSLOAD)



//-----------------------------------------------------------------------------
//	Seal::Seal():
//
//-----------------------------------------------------------------------------
Seal::Seal():
m_nSealType(0),
m_fSealValue(0.0f),
m_iHitPoints(g_MaxSealLife),
m_hAnim(INVALID_MODEL_ANIM),
m_hAssasin(NULL),
m_iAction(ACTION_SPAWNED),
m_HitTime(0),
m_WanderTime(0),
m_NextWanderTime(g_Counter_Wander_Init),
m_TurnFactor(1.0f),
m_fDistanceTraveled(g_MaxDistBeforeGroundCheck),
m_vLastPos(0.0f, 0.0f, 0.0f),
m_hFirstAttacker(NULL),
m_fFirstAttackerDmg(0.0f),
m_FallingDist(0.0f)
{
}



//-----------------------------------------------------------------------------
//	Seal::EngineMessageFn(uint32 messageID, void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Seal::EngineMessageFn(uint32 messageID, void *pData, float fData)
{

    switch (messageID)
    {
    case MID_PRECREATE:
        return PreCreate(pData, fData);
    case MID_INITIALUPDATE:
        {
			// Don't ignore small collisions.
			g_pLTSPhysics->SetForceIgnoreLimit(m_hObject, 0.0f);

			g_pLTServer->SetNextUpdate(m_hObject, 0.0001f);
            ChangeAnimation(ANIM_IDLE);
        }
        break;
    case MID_TOUCHNOTIFY:
        return TouchNotify(pData, fData);
    case MID_UPDATE:
        return Update();
        break;
    case MID_MODELSTRINGKEY:
        {
			ArgList* pArgList = (ArgList*)pData;

			char szBuffer[256];
			sprintf(szBuffer, "");

			for ( int i = 0 ; i < pArgList->argc ; i++ )
			{
                if (strcmp("BARK1", pArgList->argv[i]) == 0 ||
					strcmp("BARK2", pArgList->argv[i]) == 0)
                {
                    PlayClientFX("SealYelp", m_hObject, LTNULL, LTNULL, 0);
                }
                else if(strcmp("HIT_DONE", pArgList->argv[i]) == 0)
                {
					// flee
					ChangeAnimation(ANIM_RUN);
					m_iAction = ACTION_FLEE;
					m_HitTime = g_Counter_Flee;
				}
                else if(strcmp("DEATH_DONE", pArgList->argv[i]) == 0)
                {
					Die();
				}
                else if(strcmp("BITE", pArgList->argv[i]) == 0)
                {
					//TODO: Raycast and deal damage to a player if intersection is detected.
				}
			}
            return 1;
        }
		break;

    default:
        break;
    }

    // Pass the message along to parent class.
    return BaseClass::EngineMessageFn(messageID, pData, fData);
}



//-----------------------------------------------------------------------------
//	Seal::ObjectMessageFn(HOBJECT hSender, void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Seal::ObjectMessageFn(HOBJECT hSender, ILTMessage_Read *pMsg)
{
	pMsg->SeekTo(0);
	uint32 messageID = pMsg->Readuint32();
	switch(messageID)
	{
		case OBJ_MID_DAMAGE:
            {
				// Stop moving if the seal is hit.
				StopMoving();
				m_iAction = ACTION_HIT;

				uint8 iDamage   = pMsg->Readuint8();

                if(NULL == m_hFirstAttacker)
                {
                    m_hFirstAttacker = hSender;
                }

                if(hSender == m_hFirstAttacker)
                {
                    m_fFirstAttackerDmg += iDamage;
                }

                m_iHitPoints -= iDamage;
                if(m_iHitPoints < 1)
                {
                    m_hAssasin = hSender;
                    float fRand = g_pLTServer->Random(0.0f, 100.0f);
                    g_pLTServer->CPrint("Seal Death Rand: %f", fRand);

                    if(fRand > 50.0f)
                    {
                        ChangeAnimation(ANIM_DIE);
                    }
					else
                    {
                        ChangeAnimation(ANIM_DIE2);
                    }

					m_iAction = ACTION_DYING;

					PlayClientFX("SealFall", m_hObject, LTNULL, LTNULL, 0);
                }
				else
                {
                    //change to hit animation if it's not already playing
                    ChangeAnimation(ANIM_HIT);
                }
            }
            break;
        default:
            break;
    }

    return BaseClass::ObjectMessageFn(hSender, pMsg);
}



//-----------------------------------------------------------------------------
//	Seal::PreCreate(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Seal::PreCreate(void *pData, float fData)
{
    // Let parent class handle it first
    BaseClass::EngineMessageFn(MID_PRECREATE, pData, fData);

    // Cast pData to a ObjectCreateStruct* for convenience
    ObjectCreateStruct* pStruct = (ObjectCreateStruct*)pData;

    // Setup flags
    pStruct->m_Flags = FLAG_VISIBLE | FLAG_SOLID | FLAG_MODELKEYS; //| FLAG_TOUCH_NOTIFY;

	pStruct->m_Flags2 |= FLAG2_SERVERDIMS;

    // Set the object type to OT_MODEL
    pStruct->m_ObjectType = OT_MODEL;

    // Check to see if this is coming from a world file
    if(fData == PRECREATE_WORLDFILE)
    {
        ReadProps(pStruct);
    }

	// Randomize the seal's initial rotation.
	m_fYaw 		= g_pLTServer->Random(0.0f, MATH_PI*2.0f);
	m_fPitch 	= 0.0f;
	m_fRoll 	= 0.0f;
	g_pLTSCommon->SetupEuler(pStruct->m_Rotation, m_fPitch, m_fYaw, m_fRoll);

    // Return default of 1
    return 1;
}



//-----------------------------------------------------------------------------
//	Seal::Update()
//
//-----------------------------------------------------------------------------
uint32 Seal::Update()
{
    switch(m_iAction)
    {
		case ACTION_SPAWNED:
		{
			Spawn();

			break;
		}

		case ACTION_WARPING:
        {
			Warp();

			break;
		}

        case ACTION_NORMAL:
        {
			// Should we wander?
			if (--m_NextWanderTime <= 0)
			{
				m_NextWanderTime = g_pLTServer->IntRandom(g_Counter_Wander_Init, g_Counter_Wander_Init*10);

				// wander
				ChangeAnimation(ANIM_RUN);
				m_WanderTime = g_Counter_Wander;
				m_iAction = ACTION_WANDER;
			}

			break;
        }

  		case ACTION_WANDER:
		{
			StartMoving(g_Speed_Wander);
			FlopAround(m_WanderTime, g_Speed_Wander);

			break;
		}

  		case ACTION_FLEE:
		{
			// Give some randomness to the speed.
			float SpeedFactor = g_pLTServer->Random(0.5f, 1.5f);

			// RUN!
			StartMoving(g_Speed_Flee*SpeedFactor);
			FlopAround(m_HitTime, g_Speed_Flee);

			break;
		}

		case ACTION_DEAD:
		{
			if(m_fTimeToDie < 0.0f)
			{
				// Remove the seal object.
				g_pLTServer->RemoveObject(m_hObject);

				// Ask the server for a handle to the object
				ObjArray<HOBJECT, 1> objArray;
				uint32 iNumObjects = 0;
				g_pLTServer->FindNamedObjects("AIVolume0", objArray, &iNumObjects);

				// Check to make sure that there were objects found.
				if (iNumObjects > 0)
				{
					// Decrement the number of seals.
					HOBJECT hObj = objArray.GetObject(0);
					AIVolume *pAI = (AIVolume*)g_pLTServer->HandleToObject(hObj);
					pAI->DecrementSealCount();
				}
			}
			else
			{
				m_fTimeToDie -= g_pLTServer->GetFrameTime();
			}

			break;
		}

		case ACTION_HIT:
		case ACTION_DYING:
		{
			// Do nothing.
			break;
		}

        default:
        {
			g_pLTServer->CPrint("(Seal) Unknown seal action!");
			break;
        }
    }

    g_pLTServer->SetNextUpdate(m_hObject, 0.01667f);

	return 1;
}



//-----------------------------------------------------------------------------
//	Seal::TouchNotify(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Seal::TouchNotify(void *pData, float fData)
{
    HOBJECT hObj = (HOBJECT)pData;

    LTRESULT result = g_pLTSPhysics->IsWorldObject( hObj );

    if (result == LT_YES)
    {
		return 1;
    }

    return 1;
}



//-----------------------------------------------------------------------------
//	Seal::ReadProps(ObjectCreateStruct* pStruct)
//
//-----------------------------------------------------------------------------
void Seal::ReadProps(ObjectCreateStruct* pStruct)
{
    g_pLTServer->GetPropString("Filename", pStruct->m_Filename, MAX_CS_FILENAME_LEN);
    g_pLTServer->GetPropString("Texture", pStruct->m_SkinNames[0], MAX_CS_FILENAME_LEN);
    g_pLTServer->GetPropString("RenderStyle", pStruct->m_RenderStyleNames[0], MAX_CS_FILENAME_LEN);
    g_pLTServer->GetPropReal("SealValue", &m_fSealValue);
}



//-----------------------------------------------------------------------------
//	Seal::PlaySound()
//
//-----------------------------------------------------------------------------
void Seal::PlaySound()
{
/*
    PlaySoundInfo playSoundInfo;
    PLAYSOUNDINFO_INIT(playSoundInfo);
    HLTSOUND hSnd = LTNULL;

    playSoundInfo.m_dwFlags = PLAYSOUND_ATTACHED | PLAYSOUND_3D;
    playSoundInfo.m_hObject = m_hObject;
    playSoundInfo.m_fInnerRadius = 100.0f;
    playSoundInfo.m_fOuterRadius = 500.0f;
    playSoundInfo.m_nVolume = 100;

    SAFE_STRCPY(playSoundInfo.m_szSoundName, "Snd/Seal2.wav");

    LTRESULT hResult = g_pLTServer->SoundMgr()->PlaySound(&playSoundInfo, hSnd);

    if(hResult != LT_OK)
    {
        g_pLTServer->CPrint("(Seal) Error playing sound");
    }
*/
}



//-----------------------------------------------------------------------------
//	Seal::ChangeAnimation()
//
//-----------------------------------------------------------------------------
void Seal::ChangeAnimation(const char* sAnimName, bool bLooping, bool bInterable)
{
    HMODELANIM hAnim = g_pLTServer->GetAnimIndex(m_hObject, sAnimName);

    if((hAnim != m_hAnim) || (bInterable == true))
    {
        // change it
        m_hAnim = hAnim;
        g_pLTServer->SetModelAnimation(m_hObject, m_hAnim);
        g_pLTServer->SetModelLooping(m_hObject, bLooping);
        g_pLTServer->SetModelPlaying(m_hObject, true);
        g_pLTServer->ResetModelAnimation(m_hObject);

		// Change Dim based on the current animation
		LTVector vDims;
		g_pLTSCommon->GetModelAnimUserDims(m_hObject, &vDims, hAnim);
		g_pLTSPhysics->SetObjectDims(m_hObject, &vDims, 0);
     }
}



//-----------------------------------------------------------------------------
//	Seal::Spawn()
//
//-----------------------------------------------------------------------------
void Seal::Spawn()
{
	LTVector Pos;
	LTRotation Rot;
	GetPosRot(&Pos, &Rot);

	// Ray cast to the ground to see how far we need to fall.
	LTVector IntersectPt;
	if (RayCastToGround(Pos, Rot, &IntersectPt))
	{
		// Set the falling distance.
		m_FallingDist = Pos.y - IntersectPt.y;
		if (m_FallingDist < 0.0f)
		{
			g_pLTServer->CPrint("(Seal) Seal spawned below ground!");
		}

		// Offset the falling distance by the seal's bounding box.
		LTVector vDims;
		g_pLTSCommon->GetModelAnimUserDims(m_hObject, &vDims, m_hAnim);
		m_FallingDist -= vDims.y;

		// Change the state to warping.
		m_iAction = ACTION_WARPING;
	}
	else
	{
		g_pLTServer->CPrint("(Seal) Could not find ground!");
	}
}



//-----------------------------------------------------------------------------
//	Seal::Warp()
//
//-----------------------------------------------------------------------------
void Seal::Warp()
{
	// Make the seal fall from the sky.
	m_FallingDist -= g_Speed_Warping;

	// Have we hit the ground?
	if (m_FallingDist < 0.0f)
	{
		// We have, so start running.
		ChangeAnimation(ANIM_RUN);
		m_iAction = ACTION_FLEE;
		m_HitTime = g_Counter_Flee;
	}
	else
	{
		// Nope. Just keep falling.

		LTVector Pos;
		LTRotation Rot;
		GetPosRot(&Pos, &Rot);

		Pos += Rot.Up() * -g_Speed_Warping;

		g_pLTServer->SetObjectRotation(m_hObject, &Rot);
		g_pLTServer->SetObjectPos(m_hObject, &Pos);
	}
}



//-----------------------------------------------------------------------------
//	Seal::StartMoving()
//
//		Set an initial velocity for the seal.
//-----------------------------------------------------------------------------
void Seal::StartMoving(float Speed)
{
	LTVector vPos;
	LTRotation rRot;
	GetPosRot(&vPos, &rRot);

	// Set the velocity with some downward motion.
	LTVector Vel = rRot.Forward() * Speed;
	Vel.y = -100.0f;
	g_pLTSPhysics->SetVelocity(m_hObject, &Vel);
}



//-----------------------------------------------------------------------------
//	Seal::StopMoving()
//
//		Set the velocity to zero.
//-----------------------------------------------------------------------------
void Seal::StopMoving()
{
	LTVector Vel(0.0f, 0.0f, 0.0f);
	g_pLTSPhysics->SetVelocity(m_hObject, &Vel);
}



//-----------------------------------------------------------------------------
//	Seal::FlopAround()
//
//-----------------------------------------------------------------------------
void Seal::FlopAround(int &Counter, float Speed)
{
	// Only move for a designated amount of frames.
	--Counter;
	if (Counter <= 0)
	{
		StopMoving();

		Counter = 0;
		ChangeAnimation(ANIM_IDLE);
		m_iAction = ACTION_NORMAL;

		return;
	}

	LTVector vPos;
	LTRotation rRot;
	GetPosRot(&vPos, &rRot);

	// Check to see if we are able to move.
	if (vPos.x - m_LastPos.x < 0.001f &&
		vPos.y - m_LastPos.y < 0.001f &&
		vPos.z - m_LastPos.z < 0.001f)
	{
		// Setup the next turn.
		if ((Counter % 30) == 0)
		{
			m_TurnFactor = (g_pLTServer->Random(-1.0f, 1.0f) < 0.0f) ? -1.0f : 1.0f;
		}

		// Turn left or right.
		m_fYaw += 0.1f * m_TurnFactor;

		// rotation
		g_pLTSCommon->SetupEuler(rRot, m_fPitch, m_fYaw, m_fRoll);
		g_pLTServer->SetObjectRotation(m_hObject, &rRot);
	}
	m_LastPos = vPos;
}



//-----------------------------------------------------------------------------
//	Seal::GetPosRot()
//
//		Helper function for getting the object position and rotation.
//-----------------------------------------------------------------------------
void Seal::GetPosRot(LTVector *pPos, LTRotation *pRot)
{
	// position
	if (pPos != NULL)
	{
		g_pLTServer->GetObjectPos(m_hObject, pPos);
	}

	// rotation
	if (pRot != NULL)
	{
		g_pLTServer->GetObjectRotation(m_hObject, pRot);
	}
}



//-----------------------------------------------------------------------------
//	Seal::RayCastToGround()
//
//		Casts a ray straight down from the object position and returns the
//		point of intersection.
//-----------------------------------------------------------------------------
bool Seal::RayCastToGround(const LTVector &Pos, const LTRotation &Rot, LTVector *pIntersectPt)
{
	if (pIntersectPt != NULL)
	{
		IntersectQuery qInfo;
		IntersectInfo iInfo;

		qInfo.m_From 	= Pos;
		qInfo.m_To   	= Pos + (Rot.Up() * -g_RayCastToGroundDist);
		qInfo.m_Flags 	= INTERSECT_HPOLY | IGNORE_NONSOLID;

		if (g_pLTServer->IntersectSegment(&qInfo, &iInfo))
		{
			if (iInfo.m_hObject)
			{
				*pIntersectPt = iInfo.m_Point;

				return true;
			}
		}
	}

	return false;
}


void Seal::SendKillStealChatMessage(HOBJECT hFirst, HOBJECT hKiller)
{

    char szChatString[1024];

    CPlayerSrvr *pFirstPlayer = (CPlayerSrvr*)g_pLTServer->HandleToObject(hFirst);
    CPlayerSrvr *pKillerPlayer = (CPlayerSrvr*)g_pLTServer->HandleToObject(hKiller);

    sprintf(szChatString, "%s stole a kill from %s !!!", pKillerPlayer->GetPlayerName(), pFirstPlayer->GetPlayerName());

    ILTMessage_Write *pMsg;
    LTRESULT nResult = g_pLTSCommon->CreateMessage(pMsg);
	pMsg->IncRef();
    pMsg->Writeuint8(MSG_SC_CHAT);
    pMsg->WriteString(szChatString);
    g_pLTServer->SendToClient(pMsg->Read(), NULL, MESSAGE_GUARANTEED);
    pMsg->DecRef();
}


//-----------------------------------------------------------------------------
//	Seal::Die()
//
//-----------------------------------------------------------------------------
void Seal::Die()
{
	StopMoving();

	// Calc seal kill value
	bool bKillSteal = ( (m_hAssasin != m_hFirstAttacker) && (m_fFirstAttackerDmg > ((float)g_MaxSealLife)/4.0f) );
	float fSealValue = g_SealPeltValue * ((bKillSteal) ? 2.0f:1.0f);

    // DIE!!!!
	if (bKillSteal)
	{
		PlayClientFX("SealDieKillSteal", m_hObject, LTNULL, LTNULL, 0);
        PlayClientFX("KillStealOwned", m_hFirstAttacker, LTNULL, LTNULL, 0);
        SendKillStealChatMessage(m_hFirstAttacker, m_hAssasin);
	}
	else
	{
		PlayClientFX("SealDie", m_hObject, LTNULL, LTNULL, 0);
	}

    // Send kill message (for scoring)
    ILTMessage_Write *pMsg;
    g_pLTSCommon->CreateMessage(pMsg);
    pMsg->IncRef();
    pMsg->Writeuint32(OBJ_MID_KILLSCORE);
	pMsg->Writefloat(fSealValue);
    g_pLTServer->SendToObject(pMsg->Read(), m_hObject, m_hAssasin, 0);
    pMsg->DecRef();

	// Make him invisible
 	g_pLTSCommon->SetObjectFlags(m_hObject, OFT_Flags, 0, FLAG_VISIBLE);

    // Make him non-solid
    g_pLTSCommon->SetObjectFlags(m_hObject, OFT_Flags, 0, FLAG_SOLID);

	m_iAction = ACTION_DEAD;
	m_fTimeToDie = 2.0f;
}

