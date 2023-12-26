//------------------------------------------------------------------------------//
//
// MODULE   : Trigger.h
//
// PURPOSE  : Trigger - Definition
//
// CREATED  : 10/23/2002
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#ifndef __TRIGGER_H__
#define __TRIGGER_H__


// Engine includes
#include <ltbasedefs.h>
#include <ltengineobjects.h>
//#include "stdafx.h"


//-----------------------------------------------------------------------------
class Trigger : public BaseClass
{
public:

    Trigger()
		: m_hTarget(NULL)
	{
	}

	virtual ~Trigger()
	{
	}

	// EngineMessageFn handlers
	uint32			EngineMessageFn(uint32 messageID, void *pData, float fData);

private:

	uint32			PreCreate(void *pData, float fData);
	uint32			TouchNotify(void *pData, float fData);
	void			ReadProps(ObjectCreateStruct* pStruct);
    void        	PingTarget();

private:

    HOBJECT     	m_hTarget;
    char        	m_sTarget[32];
};


#endif // __Trigger_H__
