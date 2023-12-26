//------------------------------------------------------------------------------//
// 
// MODULE   : playersrvr.h
// 
// PURPOSE  : CPlayerSrvr - Definition
// 
// CREATED  : 07/15/2002
// 
// (c) 2002 LithTech, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//

#ifndef __PLAYERSRVR_H__
#define __PLAYERSRVR_H__


// Engine includes
#include <ltbasedefs.h>
#include <ltengineobjects.h>

//-----------------------------------------------------------------------------
class CPlayerSrvr : public BaseClass
{
	
public:
	
	// Construction / destruction
	CPlayerSrvr()  {}
	~CPlayerSrvr() {}
	
	// EngineMessageFn handlers
	uint32	EngineMessageFn(uint32 messageID, void *pData, float fData);
	
private:
	
	uint32		PreCreate(void *pData, float fData);
	void		ReadProps(ObjectCreateStruct* pStruct);
};


#endif // __PLAYERSRVR_H__
