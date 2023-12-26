//------------------------------------------------------------------------------//
//
// MODULE   : Snowman.cpp
//
// PURPOSE  : Snowman - Implementation
//
// CREATED  : 08/27/2002
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#include "snowman.h"
#include "playersrvr.h"
#include "msgids.h"

#include <iltcommon.h>
#include <iltsoundmgr.h>
#include <iltphysics.h>
#include <iltmodel.h>
#include "serverinterfaces.h"
#include "serverutilities.h"
#include "AIVolume.h"

#define ANIM_IDLE "", false, false
#define ANIM_DIE  "fallAnim", false, true

namespace
{
	// snowman life
	const uint8 g_MaxSnowmanLife 				= 100;

	// snowman worth
	const float g_SnowmanPeltValue 			    = 17.31f;

    //
    const float g_SnowmanDeathDelay             = 30.0f;
}


//-----------------------------------------------------------------------------
BEGIN_CLASS(Snowman)
/*
ADD_STRINGPROP_FLAG(Filename, "", PF_MODEL | PF_FILENAME)
ADD_STRINGPROP_FLAG(Texture, "", PF_FILENAME)
ADD_STRINGPROP_FLAG(RenderStyle, "", PF_FILENAME)
ADD_REALPROP(SnowmanValue, 0.0f)
*/
END_CLASS_DEFAULT_FLAGS(Snowman, BaseClass, LTNULL, LTNULL, CF_ALWAYSLOAD)



//-----------------------------------------------------------------------------
//	Snowman::Snowman():
//
//-----------------------------------------------------------------------------
Snowman::Snowman():
m_nSnowmanType(0),
m_fSnowmanValue(0.0f),
m_iHitPoints(g_MaxSnowmanLife),
m_hAnim(INVALID_MODEL_ANIM),
m_hAssasin(NULL),
m_iAction(ACTION_NORMAL),
m_hFirstAttacker(NULL),
m_fFirstAttackerDmg(0.0f)
{
}



//-----------------------------------------------------------------------------
//	Snowman::EngineMessageFn(uint32 messageID, void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Snowman::EngineMessageFn(uint32 messageID, void *pData, float fData)
{

    switch (messageID)
    {
    case MID_PRECREATE:
        return PreCreate(pData, fData);
    case MID_INITIALUPDATE:
        {
			g_pLTServer->SetNextUpdate(m_hObject, 0.0001f);
            LTVector vDims(45.0f, 124.0f, 45.0f);
            g_pLTSPhysics->SetObjectDims(m_hObject, &vDims, 0);
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
                if(strcmp("DEATH_DONE", pArgList->argv[i]) == 0)
                {
					Die();
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
//	Snowman::ObjectMessageFn(HOBJECT hSender, void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Snowman::ObjectMessageFn(HOBJECT hSender, ILTMessage_Read *pMsg)
{
	pMsg->SeekTo(0);
	uint32 messageID = pMsg->Readuint32();
	switch(messageID)
	{
		case OBJ_MID_DAMAGE:
            {
				//m_iAction = ACTION_HIT;
                if(ACTION_DYING == m_iAction)
                {
                    break;
                }

                if(m_iHitPoints < 1)
                {
                    break;
                }

				uint8 iDamage   = pMsg->Readuint8();

                m_iHitPoints -= iDamage;
                g_pLTServer->CPrint("Snow man remaining hitpoints: %d", m_iHitPoints);
                if(m_iHitPoints < 1)
                {
                    m_hAssasin = hSender;
                    ChangeAnimation(ANIM_DIE);

					m_iAction = ACTION_DYING;
					PlayClientFX("SnowmanFall", m_hObject, LTNULL, LTNULL, 0);
                    g_pLTSCommon->SetObjectFlags(m_hObject, OFT_Flags, 0, FLAG_SOLID);
                }
				else
                {
                    //change to hit animation if it's not already playing
                    //ChangeAnimation(ANIM_HIT);
                }
            }
            break;
        default:
            break;
    }

    return BaseClass::ObjectMessageFn(hSender, pMsg);
}



//-----------------------------------------------------------------------------
//	Snowman::PreCreate(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Snowman::PreCreate(void *pData, float fData)
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

    sprintf(pStruct->m_Filename, "Models/SnowMan.ltb");
    sprintf(pStruct->m_SkinName, "ModelTextures/Snowman.dtx");

    // Check to see if this is coming from a world file
    if(fData == PRECREATE_WORLDFILE)
    {
        ReadProps(pStruct);
    }

    // Return default of 1
    return 1;
}



//-----------------------------------------------------------------------------
//	Snowman::Update()
//
//-----------------------------------------------------------------------------
uint32 Snowman::Update()
{
    switch(m_iAction)
    {
        case ACTION_NORMAL:
        {
			break;
        }


		case ACTION_DEAD:
		{
			if(m_fTimeToDie < 0.0f)
			{
                //Reset
                ChangeAnimation(ANIM_IDLE);
                m_iHitPoints = g_MaxSnowmanLife;
                m_hAssasin = NULL;
                g_pLTSCommon->SetObjectFlags(m_hObject, OFT_Flags, FLAG_SOLID, FLAG_SOLID);
                m_iAction = ACTION_NORMAL;
                m_fTimeToDie = g_SnowmanDeathDelay;

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
			g_pLTServer->CPrint("(Snowman) Unknown Snowman action!");
			break;
        }
    }

    g_pLTServer->SetNextUpdate(m_hObject, 0.0166667f);

	return 1;
}



//-----------------------------------------------------------------------------
//	Snowman::TouchNotify(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Snowman::TouchNotify(void *pData, float fData)
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
//	Snowman::ReadProps(ObjectCreateStruct* pStruct)
//
//-----------------------------------------------------------------------------
void Snowman::ReadProps(ObjectCreateStruct* pStruct)
{
    /*
    g_pLTServer->GetPropString("Filename", pStruct->m_Filename, MAX_CS_FILENAME_LEN);
    g_pLTServer->GetPropString("Texture", pStruct->m_SkinNames[0], MAX_CS_FILENAME_LEN);
    g_pLTServer->GetPropString("RenderStyle", pStruct->m_RenderStyleNames[0], MAX_CS_FILENAME_LEN);
    g_pLTServer->GetPropReal("SnowmanValue", &m_fSnowmanValue);
    */
}



//-----------------------------------------------------------------------------
//	Snowman::PlaySound()
//
//-----------------------------------------------------------------------------
void Snowman::PlaySound()
{

}



//-----------------------------------------------------------------------------
//	Snowman::ChangeAnimation()
//
//-----------------------------------------------------------------------------
void Snowman::ChangeAnimation(const char* sAnimName, bool bLooping, bool bInterable)
{

    if(strlen(sAnimName) < 1)
    {
        g_pLTServer->SetModelPlaying(m_hObject, false);
        g_pLTServer->SetModelLooping(m_hObject, false);
        g_pLTServer->ResetModelAnimation(m_hObject);
        return;
    }


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
//	Snowman::SendKillChatMessage(HOBJECT hKiller)
//
//-----------------------------------------------------------------------------
void Snowman::SendKillChatMessage(HOBJECT hKiller)
{

    char szChatString[1024];

    CPlayerSrvr *pKillerPlayer = (CPlayerSrvr*)g_pLTServer->HandleToObject(hKiller);
    sprintf(szChatString, "%s killed the snowman !!!", pKillerPlayer->GetPlayerName());

    ILTMessage_Write *pMsg;
    LTRESULT nResult = g_pLTSCommon->CreateMessage(pMsg);
	pMsg->IncRef();
    pMsg->Writeuint8(MSG_SC_CHAT);
    pMsg->WriteString(szChatString);
    g_pLTServer->SendToClient(pMsg->Read(), NULL, MESSAGE_GUARANTEED);
    pMsg->DecRef();
}


//-----------------------------------------------------------------------------
//	Snowman::Die()
//
//-----------------------------------------------------------------------------
void Snowman::Die()
{
	//StopMoving();

	// Calc seal kill value
	PlayClientFX("SnowmanDie", m_hObject, LTNULL, LTNULL, 0);
    SendKillChatMessage(m_hAssasin);


    // Send kill message (for scoring)
    ILTMessage_Write *pMsg;
    g_pLTSCommon->CreateMessage(pMsg);
    pMsg->IncRef();
    pMsg->Writeuint32(OBJ_MID_KILLSCORE_SNOWMAN);
	pMsg->Writefloat(g_SnowmanPeltValue);
    g_pLTServer->SendToObject(pMsg->Read(), m_hObject, m_hAssasin, 0);
    pMsg->DecRef();

	// Make him invisible
 	//g_pLTSCommon->SetObjectFlags(m_hObject, OFT_Flags, 0, FLAG_VISIBLE);

    // Make him non-solid
    //g_pLTSCommon->SetObjectFlags(m_hObject, OFT_Flags, 0, FLAG_SOLID);

	m_iAction = ACTION_DEAD;
	m_fTimeToDie = g_SnowmanDeathDelay;
}

