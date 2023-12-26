//------------------------------------------------------------------------------//
//
// MODULE   : ShaderMgr.h
//
// PURPOSE  : Loads and sets vertex and pixel shaders
//
// CREATED  : 09/17/2003
//
// (c) 2003 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#ifndef __SHADERMGR_H__
#define __SHADERMGR_H__


#include <iclientshell.h>
#include <ltbasetypes.h>
#include <ltbasedefs.h>



enum EShaderType
{
	SHADERTYPE_FIXEDFUNCTION = 0,	// fixed function pipeline (no shader)
	SHADERTYPE_TOON,				// toon shader
	SHADERTYPE_MAX_SHADER_TYPES
};



class ShaderMgr
{
public:

	~ShaderMgr();

	// singleton access
	static ShaderMgr&		GetSingleton();

	// initialize
	LTRESULT				Init();

	// get and set the shader type
	unsigned				GetCurrentShader() const								{ return m_CurrentShader; }
	void					PrevShader();
	void					NextShader();

	// set the vertex and pixel shader constants
	void 					SetConstants(LTVertexShader *pVertexShader, unsigned iPass,
								 		 CRenderStyle *pRenderStyle, HOBJECT hObj,
										 const LTShaderDeviceState &ShaderDeviceState);
	void 					SetConstants(LTPixelShader *pPixelShader, unsigned iPass,
								 		 CRenderStyle *pRenderStyle, HOBJECT hObj,
								 		 const LTShaderDeviceState &ShaderDeviceState);

	// update
	LTRESULT        		Update();

private:

	ShaderMgr();

	LTRESULT				InitVertexShaders();
	LTRESULT				InitPixelShaders();

	LTRESULT				InitShaderVariables();
	LTRESULT				UpdateShaderVariables();

private:

	unsigned				m_CurrentShader;

	LTVector				m_StartColor;
	LTVector				m_EndColor;
	LTVector				m_CurrentColor;
	float					m_ColorTime;

	LTMatrix 				m_WorldMatrix0;
	LTMatrix 				m_WorldMatrix1;
	LTMatrix 				m_WorldMatrix2;
	LTMatrix 				m_WorldMatrix3;
	LTMatrix 				m_InvWorldMatrix0;
	LTMatrix 				m_InvWorldMatrix1;
	LTMatrix 				m_InvWorldMatrix2;
	LTMatrix 				m_InvWorldMatrix3;
	LTMatrix 				m_ViewProjMatrix;

	LTVector				m_LightDir;
	LTVector				m_ObjectLightDir;
	LTRotation				m_LightRot;
};


// global
#define g_ShaderMgr			ShaderMgr::GetSingleton()


#endif // __SHADERMGR_H__

