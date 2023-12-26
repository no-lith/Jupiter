//------------------------------------------------------------------------------//
// 
// MODULE   : camera.cpp
// 
// PURPOSE  : CCamera - Implementation
// 
// CREATED  : 6/27/2002
// 
// (c) 2002 LithTech, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//

#include "camera.h"

#include <iltclient.h>
#include <ltobjectcreate.h>

//----------------------------------------------------------------------------
// LTRESULT CCamera::CreateCamera()
//
//-----------------------------------------------------------------------------
LTRESULT CCamera::CreateCamera()
{

	uint32 nWidth, nHeight;
	ObjectCreateStruct objCreate;
	
	//	Get our screen dimensions, for the camera rectangle
	g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
	
	//	Initialize our object creation structure
	objCreate.Clear();
	objCreate.m_ObjectType = OT_CAMERA;
	m_hObject = g_pLTClient->CreateObject(&objCreate);
	
	if (NULL == m_hObject)
		return LT_ERROR;
	
	g_pLTClient->SetCameraRect(m_hObject, false, 0, 0, nWidth, nHeight);
	
	//	This is an fov of 90 degrees
	float fFovX = MATH_PI/2.0f;
	float fFovY = (fFovX * nHeight) / nWidth;
	g_pLTClient->SetCameraFOV(m_hObject, fFovX, fFovY);
	
	return LT_OK;
	
}
