//------------------------------------------------------------------------------//
// 
// MODULE   : playerclnt.h
// 
// PURPOSE  : CPlayerClnt - Definition
// 
// CREATED  : 07/15/2002
// 
// (c) 2002 LithTech, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//


#ifndef __PLAYERCLNT_H__
#define __PLAYERCLNT_H__

#include "ltclientshell.h"

// Engine includes
#include <ltbasedefs.h>


//-----------------------------------------------------------------------------
class CPlayerClnt 
{

public:

	// Construction / destruction
	CPlayerClnt() {}
	~CPlayerClnt() {}

	void	SetHObject(HLOCALOBJ hObj);
	void	SpecialEffectMessage(ILTMessage_Read *pMessage);

private:

	HLOCALOBJ m_hObject;
	
};


#endif // __PLAYERCLNT_H__
