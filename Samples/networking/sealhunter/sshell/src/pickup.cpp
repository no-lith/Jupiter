//------------------------------------------------------------------------------//
//
// MODULE   : PickUp.cpp
//
// PURPOSE  : PickUp - Implementation
//
// CREATED  : 08/27/2002
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#include "pickup.h"
#include "playersrvr.h"
#include "msgids.h"

#include <iltcommon.h>
#include <iltsoundmgr.h>
#include "serverinterfaces.h"


//-----------------------------------------------------------------------------
BEGIN_CLASS(PickUp)
ADD_STRINGPROP_FLAG(Filename, "", PF_MODEL | PF_FILENAME)
ADD_STRINGPROP_FLAG(Texture, "", PF_FILENAME)
ADD_STRINGPROP_FLAG(RenderStyle, "", PF_FILENAME)
ADD_STRINGPROP_FLAG(PickupType , "Health", PF_STATICLIST)
ADD_REALPROP(PickupValue, 0.0f)
END_CLASS_DEFAULT_FLAGS_PLUGIN(PickUp, BaseClass, LTNULL, LTNULL, CF_ALWAYSLOAD, CPickupPlugin)



//-----------------------------------------------------------------------------
//	PickUp::PickUp():
//
//-----------------------------------------------------------------------------
PickUp::PickUp():
m_nPickupType(0),
m_fPickupValue(0.0f)
{
}



//-----------------------------------------------------------------------------
//	PickUp::EngineMessageFn(uint32 messageID, void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 PickUp::EngineMessageFn(uint32 messageID, void *pData, float fData)
{
    switch (messageID)
    {
    case MID_PRECREATE:
        return PreCreate(pData, fData);
    case MID_TOUCHNOTIFY:
        return TouchNotify(pData, fData);
    case MID_UPDATE:
        return Update();

    default:
        break;
    }

    // Pass the message along to parent class.
    return BaseClass::EngineMessageFn(messageID, pData, fData);
}



//-----------------------------------------------------------------------------
//	PickUp::PreCreate(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 PickUp::PreCreate(void *pData, float fData)
{
    // Let parent class handle it first
    BaseClass::EngineMessageFn(MID_PRECREATE, pData, fData);

    // Cast pData to a ObjectCreateStruct* for convenience
    ObjectCreateStruct* pStruct = (ObjectCreateStruct*)pData;

    // Setup flags
    pStruct->m_Flags = FLAG_VISIBLE | FLAG_TOUCH_NOTIFY;

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
//	PickUp::Update()
//
//-----------------------------------------------------------------------------
uint32 PickUp::Update()
{
    // Respawn the object

    // Turn TOUCH_NOTIFY back on
    g_pLTSCommon->SetObjectFlags( m_hObject, OFT_Flags, FLAG_TOUCH_NOTIFY, FLAG_TOUCH_NOTIFY);

    // Make the object visible again
    g_pLTSCommon->SetObjectFlags( m_hObject, OFT_Flags, FLAG_VISIBLE, FLAG_VISIBLE);

    g_pLTServer->CPrint("pickup respawned!");

    return 1;
}



//-----------------------------------------------------------------------------
//	PickUp::TouchNotify(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 PickUp::TouchNotify(void *pData, float fData)
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
			g_pLTServer->CPrint("%s grabbed pickup!", pPlayer->GetPlayerName());
			g_pLTServer->CPrint("pickup will respawn in 10 seconds");

			// Send this message to the player object
			ILTMessage_Write *pMsg;
			g_pLTSCommon->CreateMessage(pMsg);
            pMsg->IncRef();
			pMsg->Writeuint32(OBJ_MID_PICKUP);
			pMsg->Writeuint8(m_nPickupType);
			pMsg->Writefloat(m_fPickupValue);
			g_pLTServer->SendToObject(pMsg->Read(), m_hObject, hObj, 0);
            pMsg->DecRef();

			// Turn TOUCH_NOTIFY off
			g_pLTSCommon->SetObjectFlags( m_hObject, OFT_Flags, 0, FLAG_TOUCH_NOTIFY);

			// Turn invisible
            g_pLTSCommon->SetObjectFlags( m_hObject, OFT_Flags, 0, FLAG_VISIBLE);

            // Play sound effect
            this->PlaySound();

			// Set the respawn time
			g_pLTServer->SetNextUpdate(m_hObject, 10.0f);
        }
    }

    return 1;
}



//-----------------------------------------------------------------------------
//	PickUp::ReadProps(ObjectCreateStruct* pStruct)
//
//-----------------------------------------------------------------------------
void PickUp::ReadProps(ObjectCreateStruct* pStruct)
{
    g_pLTServer->GetPropString("Filename", pStruct->m_Filename, MAX_CS_FILENAME_LEN);
    g_pLTServer->GetPropString("Texture", pStruct->m_SkinNames[0], MAX_CS_FILENAME_LEN);
    g_pLTServer->GetPropString("RenderStyle", pStruct->m_RenderStyleNames[0], MAX_CS_FILENAME_LEN);

    char sPickupType[16];
    g_pLTServer->GetPropString("PickupType", sPickupType, 16);

    if(strcmp("Health", sPickupType) == 0)
    {
        m_nPickupType = PT_HEALTH;
    }
	else if(strcmp("Armor", sPickupType) == 0)
    {
        m_nPickupType = PT_ARMOR;
    }

    g_pLTServer->GetPropReal("PickupValue", &m_fPickupValue);
}



//-----------------------------------------------------------------------------
//	PickUp::PlaySound()
//
//-----------------------------------------------------------------------------
void PickUp::PlaySound()
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
        g_pLTServer->CPrint("(PickUp) Error playing sound");
    }
}



//-----------------------------------------------------------------------------
//	CPickupPlugin::PreHook_EditStringList(
//
//-----------------------------------------------------------------------------
LTRESULT CPickupPlugin::PreHook_EditStringList(
	const char* szRezPath,
	const char* szPropName,
	char** aszStrings,
	uint32* pcStrings,
	const uint32 cMaxStrings,
	const uint32 cMaxStringLength)
{
	// If prop name is "PickupType", fill in the dropdown with g_aszPickupTypes[]
	if (strcmp("PickupType", szPropName) == 0)
	{
		for (int i=0; i<PT_NUM_TYPES; i++)
		{
			if (cMaxStrings > (*pcStrings) + 1)	// sanity check
			{
				if (g_aszPickupTypes[i][0])		// skip null strings
					strcpy(aszStrings[(*pcStrings)++], g_aszPickupTypes[i]);
			}
		}

		return LT_OK;
	}

	return LT_UNSUPPORTED;
}
