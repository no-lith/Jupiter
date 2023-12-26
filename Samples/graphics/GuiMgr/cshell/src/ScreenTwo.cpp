// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenTwo.cpp
//
// PURPOSE : Top level interface screen
//
// (c) 1999-2002 Monolith Productions, Inc.  All Rights Reserved
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
// ----------------------------------------------------------------------- //



#include "ScreenTwo.h"
#include "ScreenMgr.h"
#include "ClientRes.h"
#include "ltclientshell.h"

#include "ScreenCommands.h"

#include "clientinterfaces.h"


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CScreenTwo
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
CScreenTwo::CScreenTwo()
{
	m_szEdit[0] = 0;
	m_pEditCtrl = LTNULL;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CScreenTwo
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
CScreenTwo::~CScreenTwo()
{
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CScreenTwo
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
// Build the screen
LTBOOL CScreenTwo::Build()
{
	// Static background on the main screen using the draw primitive interface 
	m_hBG = g_pInterfaceResMgr->GetTexture("interface\\JupiterBG.dtx");

	if (m_hBG)
	{
		SetupQuadUVs(m_BGPoly, m_hBG, 0.0f, 0.0f, 1.00f, 1.00f);
		g_pLTCDrawPrim->SetRGBA(&m_BGPoly,0x7F7F7F7F);
	}



	CreateTitle(IDS_SCREENTWO);

	// Frame around button ( normally use layout mgr for positioning )
	char szFrame[_MAX_PATH];
	g_pCShell->GetLayoutMgr()->GetFrameTex(szFrame,sizeof(szFrame));
	HTEXTURE hFrame = g_pInterfaceResMgr->GetTexture(szFrame);
	CLTGUIFrame *pFrame = debug_new(CLTGUIFrame);
	pFrame->Create(hFrame,300,35,LTTRUE);
	pFrame->SetBasePos(LTIntPt(20,110));
	pFrame->SetBorder(2,g_pCShell->GetLayoutMgr()->GetFrameColor());
	AddControl(pFrame);

	AddTextItem(IDS_SCREENTHREE, CMD_THREE, IDS_HELP_NONE, kDefaultPos);

	// Edit Control
	m_pEditCtrl = AddEditItem( 10, CMD_NONE, m_szEdit );
	m_pEditCtrl->EnableCaret(LTTRUE);


	// Make sure to call the base class
	if (!CBaseScreen::Build()) return LTFALSE;

	UseBack(LTTRUE);

	return LTTRUE;

}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CScreenTwo
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CScreenTwo::OnFocus(LTBOOL bFocus)
{
	if (bFocus)
	{

		SetSelection(-1);
	}
	else // don't do base focus because it sets the default selection on focus
	{
		CBaseScreen::OnFocus(bFocus);
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CScreenTwo
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
uint32 CScreenTwo::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch(dwCommand)
	{
		case CMD_THREE:
		{
			m_pScreenMgr->SetCurrentScreen(SCREEN_ID_THREE);
			break;
		}

	default:
		return CBaseScreen::OnCommand(dwCommand,dwParam1,dwParam2);
	}

	return 1;

};


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CScreenTwo
//
//	PURPOSE: Screen specific rendering
//
// ----------------------------------------------------------------------- //
LTBOOL   CScreenTwo::Render()
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


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CScreenTwo
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CScreenTwo::Escape()
{
	CBaseScreen::Escape();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CScreenTwo
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
LTBOOL CScreenTwo::HandleKeyDown(int key, int rep)
{
	LTBOOL ret = LTFALSE;

	if ( m_pEditCtrl == GetSelectedControl() )
	{
		ret = m_pEditCtrl->HandleKeyDown( key,rep );
	}

	if ( !ret )
	{
		return CBaseScreen::HandleKeyDown( key,rep );
	}

	return ret;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CScreenTwo
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
LTBOOL  CScreenTwo::HandleChar(wchar_t c)
{
	if (m_pEditCtrl == GetSelectedControl() )
	{
		return m_pEditCtrl->HandleChar(c);
	}

	return CBaseScreen::HandleChar(c);

}
