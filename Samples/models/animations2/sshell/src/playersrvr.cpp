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
        return InitialUpdate(pData, fData);
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
                    //create projectile
                    CreateProjectile();
                }
			}
            return 1;
        }break;
		
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