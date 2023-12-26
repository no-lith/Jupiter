#ifndef _SHADER_MGR_H_
#define _SHADER_MGR_H_

#include "ltbasedefs.h"

enum EShaderMgrMode
{
	SHADERMGR_MODE_DEFAULT = 0,
	SHADERMGR_MODE_FULLSCREEN_BLACKANDWHITE,
	SHADERMGR_MODE_FULLSCREEN_SEPIATONE,
	SHADERMGR_MODE_FULLSCREEN_SOBELEDGE,
	SHADERMGR_MODE_FULLSCREEN_INVERSE,
};

class CRenderStyle;
class CShaderMgr
{
public:
	CShaderMgr();
	~CShaderMgr();
	LTRESULT Init();
	LTRESULT Term();
	void OnEffectShaderSetParams(LTEffectShader *pEffect, CRenderStyle *pRenderStyle, HOBJECT hObj, const LTShaderDeviceState &ShaderDeviceState);
	void OnUpdate(float fFrameTime, bool bRendering);
	void OnRender(float fFrameTime);
	void SetMode(EShaderMgrMode eMode){m_eMode = eMode;}

	void SetCamera(HOBJECT hCamera){m_hCamera = hCamera;}
	void SetGameLight(HOBJECT hGameLight){m_hGameLight = hGameLight;}

protected:

	LTRESULT QueryDeviceCaps();
	LTRESULT AddEffectShader (const char *pFileName, int EffectShaderID, const uint32 *pVertexElements, uint32 VertexElementsSize, HEFFECTPOOL EffectPoolID);

	LTRESULT RenderNormal(float fFrameTime);
	LTRESULT RenderFullscreenEffect(float fFrameTime, uint32 nID);

	HOBJECT	m_hCamera;
	HOBJECT m_hGameLight;
	EShaderMgrMode m_eMode;
};

#endif