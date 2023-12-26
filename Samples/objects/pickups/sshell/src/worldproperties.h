//------------------------------------------------------------------------------//
// 
// MODULE	: WorldProperties.h
// 
// PURPOSE	: CWorldProperties - Definition
// 
// CREATED	: 01/14/2002
// 
// (c) 2002 LithTech, Inc.	All Rights Reserved
// 
//------------------------------------------------------------------------------//

#ifndef __WORLDPROPERTIES_H__
#define __WORLDPROPERTIES_H__


// Engine includes
#include <ltbasedefs.h>
#include <ltengineobjects.h>


//-----------------------------------------------------------------------------
class WorldProperties : public BaseClass
{
	
public:
	
	// Construction / destruction
	WorldProperties();
	virtual ~WorldProperties();
	
	uint32		EngineMessageFn(uint32 messageID, void *pData, float fData);
	
	void		SendToClient(HCLIENT hClient);
	
private:
	// EngineMessageFn handlers
	
	uint32		PreCreate(void *pData, float fData);
	void		ObjectCreated();
	void		Update();
	
	// Other functions
	
	void		ReadProps(ObjectCreateStruct* pStruct);
	void		PostReadProps();
	
	// Members
	
	bool		m_bFirstUpdate;
	
	uint32		m_nFarZ;
	LTVector	m_vBackgroundColor;
	bool		m_bFogEnable;
	LTVector	m_vFogColor;
	uint32		m_nFogNearZ;
	uint32		m_nFogFarZ;
	
	bool		m_bSkyFogEnable;
	uint32		m_nSkyFogNearZ;
	uint32		m_nSkyFogFarZ;
	
	bool		m_bSkyPanEnable;
	char		m_szSkyPanTexture[256];
	int32		m_nSkyPanAutoPanX;
	int32		m_nSkyPanAutoPanZ;
	float		m_fSkyPanScaleX;
	float		m_fSkyPanScaleZ;
	float		m_fSkyScale;
};


#endif	// __WORLDPROPERTIES_H__
