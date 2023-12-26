// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenMain.cpp
//
// PURPOSE : Top level interface screen
//
// (c) 1999-2002 Monolith Productions, Inc.  All Rights Reserved
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
// ----------------------------------------------------------------------- //



#include "ScreenMain.h"
#include "ScreenMgr.h"
#include "ClientRes.h"
#include "ScreenCommands.h"

#include "ltclientshell.h"
#include "clientinterfaces.h"
#include "clienthelper.h"



//---------------------------------------------------------------------------
//	 Name:		ScreenMain
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------

CScreenMain::CScreenMain()
{
   m_pResume = LTNULL;
   m_pGameType = LTNULL;
	m_pGameTypeFrame = LTNULL;
	
	m_bLanguage = LTFALSE;

}

//---------------------------------------------------------------------------
//	 Name:		ScreenMain
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
CScreenMain::~CScreenMain()
{
}


//---------------------------------------------------------------------------
//	 Name:		ScreenMain
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
// Build the screen
LTBOOL CScreenMain::Build()
{

	// Static background on the main screen using the draw primitive interface 
	m_hBG = g_pInterfaceResMgr->GetTexture("interface\\TouchdownBG.dtx");

	if (m_hBG)
	{
		SetupQuadUVs(m_BGPoly, m_hBG, 0.0f, 0.0f, 1.00f, 1.00f);
		g_pLTCDrawPrim->SetRGBA(&m_BGPoly,0x7F7F7F7F);
	}

	// Frame around buttons ( normally use layout mgr for positioning )
	char szFrame[_MAX_PATH];
	g_pCShell->GetLayoutMgr()->GetFrameTex(szFrame,sizeof(szFrame));
	HTEXTURE hFrame = g_pInterfaceResMgr->GetTexture(szFrame);
	CLTGUIFrame *pFrame = debug_new(CLTGUIFrame);
	pFrame->Create(hFrame,200,85,LTTRUE);
	pFrame->SetBasePos(LTIntPt(20,70));
	pFrame->SetBorder(2,g_pCShell->GetLayoutMgr()->GetFrameColor());
	AddControl(pFrame);


	AddTextItem(IDS_SCREENONE, CMD_ONE, IDS_HELP_NONE, kDefaultPos);
	AddTextItem(IDS_SCREENTWO, CMD_TWO, IDS_HELP_NONE, kDefaultPos);

	AddTextItem(IDS_EXIT, CMD_EXIT, IDS_HELP_NONE, s_BackPos);




	// Make sure to call the base class
	if (!CBaseScreen::Build()) return LTFALSE;

	UseBack(LTFALSE);

	return LTTRUE;

}

//---------------------------------------------------------------------------
//	 Name:		ScreenMain
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void CScreenMain::OnFocus(LTBOOL bFocus)
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


//---------------------------------------------------------------------------
//	 Name:		ScreenMain
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
uint32 CScreenMain::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch(dwCommand)
	{
		case CMD_ONE:
		{
			m_pScreenMgr->SetCurrentScreen(SCREEN_ID_ONE);
			break;
		}

		case CMD_TWO:
		{
			m_pScreenMgr->SetCurrentScreen(SCREEN_ID_TWO);
			break;
		}

		case CMD_EXIT:
		{
         g_pLTClient->Shutdown();
			break;
		}
	default:
		return CBaseScreen::OnCommand(dwCommand,dwParam1,dwParam2);
	}

	return 1;

};


//---------------------------------------------------------------------------
//	 Name:		ScreenMain
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
// Screen specific rendering
LTBOOL   CScreenMain::Render()
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


//---------------------------------------------------------------------------
//	 Name:		ScreenMain
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void CScreenMain::Escape()
{
	SendCommand(CMD_EXIT,0,0);

}