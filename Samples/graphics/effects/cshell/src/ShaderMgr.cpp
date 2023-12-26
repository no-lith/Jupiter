#include "ShaderMgr.h"

#include <d3d9.h>
#include <iltclient.h>
#include "clientinterfaces.h"
#include <iltrendermgr.h>
#include <iltdrawprim.h>
#include <ltgraphicscaps.h>

//Pixel/Vertex shader defines. Taken from d3d9types.h.

// pixel shader version token
#define PS_VERSION(_Major,_Minor) (0xFFFF0000|((_Major)<<8)|(_Minor))

// vertex shader version token
#define VS_VERSION(_Major,_Minor) (0xFFFE0000|((_Major)<<8)|(_Minor))

namespace
{
	uint32 g_nPixelShaderMajor = 0;
	uint32 g_nPixelShaderMinor = 0;
	uint32 g_nVertexShaderMajor = 0;
	uint32 g_nVertexShaderMinor = 0;

	uint32 g_nScreenWidth = 640;
	uint32 g_nScreenHeight = 480;

	enum EffectShaderIDS
	{
		/* Fullscreen effects (not implemented currently)*/
		EFFECTSHADER_SEPIA								= 1,
		EFFECTSHADER_BLACKANDWHITE						= 2,
		EFFECTSHADER_SOBELEDGE							= 3,
		EFFECTSHADER_INVERSE							= 4,

		EFFECTSHADER_WORLD_BUMPREFLECT					= 24,
		EFFECTSHADER_MODEL_SKEL_3WEIGHTS				= 27,
		EFFECTSHADER_MODEL_RIGID						= 28,
		EFFECTSHADER_MODEL_SKEL_1WEIGHT					= 29,
		EFFECTSHADER_MODEL_VERTEXANIMATED				= 30,
		EFFECTSHADER_WORLD_PARALLAXMAP					= 31,
		EFFECTSHADER_WORLD_REFRACT						= 32,
		EFFECTSHADER_POLYGRID_REFRACT					= 34,
		EFFECTSHADER_SPRITE_REFRACT						= 35,
		EFFECTSHADER_MODEL_SKEL_3WEIGHTS_PARALLAXMAP	= 36,
		EFFECTSHADER_MODEL_SKEL_1WEIGHT_BASIS			= 38,
		EFFECTSHADER_RIGIDMODEL_BASIS					= 40,
		EFFECTSHADER_SHAREDGLOBALS						= 50,
	};

	enum ERenderTargetIDS
	{
		RENDERTARGET_NONE	 = 0,
		RENDERTARGET_OVERLAY,
		RENDERTARGET_FORCE32BITS = 0xFFFFFFFF
	};

	//
	// Vertex Declarations
	//

	// World Geometry (Gouraud Shaded)
	D3DVERTEXELEMENT9 decl_World[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, //size 12
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 }, //size 12
		{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 }, //size 12
		{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 }, //size 12
		{ 0, 48, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 }, //size 4 (one DWORD)
		{ 0, 52, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 }, //size 12
		D3DDECL_END()
	};

	D3DVERTEXELEMENT9 decl_Model_Rigid[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, //size 12
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 }, //size 12
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 }, //size 8
		D3DDECL_END()
	};

	D3DVERTEXELEMENT9 decl_Model_RigidBasis[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, //size 12
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 }, //size 12
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 }, //size 8
		{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 }, //size 8
		{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 }, //size 8
		D3DDECL_END()
	};

	// position + blend weight(3) + normal + texture
	D3DVERTEXELEMENT9 Decl_PosWeight3NormTex[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 36, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	D3DVERTEXELEMENT9 Decl_PosWeight3NormTexTangent[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 36, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
		{ 0, 56, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
		D3DDECL_END()
	};

	// position + blend weight(1) + normal + texture
	D3DVERTEXELEMENT9 Decl_PosWeight1NormTex[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};

	// position + blend weight(1) + normal + texture
	D3DVERTEXELEMENT9 Decl_PosWeight1NormTexBasis[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT1, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
		{ 0, 16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 28, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		{ 0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
		{ 0, 48, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
		D3DDECL_END()
	};

	D3DVERTEXELEMENT9 decl_VAModel[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, //size 12
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 }, //size 12
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 }, //size 8
		D3DDECL_END()
	};

	D3DVERTEXELEMENT9 decl_VAModelBasis[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, //size 12
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 }, //size 12
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 }, //size 8
		{ 0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 }, //size 12
		{ 0, 44, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 }, //size 12
		D3DDECL_END()
	};

	D3DVERTEXELEMENT9 decl_Polygrid[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, //size 12
		{ 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 }, //size 12
		{ 0, 24, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 }, //size 4 (one DWORD)
		{ 0, 28, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 }, //size 12
		{ 0, 40, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0 },
		{ 0, 52, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BINORMAL, 0 },
		D3DDECL_END()
	};

	D3DVERTEXELEMENT9 decl_Sprite[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 }, //size 12
		{ 0, 12, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 }, //size 4 (one DWORD)
		{ 0, 16, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 }, //size 12
		D3DDECL_END()
	};

	D3DVERTEXELEMENT9 decl_DrawPrimPosTColorTex[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITIONT, 0 }, //size 16
		{ 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 }, //size 4 (one DWORD)
		{ 0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 }, //size 8
		D3DDECL_END()
	};
}

CShaderMgr::CShaderMgr():
m_hCamera(NULL),
m_hGameLight(NULL),
m_eMode(SHADERMGR_MODE_DEFAULT)
{
}

CShaderMgr::~CShaderMgr()
{
	Term();
}

LTRESULT CShaderMgr::Init()
{
	// Let's create our shared effect pool.

	HEFFECTPOOL hEffectPool = 1;
	LTRESULT rEffectPool = g_pLTCRenderMgr->CreateEffectPool(hEffectPool);
	if(rEffectPool == LT_OK)
	{
		g_pLTClient->CPrint("SUCCESS: Effect Pool %d Created!", hEffectPool);
	}
	else
	{
		g_pLTClient->CPrint("FAILURE: Effect Pool %d Creation Failed!", hEffectPool);
	}

	// Let's load our shaders.

	// FULL SCREEN //
	AddEffectShader("Shaders\\Fullscreen\\SepiaTone.fx", EFFECTSHADER_SEPIA, NULL, 0, NO_EFFECT_POOL);
	AddEffectShader("Shaders\\Fullscreen\\BlackWhite.fx", EFFECTSHADER_BLACKANDWHITE, NULL, 0, NO_EFFECT_POOL);
	AddEffectShader("Shaders\\Fullscreen\\SobelEdge.fx", EFFECTSHADER_SOBELEDGE, (const uint32*)decl_DrawPrimPosTColorTex, sizeof(decl_DrawPrimPosTColorTex), NO_EFFECT_POOL);
	AddEffectShader("Shaders\\Fullscreen\\Inverse.fx", EFFECTSHADER_INVERSE, (const uint32*)decl_DrawPrimPosTColorTex, sizeof(decl_DrawPrimPosTColorTex), NO_EFFECT_POOL);

	// WORLD //
	AddEffectShader("Shaders\\World\\BumpReflect.fx", EFFECTSHADER_WORLD_BUMPREFLECT, (const uint32*)decl_World, sizeof(decl_World), NO_EFFECT_POOL);
	AddEffectShader("Shaders\\World\\ParallaxMap.fx", EFFECTSHADER_WORLD_PARALLAXMAP, (const uint32*)decl_World, sizeof(decl_World), hEffectPool);
	AddEffectShader("Shaders\\World\\refract.fx", 32, (const uint32*)decl_World, sizeof(decl_World), hEffectPool);

	// MODELS //

	// Rigid
	AddEffectShader("Shaders\\Models\\Rigid\\Skin.fx", EFFECTSHADER_MODEL_RIGID, (const uint32*)decl_Model_Rigid, sizeof(decl_Model_Rigid), NO_EFFECT_POOL);
	AddEffectShader("Shaders\\Models\\Rigid\\SkinWithBasis.fx", EFFECTSHADER_RIGIDMODEL_BASIS, (const uint32*)decl_Model_RigidBasis, sizeof(decl_Model_RigidBasis), NO_EFFECT_POOL);

	// Skeletal
	AddEffectShader("Shaders\\Models\\Skel\\Skin_3Weights.fx", EFFECTSHADER_MODEL_SKEL_3WEIGHTS, (const uint32*)Decl_PosWeight3NormTex, sizeof(Decl_PosWeight3NormTex), NO_EFFECT_POOL);
	AddEffectShader("Shaders\\Models\\Skel\\Skin_3Weights_Parallax.fx", EFFECTSHADER_MODEL_SKEL_3WEIGHTS_PARALLAXMAP, (const uint32*)Decl_PosWeight3NormTexTangent, sizeof(Decl_PosWeight3NormTexTangent), hEffectPool);
	AddEffectShader("Shaders\\Models\\Skel\\Skin_1Weight.fx", EFFECTSHADER_MODEL_SKEL_1WEIGHT, (const uint32*)Decl_PosWeight1NormTex, sizeof(Decl_PosWeight1NormTex), NO_EFFECT_POOL);
	AddEffectShader("Shaders\\Models\\Skel\\Skin_1WeightWithBasis.fx", EFFECTSHADER_MODEL_SKEL_1WEIGHT_BASIS, (const uint32*)Decl_PosWeight1NormTexBasis, sizeof(Decl_PosWeight1NormTexBasis), NO_EFFECT_POOL);

	// Vertex Animated
	AddEffectShader("Shaders\\Models\\VertexAnim\\Skin.fx", EFFECTSHADER_MODEL_VERTEXANIMATED, (const uint32*)decl_VAModel, sizeof(decl_VAModel), NO_EFFECT_POOL);	
	
	// POLYGRID //
	AddEffectShader("Shaders\\PolyGrid\\refract_pg.fx", 34, (const uint32*)decl_Polygrid, sizeof(decl_Polygrid), hEffectPool);

	// SPRITE //
	AddEffectShader("Shaders\\Sprite\\refract_spr.fx", 35, (const uint32*)decl_Sprite, sizeof(decl_Sprite), NO_EFFECT_POOL);

    // SHARED //
	AddEffectShader("Shaders\\Global\\sharedglobals.fx", EFFECTSHADER_SHAREDGLOBALS, NULL, 0, hEffectPool);


	// What support does this device have for pixel and vertex shaders?
	QueryDeviceCaps();	
	
	HSURFACE hScreenSurface = g_pLTClient->GetScreenSurface();
	g_pLTClient->GetSurfaceDims(hScreenSurface, &g_nScreenWidth, &g_nScreenHeight);

	if(g_pLTCRenderMgr->CreateRenderTarget(g_nScreenWidth, g_nScreenHeight, RTFMT_X8R8G8B8, STFMT_UNKNOWN, RENDERTARGET_OVERLAY) != LT_OK)
	{
		g_pLTClient->CPrint("Failed to create rendertarget of 320x240");
	}

	return LT_OK;
}

LTRESULT CShaderMgr::Term()
{
	m_hCamera = NULL;
	m_hGameLight = NULL;

	return LT_OK;
}

LTRESULT CShaderMgr::QueryDeviceCaps()
{
	LTGraphicsCaps caps;
	LTRESULT res = g_pLTClient->QueryGraphicDevice(&caps);
	if(res == LT_OK)
	{
		g_pLTClient->CPrint(" ");
		g_pLTClient->CPrint("QueryGraphicDevice:");
		g_pLTClient->CPrint("-------------------------------------");
		if(caps.PixelShaderVersion < PS_VERSION(1,0))
		{
			g_pLTClient->CPrint("Warning: Pixel Shader version less than 1.0!");
		}
		else
		{
			g_pLTClient->CPrint("Graphic device supports Pixel Shader version 1.0 or greater.");
			g_nPixelShaderMajor = 1;
		}

		if(caps.PixelShaderVersion < PS_VERSION(1,1))
		{
			g_pLTClient->CPrint("Warning: Pixel Shader version less than 1.1!");
		}
		else
		{
			g_pLTClient->CPrint("Graphic device supports Pixel Shader version 1.1 or greater.");
			g_nPixelShaderMajor = 1;
			g_nPixelShaderMinor = 1;
		}

		if(caps.PixelShaderVersion < PS_VERSION(2,0))
		{
			g_pLTClient->CPrint("Warning: Pixel Shader version less than 2.0!");
		}
		else
		{
			g_pLTClient->CPrint("Graphic device supports Pixel Shader version 2.0 or greater.");
			g_nPixelShaderMajor = 2;
		}

		if(caps.PixelShaderVersion < PS_VERSION(3,0))
		{
			g_pLTClient->CPrint("Warning: Pixel Shader version less than 3.0!");
		}		
		else
		{
			g_pLTClient->CPrint("Graphic device supports Pixel Shader version 3.0 or greater.");
			g_nPixelShaderMajor = 3;
		}

		if(caps.VertexShaderVersion < VS_VERSION(1,0))
		{
			g_pLTClient->CPrint("Warning: Vertex Shader version less than 1.0!");
		}
		else
		{
			g_pLTClient->CPrint("Graphic device supports Vertex Shader version 1.0 or greater.");
		}

		if(caps.VertexShaderVersion < VS_VERSION(1,1))
		{
			g_pLTClient->CPrint("Warning: Vertex Shader version less than 1.1!");
		}
		else
		{
			g_pLTClient->CPrint("Graphic device supports Vertex Shader version 1.1 or greater.");
			g_nVertexShaderMajor = 1;
			g_nVertexShaderMinor = 1;
		}

		if(caps.VertexShaderVersion < VS_VERSION(2,0))
		{
			g_pLTClient->CPrint("Warning: Vertex Shader version less than 2.0!");
		}
		else
		{
			g_pLTClient->CPrint("Graphic device supports Vertex Shader version 2.0 or greater.");
			g_nVertexShaderMajor = 2;
		}

		if(caps.VertexShaderVersion < VS_VERSION(3,0))
		{
			g_pLTClient->CPrint("Warning: Vertex Shader version less than 3.0!");
		}
		else
		{
			g_pLTClient->CPrint("Graphic device supports Vertex Shader version 3.0 or greater.");
			g_nVertexShaderMajor = 3;
		}

		g_pLTClient->CPrint("-------------------------------------");
	}
	else
	{
		g_pLTClient->CPrint("Error: Could not query the device caps!");
		return LT_ERROR;
	}

	return LT_OK;
}

LTRESULT CShaderMgr::AddEffectShader(const char *pFileName, int EffectShaderID, const uint32 *pVertexElements, uint32 VertexElementsSize, HEFFECTPOOL EffectPoolID)
{
	LTRESULT bRet = g_pLTCRenderMgr->AddEffectShader(pFileName, EffectShaderID, pVertexElements, VertexElementsSize, EffectPoolID);
	if(bRet == LT_OK)
	{
		g_pLTClient->CPrint("Effect Shader %s is good...", pFileName);

		//find a valid technique
		LTEffectShader* pEffect = g_pLTCRenderMgr->GetEffectShader(EffectShaderID);
		if(pEffect)
		{
			LTTechniqueInfo info;
			if(pEffect->FindFirstValidTechnique(&info) == LT_OK)
			{
				g_pLTClient->CPrint("    Found valid technique: %s", info.szName);
				if(pEffect->SetTechnique(info.szName) == LT_OK)
				{
					g_pLTClient->CPrint("    Using technique: %s", info.szName);
				}
			}
		}
	}
	else
	{
		g_pLTClient->CPrint("(*!!!*) Effect Shader %s failed to load!!!", pFileName);
	}

	return bRet;
}

void CShaderMgr::OnEffectShaderSetParams(LTEffectShader *pEffect, CRenderStyle *pRenderStyle, HOBJECT hObj, const LTShaderDeviceState &ShaderDeviceState)
{
	switch(pEffect->GetID())
	{
	case EFFECTSHADER_SEPIA:
	case EFFECTSHADER_BLACKANDWHITE:
	case EFFECTSHADER_SOBELEDGE:
	case EFFECTSHADER_INVERSE:
		{
			LTMatrix mWorld;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_WORLD0, true, &mWorld);
			pEffect->SetMatrix("worldMatrix", mWorld);

			LTMatrix mViewProj;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_VIEWPROJECTION, true, &mViewProj);
			pEffect->SetMatrix("viewProjMatrix", mViewProj);

			//g_pLTCRenderMgr->SnapshotCurrentFrame();
			//g_pLTCRenderMgr->UploadCurrentFrameToEffect(pEffect, "g_txSrcTexture");


			pEffect->SetTextureRT("g_txSrcTexture", (HRENDERTARGET)1);
		}
		break;
		
	case EFFECTSHADER_WORLD_BUMPREFLECT: // BUMP REFLECT WORLD
		{
			//WORLD
			LTMatrix mWorld;

			if(hObj) // This is a model
			{
				pEffect->SetTechnique("BumpReflect");

				ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_WORLD0, true, &mWorld);
				pEffect->SetMatrix("worldMatrix", mWorld);

			}
			else // This is world geometry
			{
				pEffect->SetTechnique("BumpReflect");

				ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_WORLD0, true, &mWorld);
				pEffect->SetMatrix("worldMatrix", mWorld);
			}

			//VIEW*PROJ
			LTMatrix mViewProj;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_VIEWPROJECTION, true, &mViewProj);
			pEffect->SetMatrix("viewProjMatrix", mViewProj);

			if(!m_hCamera)
			{
				g_pLTClient->CPrint("Can not obtain the camera! Erroring out of effect callback!");
				return;
			}

			LTVector vPos;
			g_pLTClient->GetObjectPos(m_hCamera, &vPos);
			float pos[4] = {vPos.x, vPos.y, vPos.z, 1.0f};	
			pEffect->SetVector("eye_position", &*(float*)pos);	

			HCONSOLEVAR hVar = g_pLTClient->GetConsoleVar("saturate");
			if (hVar)
			{
				float saturate = g_pLTClient->GetVarValueFloat(hVar);
				if(saturate == 1.0)
				{
					pEffect->SetFloat("saturate", 2.0f);
				}
				else
				{
					pEffect->SetFloat("saturate", 1.0f);
				}
			}

		}break;
	case EFFECTSHADER_MODEL_SKEL_3WEIGHTS: // Skeletal Model with 3 blend weights
		{

			LTMatrix mWorld;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_WORLD0, true, &mWorld);
			pEffect->SetMatrix("worldMatrix", mWorld);

			LTMatrix mWorld1;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_WORLD1, true, &mWorld1);
			pEffect->SetMatrix("worldMatrix1", mWorld1);

			LTMatrix mWorld2;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_WORLD2, true, &mWorld2);
			pEffect->SetMatrix("worldMatrix2", mWorld2);

			LTMatrix mWorld3;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_WORLD3, true, &mWorld3);
			pEffect->SetMatrix("worldMatrix3", mWorld3);

			LTMatrix mViewProj;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_VIEWPROJECTION, true, &mViewProj);
			pEffect->SetMatrix("viewProjMatrix", mViewProj);
		}
		break;
	case EFFECTSHADER_MODEL_RIGID:
	case EFFECTSHADER_MODEL_VERTEXANIMATED:
		{
			LTMatrix mWorld;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_WORLD0, true, &mWorld);
			pEffect->SetMatrix("worldMatrix", mWorld);

			LTMatrix mViewProj;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_VIEWPROJECTION, true, &mViewProj);
			pEffect->SetMatrix("viewProjMatrix", mViewProj);
		}
		break;
	case EFFECTSHADER_MODEL_SKEL_1WEIGHT: // Skeletal Model with 1 blend weight
	case EFFECTSHADER_MODEL_SKEL_1WEIGHT_BASIS:
		{
			LTMatrix mWorld;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_WORLD0, true, &mWorld);
			pEffect->SetMatrix("worldMatrix", mWorld);

			LTMatrix mWorld1;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_WORLD1, true, &mWorld1);
			pEffect->SetMatrix("worldMatrix1", mWorld1);

			LTMatrix mViewProj;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_VIEWPROJECTION, true, &mViewProj);
			pEffect->SetMatrix("viewProjMatrix", mViewProj);
		}
		break;
	case EFFECTSHADER_WORLD_PARALLAXMAP: // Parallax World
		{
			LTMatrix mWorld;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_WORLD0, true, &mWorld);

			LTMatrix mViewProj;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_VIEWPROJECTION, true, &mViewProj);

			LTMatrix mView;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_VIEW, true, &mView);

			pEffect->SetMatrix("worldMatrix", mWorld);
			pEffect->SetMatrix("viewProjMatrix", mViewProj);

			LTMatrix mModelViewProj = mWorld * mViewProj;
			pEffect->SetMatrix("modelviewproj", mModelViewProj);

			LTMatrix mModelView = mWorld * mView;
			pEffect->SetMatrix("modelview", mModelView);

			pEffect->SetMatrix("view", mView);

			if(hObj)
			{
				mView.Inverse();
				pEffect->SetMatrix("invViewMatrix", mView);

				if(m_hCamera)
				{
					
					LTVector vPos;
					g_pLTClient->GetObjectPos(m_hCamera, &vPos);

					float pos[4] = {vPos.x, vPos.y, vPos.z, 1.0f};	
					pEffect->SetVector("campos", &*(float*)pos);	
				}
			}

			if(m_hGameLight)
			{
				LTVector vPos;
				g_pLTClient->GetObjectPos(m_hGameLight, &vPos);
				float pos[4] = {vPos.x, vPos.y, vPos.z, 1.0f};		
				pEffect->SetVector("lightpos", &*(float*)pos);	
			}else // We don't have a game light yet, so let's use the camera
			{
				if(m_hCamera)
				{
					LTVector vPos;
					g_pLTClient->GetObjectPos(m_hCamera, &vPos);
					float pos[4] = {vPos.x, vPos.y, vPos.z, 1.0f};	
					pEffect->SetVector("lightpos", &*(float*)pos);	
				}
			}

		}
		break;
	case EFFECTSHADER_WORLD_REFRACT: // refraction shader World
	case EFFECTSHADER_POLYGRID_REFRACT: // refraction shader PolyGrid
	case EFFECTSHADER_SPRITE_REFRACT: // refraction shader Sprite
		{
			LTMatrix mWorld;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_WORLD0, false, &mWorld);

			LTMatrix mViewProj;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_VIEWPROJECTION, false, &mViewProj);

			LTMatrix mView;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_VIEW, false, &mView);

			pEffect->SetMatrixTranspose("worldMatrix", mWorld);
			pEffect->SetMatrixTranspose("viewProjMatrix", mViewProj);

			LTMatrix mModelViewProj = mWorld * mViewProj;
			pEffect->SetMatrixTranspose("modelviewproj", mModelViewProj);

			LTMatrix mModelView = mWorld * mView;
			pEffect->SetMatrixTranspose("modelview", mModelView);

			pEffect->SetMatrixTranspose("view", mView);

			// This can be expensive if used too often each frame as it copies the current frame buffer
			g_pLTCRenderMgr->SnapshotCurrentFrame();

			g_pLTCRenderMgr->UploadCurrentFrameToEffect(pEffect, "backbuffer");

			if(m_hCamera)
			{
				LTVector vPos;
				g_pLTClient->GetObjectPos(m_hCamera, &vPos);
				float pos[4] = {vPos.x, vPos.y, vPos.z, 1.0f};	
				pEffect->SetVector("eyepos", &*(float*)pos);	
			}
		}
		break;
	case EFFECTSHADER_MODEL_SKEL_3WEIGHTS_PARALLAXMAP: // Skeletal Model with 3 blend weights PARALLAX
		{

			LTMatrix mWorld;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_WORLD0, true, &mWorld);
			pEffect->SetMatrix("worldMatrix", mWorld);

			LTMatrix mWorld1;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_WORLD1, true, &mWorld1);
			pEffect->SetMatrix("worldMatrix1", mWorld1);

			LTMatrix mWorld2;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_WORLD2, true, &mWorld2);
			pEffect->SetMatrix("worldMatrix2", mWorld2);

			LTMatrix mWorld3;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_WORLD3, true, &mWorld3);
			pEffect->SetMatrix("worldMatrix3", mWorld3);

			LTMatrix mViewProj;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_VIEWPROJECTION, true, &mViewProj);
			pEffect->SetMatrix("viewProjMatrix", mViewProj);

			LTMatrix mView;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_VIEW, true, &mView);

			LTMatrix mModelViewProj = mWorld * mViewProj;
			pEffect->SetMatrix("modelviewproj", mModelViewProj);

			LTMatrix mModelView = mWorld * mView;
			pEffect->SetMatrix("modelview", mModelView);

			pEffect->SetMatrix("view", mView);

			if(m_hGameLight)
			{
				LTVector vPos;
				g_pLTClient->GetObjectPos(m_hGameLight, &vPos);
				float pos[4] = {vPos.x, vPos.y, vPos.z, 1.0f};	
				pEffect->SetVector("lightpos", &*(float*)pos);	
			}else // We don't have a game light yet, so let's use the camera
			{
				if(m_hCamera)
				{
					LTVector vPos;
					g_pLTClient->GetObjectPos(m_hCamera, &vPos);
					float pos[4] = {vPos.x, vPos.y, vPos.z, 1.0f};	
					pEffect->SetVector("lightpos", &*(float*)pos);	
				}
			}

			if(hObj)
			{
				LTRotation rRot;
				g_pLTClient->GetObjectRotation(hObj, &rRot);
				LTMatrix mRotMat;
				rRot.ConvertToMatrix(mRotMat);
				pEffect->SetMatrix("modelrotation", mRotMat);
			}
		}
		break;
	case EFFECTSHADER_RIGIDMODEL_BASIS :
		{
			LTMatrix mWorld;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_WORLD0, true, &mWorld);
			pEffect->SetMatrix("worldMatrix", mWorld);

			LTMatrix mViewProj;
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::LTMatrixType::MATRIXTYPE_VIEWPROJECTION, true, &mViewProj);
			pEffect->SetMatrix("viewProjMatrix", mViewProj);
		}
		break;
	default:
		{
			g_pLTClient->CPrint("Unsupported EffectShaderID %d.", pEffect->GetID());
		}
		break;
	}
}

void CShaderMgr::OnUpdate(float fFrameTime, bool bRendering)
{
	if(bRendering)
	{
		LTEffectShader* pEffect = g_pLTCRenderMgr->GetEffectShader(EFFECTSHADER_SHAREDGLOBALS);
		if(pEffect)
		{
			HCONSOLEVAR hVar = g_pLTClient->GetConsoleVar("saturate");
			if (hVar)
			{
				float saturate = g_pLTClient->GetVarValueFloat(hVar);
				if(saturate == 1.0)
				{
					pEffect->SetFloat("fSaturate", 2.0f);
				}
				else
				{
					pEffect->SetFloat("fSaturate", 1.0f);
				}
			}
		}
	}
}

void CShaderMgr::OnRender(float fFrameTime)
{
	switch(m_eMode)
	{
	case SHADERMGR_MODE_DEFAULT:
		{
			RenderNormal(fFrameTime);
		}
		break;
	case SHADERMGR_MODE_FULLSCREEN_BLACKANDWHITE:
		{
			RenderFullscreenEffect(fFrameTime, EFFECTSHADER_BLACKANDWHITE);
		}
		break;
	case SHADERMGR_MODE_FULLSCREEN_SEPIATONE:
		{
			RenderFullscreenEffect(fFrameTime, EFFECTSHADER_SEPIA);
		}
		break;
	case SHADERMGR_MODE_FULLSCREEN_SOBELEDGE:
		{
			RenderFullscreenEffect(fFrameTime, EFFECTSHADER_SOBELEDGE);
		}
		break;

	case SHADERMGR_MODE_FULLSCREEN_INVERSE:
		{
			RenderFullscreenEffect(fFrameTime, EFFECTSHADER_INVERSE);
		}
		break;

	default:
		break;
	}
}

LTRESULT CShaderMgr::RenderNormal(float fFrameTime)
{
	//	Tell LithTech to render the scene using this camera
	LTRESULT result = g_pLTClient->RenderCamera(m_hCamera, fFrameTime);
	if (LT_OK != result)
	{
		g_pLTClient->DebugOut("Error in RenderCamera()\n");
		g_pLTClient->Shutdown();
	}

	return result;
}

// This function will draw the scene to a render target, then pass that render target as an Effect parameter.
// DrawPrim will render 2 screen-aligned polies.
LTRESULT CShaderMgr::RenderFullscreenEffect(float fFrameTime, uint32 nID)
{
	g_pLTCRenderMgr->StoreDefaultRenderTarget();

	if(g_pLTCRenderMgr->InstallRenderTarget(RENDERTARGET_OVERLAY) != LT_OK)
	{
		g_pLTClient->CPrint("Failed to InstallRenderTarget");
	}

	//	Tell LithTech to render the scene using this camera
	LTRESULT result = g_pLTClient->RenderCamera(m_hCamera, fFrameTime);
	if (LT_OK != result)
	{
		g_pLTClient->DebugOut("Error in RenderCamera()\n");
		g_pLTClient->Shutdown();
	}

	g_pLTCRenderMgr->RestoreDefaultRenderTarget();

	if(true)
	{
		float top 		= 0.0f - 0.05f; //0.05 needed to correct the offset
		float bottom 	= (float)g_nScreenHeight;
		float left 		= 0.0f - 0.05f; //0.05 needed to correct the offset
		float right 	= (float)g_nScreenWidth;

		LT_POLYFT4 poly;
		poly.verts[0].x = left;
		poly.verts[0].y = top;
		poly.verts[0].z = 0.0f;
		poly.verts[0].u = 0.0f;
		poly.verts[0].v = 0.0f;

		poly.verts[1].x = right;
		poly.verts[1].y = top;
		poly.verts[1].z = 0.0f;
		poly.verts[1].u = 1.0f;
		poly.verts[1].v = 0.0f;

		poly.verts[2].x = right;
		poly.verts[2].y = bottom;
		poly.verts[2].z = 0.0f;
		poly.verts[2].u = 1.0f;
		poly.verts[2].v = 1.0f;

		poly.verts[3].x = left;
		poly.verts[3].y = bottom;
		poly.verts[3].z = 0.0f;
		poly.verts[3].u = 0.0f;
		poly.verts[3].v = 1.0f;

		// Set up color and alpha.
		poly.rgba.r = 255;
		poly.rgba.g = 255;
		poly.rgba.b = 255;
		poly.rgba.a = 255;

		// Set up the drawprim render state.
		g_pLTCDrawPrim->SetTransformType(DRAWPRIM_TRANSFORM_SCREEN);
		g_pLTCDrawPrim->SetColorOp(DRAWPRIM_MODULATE);
		g_pLTCDrawPrim->SetAlphaBlendMode(DRAWPRIM_NOBLEND);
		g_pLTCDrawPrim->SetZBufferMode(DRAWPRIM_NOZ);
		g_pLTCDrawPrim->SetAlphaTestMode(DRAWPRIM_NOALPHATEST);
		g_pLTCDrawPrim->SetClipMode(DRAWPRIM_FASTCLIP);
		g_pLTCDrawPrim->SetFillMode(DRAWPRIM_FILL);
		g_pLTCDrawPrim->SetCullMode(DRAWPRIM_CULL_NONE);
		g_pLTCDrawPrim->SetCamera(m_hCamera);
		g_pLTCDrawPrim->SetEffectShaderID(nID);

		// Draw the Image (just 1 quad).
		g_pLTCDrawPrim->DrawPrim(&poly, 1);
	}

	return result;
}