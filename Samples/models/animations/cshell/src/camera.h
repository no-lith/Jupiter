//------------------------------------------------------------------------------//
// 
// MODULE   : camera.h
// 
// PURPOSE  : CCamera - Definition
// 
// CREATED  : 6/27/2002
// 
// (c) 2002 LithTech, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//


#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "ltclientshell.h"

// Engine includes
#include <ltbasedefs.h>
//#include "stdafx.h"


//-----------------------------------------------------------------------------
class CCamera 
{

public:

	// Construction / destruction
	CCamera();
	~CCamera() {}


	LTRESULT CreateCamera();
	HOBJECT GetCamera()		{ return m_hObject; }

    void    UpdatePosition(HOBJECT hObject);
    void    UpdatePitch(float pitch);
    void    UpdateZoom(float zoom);

private:

	HLOCALOBJ m_hObject;
    float     m_fPitch;
    float     m_fZoom;
	
};


#endif // __CAMERA_H__
