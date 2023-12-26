//------------------------------------------------------------------------------//
//
// MODULE   : ShaderMgr.cpp
//
// PURPOSE  : Loads and sets vertex and pixel shaders
//
// CREATED  : 09/17/2003
//
// (c) 2003 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#include "ShaderMgr.h"
#include "clientinterfaces.h"
#include "clienthelper.h"

#include <iltclient.h>
#include <time.h>
#include <d3d9.h>



//-------------------------------------------------------------------------------------------------
// Constants
//-------------------------------------------------------------------------------------------------

// Vertex shaders ids WARNING! The order of these match the settings in the renderstyles!
enum EVertexShaders
{
	// toon shaders
	VERTEXSHADER_SKEL0_TOONPASS1		= 1,
	VERTEXSHADER_SKEL0_TOONPASS2		= 2,
	VERTEXSHADER_SKEL1_TOONPASS1		= 3,
	VERTEXSHADER_SKEL1_TOONPASS2		= 4,
	VERTEXSHADER_SKEL2_TOONPASS1		= 5,
	VERTEXSHADER_SKEL2_TOONPASS2		= 6,
	VERTEXSHADER_SKEL3_TOONPASS1		= 7,
	VERTEXSHADER_SKEL3_TOONPASS2		= 8,
};

// Pixel shader ids: WARNING! The order of these match the settings in the renderstyles!
enum EPixelShaders
{
	PIXELSHADER_SIMPLE					= 1,
};



//-------------------------------------------------------------------------------------------------
// ShaderMgr
//-------------------------------------------------------------------------------------------------


//---------------------------------------------------------------------------
//  Name:         ShaderMgr
//
//  Purpose:	  constructor
//
//  Returns:      n/a
//---------------------------------------------------------------------------
ShaderMgr::ShaderMgr()
	: m_CurrentShader(SHADERTYPE_FIXEDFUNCTION),
	  m_ColorTime(0.0f),
	  m_LightDir(0.0f, 0.0f, -1.0f)
{
}



//---------------------------------------------------------------------------
//  Name:         ~ShaderMgr
//
//  Purpose:	  destructor
//
//  Returns:      n/a
//---------------------------------------------------------------------------
ShaderMgr::~ShaderMgr()
{
}



//---------------------------------------------------------------------------
//  Name:         Init
//
//  Purpose:	  Load the shaders and initialize the variables.
//
//  Returns:      LTRESULT
//---------------------------------------------------------------------------
LTRESULT ShaderMgr::Init()
{
	LTRESULT result = LT_OK;

	// Initialize the vertex shaders.
	result = InitVertexShaders();
	DebugPrintIfFailed(result, "Error in InitVertexShaders().\n");

	// Initialize the pixel shaders.
	result = InitPixelShaders();
	DebugPrintIfFailed(result, "Error in InitPixelShaders().\n");

	// Initialize the shaders variables.
	result = InitShaderVariables();
	DebugPrintIfFailed(result, "Error in InitShaderVariables().\n");

	return result;
}




//---------------------------------------------------------------------------
//  Name:         GetSingleton
//
//  Purpose:	  ShaderMgr is a singleton
//
//  Returns:	  ShaderMgr&
//
//---------------------------------------------------------------------------
ShaderMgr& ShaderMgr::GetSingleton()
{
	static ShaderMgr s_Singleton;
	return s_Singleton;
}



//---------------------------------------------------------------------------
//  Name:         InitVertexShaders
//
//  Purpose:	  Loads the vertex shaders.
//
//  Returns:	  LTRESULT
//
//---------------------------------------------------------------------------
LTRESULT ShaderMgr::InitVertexShaders()
{
	LTRESULT result = LT_OK;

	//
	// Vertex shader input declarations
	//

	// position + normal + texture
    D3DVERTEXELEMENT9 Decl_PosNormTex[] =
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
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

	// position + blend weight(2) + normal + texture
    D3DVERTEXELEMENT9 Decl_PosWeight2NormTex[] =
    {
        { 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
        { 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_BLENDWEIGHT, 0 },
        { 0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
        { 0, 32, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
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

	//
	// Vertex shaders
	//

	// skel0 toon shader
	if (!g_pLTClient->AddVertexShader("VertexShaders\\WinSkel0ToonPass1.vsh", VERTEXSHADER_SKEL0_TOONPASS1,
									  (const uint32*)Decl_PosNormTex, sizeof(Decl_PosNormTex), true))
	{
		g_pLTClient->DebugOut("Error loading WinSkel0ToonPass1.vsh.\n");
	}
	if (!g_pLTClient->AddVertexShader("VertexShaders\\WinSkel0ToonPass2.vsh", VERTEXSHADER_SKEL0_TOONPASS2,
									  (const uint32*)Decl_PosNormTex, sizeof(Decl_PosNormTex), true))
	{
		g_pLTClient->DebugOut("Error loading WinSkel0ToonPass2.vsh.\n");
	}

	// skel1 toon shader
	if (!g_pLTClient->AddVertexShader("VertexShaders\\WinSkel1ToonPass1.vsh", VERTEXSHADER_SKEL1_TOONPASS1,
									  (const uint32*)Decl_PosWeight1NormTex, sizeof(Decl_PosWeight1NormTex), true))
	{
		g_pLTClient->DebugOut("Error loading WinSkel1ToonPass1.vsh.\n");
	}
	if (!g_pLTClient->AddVertexShader("VertexShaders\\WinSkel1ToonPass2.vsh", VERTEXSHADER_SKEL1_TOONPASS2,
									  (const uint32*)Decl_PosWeight1NormTex, sizeof(Decl_PosWeight1NormTex), true))
	{
		g_pLTClient->DebugOut("Error loading WinSkel1ToonPass2.vsh.\n");
	}

	// skel2 toon shader
	if (!g_pLTClient->AddVertexShader("VertexShaders\\WinSkel2ToonPass1.vsh", VERTEXSHADER_SKEL2_TOONPASS1,
									  (const uint32*)Decl_PosWeight2NormTex, sizeof(Decl_PosWeight2NormTex), true))
	{
		g_pLTClient->DebugOut("Error loading WinSkel2ToonPass1.vsh.\n");
	}
	if (!g_pLTClient->AddVertexShader("VertexShaders\\WinSkel2ToonPass2.vsh", VERTEXSHADER_SKEL2_TOONPASS2,
									  (const uint32*)Decl_PosWeight2NormTex, sizeof(Decl_PosWeight2NormTex), true))
	{
		g_pLTClient->DebugOut("Error loading WinSkel2ToonPass2.vsh.\n");
	}

	// skel3 toon shader
	if (!g_pLTClient->AddVertexShader("VertexShaders\\WinSkel3ToonPass1.vsh", VERTEXSHADER_SKEL3_TOONPASS1,
									  (const uint32*)Decl_PosWeight3NormTex, sizeof(Decl_PosWeight3NormTex), true))
	{
		g_pLTClient->DebugOut("Error loading WinSkel3ToonPass1.vsh.\n");
	}
	if (!g_pLTClient->AddVertexShader("VertexShaders\\WinSkel3ToonPass2.vsh", VERTEXSHADER_SKEL3_TOONPASS2,
									  (const uint32*)Decl_PosWeight3NormTex, sizeof(Decl_PosWeight3NormTex), true))
	{
		g_pLTClient->DebugOut("Error loading WinSkel3ToonPass2.vsh.\n");
	}

	return result;
}



//---------------------------------------------------------------------------
//  Name:         InitPixelShaders
//
//  Purpose:	  Loads the pixel shaders.
//
//  Returns:	  LTRESULT
//
//---------------------------------------------------------------------------
LTRESULT ShaderMgr::InitPixelShaders()
{
	// simple
/*
	if (!g_pLTClient->AddPixelShader(PIXELSHADER_SIMPLE_BYTES, sizeof(PIXELSHADER_SIMPLE_BYTES), "Simple", PIXELSHADER_SIMPLE))
	{
		return LT_ERROR;
	}
 */

	return LT_OK;
}



//---------------------------------------------------------------------------
//  Name:         InitShaderVariables
//
//  Purpose:	  Initializes the variables that are passed into the shaders.
//
//  Returns:	  LTRESULT
//
//---------------------------------------------------------------------------
LTRESULT ShaderMgr::InitShaderVariables()
{
	// Set the initial start and end colors.
	m_StartColor 		= LTVector(0.22f, 0.92f, 0.39f);
	m_EndColor			= LTVector(0.91f, 0.5f, 0.11f);
	m_CurrentColor		= m_StartColor;
	m_ColorTime			= 0.0f;

	// Seed the random number generator.
	srand((unsigned)time(NULL));

	m_LightRot.Init();

	return LT_OK;
}



//---------------------------------------------------------------------------
//  Name:         PrevShader
//
//  Purpose:	  decrements the current shader index
//
//  Returns:	  void
//
//---------------------------------------------------------------------------
void ShaderMgr::PrevShader()
{
	int CurrIndex = m_CurrentShader - 1;
	if (CurrIndex < 0)
	{
		CurrIndex = SHADERTYPE_MAX_SHADER_TYPES - 1;
	}
	m_CurrentShader = CurrIndex;
}



//---------------------------------------------------------------------------
//  Name:         NextShader
//
//  Purpose:	  increments the current shader index
//
//  Returns:	  void
//
//---------------------------------------------------------------------------
void ShaderMgr::NextShader()
{
	int CurrIndex = m_CurrentShader + 1;
	if (CurrIndex >= SHADERTYPE_MAX_SHADER_TYPES)
	{
		CurrIndex = 0;
	}
	m_CurrentShader = CurrIndex;
}



//---------------------------------------------------------------------------
//  Name:         SetConstants
//
//  Purpose:	  Updates the vertex shader constants for the given shader
//
//  Returns:	  LTRESULT
//
//---------------------------------------------------------------------------
void ShaderMgr::SetConstants(LTVertexShader *pVertexShader, unsigned iPass, CRenderStyle *pRenderStyle, HOBJECT hObj,
							 const LTShaderDeviceState &ShaderDeviceState)
{
	const unsigned regWorld0 		= 0;
	const unsigned regWorld1 		= 4;
	const unsigned regWorld2 		= 8;
	const unsigned regWorld3 		= 12;

	const unsigned regViewProj 		= 16;

	const unsigned regConstZero		= 20;
	const unsigned regConstOne		= 21;

	const unsigned regInvWorld0 	= 30;
	const unsigned regInvWorld1 	= 34;
	const unsigned regInvWorld2 	= 38;
	const unsigned regInvWorld3 	= 42;

	const unsigned regLightDir		= 50;
	const unsigned regDiffuse		= 51;

	const float ToonScaleFactor		= 1.025f;

	// Get the world and view projection matrices.
	ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_VIEWPROJECTION, true, &m_ViewProjMatrix);
	switch (pVertexShader->GetID())
	{
		case VERTEXSHADER_SKEL0_TOONPASS1:
		{
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD0, true, &m_WorldMatrix0);
			break;
		}
		case VERTEXSHADER_SKEL0_TOONPASS2:
		{
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD0, true, &m_WorldMatrix0);
			m_WorldMatrix0.Scale(ToonScaleFactor, ToonScaleFactor, ToonScaleFactor);
			break;
		}

		case VERTEXSHADER_SKEL1_TOONPASS1:
		{
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD0, true, &m_WorldMatrix0);
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD1, true, &m_WorldMatrix1);
			break;
		}
		case VERTEXSHADER_SKEL1_TOONPASS2:
		{
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD0, true, &m_WorldMatrix0);
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD1, true, &m_WorldMatrix1);
			m_WorldMatrix0.Scale(ToonScaleFactor, ToonScaleFactor, ToonScaleFactor);
			m_WorldMatrix1.Scale(ToonScaleFactor, ToonScaleFactor, ToonScaleFactor);
			break;
		}

		case VERTEXSHADER_SKEL2_TOONPASS1:
		{
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD0, true, &m_WorldMatrix0);
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD1, true, &m_WorldMatrix1);
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD2, true, &m_WorldMatrix2);
			break;
		}
		case VERTEXSHADER_SKEL2_TOONPASS2:
		{
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD0, true, &m_WorldMatrix0);
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD1, true, &m_WorldMatrix1);
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD2, true, &m_WorldMatrix2);
			m_WorldMatrix0.Scale(ToonScaleFactor, ToonScaleFactor, ToonScaleFactor);
			m_WorldMatrix1.Scale(ToonScaleFactor, ToonScaleFactor, ToonScaleFactor);
			m_WorldMatrix2.Scale(ToonScaleFactor, ToonScaleFactor, ToonScaleFactor);
			break;
		}

		case VERTEXSHADER_SKEL3_TOONPASS1:
		{
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD0, true, &m_WorldMatrix0);
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD1, true, &m_WorldMatrix1);
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD2, true, &m_WorldMatrix2);
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD3, true, &m_WorldMatrix3);
			break;
		}
		case VERTEXSHADER_SKEL3_TOONPASS2:
		{
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD0, true, &m_WorldMatrix0);
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD1, true, &m_WorldMatrix1);
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD2, true, &m_WorldMatrix2);
			ShaderDeviceState.GetMatrix(LTShaderDeviceState::MATRIXTYPE_WORLD3, true, &m_WorldMatrix3);
			m_WorldMatrix0.Scale(ToonScaleFactor, ToonScaleFactor, ToonScaleFactor);
			m_WorldMatrix1.Scale(ToonScaleFactor, ToonScaleFactor, ToonScaleFactor);
			m_WorldMatrix2.Scale(ToonScaleFactor, ToonScaleFactor, ToonScaleFactor);
			m_WorldMatrix3.Scale(ToonScaleFactor, ToonScaleFactor, ToonScaleFactor);
			break;
		}
	}

	// Set the constants.
	pVertexShader->SetConstant(regConstZero, 0.0f, 0.0f, 0.0f, 0.0f);
	pVertexShader->SetConstant(regConstOne, 1.0f, 1.0f, 1.0f, 1.0f);
	pVertexShader->SetConstant(regLightDir, m_LightDir.x, m_LightDir.y, m_LightDir.z, 0.0f);
	pVertexShader->SetConstant(regDiffuse, m_CurrentColor.x, m_CurrentColor.y, m_CurrentColor.z, 1.0f);
	switch (pVertexShader->GetID())
	{
		case VERTEXSHADER_SKEL0_TOONPASS1:
		case VERTEXSHADER_SKEL0_TOONPASS2:
		{
			pVertexShader->SetConstant(regWorld0, m_WorldMatrix0);
			pVertexShader->SetConstant(regViewProj, m_ViewProjMatrix);
			break;
		}

		case VERTEXSHADER_SKEL1_TOONPASS1:
		case VERTEXSHADER_SKEL1_TOONPASS2:
		{
			pVertexShader->SetConstant(regWorld0, m_WorldMatrix0);
			pVertexShader->SetConstant(regWorld1, m_WorldMatrix1);
			pVertexShader->SetConstant(regViewProj, m_ViewProjMatrix);
			break;
		}

		case VERTEXSHADER_SKEL2_TOONPASS1:
		case VERTEXSHADER_SKEL2_TOONPASS2:
		{
			pVertexShader->SetConstant(regWorld0, m_WorldMatrix0);
			pVertexShader->SetConstant(regWorld1, m_WorldMatrix1);
			pVertexShader->SetConstant(regWorld2, m_WorldMatrix2);
			pVertexShader->SetConstant(regViewProj, m_ViewProjMatrix);
			break;
		}

		case VERTEXSHADER_SKEL3_TOONPASS1:
		case VERTEXSHADER_SKEL3_TOONPASS2:
		{
			pVertexShader->SetConstant(regWorld0, m_WorldMatrix0);
			pVertexShader->SetConstant(regWorld1, m_WorldMatrix1);
			pVertexShader->SetConstant(regWorld2, m_WorldMatrix2);
			pVertexShader->SetConstant(regWorld3, m_WorldMatrix3);
			pVertexShader->SetConstant(regViewProj, m_ViewProjMatrix);
			break;
		}
	}
}



//---------------------------------------------------------------------------
//  Name:         SetConstants
//
//  Purpose:	  Updates the pixel shader constants for the given shader
//
//  Returns:	  LTRESULT
//
//---------------------------------------------------------------------------
void ShaderMgr::SetConstants(LTPixelShader *pPixelShader, unsigned iPass, CRenderStyle *pRenderStyle, HOBJECT hObj,
							 const LTShaderDeviceState &ShaderDeviceState)
{
/*
	switch (pPixelShader->GetID())
	{
		case PIXELSHADER_SIMPLE:
			break;
	}
 */
}



//---------------------------------------------------------------------------
//  Name:         Update()
//
//  Purpose:	  Updates the variables that are passed into the shaders.
//
//  Returns:	  LTRESULT
//
//---------------------------------------------------------------------------
LTRESULT ShaderMgr::Update()
{
	LTVector ColorDiff = m_EndColor - m_StartColor;

	// If we have reached the end color, then pick a new endpoint.
	const float MaxColorTime = 500.0f;
	if (++m_ColorTime > MaxColorTime)
	{
		m_ColorTime = 0.0f;
		m_StartColor = m_EndColor;

		// Pick a new end point.
		m_EndColor.x = rand() / static_cast<float>(RAND_MAX);
		m_EndColor.y = rand() / static_cast<float>(RAND_MAX);
		m_EndColor.z = rand() / static_cast<float>(RAND_MAX);
	}

	//
	// Update the color along the color vector.
	//

	float Length = ColorDiff.Length();
	float IncDist = Length / MaxColorTime;

	ColorDiff.Normalize();
	ColorDiff *= IncDist * m_ColorTime;

	m_CurrentColor = m_StartColor + ColorDiff;

	// light direction

	LTVector Axis(-1.0f, 1.0f, 0.0f);
	Axis.Normalize();
	m_LightRot.Rotate(Axis, 0.001f);
	m_LightDir = -m_LightRot.Forward();
	m_LightDir.Normalize();

//	m_LightDir.x = 0.0f;
//	m_LightDir.y = 0.0f;
//	m_LightDir.z = -1.0f;

	return LT_OK;
}
