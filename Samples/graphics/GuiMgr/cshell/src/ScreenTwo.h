// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenTwo.h
//
// PURPOSE : Sub level interface screen
//
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef _SCREEN_TWO_H_
#define _SCREEN_TWO_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseScreen.h"

class CScreenTwo : public CBaseScreen
{

public:

	CScreenTwo();
	virtual ~CScreenTwo();

	// Build the screen
   LTBOOL   Build();

	// This is called when the screen gets or loses focus
   virtual void    OnFocus(LTBOOL bFocus);

	virtual void    Escape();

	virtual LTBOOL  HandleKeyDown(int key, int rep);
	virtual LTBOOL  HandleChar(wchar_t c);

protected:
   uint32	OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
   LTBOOL   Render();

	HTEXTURE			m_hBG;
	LTPoly_GT4		m_BGPoly;
	
	wchar_t			m_szEdit[256];

	CLTGUIEditCtrl *	m_pEditCtrl;
};

#endif // _SCREEN_TWO_H_