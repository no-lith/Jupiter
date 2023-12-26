//------------------------------------------------------------------------------//
// 
// MODULE   : GameLight.h
// 
// PURPOSE  : GameLight - Definition
// 
// CREATED  : 12/2/2005
// 
// (c) 2006 LithTech, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//

#ifndef __GameLight_H__
#define __GameLight_H__


// Engine includes
#include <ltbasedefs.h>
#include <ltengineobjects.h>
#include <ltobjectcreate.h>
#include <iobjectplugin.h>

//-----------------------------------------------------------------------------
class GameLight : public BaseClass
{
    
public:
    
    // Construction / destruction
    GameLight();
    virtual ~GameLight() {}
    
    // EngineMessageFn handlers
    uint32	EngineMessageFn(uint32 messageID, void *pData, float fData);
    
protected:
    
    uint32		PreCreate(void *pData, float fData);
	uint32		InitialUpdate();
    uint32      Update();
    void		ReadProps(ObjectCreateStruct* pStruct);

	HOBJECT		m_hTarget;
	LTRotation	m_rRot;
	float		m_fDegreesPerSecond;
	float		m_fDistanceFromOrigin;

	HOBJECT		m_hLight;
};


#endif // __Prop_H__
