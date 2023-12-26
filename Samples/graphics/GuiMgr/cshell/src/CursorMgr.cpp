// ----------------------------------------------------------------------- //
//
// MODULE  : CursorMgr.cpp
//
// PURPOSE : Manages the ingame cursor
//
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
// ----------------------------------------------------------------------- //

#define WIN32_LEAN_AND_MEAN
#include "windows.h"

#include "clienthelper.h"
#include "CursorMgr.h"
#include "ClientRes.h"

#include "InterfaceResMgr.h"

CCursorMgr * g_pCursorMgr = LTNULL;

//---------------------------------------------------------------------------
//	 Name:		CCursorMgr::CCursorMgr()
//
//	 Purpose:	Constructor
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
CCursorMgr::CCursorMgr()
{
	g_pCursorMgr = this;

	m_bUseCursor			= LTFALSE;
	m_bUseHardwareCursor	= LTFALSE;
	m_bInitialized			= LTFALSE;

	m_CursorCenter.x = 0;
	m_CursorCenter.y = 0;

	m_fHeight = 0;
	m_fWidth = 0;

	m_hCursorTex = LTNULL;
	m_hCursor = LTNULL;
}


//---------------------------------------------------------------------------
//	 Name:		CCursorMgr::~CCursorMgr()
//
//	 Purpose:	destructor
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
CCursorMgr::~CCursorMgr()
{
	Term();

	g_pCursorMgr = LTNULL;

}


//---------------------------------------------------------------------------
//	 Name:		CCursorMgr::Init()
//
//	 Purpose:	Initialization 
//
//	 Returns:	LTBOOL - success or failure 
//
//---------------------------------------------------------------------------
LTBOOL CCursorMgr::Init()
{
	if (m_bInitialized)
		return LTTRUE;


	// handles the normal hardware cursor ( from the resource file )
	if (g_pLTClient->Cursor()->LoadCursorBitmapResource(MAKEINTRESOURCE(IDC_POINTER), m_hCursor) != LT_OK)
	{
		g_pLTClient->CPrint("can't load cursor resource.");
		return LTFALSE;
	}

	if (g_pLTClient->Cursor()->SetCursor(m_hCursor) != LT_OK)
	{
		g_pLTClient->CPrint("can't set cursor.");
		return LTFALSE;
	}

	UseHardwareCursor( GetConsoleInt("HardwareCursor",0) > 0 && GetConsoleInt("DisableHardwareCursor",0) == 0);


	// Load cursor texture here 
	m_hCursorTex = g_pInterfaceResMgr->GetTexture("interface\\cursor.dtx");

	if ( m_hCursorTex )
	{
		uint32 w, h;



		g_pLTCTexInterface->GetTextureDims( m_hCursorTex,w,h);

		m_fWidth = (float) w;
		m_fHeight = (float) h;

		g_pLTCDrawPrim->SetXYWH(&m_Poly,0,0,m_fWidth,m_fHeight);

		g_pLTCDrawPrim->SetUVWH(&m_Poly,0.0f,0.0f,1.0f,1.0f);
		g_pLTCDrawPrim->SetRGBA(&m_Poly,0xFFFFFFFF);

	}

	m_bInitialized = LTTRUE;

	m_CursorCenter.x = 16;
	m_CursorCenter.y = 16;

	return LTTRUE;


}


//---------------------------------------------------------------------------
//	 Name:		CCursorMgr::Term()
//
//	 Purpose:	Terminate
//
//	 Returns:	LTBOOL - success or failure 
//
//---------------------------------------------------------------------------
LTBOOL CCursorMgr::Term()
{
	if (!m_bInitialized)
		return LTFALSE;
	
	m_bInitialized = LTFALSE;

	return LTTRUE;
}



//---------------------------------------------------------------------------
//	 Name:		CCursorMgr::Render()
//
//	 Purpose:	Render the cursor
//
//	 Returns:	LTBOOL - success or failure 
//
//---------------------------------------------------------------------------
LTBOOL CCursorMgr::Render()
{

	static const HLTCOLOR kTrans	= SETRGB_T(255,0,255);
	LTBOOL bHWC = (GetConsoleInt("HardwareCursor",0) > 0 && GetConsoleInt("DisableHardwareCursor",0) == 0);

	if (bHWC != m_bUseHardwareCursor)
		UseHardwareCursor(bHWC);

	if (!m_bUseCursor)
		return LTFALSE;

	// If we are using the software cursor 
	if (m_bUseHardwareCursor )
		return LTTRUE;

	// Render the cursor 

	if ( m_hCursorTex )
	{
		g_pLTCDrawPrim->SetTransformType(DRAWPRIM_TRANSFORM_SCREEN);
		g_pLTCDrawPrim->SetZBufferMode(DRAWPRIM_NOZ); 
		g_pLTCDrawPrim->SetClipMode(DRAWPRIM_NOCLIP);
		g_pLTCDrawPrim->SetFillMode(DRAWPRIM_FILL);
		g_pLTCDrawPrim->SetColorOp(DRAWPRIM_MODULATE);
		g_pLTCDrawPrim->SetAlphaTestMode(DRAWPRIM_NOALPHATEST);
		g_pLTCDrawPrim->SetAlphaBlendMode(DRAWPRIM_BLEND_MOD_SRCALPHA);

		//setup the texture
		g_pLTCDrawPrim->SetTexture(m_hCursorTex);

		g_pLTCDrawPrim->SetXYWH(&m_Poly, (float) m_CursorPos.x, (float)m_CursorPos.y, m_fWidth, m_fHeight);

		//setup the UV coordinates on our quad, since it is dependant upon
		//the texture for correct filtering
		SetupQuadUVs(m_Poly, m_hCursorTex, 0.0f, 0.0f, 1.0f, 1.0f);

		// draw the cursor
		g_pLTCDrawPrim->DrawPrim(&m_Poly);


	}


	return LTTRUE;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCursorMgr::UseHardwareCursor
//
//	PURPOSE:	(De)activate the Windows cursor drawing routines
//
// ----------------------------------------------------------------------- //

void CCursorMgr::UseHardwareCursor(LTBOOL bUseHardwareCursor,bool bForce)
{
	m_bUseHardwareCursor = bUseHardwareCursor;

	// Turns on the hardware cursor  
	if (m_bUseHardwareCursor && m_bUseCursor )
	{
		g_pLTClient->Cursor()->SetCursorMode(CM_Hardware,bForce);
	}
	else
	{
		g_pLTClient->Cursor()->SetCursorMode(CM_None,bForce);
	}
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CCursorMgr::UseCursor
//
//	PURPOSE:	Handle activation and deactivation of visible cursor
//
// ----------------------------------------------------------------------- //

void CCursorMgr::UseCursor(LTBOOL bUseCursor, LTBOOL bLockCursorToCenter)
{
	m_bUseCursor = bUseCursor;

	if (m_bUseCursor && m_bUseHardwareCursor )
	{
		g_pLTClient->Cursor()->SetCursorMode(CM_Hardware);

		// copied the following 4 lines from Init()
		if (g_pLTClient->Cursor()->SetCursor(m_hCursor) != LT_OK)
		{
			g_pLTClient->CPrint("can't set cursor.");
		}
	}
	else // Make sure the Hardware cursor is off
	{
        g_pLTClient->Cursor()->SetCursorMode(CM_None);
	}

	// Lock or don't lock the cursor to the center of the screen
	if(bLockCursorToCenter)
	{
		g_pLTClient->RunConsoleString("CursorCenter 1");
	}
	else
	{
		g_pLTClient->RunConsoleString("CursorCenter 0");
	}
}
