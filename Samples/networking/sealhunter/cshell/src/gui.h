// ----------------------------------------------------------------------- //
//
// MODULE  : CGui.h
//
// PURPOSE : CGui - Definition and constants
//
// CREATED : 2002/05/24
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __CGUI_H__
#define __CGUI_H__

#include <iltclient.h>
#include <ltbasedefs.h>
#include <iltfontmanager.h>
#include "clientinterfaces.h"
#include "serverdefs.h"



class CGui
{
public:

	// Construction / destruction
	CGui();
	~CGui();

	LTRESULT 		Init(uint32 nCharWidth, uint32 nCharHeight);
	LTRESULT 		Term();

	LTRESULT 		Render();

	void     		HandleInput(int nCommand);

	CUIFont*		GetVectorFont()							{ return m_pFont; }
    void     		SetText(const char* sText);
	void			SetStats(unsigned Score, float Money)	{ m_Score = Score; m_Money = Money; }
    void     		SetSideText(const char* sText);

private:

	bool			LoadTexture(const char* sFilename, HTEXTURE &hTexture);
	void			ReleaseTexture(HTEXTURE &hTexture);

	void			Draw(HTEXTURE hTexture, uint8 nPosStyle, uint8 nAlpha = 255, uint8 nOffsetX = 0, uint nOffsetY = 0, uint nWidth = 0, uint nHeight = 0);

	void			ReCalcCursor(uint8 nPos, uint8 nMax, CUIFormattedPolyString* pString);
    void			CreateJoinString();
    void			DestroyJoinString();

	void			SetScoreText();

private:

	HTEXTURE			    	m_hSplashScreen;
	HTEXTURE			    	m_hSealHUD;
	HTEXTURE 					m_hFontTexture;

	CUIFont*					m_pFont;

	CUIFormattedPolyString*	    m_pFontString;
	CUIFormattedPolyString*	    m_pJoinString;
	CUIFormattedPolyString*	    m_pCursorString;
	CUIFormattedPolyString*	    m_pScoreString;
	CUIFormattedPolyString*	    m_pMoneyString;

	uint8                       m_nCurScreen;
    uint8                       m_nMainCurPos;
    uint8                       m_nJoinCurPos;

    unsigned 					m_Score;
	float  						m_Money;

    SERVERINFO                  m_ServerList[4];
};



#endif	// __CGUI_H__
