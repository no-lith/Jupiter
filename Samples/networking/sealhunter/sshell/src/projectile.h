//------------------------------------------------------------------------------//
//
// MODULE   : Projectile.h
//
// PURPOSE  : Projectile - Definition
//
// CREATED  : 05/14/2002
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#ifndef __Projectile_H__
#define __Projectile_H__


// Engine includes
#include <ltbasedefs.h>
#include <ltengineobjects.h>


//-----------------------------------------------------------------------------
class Projectile : public BaseClass
{

public:

    Projectile()
		: m_hOwner(NULL)
	{
	}

    ~Projectile()
	{
	}

    // EngineMessageFn handlers
    uint32			EngineMessageFn(uint32 messageID, void *pData, float fData);
    HOBJECT 		GetOwner()															{ return m_hOwner; }
    void    		SetOwner(HOBJECT &hOwner)											{ m_hOwner = hOwner; }

private:

    uint32			PreCreate(void *pData, float fData);
    uint32      	InitialUpdate(void *pData, float fData);
    uint32      	Update(void *pData, float fData);
    uint32      	TouchNotify(void *pData, float fData);
    void			ReadProps(ObjectCreateStruct* pStruct);

private:

    HOBJECT     m_hOwner;
};


#endif // __Projectile_H__
