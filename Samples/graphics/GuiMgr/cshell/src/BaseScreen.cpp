// ----------------------------------------------------------------------- //
//
// MODULE  : BaseScreen.cpp
//
// PURPOSE : Base class for interface screens
//				 Use this class to add functionality that all screens have
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
// ----------------------------------------------------------------------- //

#include "BaseScreen.h"
#include "ScreenMgr.h"
#include "ScreenCommands.h"
#include "clienthelper.h"

//#include "VKDefs.h"
#include "ltclientshell.h"
#include "clientinterfaces.h"


LTBOOL		CBaseScreen::s_bReadLayout = LTFALSE;

// Back and Next button positions
LTIntPt		CBaseScreen::s_BackPos;
LTIntPt		CBaseScreen::s_NextPos;

// Back button
CLTGUIButton	CBaseScreen::s_Back;

// Client shell interface 
extern CLTClientShell* g_pCShell;


namespace
{
	LTIntPt offscreen(-64,-64);
}



//---------------------------------------------------------------------------
//	 Name:		CBaseScreen::CBaseScreen()
//
//	 Purpose:	Constructor
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
CBaseScreen::CBaseScreen()
{
	m_bInit = LTFALSE;
	m_bBuilt = LTFALSE;
	m_bBack = LTFALSE;
	m_bHaveFocus = LTFALSE;
   m_pScreenMgr = LTNULL;

	// defaults for screen title
	m_TitlePos.x = 0;
	m_TitlePos.y = 0;
   m_TitleFont = 0;
   m_TitleSize = 32;
   m_TitleColor = argbYellow;

	m_nScreenID = SCREEN_ID_NONE;
	m_nContinueID = SCREEN_ID_NONE;

	// Array of controls that each screen owns 
	m_controlArray.reserve(5);

	m_nSelection = kNoSelection;
	m_nOldSelection = kNoSelection;

   m_pCaptureCtrl = LTNULL;

	m_nRMouseDownItemSel =  kNoSelection;
	m_nRMouseDownItemSel =  kNoSelection;

	m_nItemSpacing = 0;

	m_SelectedColor		= argbBlue;
	m_NonSelectedColor	= argbBlack;
	m_DisabledColor		= argbGray;

   m_pNext = LTNULL;

	// last screen scale ( 1.0f would be standard 640x480 )
	m_fLastScale = 1.0f;

	m_bVisited = LTFALSE;

}

//---------------------------------------------------------------------------
//	 Name:		CBaseScreen::~CBaseScreen()
//
//	 Purpose:	destructor
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
CBaseScreen::~CBaseScreen()
{
	if ( m_bInit )
	{
		Term();
	}

}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::Init
//
//	PURPOSE:	initialize the screen
//
//	 Returns: LTBOOL - success or failure
//
// ----------------------------------------------------------------------- //

LTBOOL CBaseScreen::Init(int nScreenID)
{
	// Read / create generic screen items
	if (!s_bReadLayout)
	{
      s_bReadLayout   = LTTRUE;

		s_BackPos		= g_pCShell->GetLayoutMgr()->GetBackPos();
		s_NextPos		= g_pCShell->GetLayoutMgr()->GetNextPos();

		LTIntPt BackPos = g_pCShell->GetLayoutMgr()->GetBackTexPos();

		char szTexture[256];

		// Back button texture
		g_pCShell->GetLayoutMgr()->GetBackTex(szTexture,sizeof(szTexture));
		HTEXTURE hBack = g_pInterfaceResMgr->GetTexture(szTexture);

		// Back button highlight texture
		g_pCShell->GetLayoutMgr()->GetBackTexH(szTexture,sizeof(szTexture));
		HTEXTURE hBackH = g_pInterfaceResMgr->GetTexture(szTexture);

		// Create the back button
		s_Back.Create(CMD_BACK,LTNULL,hBack,hBackH);
		s_Back.SetBasePos(BackPos);
		s_Back.SetScale(g_pInterfaceResMgr->GetXRatio());
	}

	m_nScreenID=nScreenID;

	m_pScreenMgr = g_pCShell->GetScreenMgr();


	// Setup title 
	SetTitleColor(argbYellow);

	//set up layout variables
	SetTitlePos(g_pCShell->GetLayoutMgr()->GetScreenTitlePos((eScreenID)nScreenID));
	SetTitleFont(g_pCShell->GetLayoutMgr()->GetScreenTitleFont((eScreenID)nScreenID));
	SetTitleSize(g_pCShell->GetLayoutMgr()->GetScreenTitleSize((eScreenID)nScreenID));

	m_PageRect  = g_pCShell->GetLayoutMgr()->GetScreenPageRect((eScreenID)nScreenID);

	SetItemSpacing(g_pCShell->GetLayoutMgr()->GetScreenItemSpacing((eScreenID)nScreenID));

	m_nAlignment = g_pCShell->GetLayoutMgr()->GetScreenItemAlign((eScreenID)nScreenID);


	m_SelectedColor		= g_pCShell->GetLayoutMgr()->GetScreenSelectedColor((eScreenID)nScreenID);
	m_NonSelectedColor	= g_pCShell->GetLayoutMgr()->GetScreenNonSelectedColor((eScreenID)nScreenID);
	m_DisabledColor		= g_pCShell->GetLayoutMgr()->GetScreenDisabledColor((eScreenID)nScreenID);


	m_nextPos.x = GetPageLeft();
	m_nextPos.y = GetPageTop();

	m_bInit=TRUE;
	return LTTRUE;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::Term
//
//	PURPOSE:	Terminate the screen
//
// ----------------------------------------------------------------------- //

void CBaseScreen::Term()
{
	RemoveAll();

	s_Back.Destroy();

	// Flush the title string texture 
	m_TitleString.FlushTexture();

	m_bInit=FALSE;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::Escape
//
//	PURPOSE:	back out of the screen
//
// ----------------------------------------------------------------------- //

void CBaseScreen::Escape()
{
	// Go back a screen 
	m_pScreenMgr->PreviousScreen();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::Render
//
//	PURPOSE:	Renders the screen to a surface
//
// ----------------------------------------------------------------------- //

LTBOOL CBaseScreen::Render()
{

	LTIntPt tmpPos;

	// Render the title
	if (!m_TitleString.IsEmpty())
	{
		float px,py;

		// drop shadow
		m_TitleString.GetPos(&px,&py);
		px += 2.0f;
		py += 2.0f;
		m_TitleString.SetColor(0xBF000000);
		m_TitleString.SetPos(px,py);
		m_TitleString.Render();

		// then the title
		px -= 2.0f;
		py -= 2.0f;
		m_TitleString.SetColor(m_TitleColor);
		m_TitleString.SetPos(px,py);
		m_TitleString.Render();
	}

	// render the controls on this screen
	for (uint16 i = 0; i < m_controlArray.size(); i++ )
	{
		m_controlArray[i]->Render ();
	}

    return LTTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::CreateTitle
//
//	PURPOSE:	Creates the string to display as the screens title
//
//	Returns: success or failure
//
// ----------------------------------------------------------------------- //


LTBOOL CBaseScreen::CreateTitle(wchar_t * lpszTitle)
{

	CFontInfo * pFont = g_pInterfaceResMgr->GetFont(m_TitleFont);
	CFontInfo TitleFont(pFont->m_szTypeface, pFont->m_nHeight, pFont->m_lfCharSet, pFont->m_nStyle);

		
	LTIntPt pos = m_TitlePos;
	g_pInterfaceResMgr->ConvertScreenPos(pos);

	TitleFont.m_nHeight = (uint8)((LTFLOAT)m_TitleSize * g_pInterfaceResMgr->GetXRatio());

	m_TitleString.SetFont(TitleFont);
		
	m_TitleString.SetColor(m_TitleColor);

	m_TitleString.SetPos( pos );

	m_TitleString.SetText(lpszTitle);
		

	return LTTRUE;

}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::CreateTitle
//
//	PURPOSE:	Creates the string to display as the screens title given a 
//				resource string ID
//
//	Returns: success or failure
//
// ----------------------------------------------------------------------- //

LTBOOL CBaseScreen::CreateTitle(int nStringID)
{

	LTBOOL created = CreateTitle(LoadTempString(nStringID));

	return created;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::Build
//
//	PURPOSE:	Construct the basic screen elements
//
//	Returns: success or failure 
// ----------------------------------------------------------------------- //

LTBOOL CBaseScreen::Build()
{
	m_bBuilt=LTTRUE;

	UseBack(LTTRUE);

	return TRUE;
}




// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::HandleChar
//
//	PURPOSE:	Handle a char message 
//
//	Returns: handled or not 
//
// ----------------------------------------------------------------------- //
LTBOOL CBaseScreen::HandleChar(wchar_t c)
{
	LTBOOL handled = LTFALSE;

	if (m_pCaptureCtrl)
	{
		if (m_pCaptureCtrl->HandleChar(c))
			handled = LTTRUE;
	}

	return handled;

}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::HandleKeyDown
//
//	PURPOSE:	Handles a key press
//
//	Returns: Returns FALSE if the key was not processed through this method.
// Left, Up, Down, Right, and Enter are automatically passed through 
// OnUp(), OnDown(), etc.
//
// ----------------------------------------------------------------------- //
LTBOOL CBaseScreen::HandleKeyDown(int key, int rep)
{
	LTBOOL handled = LTFALSE;

	switch (key)
	{
	case VK_LEFT:
		{
			if (m_pCaptureCtrl)
				handled = m_pCaptureCtrl->OnLeft();
			else
				handled = OnLeft();
			break;
		}
	case VK_RIGHT:
		{
			if (m_pCaptureCtrl)
				handled = m_pCaptureCtrl->OnRight();
			else
				handled = OnRight();
			break;
		}
	case VK_UP:
		{
			if (m_pCaptureCtrl)
				handled = m_pCaptureCtrl->OnUp();
			else
				handled = OnUp();
			break;
		}
	case VK_DOWN:
		{
			if (m_pCaptureCtrl)
				handled = m_pCaptureCtrl->OnDown();
			else
				handled = OnDown();
			break;
		}
	case VK_RETURN:
		{
			if (m_pCaptureCtrl)
				handled = m_pCaptureCtrl->OnEnter();
			else
				handled = OnEnter();
			break;
		}
	default:
		{
			if (m_pCaptureCtrl)
				handled = m_pCaptureCtrl->HandleKeyDown(key,rep);
			else
			{
				CLTGUICtrl* pCtrl = GetSelectedControl();
				if (pCtrl)
				{
					handled = pCtrl->HandleKeyDown(key,rep);

// Sound removed for simplicity
//
//					if (handled && (key == VK_NEXT || key == VK_PRIOR))
//						g_pInterfaceMgr->RequestInterfaceSound(IS_CHANGE);
				}
				else
					handled = LTFALSE;
			}
			break;
		}
	}

	// Handled the key
	return handled;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::OnUp
//
//	PURPOSE:	Handles up key
//
//	Returns: LTBOOL - handled or not
//
// ----------------------------------------------------------------------- //
LTBOOL CBaseScreen::OnUp()
{
	CLTGUICtrl* pCtrl = GetSelectedControl();

	if (pCtrl && pCtrl->OnUp())
	{
		return LTTRUE;
	}

	uint16 sel = m_nSelection;

	return (sel != PreviousSelection());
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::OnDown
//
//	PURPOSE:	Handle down key
//
//	Returns: LTBOOL - handled or not 
//
// ----------------------------------------------------------------------- //
LTBOOL CBaseScreen::OnDown()
{
	CLTGUICtrl* pCtrl = GetSelectedControl();

	if (pCtrl && pCtrl->OnDown())
	{
		return LTTRUE;
	}

	uint16 sel = m_nSelection;
	return (sel != NextSelection());
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::OnLeft
//
//	PURPOSE:	Handle left key
//
//	Returns: LTBOOL - handled or not 
//
// ----------------------------------------------------------------------- //
LTBOOL CBaseScreen::OnLeft()
{
	LTBOOL handled = LTFALSE;

	CLTGUICtrl* pCtrl = GetSelectedControl();

	if (pCtrl)
		handled = pCtrl->OnLeft();

// Sound removed for simplicity
//	if (handled)
//		g_pInterfaceMgr->RequestInterfaceSound(IS_SELECT);

	return handled;

}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::OnRight
//
//	PURPOSE:	Handle left key
//
//	Returns: LTBOOL - handled or not 
//
// ----------------------------------------------------------------------- //
LTBOOL CBaseScreen::OnRight()
{
	LTBOOL handled = LTFALSE;

	CLTGUICtrl* pCtrl = GetSelectedControl();

	if (pCtrl)
		handled = pCtrl->OnRight();

// Sound removed for simplicity
//	if (handled)
//		g_pInterfaceMgr->RequestInterfaceSound(IS_SELECT);

	return handled;

}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::OnEnter
//
//	PURPOSE:	Handle enter key
//
//	Returns: LTBOOL - handled or not 
//
// ----------------------------------------------------------------------- //
LTBOOL CBaseScreen::OnEnter()
{
	LTBOOL handled = LTFALSE;

	CLTGUICtrl* pCtrl = GetSelectedControl();

	if (pCtrl)
	{
		handled = pCtrl->OnEnter();

// Sound removed for simplicity
//		if (handled)
//		{
//			if (pCtrl == &s_BackArrow || pCtrl == m_pNext)
//				g_pInterfaceMgr->RequestInterfaceSound(IS_PAGE);
//			else
//				g_pInterfaceMgr->RequestInterfaceSound(IS_SELECT);
//		}
	}
	return handled;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::NextSelection()
//
//	PURPOSE:	Next selection ( in a control )
//
//	Returns: uint16 - next selection index
//
// ----------------------------------------------------------------------- //
uint16 CBaseScreen::NextSelection()
{
	uint16 select = m_nSelection;

	if (select == kNoSelection)
		select = m_controlArray.size()-1;

	uint16 oldSelect = select;
	
	CLTGUICtrl* pCtrl = LTNULL;	

	do
	{
		select++;

		if (select >= m_controlArray.size())
		{
			select = 0;
		}
	
		pCtrl = GetControl(select);	

	} while (select != oldSelect && pCtrl && !pCtrl->IsEnabled() );


	if (!pCtrl || !pCtrl->IsEnabled() )
		select = m_nSelection;

	return SetSelection(select);

}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::PreviousSelection()
//
//	PURPOSE:	Previous selection ( in a control )
//
//	Returns: uint16 - previous selection index
//
// ----------------------------------------------------------------------- //
uint16 CBaseScreen::PreviousSelection()
{
	uint16 select = m_nSelection;

	if (select == kNoSelection)
		select = 0;

	uint16 oldSelect = select;
	
	CLTGUICtrl* pCtrl = LTNULL;	

	do
	{
		if (select == 0)
		{
			select = m_controlArray.size()-1;
		}
		else
			select--;
	
		pCtrl = GetControl(select);	

	} while (select != oldSelect && pCtrl && !pCtrl->IsEnabled() );


	if (!pCtrl || !pCtrl->IsEnabled() )
		select = m_nSelection;

	return SetSelection(select);

}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::OnLButtonDown()
//
//	PURPOSE: Handle left mouse button down message
//
//	Returns: LTBOOL - handled or not
//
// ----------------------------------------------------------------------- //
LTBOOL CBaseScreen::OnLButtonDown(int x, int y)
{
	// Get the control that the click was on
	uint16 nControlIndex=0;

	if (GetControlUnderPoint(x, y, &nControlIndex))
	{
		CLTGUICtrl* pCtrl = GetControl(nControlIndex);
		if (m_pCaptureCtrl && pCtrl != m_pCaptureCtrl)
            return LTFALSE;

		// Select the control
		SetSelection(nControlIndex);

		// Record this control as the one being selected from the mouse click.
		// If the mouse is still over it on the UP message, then the "enter" message will be sent.
		m_nLMouseDownItemSel=nControlIndex;
		return pCtrl->OnLButtonDown(x,y);

	}
	else
		m_nLMouseDownItemSel=kNoSelection;

    return LTFALSE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::OnLButtonUp()
//
//	PURPOSE: Handle left mouse button up message
//
//	Returns: LTBOOL - handled or not
//
// ----------------------------------------------------------------------- //
LTBOOL CBaseScreen::OnLButtonUp(int x, int y)
{
	// Get the control that the click was on
	uint16 nControlIndex=0;

	if (GetControlUnderPoint(x, y, &nControlIndex))
	{
		CLTGUICtrl* pCtrl = GetControl(nControlIndex);
		if (m_pCaptureCtrl && pCtrl != m_pCaptureCtrl)
            return LTFALSE;

		// If the mouse is over the same control now as it was when the down message was called
		// then send the "enter" message to the control.
		if (nControlIndex == m_nLMouseDownItemSel)
		{
			if (pCtrl->IsEnabled() )
			{
				SetSelection(nControlIndex);
				LTBOOL bHandled = pCtrl->OnLButtonUp(x,y);

// Sound removed for simplicity
//				if (bHandled)
//				{
//					if (pCtrl == &s_BackArrow || pCtrl == m_pNext)
//						g_pInterfaceMgr->RequestInterfaceSound(IS_PAGE);
//					else
//						g_pInterfaceMgr->RequestInterfaceSound(IS_SELECT);
//				}

				return bHandled;

			}
		}
	}
	else
	{
		m_nLMouseDownItemSel= kNoSelection;
	}

	return LTFALSE;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::OnRButtonDown()
//
//	PURPOSE: Handle right mouse button down message
//
//	Returns: LTBOOL - handled or not
//
// ----------------------------------------------------------------------- //
LTBOOL CBaseScreen::OnRButtonDown(int x, int y)
{
	// Get the control that the click was on
	uint16 nControlIndex=0;
	if (GetControlUnderPoint(x, y, &nControlIndex))
	{
		CLTGUICtrl* pCtrl = GetControl(nControlIndex);
		if (m_pCaptureCtrl && pCtrl != m_pCaptureCtrl)
            return LTFALSE;

		// Select the control
		SetSelection(nControlIndex);

		// Record this control as the one being selected from the mouse click.
		// If the mouse is still over it on the UP message, then the "enter" message will be sent.
		m_nRMouseDownItemSel=nControlIndex;

		return pCtrl->OnRButtonDown(x,y);
	}
	else
		m_nRMouseDownItemSel=kNoSelection;

    return LTFALSE;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::OnRButtonUp()
//
//	PURPOSE: Handle right mouse button up message
//
//	Returns: LTBOOL - handled or not
//
// ----------------------------------------------------------------------- //
LTBOOL CBaseScreen::OnRButtonUp(int x, int y)
{
	// Get the control that the click was on
	uint16 nControlIndex=0;

	if (GetControlUnderPoint(x, y, &nControlIndex))
	{
		CLTGUICtrl* pCtrl = GetControl(nControlIndex);

		if (m_pCaptureCtrl && pCtrl != m_pCaptureCtrl)
            return LTFALSE;

		// If the mouse is over the same control now as it was when the down message was called
		// then send the "left" message to the control.
		if (nControlIndex == m_nRMouseDownItemSel)
		{
			if (pCtrl->IsEnabled())
			{
				SetSelection(nControlIndex);

				LTBOOL bHandled = pCtrl->OnRButtonUp(x,y);

// Sound removed for simplicity
//				if (pCtrl == &s_BackArrow || pCtrl == m_pNext)
//					g_pInterfaceMgr->RequestInterfaceSound(IS_PAGE);
//				else
//					g_pInterfaceMgr->RequestInterfaceSound(IS_SELECT);

				return bHandled;
			}
		}
	}
	else
	{
		m_nRMouseDownItemSel= kNoSelection;
	}

	return LTFALSE;

}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::OnLButtonDblClick()
//
//	PURPOSE: Handle left mouse button double click message
//
//	Returns: LTBOOL - handled or not
//
// ----------------------------------------------------------------------- //
LTBOOL CBaseScreen::OnLButtonDblClick(int x, int y)
{
	CLTGUICtrl* pCtrl = GetSelectedControl();

	if (m_pCaptureCtrl && pCtrl != m_pCaptureCtrl)
		return LTFALSE;

	if (pCtrl)
		return pCtrl->OnLButtonDblClick(x, y);
	else
		return LTFALSE;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::OnRButtonDblClick()
//
//	PURPOSE: Handle right mouse button double click message
//
//	Returns: LTBOOL - handled or not
//
// ----------------------------------------------------------------------- //
LTBOOL CBaseScreen::OnRButtonDblClick(int x, int y)
{
	CLTGUICtrl* pCtrl = GetSelectedControl();

	if (m_pCaptureCtrl && pCtrl != m_pCaptureCtrl)
		return LTFALSE;

	if (pCtrl)
		return pCtrl->OnRButtonDblClick(x, y);
	else
        return LTFALSE;
}




// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::OnMouseMove()
//
//	PURPOSE: Handle mouse move message
//
//	Returns: LTBOOL - handled or not
//
// ----------------------------------------------------------------------- //
LTBOOL CBaseScreen::OnMouseMove(int x, int y)
{
	uint16 nControlUnderPoint=kNoSelection;
	LTBOOL onCtrl = GetControlUnderPoint(x,y,&nControlUnderPoint);

	if (onCtrl)
	{
		CLTGUICtrl* pCtrl = GetControl(nControlUnderPoint);

		if (m_pCaptureCtrl && m_pCaptureCtrl != pCtrl)
			return LTFALSE;

		if (pCtrl->OnMouseMove(x,y))
		{
// Sound removed for simplicity
//			g_pInterfaceMgr->RequestInterfaceSound(IS_CHANGE);
//			UpdateHelpText();
		}

	}
	else if (m_pCaptureCtrl)
		return LTFALSE;

	if (onCtrl)
	{
		if (GetSelection() != nControlUnderPoint)
		{

			if (GetControl(nControlUnderPoint)->IsEnabled())
			{
				SetSelection(nControlUnderPoint);
			}
		}
		return LTTRUE;
	}

	return LTFALSE;

}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::SetSelection()
//
//	PURPOSE: Set control selection 
//
//	Returns: uint16 - control id or no selection
//
// ----------------------------------------------------------------------- //
uint16 CBaseScreen::SetSelection(uint16 select, LTBOOL bFindSelectable)
{
	if (select == m_nSelection) return select;

	int nOldSelect=m_nSelection;

	if (select == kNoSelection)
	{
		if (nOldSelect != kNoSelection)
		{
			GetControl(nOldSelect)->Select(LTFALSE);
			OnSelectionChange();
		}

		m_nOldSelection = m_nSelection;
		m_nSelection = kNoSelection;

		return kNoSelection;
	}


	CLTGUICtrl *pSelCtrl;


	if (select >= 0)
	{
		if (select >= m_controlArray.size())
			select = m_controlArray.size()-1;
	}


	pSelCtrl = GetControl(select);
	if (!pSelCtrl)
	{
		return nOldSelect;
	}

	//check to see if we can select this item
	if (!pSelCtrl->IsEnabled())
	{
		//if we don't need to find a selectable item return
		if (!bFindSelectable)
		{
			return nOldSelect;
		}

		//keep looking until we run out of on screen items or find a selectable one
		while (pSelCtrl && !pSelCtrl->IsEnabled())
		{
			select++;
			pSelCtrl = GetControl(select);
		}

		if (!pSelCtrl || !pSelCtrl->IsEnabled())
		{
			return nOldSelect;
		}
	}


	if (nOldSelect != kNoSelection)
	{
		GetControl(nOldSelect)->Select(LTFALSE);
	}

	m_nOldSelection = m_nSelection;
	m_nSelection = select;

	if (m_nSelection == kNoSelection)
	{
		return nOldSelect;
	}

	LTIntPt pos = pSelCtrl->GetPos();

	pos.y += (pSelCtrl->GetHeight() / 2);

	pSelCtrl->Select(LTTRUE);

	OnSelectionChange();

	return m_nSelection;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::GetControlUnderPoint()
//
//	PURPOSE: Get the control given a x,y 
//
//	Returns: LTBOOL - success or failure and control index if success
//
// ----------------------------------------------------------------------- //
LTBOOL CBaseScreen::GetControlUnderPoint(int xPos, int yPos, uint16 *pnIndex)
{
	if ( !pnIndex )
		return LTFALSE;

	if (m_pCaptureCtrl && m_pCaptureCtrl->IsOnMe(xPos,yPos))
	{
		*pnIndex = GetIndex(m_pCaptureCtrl);
		return LTTRUE;
	}


	// See if the user clicked on any of the controls.
	for (uint16 i=0; i < m_controlArray.size() ; i++)
	{
		//start with last control
		int ndx = (m_controlArray.size()-1) - i;

		// Check to see if the click is in the bounding box for the control
		if (m_controlArray[i]->IsOnMe(xPos,yPos) && m_controlArray[i]->IsEnabled())
		{
			*pnIndex=i;

			return LTTRUE;
		}
	}

	return LTFALSE;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::GetControl()
//
//	PURPOSE: Given an index return the control pointer
//
//	Returns: CLTGUICtrl * - NULL failure or the pointer to the control
//
// ----------------------------------------------------------------------- //
CLTGUICtrl *CBaseScreen::GetControl ( uint16 nIndex )
{
	if (nIndex < m_controlArray.size() )
		return m_controlArray[nIndex];

	return LTNULL;
}






// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::RemoveAll()
//
//	PURPOSE: Remove controls - optionaly deleting them 
//
//	Returns: n/a
//
// ----------------------------------------------------------------------- //
void CBaseScreen::RemoveAll(LTBOOL bDelete)
{
	RemoveControl(&s_Back, LTFALSE);

	// Terminate the ctrls
	if (bDelete)
	{
		for (uint16 i=0; i < m_controlArray.size(); i++)
		{
			m_controlArray[i]->Destroy();
			debug_delete(m_controlArray[i]);
		}
	}

	m_controlArray.clear();

	if (m_nSelection >= 0)
		m_nSelection = kNoSelection;

	m_nextPos.x = GetPageLeft();
	m_nextPos.y = GetPageTop();

}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::AddControl()
//
//	PURPOSE: Add a control 
//
//	Returns: control id or kNoSelection if failure
//
// ----------------------------------------------------------------------- //
uint16 CBaseScreen::AddControl(CLTGUICtrl* pCtrl)
{
	m_controlArray.push_back(pCtrl);

	uint16 num = m_controlArray.size();

	if (num == m_nSelection+1)
		pCtrl->Select(LTTRUE);

	if (num > 0)
		return num-1;
	else
		return kNoSelection;

}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CBaseScreen::UpdateData()
//
//	PURPOSE: Calls UpdateData on each control in the menu
//
//	Returns: n/a
//
// ----------------------------------------------------------------------- //
void CBaseScreen::UpdateData(LTBOOL bSaveAndValidate)
{
	for (uint16 i=0; i < m_controlArray.size(); i++)
	{
		m_controlArray[i]->UpdateData(bSaveAndValidate);
	}
}





CLTGUIEditCtrl* CBaseScreen::CreateEditItem( uint16 nMaxLength, uint32 nCommandID, wchar_t * pStringValue, LTIntPt pos, int nFont)
{
	CLTGUIEditCtrl* pCtrl=debug_new(CLTGUIEditCtrl);

	if (nFont < 0)
	{
		nFont	= g_pCShell->GetLayoutMgr()->GetScreenFontFace((eScreenID)m_nScreenID);
	}

	CFontInfo*	pFont	= g_pInterfaceResMgr->GetFont(nFont);
	uint8	nFontSize =	g_pCShell->GetLayoutMgr()->GetScreenFontSize((eScreenID)m_nScreenID);

	if (!pCtrl->Create( g_pLTClient, nCommandID, pFont, nFontSize, nMaxLength, this, pStringValue ))
	{
		debug_delete(pCtrl);
		return	LTNULL;
	}

	if	(pos.x <	0 && pos.y < 0)
	{
		pos =	m_nextPos;
	}
	else
	{
		m_nextPos =	pos;
	}

	pCtrl->SetBasePos(pos);

	m_nextPos.y	+=	(pCtrl->GetBaseHeight()	+ m_nItemSpacing);

	pCtrl->SetScale(g_pInterfaceResMgr->GetXRatio());

	pCtrl->SetColors(m_SelectedColor,m_NonSelectedColor,m_DisabledColor);

	return pCtrl;



}


CLTGUITextCtrl* CBaseScreen::CreateTextItem(int	stringID, uint32 commandID, int helpID, LTIntPt	pos, LTBOOL	bFixed, int	nFont)
{
	CLTGUITextCtrl* pCtrl=CreateTextItem(LoadTempString(stringID),	commandID, helpID, pos,	bFixed, nFont);
	return pCtrl;

}

CLTGUITextCtrl* CBaseScreen::CreateTextItem(wchar_t *pString, uint32	commandID, int	helpID, LTIntPt pos,	LTBOOL bFixed,	int nFont)
{
	CLTGUITextCtrl* pCtrl=debug_new(CLTGUITextCtrl);

	if (nFont < 0)
	{
		nFont	= g_pCShell->GetLayoutMgr()->GetScreenFontFace((eScreenID)m_nScreenID);
	}

	CFontInfo*	pFont	= g_pInterfaceResMgr->GetFont(nFont);
	uint8	nFontSize =	g_pCShell->GetLayoutMgr()->GetScreenFontSize((eScreenID)m_nScreenID);


	if (!pCtrl->Create(pString, commandID, helpID,	pFont, nFontSize,	this))
	{
		debug_delete(pCtrl);
		return	LTNULL;
	}

	if	(pos.x <	0 && pos.y < 0)
	{
		pos =	m_nextPos;
	}
	else
	{
		m_nextPos =	pos;
	}

	pCtrl->SetBasePos(pos);

	m_nextPos.y	+=	(pCtrl->GetBaseHeight()	+ m_nItemSpacing);

	pCtrl->SetScale(g_pInterfaceResMgr->GetXRatio());


	if	(bFixed)
	{
		pCtrl->SetColors(m_NonSelectedColor,m_NonSelectedColor,m_NonSelectedColor);
		pCtrl->Enable(LTFALSE);
	}
	else
	{
		pCtrl->SetColors(m_SelectedColor,m_NonSelectedColor,m_DisabledColor);
	}

	return pCtrl;

}


CLTGUITextCtrl* CBaseScreen::CreateTextItem(int	stringID, uint32 commandID, int helpID, char * pPos, char * pSize, eTextAlign halign, LTBOOL	bFixed, int	nFont )
{
	LTIntPt pos = kDefaultPos;
	uint8 size = 0;
	CLTGUIPolyString* pPoly = NULL;

	if ( pPos )
	{
		pos = g_pCShell->GetLayoutMgr()->GetScreenCustomPoint((eScreenID)m_nScreenID, pPos );
	}

	CLTGUITextCtrl* pCtrl=CreateTextItem(LoadTempString(stringID),	commandID, helpID, pos,	bFixed, nFont);

	if ( pCtrl )
	{

		if ( pSize )
		{
			size = (uint8)g_pCShell->GetLayoutMgr()->GetScreenCustomInt((eScreenID)m_nScreenID, pSize );
			pCtrl->SetFont(NULL,size);
		}

		if ( halign != kLeft )
		{
			pPoly = pCtrl->GetString();
			pPoly->SetAlignment(halign);
		}
	}

	return pCtrl;
}


CLTGUITextCtrl* CBaseScreen::CreateTextItemRect(int	stringID, uint32 commandID, int helpID, char * pRect, char * pSize, eTextAlign halign, LTBOOL	bFixed, int	nFont )
{
	LTIntPt pos = kDefaultPos;
	uint8 size = 0;
	CLTGUIPolyString* pPoly = NULL;
	int nWd = 10;
	int nHt = 10;

	if ( pRect )
	{
		LTRect frameRect = g_pCShell->GetLayoutMgr()->GetScreenCustomRect((eScreenID)m_nScreenID, pRect );
		pos.x = frameRect.left;
		pos.y = frameRect.top;
		nHt = frameRect.bottom - frameRect.top;
		nWd = frameRect.right - frameRect.left;

		pos.x+=8;
		pos.y+=8;
	}

	CLTGUITextCtrl* pCtrl=CreateTextItem(LoadTempString(stringID), commandID, helpID, pos, bFixed, nFont);

	if ( pCtrl )
	{
		pCtrl->SetFixedWidth( nWd );

		if ( pSize )
		{
			size = (uint8)g_pCShell->GetLayoutMgr()->GetScreenCustomInt((eScreenID)m_nScreenID, pSize );
			pCtrl->SetFont(NULL,size);
		}

		if ( halign != kLeft )
		{
			pPoly = pCtrl->GetString();
			pPoly->SetAlignment(halign);
		}
	}

	return pCtrl;
}




CLTGUIListCtrl* CBaseScreen::CreateList(LTIntPt	pos, uint16	nHeight,	LTBOOL bUseArrows, uint16 nArrowOffset)
{

	CLTGUIListCtrl* pList=debug_new(CLTGUIListCtrl);
	if (pList->Create(nHeight))
	{
		pList->SetBasePos(pos);
		pList->SetScale(g_pInterfaceResMgr->GetXRatio());
		if	(bUseArrows)
		{
			HTEXTURE	hUp =	g_pInterfaceResMgr->GetTexture("interface\\arrowup.dtx");
			HTEXTURE	hUpH = g_pInterfaceResMgr->GetTexture("interface\\arrowup_h.dtx");
			HTEXTURE	hDown	= g_pInterfaceResMgr->GetTexture("interface\\arrowdn.dtx");
			HTEXTURE	hDownH =	g_pInterfaceResMgr->GetTexture("interface\\arrowdn_h.dtx");
			pList->UseArrows(nArrowOffset,1.0f,hUp,hUpH,hDown,hDownH);
		}
		pList->SetColors(m_SelectedColor,m_NonSelectedColor,m_DisabledColor);
	}

	return pList;
}



CLTGUICycleCtrl* CBaseScreen::CreateCycle(int stringID, int	helpID, int	nHeaderWidth, uint8 *pnValue,	LTIntPt pos, LTBOOL bFixed, int nFont)
{
	CLTGUICycleCtrl* pCtrl=CreateCycle(LoadTempString(stringID), helpID,	nHeaderWidth, pnValue, pos, bFixed,	nFont);

	return pCtrl;

}

CLTGUICycleCtrl* CBaseScreen::CreateCycle(wchar_t *pString,	int helpID,	int nHeaderWidth,	uint8	*pnValue, LTIntPt	pos, LTBOOL	bFixed, int	nFont)
{
	CLTGUICycleCtrl* pCtrl=debug_new(CLTGUICycleCtrl);

	if (nFont < 0)
	{
		nFont	= g_pCShell->GetLayoutMgr()->GetScreenFontFace((eScreenID)m_nScreenID);
	}

	CFontInfo*	pFont	= g_pInterfaceResMgr->GetFont(nFont);
	uint8	nFontSize =	g_pCShell->GetLayoutMgr()->GetScreenFontSize((eScreenID)m_nScreenID);


	if (!pCtrl->Create(pString, helpID, pFont, nFontSize, nHeaderWidth,	pnValue))
	{
		debug_delete(pCtrl);
		return	LTNULL;
	}

	if	(pos.x <	0 && pos.y < 0)
	{
		pos =	m_nextPos;
	}
	else
	{
		m_nextPos =	pos;
	}

	pCtrl->SetBasePos(pos);

	m_nextPos.y	+=	(pCtrl->GetBaseHeight()	+ m_nItemSpacing);

	pCtrl->SetScale(g_pInterfaceResMgr->GetXRatio());


	if	(bFixed)
	{
		pCtrl->SetColors(m_NonSelectedColor,m_NonSelectedColor,m_NonSelectedColor);
		pCtrl->Enable(LTFALSE);
	}
	else
	{
		pCtrl->SetColors(m_SelectedColor,m_NonSelectedColor,m_DisabledColor);
	}

	return pCtrl;

}





CLTGUIToggle* CBaseScreen::CreateToggle(int stringID,	int helpID,	int nHeaderWidth,	LTBOOL *pbValue, LTIntPt pos,	LTBOOL bFixed,	int nFont)
{

	CLTGUIToggle* pCtrl=CreateToggle(LoadTempString(stringID), helpID, nHeaderWidth,	pbValue,	pos, bFixed, nFont);

	return pCtrl;

}

CLTGUIToggle* CBaseScreen::CreateToggle(wchar_t	*pString, int helpID, int nHeaderWidth, LTBOOL *pbValue,	LTIntPt pos, LTBOOL bFixed, int nFont)
{
	CLTGUIToggle* pCtrl=debug_new(CLTGUIToggle);

	if (nFont < 0)
	{
		nFont	= g_pCShell->GetLayoutMgr()->GetScreenFontFace((eScreenID)m_nScreenID);
	}

	CFontInfo*	pFont	= g_pInterfaceResMgr->GetFont(nFont);
	uint8	nFontSize =	g_pCShell->GetLayoutMgr()->GetScreenFontSize((eScreenID)m_nScreenID);


	if (!pCtrl->Create(pString, helpID, pFont, nFontSize, nHeaderWidth,	pbValue))
	{
		debug_delete(pCtrl);
		return	LTNULL;
	}

	if	(pos.x <	0 && pos.y < 0)
	{
		pos =	m_nextPos;
	}
	else
	{
		m_nextPos =	pos;
	}

	pCtrl->SetBasePos(pos);

	m_nextPos.y	+=	(pCtrl->GetBaseHeight()	+ m_nItemSpacing);

	pCtrl->SetScale(g_pInterfaceResMgr->GetXRatio());

	pCtrl->SetOnString(LoadTempString(IDS_ON));
	pCtrl->SetOffString(LoadTempString(IDS_OFF));

	if	(bFixed)
	{
		pCtrl->SetColors(m_NonSelectedColor,m_NonSelectedColor,m_NonSelectedColor);
		pCtrl->Enable(LTFALSE);
	}
	else
	{
		pCtrl->SetColors(m_SelectedColor,m_NonSelectedColor,m_DisabledColor);
	}

	return pCtrl;

}


CLTGUISlider* CBaseScreen::CreateSlider(int stringID,	int helpID,	int nHeaderWidth,	int nBarWidth,
													 int nBarHeight, int *pnValue,	LTIntPt pos, LTBOOL bFixed, int nFont)
{
	CLTGUISlider* pCtrl=CreateSlider(LoadTempString(stringID), helpID, nHeaderWidth,	nBarWidth, nBarHeight, pnValue, pos, bFixed,	nFont);

	return pCtrl;

}

CLTGUISlider* CBaseScreen::CreateSlider(wchar_t	*pString, int helpID, int nHeaderWidth, int nBarWidth,
													 int nBarHeight, int *pnValue,	LTIntPt pos, LTBOOL bFixed, int nFont)
{
	CLTGUISlider* pCtrl=debug_new(CLTGUISlider);

	if (nFont < 0)
	{
		nFont	= g_pCShell->GetLayoutMgr()->GetScreenFontFace((eScreenID)m_nScreenID);
	}

	CFontInfo*	pFont	= g_pInterfaceResMgr->GetFont(nFont);
	uint8	nFontSize =	g_pCShell->GetLayoutMgr()->GetScreenFontSize((eScreenID)m_nScreenID);

	char szTex[128];
	g_pCShell->GetLayoutMgr()->GetScreenSliderTex((eScreenID)m_nScreenID, szTex, 128);
	HTEXTURE	hBar = g_pInterfaceResMgr->GetTexture(szTex);

	//check for the highlight texture 
	g_pCShell->GetLayoutMgr()->GetScreenSliderTexH((eScreenID)m_nScreenID, szTex, 128);
	HTEXTURE	hBarH = g_pInterfaceResMgr->GetTexture(szTex);


	if	(nBarHeight	< 0)
	{
		uint32 w,h;
		g_pLTCTexInterface->GetTextureDims(hBar,w,h);
		nBarHeight = h	/ 3;

	}


	if (!pCtrl->Create(pString, helpID, pFont, nFontSize, hBar, nHeaderWidth,	nBarWidth, nBarHeight, pnValue))
	{
		debug_delete(pCtrl);
		return	LTNULL;
	}


	if ( hBarH )
		pCtrl->SetHighlightTexture(hBarH);

	if	(pos.x <	0 && pos.y < 0)
	{
		pos =	m_nextPos;
	}
	else
	{
		m_nextPos =	pos;
	}

	pCtrl->SetBasePos(pos);

	m_nextPos.y	+=	(pCtrl->GetBaseHeight()	+ m_nItemSpacing);

	pCtrl->SetScale(g_pInterfaceResMgr->GetXRatio());


	if	(bFixed)
	{
		pCtrl->SetColors(m_NonSelectedColor,m_NonSelectedColor,m_NonSelectedColor);
		pCtrl->Enable(LTFALSE);
	}
	else
	{
		pCtrl->SetColors(m_SelectedColor,m_NonSelectedColor,m_DisabledColor);
	}

	return pCtrl;

}


CLTGUIColumnCtrl*	CBaseScreen::CreateColumnCtrl(uint32 commandID,	int helpID,	LTIntPt pos, LTBOOL bFixed, int nFont)
{
	CLTGUIColumnCtrl*	pCtrl=debug_new(CLTGUIColumnCtrl);

	if (nFont < 0)
	{
		nFont	= g_pCShell->GetLayoutMgr()->GetScreenFontFace((eScreenID)m_nScreenID);
	}

	CFontInfo*	pFont	= g_pInterfaceResMgr->GetFont(nFont);
	uint8	nFontSize =	g_pCShell->GetLayoutMgr()->GetScreenFontSize((eScreenID)m_nScreenID);


	if (!pCtrl->Create(commandID, helpID,	pFont, nFontSize,	this))
	{
		debug_delete(pCtrl);
		return	LTNULL;
	}

	if	(pos.x <	0 && pos.y < 0)
	{
		pos =	m_nextPos;
	}
	else
	{
		m_nextPos =	pos;
	}

	pCtrl->SetBasePos(pos);

	m_nextPos.y	+=	(nFontSize + m_nItemSpacing);

	pCtrl->SetScale(g_pInterfaceResMgr->GetXRatio());


	if	(bFixed)
	{
		pCtrl->SetColors(m_NonSelectedColor,m_NonSelectedColor,m_NonSelectedColor);
		pCtrl->Enable(LTFALSE);
	}
	else
	{
		pCtrl->SetColors(m_SelectedColor,m_NonSelectedColor,m_DisabledColor);
	}

	return pCtrl;

}

//
//------ Create Buttons
//


CLTGUIButton* CBaseScreen::CreateButton(int stringID,	uint32 commandID, int helpID, HTEXTURE hNormal, HTEXTURE hSelected, HTEXTURE hDisabled,  LTIntPt pos, LTBOOL bFixed, int nFont )
{
	CLTGUIButton* pCtrl=CreateButton(LoadTempString(stringID), commandID, helpID, hNormal, hSelected, hDisabled, pos, bFixed, nFont );

	return pCtrl;

}

CLTGUIButton* CBaseScreen::CreateButton(wchar_t	*pString, uint32 commandID, int helpID, HTEXTURE hNormal, HTEXTURE hSelected, HTEXTURE hDisabled, LTIntPt pos, LTBOOL bFixed, int nFont  )
{
	CLTGUIButton *  pCtrl=debug_new(CLTGUIButton);

	if (nFont < 0)
	{
		nFont	= g_pCShell->GetLayoutMgr()->GetScreenFontFace((eScreenID)m_nScreenID);
	}

	CFontInfo*	pFont	= g_pInterfaceResMgr->GetFont(nFont);
	uint8	nFontSize =	g_pCShell->GetLayoutMgr()->GetScreenFontSize((eScreenID)m_nScreenID);

	if (!pCtrl->Create(commandID,  helpID,	hNormal, hSelected, hDisabled, this))
	{
		debug_delete(pCtrl);
		return	LTNULL;
	}

	pCtrl->SetFont(pFont,nFontSize);
	pCtrl->SetText(pString,LTTRUE);

	if	(pos.x <	0 && pos.y < 0)
	{
		pos =	m_nextPos;
	}
	else
	{
		m_nextPos =	pos;
	}

	pCtrl->SetBasePos(pos);

	m_nextPos.y	+=	(pCtrl->GetBaseHeight()	+ m_nItemSpacing);

	pCtrl->SetScale(g_pInterfaceResMgr->GetXRatio());

	if	(bFixed)
	{
		pCtrl->SetColors(m_NonSelectedColor,m_NonSelectedColor,m_NonSelectedColor);
		pCtrl->Enable(LTFALSE);
	}
	else
	{
		pCtrl->SetColors(m_SelectedColor,m_NonSelectedColor,m_DisabledColor);
	}

	return pCtrl;

}

CLTGUIButton *		CBaseScreen::CreateButton(int stringID, uint32 commandID, int helpID, char * pNormal, char * pSelected, char * pDisabled, char * pPos, char * pSize, eTextAlign halign, LTBOOL bFixed, int nFont)
{
	LTIntPt pos = kDefaultPos;
	uint8 size = 0;
	CLTGUIPolyString* pPoly = NULL;
	HTEXTURE hNormal = NULL;
	HTEXTURE hSelected = NULL;
	HTEXTURE hDisabled = NULL;
	char szTemp[256] = "";

	if ( pPos )
	{
		pos = g_pCShell->GetLayoutMgr()->GetScreenCustomPoint((eScreenID)m_nScreenID, pPos );
	}

	if ( pNormal )
	{
		g_pCShell->GetLayoutMgr()->GetScreenCustomString((eScreenID)m_nScreenID, pNormal, szTemp, sizeof(szTemp));
		hNormal = g_pInterfaceResMgr->GetTexture( szTemp );
	}

	if ( pSelected )
	{
		g_pCShell->GetLayoutMgr()->GetScreenCustomString((eScreenID)m_nScreenID, pSelected, szTemp, sizeof(szTemp));
		hSelected = g_pInterfaceResMgr->GetTexture( szTemp );
	}

	if ( pDisabled )
	{
		g_pCShell->GetLayoutMgr()->GetScreenCustomString((eScreenID)m_nScreenID, pDisabled, szTemp, sizeof(szTemp));
		hDisabled = g_pInterfaceResMgr->GetTexture( szTemp );
	}

	CLTGUIButton* pCtrl=CreateButton(LoadTempString(stringID), commandID, helpID, hNormal, hSelected, hDisabled, pos, bFixed, nFont );

	if ( pCtrl )
	{

		if ( pSize )
		{
			size = (uint8)g_pCShell->GetLayoutMgr()->GetScreenCustomInt((eScreenID)m_nScreenID, pSize );
			pCtrl->SetFont(NULL,size);
		}

		if ( halign != kLeft )
		{
			pPoly = pCtrl->GetString();
			pPoly->SetAlignment(halign);
		}
	}

	return pCtrl;

}




CLTGUIButton *		CBaseScreen::CreateButton(int stringID, uint32 commandID, int helpID, HTEXTURE hNormal, HTEXTURE hSelected, HTEXTURE hDisabled, char * pPos, char * pSize, eTextAlign halign, LTBOOL bFixed, int nFont)
{
	LTIntPt pos = kDefaultPos;
	uint8 size = 0;
	CLTGUIPolyString* pPoly = NULL;

	if ( pPos )
	{
		pos = g_pCShell->GetLayoutMgr()->GetScreenCustomPoint((eScreenID)m_nScreenID, pPos );
	}

	CLTGUIButton* pCtrl=CreateButton(LoadTempString(stringID), commandID, helpID, hNormal, hSelected, hDisabled, pos, bFixed, nFont );

	if ( pCtrl )
	{

		if ( pSize )
		{
			size = (uint8)g_pCShell->GetLayoutMgr()->GetScreenCustomInt((eScreenID)m_nScreenID, pSize );
			pCtrl->SetFont(NULL,size);
		}

		if ( halign != kLeft )
		{
			pPoly = pCtrl->GetString();
			pPoly->SetAlignment(halign);
		}
	}

	return pCtrl;

}



void CBaseScreen::UseBack(LTBOOL bBack,LTBOOL bOK,LTBOOL bReturn)
{
	if (bBack)
	{
		CreateBack(bOK,bReturn);

		if (GetIndex(&s_Back) >= m_controlArray.size())
			AddControl(&s_Back);
	}
	else
	{
		RemoveControl(&s_Back,LTFALSE);
	}

	m_bBack = bBack;
}


void CBaseScreen::CreateBack(LTBOOL bOK, LTBOOL bReturn)
{

	int nStr = 0;
	int nHelp = 0;
	if (bOK)
	{
		nStr = IDS_OK;
		nHelp = IDS_HELP_NONE;
	}
	else
	{
		nStr = IDS_BACK;
		nHelp = IDS_HELP_NONE;
	}

	uint8 nFont = g_pCShell->GetLayoutMgr()->GetBackFont();
	uint8 nFontSize = g_pCShell->GetLayoutMgr()->GetBackSize();

	s_Back.SetFont(g_pInterfaceResMgr->GetFont(nFont),nFontSize);
	s_Back.SetHelpID(nHelp);
	s_Back.SetText(LoadTempString(nStr),LTTRUE);
	s_Back.SetColors(m_SelectedColor,m_NonSelectedColor,m_DisabledColor);

	if (bReturn)
		s_Back.SetHelpID(IDS_HELP_NONE);
	else if (bOK)
		s_Back.SetHelpID(IDS_HELP_NONE);
	else
		s_Back.SetHelpID(IDS_HELP_NONE);

	s_Back.SetCommandHandler(this);

}


void CBaseScreen::RemoveControl(CLTGUICtrl* pControl,LTBOOL bDelete)
{
	if (!pControl) return;

	ControlArray::iterator iter = m_controlArray.begin();

	while (iter != m_controlArray.end() && (*iter) != pControl)
		iter++;

	if (iter != m_controlArray.end())
	{
		m_controlArray.erase(iter);
	}

	if (bDelete && pControl != &s_Back)
	{
		debug_delete(pControl);
	}


}

uint32 CBaseScreen::OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2)
{
	switch(dwCommand)
	{
	case CMD_BACK:
		{
			m_pScreenMgr->EscapeCurrentScreen();
			break;
		}
	case CMD_MAIN:
		{
			m_pScreenMgr->SetCurrentScreen(SCREEN_ID_MAIN);
			break;
		}
	case CMD_CONTINUE:
		{
			if (m_nContinueID != SCREEN_ID_NONE)
			{
				m_pScreenMgr->SetCurrentScreen((eScreenID)m_nContinueID);
				return 1;
			}
			else
				return 0;
		} break;

	default:
		return 0;
	}

	return 1;
}


void CBaseScreen::ForceMouseUpdate()
{
//	SetSelection(kNoSelection);
	LTIntPt cPos = g_pCursorMgr->GetCursorPos();
	OnMouseMove(cPos.x,cPos.y);
}


// This is called when the screen gets or loses focus
void CBaseScreen::OnFocus(LTBOOL bFocus)
{
    m_pCaptureCtrl = LTNULL;

	if (bFocus)
	{
		if (m_fLastScale != g_pInterfaceResMgr->GetXRatio())
		{
			ScreenDimsChanged();
		}
		m_nOldSelection = kNoSelection;
		if (m_nSelection == kNoSelection)
		{
			if (m_pNext)
				SetSelection(GetIndex(m_pNext));
			else
			{
				SetSelection(0,LTTRUE);
				if (m_nSelection == kNoSelection && m_bBack)
				{
					SetSelection(GetIndex(&s_Back));
				}
		
			}
		}

		
		ForceMouseUpdate();
		m_bHaveFocus = LTTRUE;
		m_bVisited = LTTRUE; //set this last
	}
	else
	{
		SetSelection(kNoSelection);
		m_bHaveFocus = LTFALSE;
	}
}




uint16 CBaseScreen::GetIndex(CLTGUICtrl* pCtrl)
{
	ControlArray::iterator iter = m_controlArray.begin();

	uint16 dwIndex = 0;
	while (iter != m_controlArray.end() && (*iter) != pCtrl)
	{
		++dwIndex;
		iter++;
	}
	if (dwIndex < m_controlArray.size())
		return dwIndex;
	return kNoSelection;
}




void CBaseScreen::ScreenDimsChanged()
{
	m_fLastScale = g_pInterfaceResMgr->GetXRatio();
	unsigned int i;
	for ( i = 0; i < m_controlArray.size(); i++ )
	{
		m_controlArray[i]->SetScale(g_pInterfaceResMgr->GetXRatio());
	}

	if (!m_TitleString.IsEmpty())
	{
		LTIntPt pos = m_TitlePos;
		g_pInterfaceResMgr->ConvertScreenPos(pos);
		m_TitleString.SetPos(pos);
		uint32 nFontSize = (uint8)((LTFLOAT)m_TitleSize * g_pInterfaceResMgr->GetXRatio());
		m_TitleString.SetFontHeight(nFontSize);
	}

}


// Creates the title for the screen
void CBaseScreen::SetTitlePos(LTIntPt pt)
{
	m_TitlePos = pt;

	LTIntPt pos = m_TitlePos;
	g_pInterfaceResMgr->ConvertScreenPos(pos);
	m_TitleString.SetPos(pos);
}

void CBaseScreen::SetTitleFont(uint8 nFont)
{
	m_TitleFont = nFont;
	
	CFontInfo* pFont = g_pInterfaceResMgr->GetFont(m_TitleFont);
	m_TitleString.SetFont(*pFont);
}


void CBaseScreen::SetTitleSize(uint8 nFontSize)
{
	m_TitleSize = nFontSize;

	uint32 nAdjustedFontSize = (uint32)((LTFLOAT)m_TitleSize * g_pInterfaceResMgr->GetXRatio());
	m_TitleString.SetFontHeight(nAdjustedFontSize);
}

void CBaseScreen::SetTitleColor(uint32 titleColor)
{
	m_TitleColor = titleColor;
	m_TitleString.SetColor(titleColor);
}

