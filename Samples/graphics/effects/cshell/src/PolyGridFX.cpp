// ----------------------------------------------------------------------- //
//
// MODULE  : PolyGridFX.cpp
//
// PURPOSE : PolyGrid special FX - Implementation
//
// CREATED : 10/13/97
//
// (c) 1997-2000 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "PolyGridFX.h"
#include "iltclient.h"
#include "LtClientShell.h"
#include "ILTCommon.h"
//#include "VarTrack.h"
#include "iltphysics.h"
#include <ltobjectcreate.h>
#include "clientinterfaces.h"
#include <iltcommon.h>

//variable to track if the artist wants to simulate the minimum frame rate of the polygrids
//VarTrack g_cvarMinPGFrameRate;

//this is the default movement. Movement slower will cause more subtle waves, faster will create
//larger
//VarTrack g_cvarPGDisplaceMoveScale;

static char g_SinTable[256];
static LTBOOL g_bSinTableInitted=LTFALSE;


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	FindClosestPowerOf2
//
//	PURPOSE:	Given a number it will find the closest power of 2 to that
//				number. 
//
// ----------------------------------------------------------------------- //
static uint32 FindClosestPowerOf2(uint32 nVal)
{
	//our current val, the highest bit possible
	uint32 nRV = (1 << 31);

	//now repeat until we find a value
	while(nRV > 0)
	{
		//see if we are sharing a bit, if so we have found the highest bit, and
		//need to decide between this value or the one higher
		if(nRV & nVal)
		{
			//ok, now find the distance to this
			uint32 nDist1 = nVal - nRV;

			//keep that one if the distance is 0 or if we can't check higher
			if((nDist1 == 0) || (nRV == (1 << 31)))
				return nRV;

			//check the one higher and see if it is a better match
			uint32 nDist2 = (nRV << 1) - nVal;

			//return the minimum
			return (nDist2 < nDist1) ? (nRV << 1) : nRV;
		}

		//move along...
		nRV >>= 1;
	}

	return 0;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPolyGridFX::Init
//
//	PURPOSE:	Create the poly grid
//
// ----------------------------------------------------------------------- //

LTBOOL CPolyGridFX::Init(SFXCREATESTRUCT* psfxCreateStruct)
{
    if (!psfxCreateStruct) return LTFALSE;

	CSpecialFX::Init(psfxCreateStruct);

	PGCREATESTRUCT* pg = (PGCREATESTRUCT*)psfxCreateStruct;

	m_vDims				= pg->vDims;
	m_vColor1			= pg->vColor1;
	m_vColor2			= pg->vColor2;
	m_fXScaleMin		= pg->fXScaleMin;
	m_fXScaleMax		= pg->fXScaleMax;
	m_fYScaleMin		= pg->fYScaleMin;
	m_fYScaleMax		= pg->fYScaleMax;
	m_fXScaleDuration	= pg->fXScaleDuration;
	m_fYScaleDuration	= pg->fYScaleDuration;
	m_fXPan				= pg->fXPan;
	m_fYPan				= pg->fYPan;
	m_fBaseReflection	= pg->fBaseReflection;
	m_fVolumeIOR		= pg->fVolumeIOR;
	m_nEffectID			= pg->nEffectID;
	m_sSurfaceSprite	= pg->sSurfaceSprite;
	m_sSurfaceEnvMap	= pg->sSurfaceEnvMap;
	m_sDampenImage		= pg->sDampenImage;
	m_dwNumPoliesX		= pg->dwNumPoliesX;
	m_dwNumPoliesY		= pg->dwNumPoliesY;
	m_nNumStartupFrames	= pg->nNumStartupFrames;

	m_fAlpha			= pg->fAlpha;
	m_nSurfaceType		= pg->nPlasmaType;
	m_bAdditive			= pg->bAdditive;
	m_bMultiply			= pg->bMultiply;
	m_bFresnel			= pg->bFresnel;
	m_bBackfaceCull		= pg->bBackfaceCull;
	m_bRenderEarly		= pg->bRenderEarly;
	m_bNormalMapSprite	= pg->bNormalMapSprite;

	m_nRingRate[0]		= pg->nRingRate[0];
	m_nRingRate[1]		= pg->nRingRate[1];
	m_nRingRate[2]		= pg->nRingRate[2];
	m_nRingRate[3]		= pg->nRingRate[3];

	m_fDampenScale		= pg->fDampenScale;
	m_fTimeScale		= pg->fTimeScale;
	m_fSpringCoeff		= pg->fSpringCoeff;
	m_fModelDisplace	= pg->fModelDisplace;
	m_fMinFrameRate		= pg->fMinFrameRate;
	m_nActiveModifiers	= pg->nActiveModifiers;

	for(uint32 nCurrTrack = 0; nCurrTrack < MAX_MODELS_TO_TRACK; nCurrTrack++)
	{
		m_hTrackedModels[nCurrTrack] = NULL;
	}

	for(uint32 nCurrMod = 0; nCurrMod < PG_MAX_MODIFIERS; nCurrMod++)
	{
		m_nXMin[nCurrMod]			= pg->nXMin[nCurrMod];
		m_nYMin[nCurrMod]			= pg->nYMin[nCurrMod];
		m_nXMax[nCurrMod]			= pg->nXMax[nCurrMod];
		m_nYMax[nCurrMod]			= pg->nYMax[nCurrMod];
		m_nNumAccelPoints[nCurrMod] = pg->nNumAccelPoints[nCurrMod];
		m_fAccelAmount[nCurrMod]	= pg->fAccelAmount[nCurrMod];
	}
	
	m_hVolumeBrush		= pg->hVolumeBrush;

	//setup any console variables
	/*
	if (!g_cvarMinPGFrameRate.IsInitted())
	{
		g_cvarMinPGFrameRate.Init(g_pLTClient, "MinPGFrameRate", LTNULL, 0.0f);
	}
	*/

	/*
	if (!g_cvarPGDisplaceMoveScale.IsInitted())
	{
		g_cvarPGDisplaceMoveScale.Init(g_pLTClient, "PGDisplaceMoveScale", LTNULL, 70.0f);
	}
	*/


    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPolyGridFX::CreateObject
//
//	PURPOSE:	Create object associated the poly grid
//
// ----------------------------------------------------------------------- //

LTBOOL CPolyGridFX::CreateObject(ILTClient *pClientDE)
{
	ColorRamp colorRamps[] =
	{
        ColorRamp(LTVector(m_vColor1.x, m_vColor1.y, m_vColor1.z), 0),
        ColorRamp(LTVector(m_vColor2.x, m_vColor2.y, m_vColor2.z), 256)
	};

    if (!CSpecialFX::CreateObject(pClientDE)) return LTFALSE;

    if (!g_pCShell) return LTFALSE;

    LTVector vPos;
	g_pLTClient->GetObjectPos(m_hServerObject, &vPos);

    LTRotation rRot;
	g_pLTClient->GetObjectRotation(m_hServerObject, &rRot);

	// Setup the PolyGrid...

	ObjectCreateStruct createStruct;
	INIT_OBJECTCREATESTRUCT(createStruct);

	createStruct.m_ObjectType = OT_POLYGRID;
	createStruct.m_Flags = FLAG_VISIBLE;
	createStruct.m_Pos = vPos;
    createStruct.m_Rotation = rRot;

    uint32 dwSizeX = 2;
	uint32 dwSizeY = 2;

	if ((m_dwNumPoliesX >= 2) && (m_dwNumPoliesY >= 2))
	{
        dwSizeX = m_dwNumPoliesX;
		dwSizeY = m_dwNumPoliesY;

		if (m_nSurfaceType == PGSURFACE_PLASMA_FOUR_RING)
		{
			// Make sure dwSize is a factor of 2 if using four ring plasma...
			dwSizeX = FindClosestPowerOf2(dwSizeX);
			dwSizeY = FindClosestPowerOf2(dwSizeY);

			//make sure they are at least 2 wide
			if(dwSizeX < 2)
				dwSizeX = 2;
			if(dwSizeY < 2)
				dwSizeY = 2;			
		}
	}

	uint32 nFlags = 0;

	if(m_bFresnel)
		nFlags |= PG_FRESNEL;
	if(!m_bBackfaceCull)
		nFlags |= PG_NOBACKFACECULL;
	if(m_bRenderEarly)
		nFlags |= PG_RENDERBEFORETRANSLUCENTS;
	if(m_bNormalMapSprite)
		nFlags |= PG_NORMALMAPSPRITE;

	//load in the dampening buffer if applicable
	LoadDampenImage(dwSizeX, dwSizeY);

	//see if we need to setup a cutout mask
	CPolyGridBuffer<bool> CutoutMask;
	if(m_DampenBuffer.GetWidth() > 0)
	{
		bool bUseCutout = false;

		//resize our buffer
		CutoutMask.Resize(dwSizeX, dwSizeY);

		//now run through and translate the dampening image to the cutout
		uint8* pImage = m_DampenBuffer.GetBuffer();
		uint8* pEnd = pImage + dwSizeY * dwSizeX;
		bool*  pCutout = CutoutMask.GetBuffer();

		while(pImage < pEnd)
		{
			if(*pImage == 0)
			{
				*pCutout = false;
				bUseCutout = true;
			}
			else
			{
				*pCutout = true;
			}

			++pCutout;
			++pImage;
		}
	}

	m_hObject = m_pClientDE->CreateObject(&createStruct);
    m_pClientDE->SetupPolyGrid(m_hObject, dwSizeX, dwSizeY, nFlags, CutoutMask.GetBuffer());

	//update our sizes
	m_dwNumPoliesX = dwSizeX;
	m_dwNumPoliesY = dwSizeY;

	// Set alpha value...

    LTFLOAT r, g, b, a;
	m_pClientDE->GetObjectColor(m_hObject, &r, &g, &b, &a);
	m_pClientDE->SetObjectColor(m_hObject, r, g, b, m_fAlpha);

	if (m_sSurfaceSprite.size( ))
	{
		m_pClientDE->SetPolyGridTexture(m_hObject, m_sSurfaceSprite.c_str( ));
		m_pClientDE->SetPolyGridTextureInfo(m_hObject, m_fXPan, m_fYPan, m_fXScaleMin, m_fYScaleMin, m_fBaseReflection, m_fVolumeIOR);
	}

	if (m_sSurfaceEnvMap.size( ))
	{
		m_pClientDE->SetPolyGridEnvMap(m_hObject, m_sSurfaceEnvMap.c_str());
	}

	m_pClientDE->SetObjectPos(m_hObject, &vPos);
	m_pClientDE->Physics()->SetObjectDims(m_hObject, &m_vDims, 0);

	SetPolyGridPalette(colorRamps, sizeof(colorRamps)/sizeof(colorRamps[0]));

	m_pClientDE->SetPolyGridEffectShaderID(m_hObject, m_nEffectID);

	//handle initializing our appropriate updaters
	switch(m_nSurfaceType)
	{
	case PGSURFACE_PLASMA_NORMAL:
	case PGSURFACE_PLASMA_FOUR_RING:
		PrecalculatePlasma();
		break;
	case PGSURFACE_WAVE_PROP:
		InitWaveProp();
		break;
	default:
	PrecalculatePlasma();
		break;
	}

	// Set blend modes if applicable...

    uint32 dwFlags = 0;

    LTBOOL bFog = LTTRUE;
	if (m_bAdditive)
	{
		dwFlags |= FLAG2_ADDITIVE;
        bFog = LTFALSE;
	}
	else if (m_bMultiply)
	{
		dwFlags |= FLAG2_MULTIPLY;
        bFog = LTFALSE;
	}
    g_pLTCCommon->SetObjectFlags(m_hObject, OFT_Flags2, dwFlags, dwFlags);

	//handle any startup frames that we want
	HandleStartupFrames();

    return LTTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPolyGridFX::Update
//
//	PURPOSE:	Update the grid
//
// ----------------------------------------------------------------------- //

LTBOOL CPolyGridFX::Update()
{
    if(!m_hObject || !m_pClientDE || !m_hServerObject) 
		return LTFALSE;


	if(m_bWantRemove)
        return LTFALSE;


	// Set the flags of the polygrid based on the the server object...

    uint32 dwServerFlags;
	g_pLTCCommon->GetObjectFlags(m_hServerObject, OFT_Flags, dwServerFlags);

	if (dwServerFlags & FLAG_VISIBLE)
	{
		g_pLTCCommon->SetObjectFlags(m_hObject, OFT_Flags, FLAG_VISIBLE, FLAG_VISIBLE);
	}
	else  // We're hidden, no need to update...
	{
		g_pLTCCommon->SetObjectFlags(m_hObject, OFT_Flags, 0, FLAG_VISIBLE);
        return LTTRUE;
	}


	// Don't update if not drawn :)

    uint32 dwFlags;
	g_pLTCCommon->GetObjectFlags(m_hObject, OFT_Flags, dwFlags);

	if (!(dwFlags & FLAG_WASDRAWN) && !m_bAlwaysUpdate)
	{
        return LTTRUE;
	}


	// Update the position of the polygrid to reflect the position of the
	// server object...

    LTVector vPos;
	g_pLTClient->GetObjectPos(m_hServerObject, &vPos);
	g_pLTClient->SetObjectPos(m_hObject, &vPos);

	//if we are paused we shouldn't bother updating
	/*
	if(g_pCShell->IsServerPaused())
	{
		g_pLTClient->Common()->SetObjectFlags(m_hObject, OFT_Flags, FLAG_PAUSED, FLAG_PAUSED);
		return LTTRUE;
	}
	else
	{
		g_pLTClient->Common()->SetObjectFlags(m_hObject, OFT_Flags, 0, FLAG_PAUSED);
	}
	*/


	// Update the plasma based on the type of plasma...

	switch (m_nSurfaceType)
	{
		case PGSURFACE_PLASMA_FOUR_RING :
			UpdateFourRingPlasma();
		break;

		case PGSURFACE_PLASMA_NORMAL:
			UpdatePlasma();
			break;

		case PGSURFACE_WAVE_PROP:
			UpdateWaveProp(g_pLTClient->GetFrameTime());
			break;

		default :
			//default surface...
			 UpdatePlasma();
		break;
	}


	if( m_sSurfaceSprite.size( ))
	{
		UpdateSurface();
	}


    return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPolyGridFX::OnServerMessage
//
//	PURPOSE:	Handles messages from the server
//
// ----------------------------------------------------------------------- //
LTBOOL CPolyGridFX::OnServerMessage(ILTMessage_Read *pMsg)
{
	//the only message we should be receiving from the server is one
	//telling us which modifiers are activated, so read that bad boy in
	m_nActiveModifiers = pMsg->Readuint8();

	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPolyGridFX::HandleStartupFrames
//
//	PURPOSE:	runs through several iterations of updating specified in NumStartupFrames so that
//				the water won't be completely calm when starting
//
// ----------------------------------------------------------------------- //
void CPolyGridFX::HandleStartupFrames()
{
	//if we aren't a wave prop though, we don't need to fake updates
	if(m_nSurfaceType != PGSURFACE_WAVE_PROP)
		return;

	//if we don't have a minimum frame rate, there is no point in doing tihs
	if(m_fMinFrameRate < 0.01f)
		return;

	//alright, run through as many updates as specified
	for(uint32 nCurrUpdate = 0; nCurrUpdate < m_nNumStartupFrames; nCurrUpdate++)
	{
		//and update the surface based upon the minimum frame rate
		UpdateWaveProp(1.0f / m_fMinFrameRate);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CPolyGridFX::UpdateSurface
//
//	PURPOSE:	Update the polygrid's surface fx
//
// ----------------------------------------------------------------------- //

void CPolyGridFX::UpdateSurface()
{
	if (!m_pClientDE || !m_sSurfaceSprite.size( )) 
		return;

	/*
	//don't bother updating if we are paused
	if(g_pCShell->IsServerPaused())
		return;
	*/

    LTFLOAT fXPan, fYPan, fXScale, fYScale, fBaseReflection, fVolumeIOR;
	m_pClientDE->GetPolyGridTextureInfo(m_hObject, &fXPan, &fYPan, &fXScale, &fYScale, &fBaseReflection, &fVolumeIOR);

    LTFLOAT fDeltaTime = g_pLTClient->GetFrameTime();

	// Scale in X direction...

	if (m_fXScaleDuration > 0.0f)
	{
		if (m_bScalingXUp)
		{
			fXScale += fDeltaTime * (m_fXScaleMax - m_fXScaleMin) / m_fXScaleDuration;
			if (fXScale > m_fXScaleMax)
			{
				fXScale = m_fXScaleMax;
                m_bScalingXUp = LTFALSE;
			}
		}
		else
		{
			fXScale -= fDeltaTime * (m_fXScaleMax - m_fXScaleMin) / m_fXScaleDuration;
			if (fXScale < m_fXScaleMin)
			{
				fXScale = m_fXScaleMin;
                m_bScalingXUp = LTTRUE;
			}
		}
	}

	// Scale in Y direction...

	if (m_fYScaleDuration > 0.0f)
	{
		if (m_bScalingYUp)
		{
			fYScale += fDeltaTime * (m_fYScaleMax - m_fYScaleMin) / m_fYScaleDuration;
			if (fYScale > m_fYScaleMax)
			{
				fYScale = m_fYScaleMax;
                m_bScalingYUp = LTFALSE;
			}
		}
		else
		{
			fYScale -= fDeltaTime * (m_fYScaleMax - m_fYScaleMin) / m_fYScaleDuration;
			if (fYScale < m_fYScaleMin)
			{
				fYScale = m_fYScaleMin;
                m_bScalingYUp = LTTRUE;
			}
		}
	}

	// Handle panning...
	fXPan += m_fXPan * fDeltaTime;
	fYPan += m_fYPan * fDeltaTime;

	m_pClientDE->SetPolyGridTextureInfo(m_hObject, fXPan, fYPan, fXScale, fYScale, fBaseReflection, fVolumeIOR);
}

// ----------------------------------------------------------------------- //
// Loads up the dampen image and assigns it to a map that can be used to
// dampen the surface
// ----------------------------------------------------------------------- //

void CPolyGridFX::LoadDampenImage(uint32 nPGWidth, uint32 nPGHeight)
{
	if (!m_sDampenImage.size( ))
	{
		return;
	}

	//ok, now try and load the image
	uint32 nWidth, nHeight;
	uint8* pData;

	if(m_pClientDE->CreateHeightmapFromBitmap(m_sDampenImage.c_str( ), &nWidth, &nHeight, &pData) != LT_OK)
		return;
	
	//alright, now we need to do some filtering to generate our final dampen map
	m_DampenBuffer.Resize(nPGWidth, nPGHeight);

	//find out our dimensions with respect to the image
	float fXSize = (float)nWidth / (float)nPGWidth;
	float fYSize = (float)nHeight / (float)nPGHeight;

	float fSampleArea = fXSize * fYSize;

	//now run through for every sample in the dampen buffer
	for(uint32 nY = 0; nY < nPGHeight; nY++)
	{
		for(uint32 nX = 0; nX < nPGWidth; nX++)
		{
			//ok, now we build up a rectangle and filter the image buffer through
			//this rectangle to find the final value
			float fXMin = nX * fXSize;
			float fYMin = (nPGHeight - 1 - nY) * fYSize;
			float fXMax = fXMin + fXSize;
			float fYMax = fYMin + fYSize;

			//now we can find the extents to filter into this rectangle
			uint32 nXMin = (uint32)fXMin;
			uint32 nYMin = (uint32)fYMin;
			uint32 nXMax = LTMIN((uint32)fXMax, nWidth - 1);
			uint32 nYMax = LTMIN((uint32)fYMax, nHeight - 1);

			float fVal = 0.0f;

			for(uint32 nIY = nYMin; nIY <= nYMax; nIY++)
			{
				for(uint32 nIX = nXMin; nIX <= nXMax; nIX++)
				{
					//now we have a rectangle of width/height of one at this position,
					//so we need to see how much intersects with the bounding rect
					float fIXMin = LTMAX(fXMin, (float)nIX);
					float fIYMin = LTMAX(fYMin, (float)nIY);
					float fIXMax = LTMIN(fXMax, (float)nIX + 1.0f);
					float fIYMax = LTMIN(fYMax, (float)nIY + 1.0f);

					//figure out how much of the rectangle is occupied by this
					float fWeight = (fIXMax - fIXMin) * (fIYMax - fIYMin) / fSampleArea;

					//add our contribution
					fVal += fWeight * pData[nIY * nWidth + nIX];
				}
			}

			//save this value
			m_DampenBuffer.Get(nX, nY) = (uint8)fVal;
		}
	}

	//free our heightmap
	m_pClientDE->FreeHeightmap(pData);
}


// ----------------------------------------------------------------------- //
// Precalculates the lookup tables for plasma FX.
// ----------------------------------------------------------------------- //

void CPolyGridFX::PrecalculatePlasma()
{
    uint32 width, height;
	int halfWidth, halfHeight, x, y;
	char *pData;
	PGColor *pColorTable;
	float scale, val, maxDistSqr, testDist, t;
	int i;


	// Create the (scaled) sin table.
	if(!g_bSinTableInitted)
	{
		scale = (MATH_CIRCLE / 255.0f) * 3.0f;
		for(i=0; i < 256; i++)
		{
			val = (float)i * scale;
			g_SinTable[i] = (char)(sin(val) * 128.0f);
		}

        g_bSinTableInitted = LTTRUE;
	}


	// Fill in the distance grid.
	m_pClientDE->GetPolyGridInfo(m_hObject, &pData, &width, &height, &pColorTable);


    m_DistanceGrid.Resize(width, height);
	halfWidth = width >> 1;
	halfHeight = height >> 1;
	maxDistSqr = (float)(halfWidth*halfWidth + halfHeight*halfHeight);
	for(y=0; y < (int)height; y++)
	{
		for(x=0; x < (int)width; x++)
		{
			testDist = (float)((x-halfWidth)*(x-halfWidth) + (y-halfHeight)*(y-halfHeight));
			t = testDist / maxDistSqr;
            m_DistanceGrid[y*width+x] = (uint8)(t * 255.0f);
		}
	}
}

// ----------------------------------------------------------------------- //
// Inits the buffers for the wave propagation
// ----------------------------------------------------------------------- //
void CPolyGridFX::InitWaveProp()
{
	//get the information about the polygrid
	char*		pPGData;
	uint32		nPGWidth, nPGHeight;
	PGColor*	pPGColorTable;
	m_pClientDE->GetPolyGridInfo(m_hObject, &pPGData, &nPGWidth, &nPGHeight, &pPGColorTable);

	//alright, now we need to allocate our 2 buffers, and clear them out
	m_WaveBuffer[0].Resize(nPGWidth, nPGHeight);

	//now clear it out
	memset(m_WaveBuffer[0].GetBuffer(), 0, sizeof(float) * nPGWidth * nPGHeight);

	//copy it to the second one so they are both initialized
	m_WaveBuffer[1] = m_WaveBuffer[0];

	m_nCurrWaveBuffer		= 0;
	m_fPrevFrameTime		= 1.0f;
}


static inline void CalcSample(float* pCurr, const float* pPrev, float fVelocCoeff, float fAccelCoeff, float fDampen, uint32 nPGWidth)
{			
	//find all the forces bineg applied to this point. The layout is to
	//be as cache friendly as possible
	float fResult =	*(pPrev - nPGWidth - 1) + 
					*(pPrev - nPGWidth) + 
					*(pPrev - nPGWidth + 1) +					
					*(pPrev - 1) -
					*pPrev * 8.0f +
					*(pPrev + 1) +
					*(pPrev + nPGWidth - 1) +
					*(pPrev + nPGWidth) +
					*(pPrev + nPGWidth + 1);


	//now find the new position, and apply dampening
	*pCurr = (*pPrev + (*pPrev - *pCurr) * fVelocCoeff + fResult * fAccelCoeff) * fDampen;

	//clamp to be in range
	if(*pCurr < -127.0f)
		*pCurr = 127.0f;
	else if(*pCurr > 127.0f)
		*pCurr = 127.0f;
}

//outputs a standard height to the buffer and moves both pointers forward
static inline void OutputHeight(char*& pOut, const float*& pCurr)
{
	*pOut = (char)*pCurr;
	pOut++;
	pCurr++;
}

//outputs a dampened standard height to the buffer and moves all pointers forward
static inline void OutputDampenedHeight(char*& pOut, const float*& pCurr, const uint8*& pDampen)
{
	*pOut = (char)(((int32)((*pCurr) * (*pDampen))) >> 8);
	pOut++;
	pCurr++;
	pDampen++;
}
	
void CPolyGridFX::UpdateWaveProp(float fFrameTime)
{
	//constant on how large our kernal is, extending beyond the source pixel on a side
	static const uint32 knKernalSize = 1;

	static const float kfInvWaterMass = 0.05f;

	//adjust for user scale
	fFrameTime *= m_fTimeScale;

	//clamp the frame time to prevent explosions
	if(m_fMinFrameRate > 0.01f)
	{
		float fInvMinFrameRate = 1.0f / m_fMinFrameRate;
		/*
		if(g_cvarMinPGFrameRate.GetFloat() > 0.1f)
		{
			//we want to simulate the slowest possible time (to allow artists a chance to see if it
			//will blow up)
			fFrameTime = fInvMinFrameRate;
		}
		*/

		if(fFrameTime > fInvMinFrameRate)
			fFrameTime = fInvMinFrameRate;
	}
		
	//get the information about the polygrid
	char*		pPGData;
	uint32		nPGWidth, nPGHeight;
	PGColor*	pPGColorTable;
	m_pClientDE->GetPolyGridInfo(m_hObject, &pPGData, &nPGWidth, &nPGHeight, &pPGColorTable);

	//figure out indices for our buffers
	uint32 nCurrBufferIndex = m_nCurrWaveBuffer;
	uint32 nPrevBufferIndex = (m_nCurrWaveBuffer + 1) % 2;


	//run through our active modifiers
	for(uint32 nCurrMod = 0; nCurrMod < PG_MAX_MODIFIERS; nCurrMod++)
	{
		//see if this modifier is active
		if(!(m_nActiveModifiers & (1 << nCurrMod)))
			continue;

		//find the width and heights
		uint32 nWidth  = m_nXMax[nCurrMod] - m_nXMin[nCurrMod];
		uint32 nHeight = m_nYMax[nCurrMod] - m_nYMin[nCurrMod];

		//bail if invalid
		if((nWidth == 0) || (nHeight == 0) || (m_fAccelAmount[nCurrMod] == 0.0f))
			continue;

		//instead of scaling the impact amount by the frame time (which produces a lot of noise
		//due to tons of little punches), we scale the number of samples, and scale the last sample
		//to be of the appropriate size which reduces noise considerably
		float fModifierEnergy	= m_nNumAccelPoints[nCurrMod] * fFrameTime;

		//this modifier is active, so let us offset the desired number of points
		while(fModifierEnergy > 0.01f)
		{
			//figure out how much we are contributing
			float fEnergyToUse = LTMIN(fModifierEnergy, m_fPrevImpactAmountLeft[nCurrMod]);

			fModifierEnergy						-= fEnergyToUse;
			m_fPrevImpactAmountLeft[nCurrMod]	-= fEnergyToUse;

			float fAccelAmount = m_fAccelAmount[nCurrMod] * fEnergyToUse;

			uint32 nX = m_nPrevImpactX[nCurrMod];
			uint32 nY = m_nPrevImpactY[nCurrMod];

			m_WaveBuffer[nPrevBufferIndex].Get(nX, nY) -= fAccelAmount;
			m_WaveBuffer[nCurrBufferIndex].Get(nX, nY) -= fAccelAmount;

			//see if we need to generate a new position
			if(m_fPrevImpactAmountLeft[nCurrMod] < 0.01f)
			{
				m_fPrevImpactAmountLeft[nCurrMod] = 1.0f;
				m_nPrevImpactX[nCurrMod] = (rand() % nWidth) + m_nXMin[nCurrMod];
				m_nPrevImpactY[nCurrMod] = (rand() % nHeight) + m_nYMin[nCurrMod];
			}
		}
	}

	//get our primary buffer
	float* pCurr = m_WaveBuffer[nCurrBufferIndex].GetBuffer();

	//now get this buffer which for the duration of this function is still our
	//secondary buffer
	float* pPrev = m_WaveBuffer[nPrevBufferIndex].GetBuffer() + knKernalSize * (nPGWidth + 1);

	//need to make sure that the dampening scale is not frame rate dependant, so
	//that for every second, that amount of energy will be left in the system
	float fDampen = (float)pow(m_fDampenScale, fFrameTime);

	//sanity check...
	assert(fDampen <= 1.0f);

	//precalculate some variables
	float fSpringForce = (m_fSpringCoeff * kfInvWaterMass);
	float fAccelCoeff = fSpringForce * fFrameTime * fFrameTime;
	float fVelocCoeff = fFrameTime / m_fPrevFrameTime;

	char*  pOut = pPGData;
	uint32 nX, nY;

	if(m_DampenBuffer.GetWidth() > 0)
	{
		//update our buffer, dampened
		uint8* pDampen = m_DampenBuffer.GetBuffer();

		for(nY = 0; nY < nPGWidth * knKernalSize; nY++)
		{
			OutputDampenedHeight(pOut, pCurr, pDampen);
		}
		for(nY = knKernalSize; nY < nPGHeight - knKernalSize; nY++)
		{
			for(nX = 0; nX < knKernalSize; nX++)
			{
				OutputDampenedHeight(pOut, pCurr, pDampen);
			}
			for(; nX < nPGWidth - knKernalSize; nX++)
			{
				CalcSample(pCurr, pPrev, fVelocCoeff, fAccelCoeff, fDampen, nPGWidth);
				OutputDampenedHeight(pOut, pCurr, pDampen);

				//update our pointers
				pPrev++;
			}
			for(nX = 0; nX < knKernalSize; nX++)
			{
				OutputDampenedHeight(pOut, pCurr, pDampen);
			}

			//now update our current pointers to skip over the kernal buffers on either side
			pPrev += knKernalSize * 2;
		}
		for(nY = 0; nY < nPGWidth * knKernalSize; nY++)
		{
			OutputDampenedHeight(pOut, pCurr, pDampen);
		}
	}
	else
	{
		//update our buffer undampened
		for(nY = 0; nY < nPGWidth * knKernalSize; nY++)
		{
			OutputHeight(pOut, pCurr);
		}
		for(nY = knKernalSize; nY < nPGHeight - knKernalSize; nY++)
		{
			for(nX = 0; nX < knKernalSize; nX++)
			{
				OutputHeight(pOut, pCurr);
			}
			for(; nX < nPGWidth - knKernalSize; nX++)
			{
				CalcSample(pCurr, pPrev, fVelocCoeff, fAccelCoeff, fDampen, nPGWidth);
				OutputHeight(pOut, pCurr);

				//update our pointers
				pPrev++;
			}
			for(nX = 0; nX < knKernalSize; nX++)
			{
				OutputHeight(pOut, pCurr);
			}

			//now update our current pointers to skip over the kernal buffers on either side
			pPrev += knKernalSize * 2;
		}
		for(nY = 0; nY < nPGWidth * knKernalSize; nY++)
		{
			OutputHeight(pOut, pCurr);
		}
	}

	//switch our buffer to be the other one
	m_nCurrWaveBuffer = nPrevBufferIndex;

	//save the frame time
	m_fPrevFrameTime = fFrameTime;
}


// ----------------------------------------------------------------------- //
// Inits the palette for the grid.  Pass in a ramp list for it to interpolate thru.
// ----------------------------------------------------------------------- //

void CPolyGridFX::SetPolyGridPalette(ColorRamp *pRamps, int nRamps)
{
	float t;
    uint32 width, height;
	char *pData;
	PGColor *pColorTable, color1, color2;
	int ramp, i, index1, index2;

	// Randomize the poly grid values.
	m_pClientDE->GetPolyGridInfo(m_hObject, &pData, &width, &height, &pColorTable);

	// Make the color table go from white to black.
	for(ramp=0; ramp < nRamps-1; ramp++)
	{
		index1 = pRamps[ramp].m_Index;
		index2 = pRamps[ramp+1].m_Index;
		VEC_COPY(color1, pRamps[ramp].m_Color);
		VEC_COPY(color2, pRamps[ramp+1].m_Color);

		for(i=index1; i < index2; i++)
		{
			t = (float)(i - index1) / (index2 - index1);
			VEC_LERP(pColorTable[i], color1, color2, t);
			pColorTable[i].a = 255.0f;
		}
	}
}


// ----------------------------------------------------------------------- //
// Updates the plasma effect.
// ----------------------------------------------------------------------- //

void CPolyGridFX::UpdatePlasma()
{
    uint32 width, height;
	char *pData, *pCur, *pEnd;
	PGColor *pColorTable;
    uint8 count, *pDistanceGrid;

	m_fCount += 50.0f * g_pLTClient->GetFrameTime();
    count = (uint8)m_fCount;

	// Randomize the poly grid values.
	m_pClientDE->GetPolyGridInfo(m_hObject, &pData, &width, &height, &pColorTable);

	
	//see if we have a dampening buffer, if so we want to do the slightly slower route
	//and apply the dampening factors
	if(m_DampenBuffer.GetWidth() > 0)
	{
		uint8* pCurrDampen = m_DampenBuffer.GetBuffer();
		pDistanceGrid = m_DistanceGrid.GetBuffer();
		pCur = pData;
		pEnd = pCur + width * height;

		while(pCur < pEnd)
		{
			*pCur = (char)((*pCurrDampen * (int32)g_SinTable[(uint8)((*pDistanceGrid) + count)]) >> 8);
			++pDistanceGrid;
			++pCurrDampen;
			++pCur;
		}
	}
	else
	{
		//no dampening, the standard route
		pDistanceGrid = m_DistanceGrid.GetBuffer();
		pCur = pData;
		pEnd = pCur + width * height;
		
		while(pCur < pEnd)
		{
            *pCur = g_SinTable[(uint8)((*pDistanceGrid) + count)];
			++pDistanceGrid;
			++pCur;
		}
	}
}

float g_Counts[4] = {0.0f, 1.0f, 4.0f, 3.0f};
long g_Offsets[4][2] =
{
	6, 10,
	13, 3,
	17, 1,
	31, 22
};

// ----------------------------------------------------------------------- //
// Updates the plasma effect.
// ----------------------------------------------------------------------- //

void CPolyGridFX::UpdateFourRingPlasma()
{

    uint32 dwWidth, dwHeight;
	long width, height, x, y, xMask, yMask, yOffsets[4];
	char *pData, *pCur;
	PGColor *pColorTable;
    uint8 val[4], counts[4], *pDistanceGrid;
	long i;

	for(i=0; i < 4; i++)
	{
		g_Counts[i] += ((float)m_nRingRate[i]) * g_pLTClient->GetFrameTime();
        counts[i] = (uint8)g_Counts[i];
	}

	m_pClientDE->GetPolyGridInfo(m_hObject, &pData, &dwWidth, &dwHeight, &pColorTable);
	width = (long)dwWidth;
	height = (long)dwHeight;

	xMask = width - 1;
	yMask = height - 1;

	//see if we have a dampening buffer, if so we want to do the slightly slower route
	//and apply the dampening factors
	if(m_DampenBuffer.GetWidth() > 0)
	{
		uint8* pCurrDampen = m_DampenBuffer.GetBuffer();

	for(y=0; y < height; y++)
	{
		pCur = pData + (y*width);
			pDistanceGrid = m_DistanceGrid.GetRow(0);

		yOffsets[0] = ((y+g_Offsets[0][1]) & yMask)*width;
		yOffsets[1] = ((y+g_Offsets[1][1]) & yMask)*width;
		yOffsets[2] = ((y+g_Offsets[2][1]) & yMask)*width;
		yOffsets[3] = ((y+g_Offsets[3][1]) & yMask)*width;

		x = width;
		while(x--)
		{
			val[0] = pDistanceGrid[yOffsets[0] + ((x+g_Offsets[0][0]) & xMask)];
			val[1] = pDistanceGrid[yOffsets[1] + ((x+g_Offsets[1][0]) & xMask)];
			val[2] = pDistanceGrid[yOffsets[2] + ((x+g_Offsets[2][0]) & xMask)];
			val[3] = pDistanceGrid[yOffsets[3] + ((x+g_Offsets[3][0]) & xMask)];

					*pCur = (char)((*pCurrDampen * 
								((	(int32)	g_SinTable[(uint8)(val[0] + counts[0])] +
                g_SinTable[(uint8)(val[1] + counts[1])] +
                g_SinTable[(uint8)(val[2] + counts[2])] +
                g_SinTable[(uint8)(val[3] + counts[3])]
								 ) >> 2)) >> 8);
				++pCur;
				++pCurrDampen;
			}
		}
	}
	else
	{
		//no dampening, do the standard route
		for(y=0; y < height; y++)
		{
			pCur = pData + (y*width);
			pDistanceGrid = m_DistanceGrid.GetRow(0);

			yOffsets[0] = ((y+g_Offsets[0][1]) & yMask)*width;
			yOffsets[1] = ((y+g_Offsets[1][1]) & yMask)*width;
			yOffsets[2] = ((y+g_Offsets[2][1]) & yMask)*width;
			yOffsets[3] = ((y+g_Offsets[3][1]) & yMask)*width;

			x = width;
			while(x--)
			{
				val[0] = pDistanceGrid[yOffsets[0] + ((x+g_Offsets[0][0]) & xMask)];
				val[1] = pDistanceGrid[yOffsets[1] + ((x+g_Offsets[1][0]) & xMask)];
				val[2] = pDistanceGrid[yOffsets[2] + ((x+g_Offsets[2][0]) & xMask)];
				val[3] = pDistanceGrid[yOffsets[3] + ((x+g_Offsets[3][0]) & xMask)];

					*pCur = (char)(((int32)	g_SinTable[(uint8)(val[0] + counts[0])] +
											g_SinTable[(uint8)(val[1] + counts[1])] +
											g_SinTable[(uint8)(val[2] + counts[2])] +
											g_SinTable[(uint8)(val[3] + counts[3])]
								 ) >> 2);
				++pCur;
			}
		}
	}
}

//Given a point and a direction to look in, this function will find out where it intersects
//the polygrid, and determine the displacement from the polygrid at that point. It will
//return false if it doesn't intersect. This assumes an axis aligned polygrid.
bool CPolyGridFX::GetOrientedIntersectionHeight(const LTVector& vPos, const LTVector& vUnitDir, float& fDisplacement, LTVector& vIntersection)
{
	//Just assume an axis aligned polygrid, and find the intersection point
	if(fabs(vUnitDir.y) < 0.01f)
	{
		//we are parallel to the plane
		return false;
	}

	//get the position of the polygrid so we know the plane distance
	LTVector vPGPos;
	g_pLTClient->GetObjectPos(m_hObject, &vPGPos);

	float fT = (vPGPos.y - vPos.y) / vUnitDir.y;

	vIntersection = vPos + vUnitDir * fT;

	//find the unit displacements along the polygrid axis
	float fX = ((vIntersection.x - vPGPos.x) + m_vDims.x) / (m_vDims.x * 2.0f);
	float fZ = ((vIntersection.z - vPGPos.z) + m_vDims.z) / (m_vDims.z * 2.0f);

	//determine the actual grid cels
	int32 nXGrid = (int32)(fX * m_dwNumPoliesX);
	int32 nZGrid = (int32)(fZ * m_dwNumPoliesY);

	//now determine if the point is outside of the box
	if(	(nXGrid < 0) || (nXGrid >= (int32)m_dwNumPoliesX) || (nZGrid < 0) || (nZGrid >= (int32)m_dwNumPoliesY))
	{
		//outside of the box
		return false;
	}

	//now just grab our interpolants
	float fXRemainder = fX * m_dwNumPoliesX - (float)nXGrid;
	float fZRemainder = fZ * m_dwNumPoliesY - (float)nZGrid;

	//grab our polygrid height data (we can't use the wave tables in order to support other
	//wave modes)
	char* pHeights = LTNULL;
	uint32 dwWidth, dwHeight;
	PGColor *pColor = LTNULL;

	if( LT_OK != g_pLTClient->GetPolyGridInfo(m_hObject, &pHeights, &dwWidth, &dwHeight, &pColor) || !pHeights )
	{
		return false;
	}

	//find our sample points
	float fUL = (float)pHeights[nZGrid * m_dwNumPoliesX + nXGrid];
	float fUR = (float)pHeights[nZGrid * m_dwNumPoliesX + nXGrid + 1];
	float fLL = (float)pHeights[(nZGrid + 1) * m_dwNumPoliesX + nXGrid];
	float fLR = (float)pHeights[(nZGrid + 1) * m_dwNumPoliesX + nXGrid + 1];

	//and now interpolate
	float fLeft = fUL * (1.0f - fZRemainder) + fLL * fZRemainder;
	float fRight = fUR * (1.0f - fZRemainder) + fLR * fZRemainder;

	//and the final value
	fDisplacement = fLeft * (1.0f - fXRemainder) + fRight;

	//scale the displacement to be along the dims
	fDisplacement *= m_vDims.y / 127.0f;

	//success
	return true;
}
