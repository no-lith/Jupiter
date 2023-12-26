// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenOne.cpp
//
// PURPOSE : Top level interface screen
//
// (c) 1999-2002 Monolith Productions, Inc.  All Rights Reserved
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
// ----------------------------------------------------------------------- //



#include "ScreenMgr.h"
#include "ScreenOne.h"
#include "ClientRes.h"
#include "ltclientshell.h"

#include "ScreenCommands.h"

#include "clientinterfaces.h"


//---------------------------------------------------------------------------
//	 Name:		CScreenOne
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
CScreenOne::CScreenOne()
{
	m_pSlider = LTNULL;
	m_Value = 45;
}



//---------------------------------------------------------------------------
//	 Name:		CScreenOne
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
CScreenOne::~CScreenOne()
{
}



//---------------------------------------------------------------------------
//	 Name:		CScreenOne
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
LTBOOL CScreenOne::Build()
{
	// Static background on the main screen using the draw primitive interface 
	m_hBG = g_pInterfaceResMgr->GetTexture("interface\\JupiterBG.dtx");

	if (m_hBG)
	{
		SetupQuadUVs(m_BGPoly, m_hBG, 0.0f, 0.0f, 1.00f, 1.00f);
		g_pLTCDrawPrim->SetRGBA(&m_BGPoly,0x7F7F7F7F);
	}

	CreateTitle(IDS_SCREENONE);

	// Frame around slider ( normally use layout mgr for positioning )
	char szFrame[_MAX_PATH];
	g_pCShell->GetLayoutMgr()->GetFrameTex(szFrame,sizeof(szFrame));
	HTEXTURE hFrame = g_pInterfaceResMgr->GetTexture(szFrame);
	CLTGUIFrame *pFrame = debug_new(CLTGUIFrame);
	pFrame->Create(hFrame,400,35,LTTRUE);
	pFrame->SetBasePos(LTIntPt(20,70));
	pFrame->SetBorder(2,g_pCShell->GetLayoutMgr()->GetFrameColor());
	AddControl(pFrame);


	m_pSlider = AddSlider(IDS_SLIDER, IDS_HELP_NONE, 100, 200, -1, &m_Value);
	m_pSlider->SetSliderRange(0, 100);
	m_pSlider->SetSliderIncrement(5);
	m_pSlider->SetNumericDisplay(LTTRUE);

	// Make sure to call the base class
	if (!CBaseScreen::Build()) return LTFALSE;

	UseBack(LTTRUE);

	return LTTRUE;

}

//---------------------------------------------------------------------------
//	 Name:		CScreenOne
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void CScreenOne::OnFocus(LTBOOL bFocus)
{
	// On screen focus
	if (bFocus)
	{
		m_Value = 45;

		SetSelection(-1);

		// Update controls
		UpdateData(LTFALSE);
	}
	else // don't do base focus because it sets the default selection on focus
	{
		CBaseScreen::OnFocus(bFocus);
	}
}


//---------------------------------------------------------------------------
//	 Name:		CScreenOne
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
uint32 CScreenOne::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	return CBaseScreen::OnCommand(dwCommand,dwParam1,dwParam2);
}


//---------------------------------------------------------------------------
//	 Name:		CScreenOne
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
// Screen specific rendering
LTBOOL   CScreenOne::Render()
{
	if ( m_hBG )
	{
		g_pLTCDrawPrim->SetTexture(m_hBG);

		float fScale = g_pInterfaceResMgr->GetXRatio();
		float fx = (float)0 * fScale;
		float fy = (float)0 * fScale;

		float fw = (float)g_pInterfaceResMgr->GetScreenWidth() * fScale;
		float fh = (float)g_pInterfaceResMgr->GetScreenHeight() * fScale;

		g_pLTCDrawPrim->SetXYWH(&m_BGPoly,fx,fy,fw,fh);
		g_pLTCDrawPrim->DrawPrim(&m_BGPoly);
	}

	return CBaseScreen::Render();
}


