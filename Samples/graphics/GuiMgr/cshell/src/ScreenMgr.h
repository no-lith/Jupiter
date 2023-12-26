// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenMgr.h
//
// PURPOSE : screen manager 
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
// ----------------------------------------------------------------------- //


#if !defined(_SCREEN_MGR_H_)
#define _SCREEN_MGR_H_


#include <vector>
#include "lithtech.h"

#include "DebugNew.h"

enum eScreenID
{
	#define INCLUDE_AS_ENUM
	#include "ScreenEnum.h"
	#undef INCLUDE_AS_ENUM
};

#define MAX_SCREEN_HISTORY 20


// BaseScreen that all screens inherit from 
class CBaseScreen;


class CScreenMgr
{

public:

	CScreenMgr();
	virtual ~CScreenMgr();

   virtual LTBOOL    Init();
	virtual void		Term();

	// keyboard
	virtual void		HandleKeyDown (int vkey, int rep);
	virtual void		HandleKeyUp (int vkey);
	virtual void		HandleChar (wchar_t c);

	// Mouse messages
	virtual void		OnLButtonDown(int x, int y);
	virtual void		OnLButtonUp(int x, int y);
	virtual void		OnLButtonDblClick(int x, int y);
	virtual void		OnRButtonDown(int x, int y);
	virtual void		OnRButtonUp(int x, int y);
	virtual void		OnRButtonDblClick(int x, int y);
	virtual void		OnMouseMove(int x, int y);


   virtual LTBOOL		ForceScreenUpdate(eScreenID screenID);

	// screen maintenance
	virtual eScreenID	GetCurrentScreenID()		{return m_eCurrentScreenID;}
	virtual eScreenID	GetLastScreenID()			{return m_eLastScreenID;}
	virtual uint16		GetScreenIDFromName(char * pName) = 0;
   virtual LTBOOL    SetCurrentScreen(eScreenID screenID);
   virtual LTBOOL    PreviousScreen();
	virtual void		EscapeCurrentScreen();
	virtual void		ExitScreens();

	// Get from history, where nHistoryIndex=0 is the most recent in history.
	eScreenID			GetFromHistory( int nHistoryIndex );

	virtual const char *GetScreenName(eScreenID id) = 0;

	// Renders the screen to a surface
   virtual LTBOOL		Render();
	virtual void		ScreenDimsChanged();

	virtual CBaseScreen*	GetScreenFromID(eScreenID screenID);


	virtual void ClearHistory();
	virtual void AddScreenToHistory(eScreenID screenID);

protected:

	// note that the screen should already have been inited before being added
	virtual void	AddScreen(CBaseScreen* pScreen);

	virtual void	SwitchToScreen(CBaseScreen *pNewScreen);

protected:

	int				m_nHistoryLen;
	eScreenID		m_eScreenHistory[MAX_SCREEN_HISTORY];
	eScreenID		m_eCurrentScreenID;
	eScreenID		m_eLastScreenID;
	CBaseScreen*	m_pCurrentScreen;		// The current screen
	
	//screens
	typedef std::vector<CBaseScreen *> ScreenArray;
	ScreenArray m_screenArray;									// Pointer to each screen
};

#endif // _SCREENMGR_H_