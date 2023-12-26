//------------------------------------------------------------------------------//
//
// MODULE   : WorldPropsClient.h
//
// PURPOSE  : CWorldPropsClnt - Definition and constants
//
// CREATED  : 02/14/2001
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//


#ifndef __WORLDPROPS_CLNT_H__
#define __WORLDPROPS_CLNT_H__


#include <ltbasedefs.h>
#include <ilttexinterface.h>
#include <iltmessage.h>


//-----------------------------------------------------------------------------
class CWorldPropsClnt
{
public:

	CWorldPropsClnt();
	virtual ~CWorldPropsClnt();

	LTRGB			GetBackgroundColor();
	void			UnpackWorldProps(ILTMessage_Read *pMsgProps);
	void			Update();

private:

	void 			ApplyWorldProps();

private:

	// Members

	float			m_fSkyPanCurXOffset;
	float			m_fSkyPanCurZOffset;
	HTEXTURE		m_hSkyPan;
	uint32			m_nSkyPanTexW;
	uint32			m_nSkyPanTexH;

	int32			m_nFarZ;
	LTVector		m_vBackgroundColor;

	bool			m_bFogEnable;
	LTVector		m_vFogColor;
	uint32			m_nFogNearZ;
	uint32			m_nFogFarZ;

	bool			m_bSkyFogEnable;
	uint32			m_nSkyFogNearZ;
	uint32			m_nSkyFogFarZ;

	bool			m_bSkyPanEnable;
	char			m_szSkyPanTexture[256];
	int32			m_nSkyPanAutoPanX;
	int32			m_nSkyPanAutoPanZ;
	float			m_fSkyPanScaleX;
	float			m_fSkyPanScaleZ;

	float			m_fSkyScale;
};


#endif	// __WORLDPROPS_CLNT_H__
