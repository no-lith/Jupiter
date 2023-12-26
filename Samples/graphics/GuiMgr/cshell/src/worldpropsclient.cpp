//------------------------------------------------------------------------------//
// 
// MODULE   : worldpropclnt.cpp
// 
// PURPOSE  : CWorldPropsClnt - Implementation
// 
// CREATED  : 02/14/2002
// 
// (c) 2002 LithTech, Inc.  All Rights Reserved
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
// 
//------------------------------------------------------------------------------//

//--Project Includes--

// Definition of this class
#include "worldpropsclient.h"

// Global pointers to client interfaces
#include "clientinterfaces.h"

//--Engine Includes--

// ILTClient definition
#include <iltclient.h>

//--Other includes--

// for sprintf
#include <stdio.h>

extern ILTTexInterface* g_pTexInterfaceLT;


//------------------------------------------------------------------------------
//	CWorldPropsClnt::CWorldPropsClnt()
//
//------------------------------------------------------------------------------
CWorldPropsClnt::CWorldPropsClnt()		
:
m_nFarZ(10000),
m_vBackgroundColor(1.0f,1.0f,1.0f),
m_bFogEnable(false),
m_vFogColor(1.0f,1.0f,1.0f),
m_nFogNearZ(0),
m_nFogFarZ(5000),
m_bSkyFogEnable(false),
m_nSkyFogNearZ(0),
m_nSkyFogFarZ(5000),
m_fSkyScale(1.0f)
{}


//------------------------------------------------------------------------------
//	CWorldPropsClnt::~CWorldPropsClnt()
//
//------------------------------------------------------------------------------
CWorldPropsClnt::~CWorldPropsClnt()		
{}


//------------------------------------------------------------------------------
//	CWorldPropsClnt::UnpackWorldProps(ILTMessage_Read *pMsgProps)
//
//------------------------------------------------------------------------------
void CWorldPropsClnt::UnpackWorldProps(ILTMessage_Read *pMsgProps)
{
	
	m_nFarZ = pMsgProps->Readuint32();
	m_vBackgroundColor = pMsgProps->ReadLTVector();
	m_bFogEnable = pMsgProps->Readbool();
	m_vFogColor = pMsgProps->ReadLTVector();
	m_nFogNearZ = pMsgProps->Readuint32();
	m_nFogFarZ = pMsgProps->Readuint32();
	m_bSkyFogEnable = pMsgProps->Readbool();
	m_nSkyFogNearZ = pMsgProps->Readuint32();
	m_nSkyFogFarZ = pMsgProps->Readuint32();
	m_fSkyScale = pMsgProps->Readfloat();
				
	this->ApplyWorldProps();
	
}


//------------------------------------------------------------------------------
//	CWorldPropsClnt::ApplyWorldProps()
//
//------------------------------------------------------------------------------
void CWorldPropsClnt::ApplyWorldProps()
{
	
	// Set worldprop console variables.
	char buffer[255];
	
	sprintf(buffer, "FarZ %d", m_nFarZ);
	g_pLTClient->RunConsoleString(buffer);
	//		g_pLTClient->CPrint(buffer);
	
	sprintf(buffer, "BackgroundColor %d %d %d", (uint8)m_vBackgroundColor.x, 
		(uint8)m_vBackgroundColor.y, (uint8)m_vBackgroundColor.z);
	g_pLTClient->RunConsoleString(buffer);
	//		g_pLTClient->CPrint(buffer);
	
	sprintf(buffer, "FogEnable %d", m_bFogEnable ? 1 : 0);
	g_pLTClient->RunConsoleString(buffer);
	//		g_pLTClient->CPrint(buffer);
	
	if (m_bFogEnable)
	{
		// Set fog rgb colors
		sprintf(buffer, "FogR %d", (uint8)m_vFogColor.x);
		g_pLTClient->RunConsoleString(buffer);
		sprintf(buffer, "FogG %d", (uint8)m_vFogColor.y);
		g_pLTClient->RunConsoleString(buffer);
		sprintf(buffer, "FogB %d", (uint8)m_vFogColor.z);
		g_pLTClient->RunConsoleString(buffer);
		
		sprintf(buffer, "FogNearZ %d; FogFarZ %d", 
			m_nFogNearZ, m_nFogFarZ);
		g_pLTClient->RunConsoleString(buffer);
		//			g_pLTClient->CPrint(buffer);
	}
	
	sprintf(buffer, "SkyFogEnable %d", m_bSkyFogEnable ? 1 : 0);
	g_pLTClient->RunConsoleString(buffer);
	//		g_pLTClient->CPrint(buffer);
	if (m_bSkyFogEnable)
	{
		sprintf(buffer, "SkyFogNearZ %d; SkyFogFarZ %d", 
			m_nSkyFogNearZ, m_nSkyFogFarZ);
		g_pLTClient->RunConsoleString(buffer);
		//			g_pLTClient->CPrint(buffer);
	}
	
	sprintf(buffer, "SkyScale %f", m_fSkyScale);
	g_pLTClient->RunConsoleString(buffer);
	
}


//------------------------------------------------------------------------------
//	CWorldPropsClnt::Update()
//
//------------------------------------------------------------------------------
void CWorldPropsClnt::Update()
{
}


//------------------------------------------------------------------------------
//	CWorldPropsClnt::GetBackgroundColor()
//
//------------------------------------------------------------------------------
LTRGB CWorldPropsClnt::GetBackgroundColor()
{
	
	LTRGB rgbColor;
	HCONSOLEVAR hCVar;
	
	rgbColor.r = 0;
	rgbColor.g = 0;
	rgbColor.b = 0;
	
	if (hCVar = g_pLTClient->GetConsoleVar("BackgroundR"))
		rgbColor.r = (uint8)g_pLTClient->GetVarValueFloat(hCVar);
	
	if (hCVar = g_pLTClient->GetConsoleVar("BackgroundG"))
		rgbColor.g = (uint8)g_pLTClient->GetVarValueFloat(hCVar);
	
	if (hCVar = g_pLTClient->GetConsoleVar("BackgroundB"))
		rgbColor.b = (uint8)g_pLTClient->GetVarValueFloat(hCVar);
	
	rgbColor.a = 255;
	
	return rgbColor;
}


