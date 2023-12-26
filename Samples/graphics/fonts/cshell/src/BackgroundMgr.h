//------------------------------------------------------------------------------//
//
// MODULE   : BackgroundMgr.h
//
// PURPOSE  : Sets the background color.
//
// CREATED  : 09/17/2003
//
// (c) 2003 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#ifndef __BACKGROUNDMGR_H__
#define __BACKGROUNDMGR_H__


#include <ltbasetypes.h>
#include <ltbasedefs.h>



class BackgroundMgr
{
public:

	BackgroundMgr();
	~BackgroundMgr();

	// get the current background color
	LTRGB*					GetColor()								{ return &m_BackgroundColor; }

	// increment/decrement the current background color
	void					IncColor();
	void					DecColor();

private:

	void					SetColor();

private:

	unsigned				m_Index;
	enum { MAX_COLORS = 14 };

	// screen color
	LTRGB					m_BackgroundColor;
};


#endif // __BACKGROUNDMGR_H__

