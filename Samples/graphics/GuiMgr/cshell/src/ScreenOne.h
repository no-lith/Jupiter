// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenOne.h
//
// PURPOSE : Sub level interface screen
//
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef _SCREEN_ONE_H_
#define _SCREEN_ONE_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseScreen.h"

class CScreenOne : public CBaseScreen
{

public:

	CScreenOne();
	virtual ~CScreenOne();

	// Build the screen
   LTBOOL   Build();

	// This is called when the screen gets or loses focus
   virtual void    OnFocus(LTBOOL bFocus);


protected:
   uint32	OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);
   LTBOOL   Render();

	HTEXTURE			m_hBG;
	LTPoly_GT4		m_BGPoly;

	CLTGUISlider *	m_pSlider;
	int				m_Value;
};

#endif // _SCREEN_ONE_H_