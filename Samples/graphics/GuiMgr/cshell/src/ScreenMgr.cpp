//	-----------------------------------------------------------------------	//
//
//	MODULE  : ScreenMgr.cpp
//
//	PURPOSE : Interface screen	manager
//
//	(c) 1999-2001 Monolith Productions,	Inc.	All Rights Reserved
//	(c) 2005	Touchdown Entertainment	Inc. All	Rights Reserved
//
//	-----------------------------------------------------------------------	//

#include	"ScreenMgr.h"
#include	"BaseScreen.h"

#include	"vkdefs.h"

//---------------------------------------------------------------------------
//	 Name:		  ScreenMgr()
//
//	 Purpose:	  Screen	manager contructor
//
//	 Returns:	  n/a
//
//---------------------------------------------------------------------------
CScreenMgr::CScreenMgr()
{
	m_pCurrentScreen = LTNULL;
	m_eCurrentScreenID =	(eScreenID)0;
	m_eLastScreenID =	(eScreenID)0;
	m_nHistoryLen = 0;
}


//---------------------------------------------------------------------------
//	 Name:		  ~ScreenMgr()
//
//	 Purpose:	  Screen	manager destructor
//
//	 Returns:	  n/a
//
//---------------------------------------------------------------------------
CScreenMgr::~CScreenMgr()
{

}


//---------------------------------------------------------------------------
//	 Name:		  ScreenMgr::Init()
//
//	 Purpose:	  Screen	manager initialization
//
//	 Returns:	  LTBOOL	- success or failure
//
//---------------------------------------------------------------------------

LTBOOL CScreenMgr::Init()
{
	 return LTTRUE;
}

//---------------------------------------------------------------------------
//	 Name:		  ScreenMgr::Term()
//
//	 Purpose:	  Screen	manager termination
//
//	 Returns:	  n/a
//
//---------------------------------------------------------------------------
void CScreenMgr::Term()
{
	//	Term/remove	all the screens we have	allocated 
	for (uint16	i=0; i <	m_screenArray.size(); ++i)
	{
		m_screenArray[i]->Term();
		debug_delete(m_screenArray[i]);
	}

	m_screenArray.clear();

}



//---------------------------------------------------------------------------
//	 Name:		  ScreenMgr::Render()
//
//	 Purpose:	  Render	the current	screen to a	surface 
//
//	 Returns:	LTBOOL -	success or failure 
//
//---------------------------------------------------------------------------
LTBOOL CScreenMgr::Render()
{
	if	(m_pCurrentScreen)
	{
		return m_pCurrentScreen->Render();
	}
	 return LTFALSE;
}



//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::HandleKeyDown()
//
//	 Purpose:	Process key down messages 
//
//	 Returns:	n/a 
//
//---------------------------------------------------------------------------
void CScreenMgr::HandleKeyDown (int	vkey,	int rep)
{
	if	(m_pCurrentScreen)
	{
		if	(vkey	==	VK_ESCAPE)
		{
			m_pCurrentScreen->Escape();
		}
		else
			m_pCurrentScreen->HandleKeyDown(vkey,rep);
	}
}


//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::HandleKeyUp()
//
//	 Purpose:	Process key up messages 
//
//	 Returns:	n/a 
//
//---------------------------------------------------------------------------
void CScreenMgr::HandleKeyUp (int vkey)
{
	if	(m_pCurrentScreen)
	{
		m_pCurrentScreen->HandleKeyUp(vkey);
	}
}


//---------------------------------------------------------------------------
//	 Name:		  ScreenMgr::HandleChar()
//
//	 Purpose:	  Process char	messages	
//
//	 Returns:	n/a 
//
//---------------------------------------------------------------------------
void CScreenMgr::HandleChar ( wchar_t c)
{
	if	(m_pCurrentScreen)
	{
		m_pCurrentScreen->HandleChar(c);
	}
}


//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::GetFromHistory()
//
//	 Purpose:	Get a specific screen	id	in	the history	given	a index 
//
//	 Returns:	eScreenID -	screen ID or NONE	if	not found 
//
//---------------------------------------------------------------------------
eScreenID CScreenMgr::GetFromHistory( int	nHistoryIndex )
{ 
	if( nHistoryIndex	< MAX_SCREEN_HISTORY	)
		return m_eScreenHistory[m_nHistoryLen - nHistoryIndex	- 1];

	return SCREEN_ID_NONE;
}



//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::PreviousScreen()
//
//	 Purpose:	Switch to the previous screen 
//
//	 Returns:	LTBOOl - success or failure 
//
//---------------------------------------------------------------------------
LTBOOL CScreenMgr::PreviousScreen()
{
	if	(m_nHistoryLen	< 1) return	LTFALSE;

	CBaseScreen	*pNewScreen=GetScreenFromID(m_eScreenHistory[m_nHistoryLen-1]);

	if	(pNewScreen)
	{
		SwitchToScreen(pNewScreen);

		return LTTRUE;
	}

	return LTFALSE;
}



//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::SetCurrentScreen()
//
//	 Purpose:	Set to a specific screen 
//
//	 Returns:	LTBOOl - success or failure 
//
//---------------------------------------------------------------------------
LTBOOL CScreenMgr::SetCurrentScreen(eScreenID screenID)
{

	CBaseScreen	*pNewScreen=GetScreenFromID(screenID);

	if	(pNewScreen)
	{
		SwitchToScreen(pNewScreen);

		return LTTRUE;
	}

	return LTFALSE;
}


//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::EscCurrentScreen()
//
//	 Purpose:	Execute escape on the current screen ( usually means go back )
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void CScreenMgr::EscapeCurrentScreen()
{
	if	(m_pCurrentScreen)
		m_pCurrentScreen->Escape();
}



//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::ExitScreens()
//
//	 Purpose:	Exists all screens and remove history
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void CScreenMgr::ExitScreens()
{
	//	Tell the	current screen that it is losing focus
	if	(m_pCurrentScreen)
	{
		m_pCurrentScreen->OnFocus(LTFALSE);
	}

	//clear our	screen history	(no longer relevant)
	ClearHistory();
	
}


//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::SwitchToScreen()
//
//	 Purpose:	Switch to a new screen
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void CScreenMgr::SwitchToScreen(CBaseScreen *pNewScreen)
{

	CBaseScreen	*pOldScreen	= m_pCurrentScreen;

	//	Tell the	old screen that it is losing focus
	if	(m_pCurrentScreen	&&	pNewScreen != m_pCurrentScreen)
	{
		int insert = 0;

		m_pCurrentScreen->OnFocus(LTFALSE);

		//look through	the list	of	screens we've visited, if we find the
		//	one we're going to cut the	history back to that	point.
		//	otherwise add it at the	end.
		eScreenID nextScreenID = (eScreenID)pNewScreen->GetScreenID();
		eScreenID currentScreenID = (eScreenID)m_pCurrentScreen->GetScreenID();

		while	(insert < m_nHistoryLen	&&	m_eScreenHistory[insert] != nextScreenID)
			++insert;

		if	(insert == m_nHistoryLen)
		{
			if	(m_nHistoryLen	< MAX_SCREEN_HISTORY)
			{
				++m_nHistoryLen;
				m_eScreenHistory[insert] =	currentScreenID;
			}
		}
		else
		{
			m_nHistoryLen = insert;
		}
	}

	m_pCurrentScreen=pNewScreen;
	m_eLastScreenID =	m_eCurrentScreenID;

	m_eCurrentScreenID =	(eScreenID)m_pCurrentScreen->GetScreenID();

	//	If	the new screen	hasn't been	built	yet... better build it!
	if	(!m_pCurrentScreen->IsBuilt())
	{
		m_pCurrentScreen->Build();
	}

	//	Tell the	new screen that it is gaining	focus
	if	(pNewScreen	&&	pNewScreen != pOldScreen)
	{
		pNewScreen->OnFocus(LTTRUE);
	}


	//	Do	any special	case work for each screen
	if	(m_eCurrentScreenID == SCREEN_ID_MAIN)
	{
		m_nHistoryLen = 0;
		m_eScreenHistory[0] = SCREEN_ID_NONE;
	}

}


//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::GetScreenFromID()
//
//	 Purpose:	Return a screen given the Screen ID
//
//	 Returns:	CBaseScreen * - screen or NULL if not found 
//
//---------------------------------------------------------------------------
CBaseScreen*	CScreenMgr::GetScreenFromID(eScreenID	screenID)
{
	CBaseScreen	*pScreen=NULL;

	ScreenArray::iterator iter	= m_screenArray.begin();

	while	(iter	!=	m_screenArray.end() && (*iter)->GetScreenID() != (int)screenID)
		iter++;

	if	(iter	!=	m_screenArray.end())
		pScreen = (*iter);

	return pScreen;

}


//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::OnLButtonDown()
//
//	 Purpose:	Process left mouse down message 
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void	CScreenMgr::OnLButtonDown(int	x,	int y)
{
	if	(m_pCurrentScreen)
		m_pCurrentScreen->OnLButtonDown(x,y);
}

//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::OnLButtonUp()
//
//	 Purpose:	Process left mouse up message
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void	CScreenMgr::OnLButtonUp(int x, int y)
{
	if	(m_pCurrentScreen)
		m_pCurrentScreen->OnLButtonUp(x,y);
}


//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::OnLButtonDblClick()
//
//	 Purpose:	Process left double click messages
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void	CScreenMgr::OnLButtonDblClick(int x, int y)
{
	if	(m_pCurrentScreen)
		m_pCurrentScreen->OnLButtonDblClick(x,y);
}


//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::OnRButtonDown()
//
//	 Purpose:	Process right mouse down messages
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void	CScreenMgr::OnRButtonDown(int	x,	int y)
{
	if	(m_pCurrentScreen)
		m_pCurrentScreen->OnRButtonDown(x,y);
}

//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::OnRButtonUp()
//
//	 Purpose:	Process right button up
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void	CScreenMgr::OnRButtonUp(int x, int y)
{
	if	(m_pCurrentScreen)
		m_pCurrentScreen->OnRButtonUp(x,y);
}

//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::OnRButtonDblClick()
//
//	 Purpose:	Process right double click messages
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void	CScreenMgr::OnRButtonDblClick(int x, int y)
{
	if	(m_pCurrentScreen)
		m_pCurrentScreen->OnRButtonDblClick(x,y);
}

//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::OnMouseMove()
//
//	 Purpose:	Process mouse move messages
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void	CScreenMgr::OnMouseMove(int x, int y)
{
	if	(m_pCurrentScreen)
		m_pCurrentScreen->OnMouseMove(x,y);
}



//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::AddScreen()
//
//	 Purpose:	Add a screen
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void CScreenMgr::AddScreen(CBaseScreen* pScreen)
{
	if	(pScreen)
	{
		m_screenArray.push_back(pScreen);
	}

}



//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::ForceScreenUpdate()
//
//	 Purpose:	Force a screen update given a screen id 
//
//	 Returns:	LTBOOL - success or failure 
//
//---------------------------------------------------------------------------
LTBOOL CScreenMgr::ForceScreenUpdate(eScreenID screenID)
{
	if (!m_pCurrentScreen || m_eCurrentScreenID	!=	screenID) return LTFALSE;

	return m_pCurrentScreen->HandleForceUpdate();
}


//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::ScreenDimsChanged()
//
//	 Purpose:	Handle Screen dimenisions changing 
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void CScreenMgr::ScreenDimsChanged()
{
	if	(m_pCurrentScreen)
	{
		m_pCurrentScreen->ScreenDimsChanged();
	}

}



//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::ClearHistory()
//
//	 Purpose:	Screen screen history including the current screen id
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void CScreenMgr::ClearHistory()
{
	m_nHistoryLen = 0;
	m_eScreenHistory[0] = (eScreenID)0;
	m_pCurrentScreen = LTNULL;
	m_eCurrentScreenID =	(eScreenID)0;
}


//---------------------------------------------------------------------------
//	 Name:		ScreenMgr::AddScreenToHistory()
//
//	 Purpose:	Given a screen Id add it to the screen history
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void CScreenMgr::AddScreenToHistory(eScreenID screenID)
{
	int insert = 0;
	
	while	(insert < m_nHistoryLen	&&	m_eScreenHistory[insert] != screenID)
		++insert;

	if	(insert == m_nHistoryLen)
	{
		if	(m_nHistoryLen	< MAX_SCREEN_HISTORY)
		{
			++m_nHistoryLen;
			m_eScreenHistory[insert] =	screenID;
		}
	}
	else
	{
		m_nHistoryLen = insert+1;
	}

}



