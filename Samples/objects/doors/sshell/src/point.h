//------------------------------------------------------------------------------//
// 
// MODULE   : point.h
// 
// PURPOSE  : Point - Definition
// 
// CREATED  : 10/23/2002
// 
// (c) 2002 LithTech, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//

#ifndef __POINT_H__
#define __POINT_H__


// Engine includes
#include <ltbasedefs.h>
#include <ltengineobjects.h>
//#include "stdafx.h"


//-----------------------------------------------------------------------------
class Point : public BaseClass
{
	
public:
	
	// Construction / destruction
	Point()  {}
	~Point() {}
	
	// EngineMessageFn handlers
	uint32	EngineMessageFn(uint32 messageID, void *pData, float fData);
	
private:
	
	uint32		PreCreate(void *pData, float fData);
	void		ReadProps(ObjectCreateStruct* pStruct);
};


#endif // __GAMESTARTPOINT_H__
