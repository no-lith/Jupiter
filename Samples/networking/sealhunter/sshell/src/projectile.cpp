//------------------------------------------------------------------------------//
//
// MODULE   : Projectile.cpp
//
// PURPOSE  : Projectile - Implementation
//
// CREATED  : 06/27/2002
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#include "projectile.h"
#include "serverinterfaces.h"
#include "serverutilities.h"
#include <iltphysics.h>
#include <ltobjectcreate.h>
#include <iltcommon.h>
#include "FxFlags.h"



//-----------------------------------------------------------------------------
BEGIN_CLASS(Projectile)
END_CLASS_DEFAULT_FLAGS(Projectile, BaseClass, LTNULL, LTNULL, CF_ALWAYSLOAD | CF_HIDDEN)



//-----------------------------------------------------------------------------
//	Projectile::EngineMessageFn(uint32 messageID, void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Projectile::EngineMessageFn(uint32 messageID, void *pData, float fData)
{
    switch (messageID)
    {
    case MID_PRECREATE:
        return PreCreate(pData, fData);
    case MID_INITIALUPDATE:
        return InitialUpdate(pData, fData);
	case MID_TOUCHNOTIFY:
        return TouchNotify(pData, fData);
    case MID_UPDATE:
        return Update(pData, fData);

    default:
        break;
    }

    // Pass the message along to parent class.
    return BaseClass::EngineMessageFn(messageID, pData, fData);
}



//-----------------------------------------------------------------------------
//	Projectile::PreCreate(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Projectile::PreCreate(void *pData, float fData)
{
    // Let parent class handle it first
    BaseClass::EngineMessageFn(MID_PRECREATE, pData, fData);

    // Cast pData to a ObjectCreateStruct* for convenience
    ObjectCreateStruct* pStruct = (ObjectCreateStruct*)pData;

    pStruct->m_Flags |= (FLAG_VISIBLE | FLAG_REMOVEIFOUTSIDE);
    pStruct->m_Flags2 |= FLAG2_SPECIALNONSOLID;
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
//	Projectile::InitialUpdate(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Projectile::InitialUpdate(void *pData, float fData)
{
    LTVector vDims( 10.0f, 10.0f, 10.0f );
    g_pLTSPhysics->SetObjectDims( m_hObject, &vDims, 0 );

    // Don't ignore small collisions
    g_pLTSPhysics->SetForceIgnoreLimit( m_hObject , 0.0f );

    g_pLTSCommon->SetObjectFlags(m_hObject, OFT_Flags, FLAG_TOUCH_NOTIFY, FLAG_TOUCH_NOTIFY);

    g_pLTServer->SetNextUpdate( m_hObject, 0.01f );

    return 1;
}



//-----------------------------------------------------------------------------
//	Projectile::Update(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Projectile::Update(void *pData, float fData)
{
    LTVector vVel;
    LTRotation rRot;

    g_pLTServer->GetObjectRotation(m_hObject, &rRot);

    vVel = rRot.Forward() * 1200.0f;

    g_pLTSPhysics->SetVelocity(m_hObject, &vVel);

    g_pLTServer->SetNextUpdate(m_hObject, 0.01f);

    return 1;
}



//-----------------------------------------------------------------------------
//	Projectile::TouchNotify(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Projectile::TouchNotify(void *pData, float fData)
{
    HOBJECT hObj = (HOBJECT)pData;

    //Prevent projectile from hitting the owner.
    if((hObj != m_hOwner) && (m_hOwner != NULL))
    {
        CollisionInfo Info;
        memset(&Info, 0, sizeof(Info));
        g_pLTServer->GetLastCollision(&Info);

        LTRESULT result = g_pLTSPhysics->IsWorldObject( hObj );

        if (result == LT_YES)
        {
            //g_pLTServer->CPrint( "Projectile::I hit the world at poly %d", Info.m_hPoly);

            // Send clientfx message
            uint32 dwFxFlags = 0;
            PlayClientFX("CanImpact", m_hObject, LTNULL, LTNULL, dwFxFlags);
        }
        else
        {
            //g_pLTServer->CPrint( "Projectile::I hit an object %p -> %p", hObj, m_hOwner );

            // Send clientfx message
            uint32 dwFxFlags = 0;
            PlayClientFX("PlayerImpact", m_hObject, LTNULL, LTNULL, dwFxFlags);
        }

        g_pLTServer->RemoveObject(m_hObject);
    }

    return 1;
}



//-----------------------------------------------------------------------------
//	Projectile::ReadProps(ObjectCreateStruct* pStruct)
//
//-----------------------------------------------------------------------------
void Projectile::ReadProps(ObjectCreateStruct* pStruct)
{
    g_pLTServer->GetPropString("Name", pStruct->m_Filename, MAX_CS_FILENAME_LEN);
}
