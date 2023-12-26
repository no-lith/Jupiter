//------------------------------------------------------------------------------//
// 
// MODULE   : Prop.h
// 
// PURPOSE  : Prop - Definition
// 
// CREATED  : 05/14/2002
// 
// (c) 2002 LithTech, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//

#ifndef __Prop_H__
#define __Prop_H__


// Engine includes
#include <ltbasedefs.h>
#include <ltengineobjects.h>
#include <ltobjectcreate.h>
#include <iobjectplugin.h>

//-----------------------------------------------------------------------------
class Prop : public BaseClass
{
    
public:
    
    // Construction / destruction
    Prop();
    virtual ~Prop() {}
    
    // EngineMessageFn handlers
    uint32	EngineMessageFn(uint32 messageID, void *pData, float fData);
    
protected:
    
    uint32		PreCreate(void *pData, float fData);
	uint32		InitialUpdate();
    uint32      Update();
    uint32      TouchNotify(void *pData, float fData);
    void		ReadProps(ObjectCreateStruct* pStruct);
    void        PlaySound();

	char		m_szAnimation[32];
	bool		m_bAnimate;

};


#endif // __Prop_H__
