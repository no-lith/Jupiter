// ----------------------------------------------------------------------- //
//
// MODULE  : CSFXMgr.cpp
//
// PURPOSE : Special FX Mgr	- Implementation
//
// CREATED : 10/24/97
//
// (c) 1997-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "SFXMgr.h"
//#include "VarTrack.h"
#include "BankedList.h"
#include "clientinterfaces.h"
#include "ltclientshell.h"
#include "PolyGridFX.h"
#include "CommonUtilities.h"

// NOTE:  These indexes should map EXACTLY to the SFX ids defined
// in SFXMsgIds.h...

static unsigned int s_nDynArrayMaxNums[DYN_ARRAY_SIZE] =
{
	10,		// General fx (Unused?)
	50,		// Polygrid
	1,		// Particle trails - never added to sfx mgr
	50,		// Particle systems
	50,		// Particle shower
	50,		// Tracers
	1,		// Weapons - uses a single static weapon fx
	50,		// Dynamic lights
	50,		// Particle trail segments
	50,		// Smoke
	50,		// Bullet trail
	100,	// Volume brush
	200,	// Shell casings
	1,		// Camera - Unused, it has its own list
	20,		// Particle explosions
	200,	// Sprites/Models (base scale)
	100,	// Debris
	50,		// Death
	50,		// Gibs
	50,		// Projectile
	100,	// Marks - bullet holes
	100,	// Light
	50,		// Random sparks
	200,	// Pickup item
	200,	// Character
	20,		// Player sounds
	1000,	// Node Lines (used for AI nodes)
	100,	// Weather
	50,		// Lightning
	10,		// Sprinkles
	100,	// Fire
	50,		// Lens Flares
	20,		// Muzzle flash
	100,	// Search lights
	100,	// Polygon debris
	100,	// Steam,
	100,	// Explosion
	200,	// PolyLine
	200,	// Body
	100,	// Laser trigger
	100,	// Mines
	50,		// Beams
	10,		// Player vehicles
	10,		// Sound fx (not currently used)
	10,		// Objective sprites
	1,		// Light groups
	500,     // Debug line systems
	1,		// Texture FX, has its own list
	50,		// Snow
	20,		// JumpVolumes
	5,		// PlayerLure
	50,		// GadgetTargets
	1,		// DisplayTimer, doesn't need a list.
	50,		// Dynamic Occluder volumes
	50,		// Scatter
	50,		// Trigger
	20,		// RadarObject
	512,	// ActivateObject
	3,		// DoomsDay pieces
};

void Color255WordToVector( uint16 wVal, LTVector *pVal )
{
	// For red, divide by 11 bits then multiply by 8 bits and divide by 5 bits = divide by 8 bits...
	pVal->x = ( LTFLOAT )(( wVal & 0xF800 ) >> 8 );

	// For green, divide by 5 bits, multiply by 8 bits, divide by 6 bits = divide by 3 bits.
	pVal->y = ( LTFLOAT )(( wVal & 0x07E0 ) >> 3 );

	// For blue, divide by 5 bits, multiply by 8 bits = multiply by 3 bits
	pVal->z = ( LTFLOAT )(( wVal & 0x001F ) << 3 );
}

uint16 Color255VectorToWord( LTVector *pVal )
{
	uint16 wColor;

	// For red, multiply by 5 bits and divide by 8, which is a net division of 3 bits.  Then shift it
	// to the left 11 bits to fit into result, which is a net shift of 8 to left.
	wColor = ( uint16 )(((( uint32 )pVal->x & 0xFF ) << 8 ) & 0xF800 );

	// For green, multiply by 6 bits and divide by 8, which is a net division of 2 bits.  Then shift it
	// to the left 5 bits to fit into result, which is a net shift of 3 to left.
	wColor |= ( uint16 )(((( uint32 )pVal->y & 0xFF ) << 3 ) & 0x07E0 );

	// For blue, multiply by 5 bits and divide by 8 = divide by 3.
	wColor |= ( uint16 )((( uint32 )pVal->z & 0xFF ) >> 3 );

	return wColor;
}

// The special FX banks...

CBankedList<CPolyGridFX> g_SFXBank_PolyGrid;

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::Init()
//
//	PURPOSE:	Init the CSFXMgr
//
// ----------------------------------------------------------------------- //

LTBOOL CSFXMgr::Init(ILTClient *pClientDE)
{
    if (!g_pLTClient) return LTFALSE;

	for (int i=0; i < DYN_ARRAY_SIZE; i++)
	{
		if (!m_dynSFXLists[i].Create(GetDynArrayMaxNum(i))) return LTFALSE;
	}

	return m_cameraSFXList.Create(CAMERA_LIST_SIZE);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::HandleSFXMsg()
//
//	PURPOSE:	Handle a special fx message
//
// ----------------------------------------------------------------------- //

void CSFXMgr::HandleSFXMsg(HLOCALOBJ hObj, ILTMessage_Read *pMsg)
{
    uint8 nId = pMsg->Readuint8();

	switch (nId)
	{
		case SFX_POLYGRID_ID :
		{
			PGCREATESTRUCT pg;

			pg.hServerObj = hObj;

			pg.vDims = pMsg->ReadLTVector();
			uint16 wColor = pMsg->Readuint16();
			Color255WordToVector(wColor, &(pg.vColor1));
			wColor = pMsg->Readuint16();
			Color255WordToVector(wColor, &(pg.vColor2));
			pg.fXScaleMin = pMsg->Readfloat();
			pg.fXScaleMax = pMsg->Readfloat();
			pg.fYScaleMin = pMsg->Readfloat();
			pg.fYScaleMax = pMsg->Readfloat();
			pg.fXScaleDuration = pMsg->Readfloat();
			pg.fYScaleDuration = pMsg->Readfloat();
			pg.fXPan = pMsg->Readfloat();
			pg.fYPan = pMsg->Readfloat();
			pg.fAlpha = pMsg->Readfloat();
			pg.fTimeScale = pMsg->Readfloat();
			pg.fDampenScale = pMsg->Readfloat();
			pg.fSpringCoeff	= pMsg->Readfloat();
			pg.fModelDisplace = pMsg->Readfloat();
			pg.fMinFrameRate = pMsg->Readfloat();
			pg.fBaseReflection = pMsg->Readfloat();
			pg.fVolumeIOR = pMsg->Readfloat();
			pg.nEffectID = pMsg->Readuint32();

			char szString[256];
			pMsg->ReadString( szString, ARRAY_LEN( szString ));
			pg.sSurfaceSprite = szString;

			pMsg->ReadString( szString, ARRAY_LEN( szString ));
			pg.sSurfaceEnvMap = szString;

			pMsg->ReadString( szString, ARRAY_LEN( szString ));
			pg.sDampenImage = szString;

            pg.dwNumPoliesX = (uint32)pMsg->Readuint16();
			pg.dwNumPoliesY = (uint32)pMsg->Readuint16();
			pg.nNumStartupFrames = (uint32)pMsg->Readuint16();
            pg.bAdditive = (LTBOOL)pMsg->Readbool();
            pg.bMultiply = (LTBOOL)pMsg->Readbool();
			pg.bFresnel = (LTBOOL)pMsg->Readbool();
			pg.bBackfaceCull = (LTBOOL)pMsg->Readbool();
			pg.bRenderEarly = (LTBOOL)pMsg->Readbool();
			pg.bNormalMapSprite = (LTBOOL)pMsg->Readbool();
			pg.nPlasmaType = pMsg->Readuint8();
			pg.nRingRate[0] = pMsg->Readuint8();
			pg.nRingRate[1] = pMsg->Readuint8();
			pg.nRingRate[2] = pMsg->Readuint8();
			pg.nRingRate[3] = pMsg->Readuint8();

			//read in our modifier data
			pg.nActiveModifiers = pMsg->Readuint8();
			for(uint32 nCurrMod = 0; nCurrMod < PG_MAX_MODIFIERS; nCurrMod++)
			{
				pg.fAccelAmount[nCurrMod] = pMsg->Readfloat();
				pg.nNumAccelPoints[nCurrMod] = pMsg->Readuint16();				
				pg.nXMin[nCurrMod] = pMsg->Readuint16();
				pg.nYMin[nCurrMod] = pMsg->Readuint16();
				pg.nXMax[nCurrMod] = pMsg->Readuint16();
				pg.nYMax[nCurrMod] = pMsg->Readuint16();
			}

			pg.hVolumeBrush = pMsg->ReadObject();

			CreateSFX(nId, &pg);
		}
		break;


		default : break;
	}
}


void CSFXMgr::DeleteSFX(CSpecialFX* pFX)
{
	// Make sure we've got a valid pointer
	if (!pFX)
		return;

	// Alrighty, get the ID from the effect...
	switch(pFX->GetSFXID())
	{
		// Delete it from the right place...

		case SFX_POLYGRID_ID :
		{
			g_SFXBank_PolyGrid.Delete((CPolyGridFX*)pFX);
		}
		break;

		// Ok, it's not a banked effect
		default:
		{
			debug_delete(pFX);
		}
		break;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::CreateSFX()
//
//	PURPOSE:	Create the special fx
//
// ----------------------------------------------------------------------- //

CSpecialFX* CSFXMgr::CreateSFX(uint8 nId, SFXCREATESTRUCT *psfxCreateStruct,
							   ILTMessage_Read *pMsg, HOBJECT hServerObj)
{
    CSpecialFX* pSFX = LTNULL;

	switch (nId)
	{
		case SFX_POLYGRID_ID :
		{
			pSFX = g_SFXBank_PolyGrid.New();
		}
		break;

		default :
            return LTNULL;
		break;
	}


	// Initialize the sfx, and add it to the appropriate array...

    if (!pSFX) return LTNULL;


	// First initialize with the create struct...

	if (psfxCreateStruct)
	{
		if (!pSFX->Init(psfxCreateStruct))
		{
			DeleteSFX(pSFX);
            return LTNULL;
		}
	}
	else if (pMsg)  // Initialize using the hMessage
	{
		if (!pSFX->Init(hServerObj, pMsg))
		{
			DeleteSFX(pSFX);
            return LTNULL;
		}
	}
	else
	{
		DeleteSFX(pSFX);
        return LTNULL;
	}

	if (!pSFX->CreateObject(g_pLTClient))
	{
		DeleteSFX(pSFX);
        return LTNULL;
	}

	if( !AddDynamicSpecialFX(pSFX, nId))
	{
		DeleteSFX( pSFX );
		return NULL;
	}

	return pSFX;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::UpdateSpecialFX()
//
//	PURPOSE:	Update any the special FX
//
// ----------------------------------------------------------------------- //

void CSFXMgr::UpdateSpecialFX()
{
    LTFLOAT fTime = g_pLTClient->GetTime();

	// Update dynamic sfx...

	UpdateDynamicSpecialFX();


	// Update camera sfx...

	int nNumSFX = m_cameraSFXList.GetSize();

	for (int i=0; i < nNumSFX; i++)
	{
		if (m_cameraSFXList[i])
		{
			if (fTime >= m_cameraSFXList[i]->m_fNextUpdateTime)
			{
				if (!m_cameraSFXList[i]->Update())
				{
					m_cameraSFXList.Remove(m_cameraSFXList[i]);
				}
				else
				{
					m_cameraSFXList[i]->m_fNextUpdateTime = fTime + m_cameraSFXList[i]->GetUpdateDelta();
				}
			}
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::RemoveSpecialFX()
//
//	PURPOSE:	Remove the specified special fx
//
// ----------------------------------------------------------------------- //

void CSFXMgr::RemoveSpecialFX(HLOCALOBJ hObj)
{
	// Remove the dynamic special fx associated with this object..

	RemoveDynamicSpecialFX(hObj);


	// See if this was a camera...

	int nNumSFX = m_cameraSFXList.GetSize();

	for (int i=0; i < nNumSFX; i++)
	{
		if (m_cameraSFXList[i] && m_cameraSFXList[i]->GetServerObj() == hObj)
		{
			m_cameraSFXList[i]->WantRemove();
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::RemoveAll()
//
//	PURPOSE:	Remove all the special fx
//
// ----------------------------------------------------------------------- //

void CSFXMgr::RemoveAll()
{
	RemoveAllDynamicSpecialFX();

	int nNumSFX = m_cameraSFXList.GetSize();

	for (int i=0; i < nNumSFX; i++)
	{
		m_cameraSFXList.Remove(m_cameraSFXList[i]);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::AddDynamicSpecialFX()
//
//	PURPOSE:	Add a dyanamic special fx to our lists
//
// ----------------------------------------------------------------------- //

bool CSFXMgr::AddDynamicSpecialFX(CSpecialFX* pSFX, uint8 nId)
{
	int nIndex = GetDynArrayIndex(nId);

	if (0 <= nIndex && nIndex < DYN_ARRAY_SIZE)
	{
		return !!m_dynSFXLists[nIndex].Add(pSFX);
	}

	return false;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::UpdateDynamicSpecialFX()
//
//	PURPOSE:	Update the dyanamic special fxs
//
// ----------------------------------------------------------------------- //

void CSFXMgr::UpdateDynamicSpecialFX()
{
    LTFLOAT fTime = g_pLTClient->GetTime();

	for (int j=0; j < DYN_ARRAY_SIZE; j++)
	{
		int nNumSFX  = m_dynSFXLists[j].GetSize();

		for (int i=0; i < nNumSFX; i++)
		{
			if (m_dynSFXLists[j][i])
			{
				if (fTime >= m_dynSFXLists[j][i]->m_fNextUpdateTime)
				{
					if (!m_dynSFXLists[j][i]->Update())
					{
						m_dynSFXLists[j].Remove(m_dynSFXLists[j][i]);
					}
					else
					{
						m_dynSFXLists[j][i]->m_fNextUpdateTime = fTime + m_dynSFXLists[j][i]->GetUpdateDelta();
					}
				}
			}
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::RemoveDynamicSpecialFX()
//
//	PURPOSE:	Remove the specified special fx
//
// ----------------------------------------------------------------------- //

void CSFXMgr::RemoveDynamicSpecialFX(HOBJECT hObj)
{
	for (int j=0; j < DYN_ARRAY_SIZE; j++)
	{
		int nNumSFX  = m_dynSFXLists[j].GetSize();

		for (int i=0; i < nNumSFX; i++)
		{
			// More than one sfx may have the same server handle, so let them
			// all have an opportunity to remove themselves...

			if (m_dynSFXLists[j][i] && m_dynSFXLists[j][i]->GetServerObj() == hObj)
			{
				m_dynSFXLists[j][i]->WantRemove();
			}
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::RemoveAllDynamicSpecialFX()
//
//	PURPOSE:	Remove all dynamic special fx
//
// ----------------------------------------------------------------------- //

void CSFXMgr::RemoveAllDynamicSpecialFX()
{
	for (int j=0; j < DYN_ARRAY_SIZE; j++)
	{
		int nNumSFX  = m_dynSFXLists[j].GetSize();

		for (int i=0; i < nNumSFX; i++)
		{
			m_dynSFXLists[j].Remove(m_dynSFXLists[j][i]);
		}
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::FindSpecialFX()
//
//	PURPOSE:	Find the specified special fx type associated with the
//				object (see SFXMsgIds.h for valid types)
//
// ----------------------------------------------------------------------- //

CSpecialFX* CSFXMgr::FindSpecialFX(uint8 nType, HLOCALOBJ hObj)
{
	if (0 <= nType && nType < DYN_ARRAY_SIZE)
	{
		int nNumSFX  = m_dynSFXLists[nType].GetSize();

		for (int i=0; i < nNumSFX; i++)
		{
			if (m_dynSFXLists[nType][i] && m_dynSFXLists[nType][i]->GetServerObj() == hObj)
			{
				return m_dynSFXLists[nType][i];
			}
		}
	}

    return LTNULL;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::GetDynArrayIndex()
//
//	PURPOSE:	Get the array index associated with a particular type of
//				dynamic special fx
//
// ----------------------------------------------------------------------- //

int CSFXMgr::GetDynArrayIndex(uint8 nFXId)
{
	// All valid fxids should map directly to the array index...If this is
	// an invalid id, use the general fx index (i.e., 0)...

	if (nFXId < 0 || nFXId >= DYN_ARRAY_SIZE)
	{
		return 0;
	}

	return nFXId;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::GetDynArrayMaxNum()
//
//	PURPOSE:	Find a dyanamic special fx associated with an object
//
// ----------------------------------------------------------------------- //

unsigned int CSFXMgr::GetDynArrayMaxNum(uint8 nIndex)
{
	if (0 <= nIndex && nIndex < DYN_ARRAY_SIZE)
	{
		return s_nDynArrayMaxNums[nIndex];
	}

	return 0;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CSFXMgr::OnSFXMessage()
//
//	PURPOSE:	Handle server-to-sfx messages
//
// ----------------------------------------------------------------------- //

void CSFXMgr::OnSFXMessage(ILTMessage_Read *pMsg)
{
	if (!pMsg) return;

    uint8 nFXType   = pMsg->Readuint8();
	HOBJECT hObj	= pMsg->ReadObject();

	if (0 <= nFXType && nFXType < DYN_ARRAY_SIZE && hObj)
	{
		CSpecialFX* pFX = FindSpecialFX(nFXType, hObj);

		if (pFX)
		{
			pFX->OnServerMessage(pMsg);
		}
	}
}


