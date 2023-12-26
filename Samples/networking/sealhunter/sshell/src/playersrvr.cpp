//------------------------------------------------------------------------------//
//
// MODULE	: playersrvr.cpp
//
// PURPOSE	: PlayerSrvr - Implementation
//
// CREATED	: 07/15/2002
//
// (c) 2002 LithTech, Inc.	All Rights Reserved
//
//------------------------------------------------------------------------------//

#include "playersrvr.h"

#include <ltobjectcreate.h>
#include <iltmodel.h>
#include <iltphysics.h>

#include "serverinterfaces.h"
#include "msgids.h"
#include "animids.h"
#include "pickup.h"
#include "projectile.h"

#include "serverutilities.h"
#include <iltphysics.h>
#include <ltobjectcreate.h>
#include <iltcommon.h>
#include "FxFlags.h"
#include "statsmanager.h"


//-----------------------------------------------------------------------------
BEGIN_CLASS(CPlayerSrvr)
END_CLASS_DEFAULT_FLAGS(CPlayerSrvr, BaseClass, LTNULL, LTNULL, CF_HIDDEN)



//-----------------------------------------------------------------------------
//	CPlayerSrvr::EngineMessageFn(uint32 messageID, void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 CPlayerSrvr::EngineMessageFn(uint32 messageID, void *pData, float fData)
{

	switch (messageID)
	{
	case MID_PRECREATE:
		return PreCreate(pData, fData);
    case MID_INITIALUPDATE:
		{
			g_pLTServer->SetNextUpdate(m_hObject, 0.0166f);
			return InitialUpdate(pData, fData);
		}
	case MID_UPDATE:
		{

            //Do we need to send score stats?

            if(m_bSendStats)
            {
                if(m_iSendStatsCounter == 0)
                {

                    // Get the stats
                    uint32 iNumPlayers = g_pStatsManager->GetNumPlayers();
                    SCORESTRUCT *scoreStruct = new SCORESTRUCT[iNumPlayers];
                    g_pStatsManager->GetPlayerScores(scoreStruct);


                    //Send the stats
	                ILTMessage_Write *pMsg;
	                LTRESULT nResult = g_pLTSCommon->CreateMessage(pMsg);
	                pMsg->IncRef();
	                pMsg->Writeint8(MSG_SERVER_SCORES);
                    pMsg->Writeint8(static_cast<uint8>(iNumPlayers));

                    //Write each entry to the message
                    for(uint8 i = 0; i <iNumPlayers; i++)
                    {
                        pMsg->Writeuint32(scoreStruct[i].iClientID);
                        pMsg->WriteString(scoreStruct[i].sPlayerName);
                        pMsg->Writeuint32(scoreStruct[i].iScore);
                        pMsg->Writefloat(scoreStruct[i].fMoney);
                    }

                    //pMsg->WriteObject(m_hClub);
	                g_pLTServer->SendToClient(pMsg->Read(), m_hClient,  MESSAGE_GUARANTEED);
	                pMsg->DecRef();
                    //Reset the counter
                    m_iSendStatsCounter = (uint8)(2.0f / 0.25f); //desired delay divided by the update frequency
                }else
                {
                    --m_iSendStatsCounter;
                }
            }
            g_pLTServer->SetNextUpdate(m_hObject, 0.25f);


/*
			g_pLTSModel->UpdateModelOBB(m_hClub, &m_WeaponOBB);

			//m_WeaponOBB.m_Pos
			//g_pLTServer->SetObjectPos(m_DebugSphere, &m_WeaponOBB.m_Pos);
			//g_pLTServer->SetNextUpdate(m_hObject, 0.0166f);
			HMODELNODE hNode;
			LTRESULT result = g_pLTSModel->GetNode(m_hClub, "joint1", hNode);
			if(LT_OK != result)
			{
				g_pLTServer->CPrint("(CPlayerSrvr) Error obtaining Node!");
				//return;
			}
			else
			{
				LTransform tTransform;
				g_pLTSModel->GetNodeTransform(m_hClub, hNode, tTransform, true);

				LTVector vPos = tTransform.m_Pos + (tTransform.m_Rot.Up() * m_WeaponOBB.m_Pos.y);

				g_pLTServer->SetObjectPos(m_DebugSphere, &vPos);
				g_pLTServer->SetNextUpdate(m_hObject, 0.0166f);
			}
			*/

		}
		break;
    case MID_MODELSTRINGKEY:
        {
			ArgList* pArgList = (ArgList*)pData;

			char szBuffer[256];
			sprintf(szBuffer, "");

			for ( int i = 0 ; i < pArgList->argc ; i++ )
			{
				//g_pLTServer->CPrint("MODEL KEY: %s", pArgList->argv[i]);
                if(strcmp("Throw", pArgList->argv[i]) == 0)
                {
                    CheckForHit();
                }
                else if(strcmp("FOOTSTEP_KEY_1", pArgList->argv[i]) == 0)
                {
                    PlayClientFX("SnowStep1", m_hObject, LTNULL, LTNULL, 0);
                }
                else if(strcmp("FOOTSTEP_KEY_2", pArgList->argv[i]) == 0)
                {
                    PlayClientFX("SnowStep2", m_hObject, LTNULL, LTNULL, 0);
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
//	CPlayerSrvr::ObjectMessageFn(HOBJECT hSender, void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 CPlayerSrvr::ObjectMessageFn(HOBJECT hSender, ILTMessage_Read *pMsg)
{
	pMsg->SeekTo(0);
	uint32 messageID = pMsg->Readuint32();
	switch(messageID)
	{
		case OBJ_MID_PICKUP:
            {
                uint8 nPickupType   = pMsg->Readuint8();
                float fPickupValue        = pMsg->Readfloat();

                g_pLTServer->CPrint("PlayerSrvr -> OBJ_MID_PICKUP  %d  %f", nPickupType, fPickupValue);
            }
            break;
        case OBJ_MID_KILLSCORE_SNOWMAN:
        case OBJ_MID_KILLSCORE:
            {
                //if it's a snowman don't inc the score.
                m_iScore+= (OBJ_MID_KILLSCORE_SNOWMAN == messageID) ? 0:1; 
                //m_fMoney += 9.95f;
				m_fMoney += pMsg->Readfloat();

                //send score to client
            	ILTMessage_Write *pMessage;
	            LTRESULT nResult = g_pLTSCommon->CreateMessage(pMessage);
	            pMessage->IncRef();
	            pMessage->Writeint8(MSG_CS_SCORE);
	            pMessage->Writeuint32(m_iScore);
                pMessage->Writefloat(m_fMoney);
	            g_pLTServer->SendToClient(pMessage->Read(), m_hClient,  MESSAGE_GUARANTEED);
	            pMessage->DecRef();

                //Play smack talk
                float fRand = g_pLTServer->Random(1.0f, 100.0f);

                g_pLTServer->CPrint("rand: %f", fRand);

                // Pick one and Play it
                if( fRand < 20.0f ) 
                {
					const int NumSmackTalkSounds = 12;
					int Index = g_pLTServer->IntRandom(0, NumSmackTalkSounds - 1);
                    char buf[32];
                    sprintf(buf, "SmackTalk%d", Index);
                    PlayClientFX(buf, m_hObject, LTNULL, LTNULL, 0);
                    g_pLTServer->CPrint("Play SmackTalk FX: %s", buf);

                    PlayClientFX("SmackTalkIcon", m_hObject, LTNULL, LTNULL, 0);
                }
            }
            break;
        default:
            break;
    }

    return BaseClass::ObjectMessageFn(hSender, pMsg);
}



//-----------------------------------------------------------------------------
//	CPlayerSrvr::SetPlayerName(const char* name)
//
//-----------------------------------------------------------------------------
void CPlayerSrvr::SetPlayerName(const char* name)
{
    strncpy(m_sName, name, 16);
}



//-----------------------------------------------------------------------------
//	CPlayerSrvr::GetPlayerName()
//
//-----------------------------------------------------------------------------
char* CPlayerSrvr::GetPlayerName()
{
    return m_sName;
}



//-----------------------------------------------------------------------------
//	CPlayerSrvr::PreCreate(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 CPlayerSrvr::PreCreate(void *pData, float fData)
{
	// Let parent class handle it first
	BaseClass::EngineMessageFn(MID_PRECREATE, pData, fData);

	// Cast pData to a ObjectCreateStruct* for convenience
	ObjectCreateStruct* pStruct = (ObjectCreateStruct*)pData;

    pStruct->m_Flags |= FLAG_MODELKEYS;

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



//-----------------------------------------------------------------------------
//	CPlayerSrvr::InitialUpdate(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 CPlayerSrvr::InitialUpdate(void *pData, float fData)
{
    // Set up animation trackers
    m_idUpperBodyTracker = ANIM_UPPER;
    g_pLTSModel->AddTracker(m_hObject, m_idUpperBodyTracker);

    m_idLowerBodyTracker = ANIM_LOWER;
    g_pLTSModel->AddTracker(m_hObject, m_idLowerBodyTracker);

    // Set up weight sets

    if ( LT_OK == g_pLTSModel->FindWeightSet(m_hObject, "Upper", m_hWeightUpper) )
    {
        g_pLTSModel->SetWeightSet(m_hObject, m_idUpperBodyTracker, m_hWeightUpper);
    }

    if ( LT_OK == g_pLTSModel->FindWeightSet(m_hObject, "Lower", m_hWeightLower) )
    {
        g_pLTSModel->SetWeightSet(m_hObject, m_idLowerBodyTracker, m_hWeightLower);
    }

    // Get hand socket
    LTRESULT SocketResult = g_pLTSModel->GetSocket(m_hObject, "RightHand", hRightHandSocket);
    if(LT_OK == SocketResult)
    {
        g_pLTServer->CPrint("Got the RightHand socket");
    }

    //Create and attach club
    HCLASS hClass = g_pLTServer->GetClass("BaseClass");
    ObjectCreateStruct ocs;
    ocs.Clear();
    ocs.m_ObjectType = OT_MODEL;
    ocs.m_Flags = FLAG_VISIBLE | FLAG_FORCECLIENTUPDATE | FLAG_SHADOW;
    ocs.m_Flags2 = FLAG2_DISABLEPREDICTION;
    float fRand = g_pLTServer->Random(1.0f, 100.0f);

    if(fRand < 33.0f)
    {
        strncpy(ocs.m_Filename, "Models/Mallet.ltb", 127);
        strncpy(ocs.m_SkinName , "ModelTextures/Mallet.dtx", 127);
    }
    else if(fRand < 66.0f)
    {
        strncpy(ocs.m_Filename, "Models/TelePole.ltb", 127);
        strncpy(ocs.m_SkinName , "ModelTextures/TelePole.dtx", 127);
    }
    else
    {
        strncpy(ocs.m_Filename, "Models/billyclub.ltb", 127);
        strncpy(ocs.m_SkinName , "ModelTextures/club.dtx", 127);
    }

    BaseClass *pClubObj = (BaseClass*)g_pLTServer->CreateObject(hClass, &ocs);

    if(pClubObj)
    {
        m_hClub = pClubObj->m_hObject;
        //Now attach it

        LTVector vOffset;
        LTVector rOffset;

        vOffset.Init();
        rOffset.Init();
        CreateAttachment(m_hClubAttach, m_hClub, "RightHand", rOffset, vOffset);

		//Get the radius and OBB for this weapon
		uint32 iNumOBBS;
		g_pLTSModel->GetNumModelOBBs(m_hClub, iNumOBBS);

		// We should only have one
		if(iNumOBBS == 1)
		{
			g_pLTSModel->GetModelOBBCopy(m_hClub, &m_WeaponOBB);
		}
		else
		{
			g_pLTServer->CPrint("(PlayerSrvr) Error: too many OBBs on weapon! Expecting only one.");
		}
    }

    return 1;
}



//-----------------------------------------------------------------------------
//	CPlayerSrvr::ReadProps(ObjectCreateStruct* pStruct)
//
//-----------------------------------------------------------------------------
void CPlayerSrvr::ReadProps(ObjectCreateStruct* pStruct)
{
	g_pLTServer->GetPropString("Name", pStruct->m_Filename, MAX_CS_FILENAME_LEN);
}



//-----------------------------------------------------------------------------
//	CPlayerSrvr::PlayAnimation(const char* sAnimName, uint8 nTracker)
//
//-----------------------------------------------------------------------------
void CPlayerSrvr::PlayAnimation(const char* sAnimName, uint8 nTracker, bool bLooping)
{
		HMODELANIM hAnim = g_pLTServer->GetAnimIndex(m_hObject, (char *)sAnimName);
        g_pLTSModel->SetCurAnim(m_hObject, nTracker, hAnim);
        g_pLTSModel->SetLooping(m_hObject, nTracker, bLooping);
}



//-----------------------------------------------------------------------------
//	CPlayerSrvr::CreateProjectile()
//
//-----------------------------------------------------------------------------
void CPlayerSrvr::CreateProjectile()
{
    LTVector vPos, vVel;
    LTRotation rRot;

    g_pLTServer->GetObjectPos(m_hObject, &vPos);
    g_pLTServer->GetObjectRotation(m_hObject, &rRot);
    g_pLTSPhysics->GetVelocity(m_hObject, &vVel);

    LTransform tSocketTransform;
    LTRESULT SocketTrResult = g_pLTSModel->GetSocketTransform(m_hObject,
                                                              hRightHandSocket,
                                                              tSocketTransform,
                                                              LTTRUE);

    // Create Projectile
    HCLASS hClass = g_pLTServer->GetClass("Projectile");

    ObjectCreateStruct ocs;
    ocs.m_ObjectType = OT_MODEL;
    ocs.m_Flags      = FLAG_VISIBLE;
    ocs.m_Rotation   = rRot;
    vPos += rRot.Forward() * 25.f;
    ocs.m_Pos        = tSocketTransform.m_Pos;

    strncpy(ocs.m_Filename, "Models/GenCan.ltb", MAX_CS_FILENAME_LEN -1);
    strncpy(ocs.m_SkinName, "ModelTextures/GenCan1.dtx", MAX_CS_FILENAME_LEN -1);
    Projectile* pObject = (Projectile*)g_pLTServer->CreateObject(hClass, &ocs);

    if(pObject)
    {
        pObject->SetOwner(m_hObject);
        g_pLTSPhysics->SetVelocity(pObject->m_hObject, &vVel);
    }
}




//-----------------------------------------------------------------------------
//	CPlayerSrvr::CreateAttachment()
//
//-----------------------------------------------------------------------------
void CPlayerSrvr::CreateAttachment(HATTACHMENT &hAttachment, HOBJECT hChildObject, const char* sSocket, LTVector &vRotOffset, LTVector &vPosOffset)
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
    }
	else
    {
        g_pLTServer->CPrint("GetSocket failed: " );
    }

    LTransform tSocketTransform;
    LTRESULT SocketTrResult = g_pLTSModel->GetSocketTransform(m_hObject, hSocket, tSocketTransform, LTTRUE);
    if(LT_OK == SocketTrResult)
    {
        g_pLTServer->CPrint("Got the transform");
    }
	else
    {
        g_pLTServer->CPrint("GetTrasform failed: ");
    }

    // We need to adjust the rotation of our weapon before we make the attachment
    rOffset.Rotate(rOffset.Right(), MATH_DEGREES_TO_RADIANS(vRotOffset.x));
    rOffset.Rotate(rOffset.Up(), MATH_DEGREES_TO_RADIANS(vRotOffset.y));
    rOffset.Rotate(rOffset.Forward(), MATH_DEGREES_TO_RADIANS(vRotOffset.z));

    LTRESULT resultAttachment = g_pLTServer->CreateAttachment(m_hObject, hChildObject, sSocket, &vOffset, &rOffset, &hAttachment);

    if(LT_OK != resultAttachment)
    {
        g_pLTServer->CPrint("Error creating attachment!!");
    }
}



//-----------------------------------------------------------------------------
//	CPlayerSrvr::CheckForHit()
//
//-----------------------------------------------------------------------------
void CPlayerSrvr::CheckForHit()
{
    IntersectQuery qInfo;
	IntersectInfo iInfo;

    LTVector vPos;
    LTRotation rRot;

    g_pLTServer->GetObjectPos(m_hObject, &vPos);
    g_pLTServer->GetObjectRotation(m_hObject, &rRot);

	LTVector vForward = rRot.Forward();

	vPos.y -= 30.0f;

	qInfo.m_From = vPos + vForward;
	qInfo.m_To   = vPos + (vForward * 35.0f);

	qInfo.m_Flags =  INTERSECT_OBJECTS;// | INTERSECT_HPOLY;

    if(g_pLTServer->IntersectSegment(&qInfo, &iInfo))
	{
		if(iInfo.m_hObject)
		{
			if(g_pLTSPhysics->IsWorldObject( iInfo.m_hObject ) == LT_YES)
			{

			}
			else
			{
                //Is it a seal?
                HCLASS hClassSeal = g_pLTServer->GetClass("Seal");
                HCLASS hClassSnowman = g_pLTServer->GetClass("Snowman");
                HCLASS hTarget = g_pLTServer->GetObjectClass(iInfo.m_hObject);

                //Yes
                // Make a sound
                if( g_pLTServer->IsKindOf(hClassSeal, hTarget))
                {
                    PlaySound(1);

                    // Send damage message to seal
                    ILTMessage_Write *pMsg;
                    g_pLTSCommon->CreateMessage(pMsg);
                    pMsg->IncRef();
                    pMsg->Writeuint32(OBJ_MID_DAMAGE);
                    pMsg->Writeuint8(5); //DMG
                    g_pLTServer->SendToObject(pMsg->Read(), m_hObject, iInfo.m_hObject, 0);
                    pMsg->DecRef();
                }
                else
                if(g_pLTServer->IsKindOf(hClassSnowman, hTarget))
                {
                    PlaySound(2);

                    // Send damage message to snowman
                    ILTMessage_Write *pMsg;
                    g_pLTSCommon->CreateMessage(pMsg);
                    pMsg->IncRef();
                    pMsg->Writeuint32(OBJ_MID_DAMAGE);
                    pMsg->Writeuint8(5); //DMG
                    g_pLTServer->SendToObject(pMsg->Read(), m_hObject, iInfo.m_hObject, 0);
                    pMsg->DecRef();                    
                }
                else
                {
                    PlaySound(3);
                }
            }
		}
	}
}



//-----------------------------------------------------------------------------
//	PickUp::PlaySound()
//
//-----------------------------------------------------------------------------
void CPlayerSrvr::PlaySound(int i)
{
	// Send clientfx message
	uint32 dwFxFlags = 0;

    switch(i)
    {
        case 1:
	    {
		    PlayClientFX("SealImpact", m_hClub, LTNULL, LTNULL, dwFxFlags);
	    }break;
        case 2:
	    {
		    PlayClientFX("SnowmanImpact", m_hClub, LTNULL, LTNULL, dwFxFlags);
	    }break;
        case 3:
	    {
		    PlayClientFX("PlayerImpact", m_hClub, LTNULL, LTNULL, dwFxFlags);
	    }break;


        default:
            break;
    }

}



//-----------------------------------------------------------------------------
//	PickUp::SetClubID()
//
//-----------------------------------------------------------------------------
void CPlayerSrvr::SetClubID()
{
   	// Send over the position of the startpoint so the client can position the player
	ILTMessage_Write *pMsg;
	LTRESULT nResult = g_pLTSCommon->CreateMessage(pMsg);
	pMsg->IncRef();
	pMsg->Writeint8(MSG_CS_MY_CLUB);
    pMsg->WriteObject(m_hClub);
	g_pLTServer->SendToClient(pMsg->Read(), m_hClient,  MESSAGE_GUARANTEED);
	pMsg->DecRef();
}
