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

#define MAX_PITCH   30.0f

//----------------------------------------------------------------------------
// CCamera::CCamera()
//
//----------------------------------------------------------------------------
CCamera::CCamera() :
m_fPitch(0.0f)
{
}

//----------------------------------------------------------------------------
// LTRESULT CCamera::CreateCamera()
//
//----------------------------------------------------------------------------
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


//----------------------------------------------------------------------------
// CCamera::UpdatePosition(HOBJECT hObject)
//
//----------------------------------------------------------------------------
void CCamera::UpdatePosition(HOBJECT hObject)
{
    LTVector vPos;
    LTRotation rRot;

    g_pLTClient->GetObjectPos(hObject, &vPos);
    g_pLTClient->GetObjectRotation(hObject, &rRot);

    rRot.Rotate(rRot.Right(), (m_fPitch * 0.0174533f));
    vPos += rRot.Forward() * -200.0f;
    vPos += rRot.Up() * 75.0f;

    g_pLTClient->SetObjectPosAndRotation(m_hObject, &vPos, &rRot);    

}


//----------------------------------------------------------------------------
// CCamera::UpdatePitch(float _pitch)
//
//----------------------------------------------------------------------------
void CCamera::UpdatePitch(float pitch)
{

    //g_pLTClient->CPrint("Pitch: %f" , pitch);
    

    if((m_fPitch + pitch) < -MAX_PITCH) 
    {
        m_fPitch = -MAX_PITCH;
    }
    else if ((m_fPitch + pitch) > MAX_PITCH)
    {
        m_fPitch = MAX_PITCH;
    }
    else
    {
      m_fPitch += pitch * 5.0f;
    }


    //g_pLTClient->CPrint("Final Pitch: %f" , m_fPitch);

}
