//------------------------------------------------------------------------------//
//
// MODULE   : AIVolume.h
//
// PURPOSE  : AIVolume - Definition
//
// CREATED  : 10/23/2002
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#ifndef __AIVolume_H__
#define __AIVolume_H__


// Engine includes
#include <ltbasedefs.h>
#include <ltengineobjects.h>


//-----------------------------------------------------------------------------
class AIVolume : public BaseClass
{

public:

    AIVolume()
		: m_hTarget(NULL),
		  m_NumSeals(0)
	{
	}

	~AIVolume()
	{
	}

	// EngineMessageFn handlers
	uint32		EngineMessageFn(uint32 messageID, void *pData, float fData);

	void		DecrementSealCount()		{ --m_NumSeals; }

private:

	uint32		PreCreate(void *pData, float fData);
	uint32		TouchNotify(void *pData, float fData);
	void		ReadProps(ObjectCreateStruct* pStruct);
    void        PingTarget();
    void        Spawn();

private:

    HOBJECT     m_hTarget;
    char        m_sTarget[32];
    LTVector    m_vDims;
	int			m_NumSeals;
};


#endif // __AIVolume_H__
