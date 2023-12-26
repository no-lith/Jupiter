// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenThree.cpp
//
// PURPOSE : Top level interface screen
//
// (c) 1999-2002 Monolith Productions, Inc.  All Rights Reserved
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
// ----------------------------------------------------------------------- //



#include "ScreenThree.h"
#include "ScreenMgr.h"
#include "ClientRes.h"

#include "ltclientshell.h"
#include "clienthelper.h"

#include "ScreenCommands.h"

#include "clientinterfaces.h"



//---------------------------------------------------------------------------
//	 Name:		CScreenThree
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
CScreenThree::CScreenThree()
{

}

//---------------------------------------------------------------------------
//	 Name:		CScreenThree
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
CScreenThree::~CScreenThree()
{
}


//---------------------------------------------------------------------------
//	 Name:		CScreenThree
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------

LTBOOL CScreenThree::Build()
{
	// Static background on the main screen using the draw primitive interface 
	m_hBG = g_pInterfaceResMgr->GetTexture("interface\\TouchdownBG.dtx");

	if (m_hBG)
	{
		SetupQuadUVs(m_BGPoly, m_hBG, 0.0f, 0.0f, 1.00f, 1.00f);
		g_pLTCDrawPrim->SetRGBA(&m_BGPoly,0x7F7F7F7F);
	}


	CreateTitle(IDS_SCREENTHREE);


	// server list -----------------------------------------------
	m_pList = AddList(LTIntPt(95,200),100,LTTRUE);
	m_pList->SetScrollWrap(LTFALSE);
	m_pList->SetFrameWidth(2);
	m_pList->SetColors(m_NonSelectedColor,m_NonSelectedColor,m_DisabledColor);
	m_pList->SetIndent(2);

	// add 10 rows of 3 columns to the list
	for ( int x = 0; x < 10; x++)
	{
		CLTGUIColumnCtrl* pCtrl = CreateColumnCtrl( CMD_NONE, IDS_HELP_NONE );

		wchar_t * szTemp = LoadTempString( IDS_COLUMN );

		pCtrl->AddColumn( szTemp, 150 );
		pCtrl->AddColumn( szTemp, 150 );
		pCtrl->AddColumn( szTemp, 150 );

		m_pList->AddControl(pCtrl);
	}


	// Make sure to call the base class
	if (!CBaseScreen::Build()) return LTFALSE;

	UseBack(LTTRUE);

	return LTTRUE;

}

//---------------------------------------------------------------------------
//	 Name:		CScreenThree
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void CScreenThree::OnFocus(LTBOOL bFocus)
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
//	 Name:		CScreenThree
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
uint32 CScreenThree::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
//	switch(dwCommand)
//	{
//	default:
		return CBaseScreen::OnCommand(dwCommand,dwParam1,dwParam2);
//	}

//	return 1;

};


//---------------------------------------------------------------------------
//	 Name:		CScreenThree
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
// Screen specific rendering
LTBOOL   CScreenThree::Render()
{
	// Render background
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
//	 Name:		CScreenThree
//
//	 Purpose:	
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void CScreenThree::Escape()
{
	CBaseScreen::Escape();
}