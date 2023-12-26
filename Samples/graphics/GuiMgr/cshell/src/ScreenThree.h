// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenThree.h
//
// PURPOSE : Sub sub screen
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef _SCREEN_THREE_H_
#define _SCREEN_THREE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseScreen.h"

class CScreenThree : public CBaseScreen
{

public:

	CScreenThree();
	virtual ~CScreenThree();

	// Build the screen
   LTBOOL   Build();

	// This is called when the screen gets or loses focus
   virtual void    OnFocus(LTBOOL bFocus);

	virtual void    Escape();


protected:
   uint32	OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
   LTBOOL   Render();

	HTEXTURE			m_hBG;
	LTPoly_GT4		m_BGPoly;

	CLTGUIListCtrl * m_pList;

};

#endif // _SCREEN_MAIN_H_