//------------------------------------------------------------------------------//
//
// MODULE   : gamestartpoint.h
//
// PURPOSE  : GameStartPoint - Definition
//
// CREATED  : 05/14/2002
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#ifndef __GAMESTARTPOINT_H__
#define __GAMESTARTPOINT_H__


// Engine includes
#include <ltbasedefs.h>
#include <ltengineobjects.h>
//#include "stdafx.h"


//-----------------------------------------------------------------------------
class GameStartPoint : public BaseClass
{
public:

	GameStartPoint()
	{
	}

	~GameStartPoint()
	{
	}

	// EngineMessageFn handlers
	uint32		EngineMessageFn(uint32 messageID, void *pData, float fData);

private:

	uint32		PreCreate(void *pData, float fData);
	void		ReadProps(ObjectCreateStruct* pStruct);
};


#endif // __GAMESTARTPOINT_H__
