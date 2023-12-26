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
//#include "iclientshell.h"
#include <iltfontmanager.h>
#include "clientinterfaces.h"
#include "serverdefs.h"

class CGui
{
  public:
	  
	// Construction / destruction
	CGui();
	~CGui();

	LTRESULT Init(const char* filename, const char* fontface);
	LTRESULT Render();
    void     HandleInput(int nCommand);
	LTRESULT Dispose();
	CUIFont  *GetVectorFont(){ return m_pFont; }
    void     SetText(const char* sText);
    void     SetSideText(const char* sText);


  private:
    void                        ReCalcCursor(uint8 nPos, uint8 nMax, CUIFormattedPolyString* pString);
    void                        CreateJoinString();
    void                        DestroyJoinString();
	CUIFormattedPolyString*	    m_pFontString;
	CUIFormattedPolyString*	    m_pJoinString;
	CUIFormattedPolyString*	    m_pCursorString;
    CUIFont                     *m_pFont;
    uint8                       m_nCurScreen;

    uint8                       m_nMainCurPos;
    uint8                       m_nJoinCurPos;

    SERVERINFO                  m_ServerList[4];

};

#endif	// __CGUI_H__