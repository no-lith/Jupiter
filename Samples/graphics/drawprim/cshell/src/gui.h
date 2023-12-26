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

class CGui
{
  public:
	  
	// Construction / destruction
	CGui();
	virtual ~CGui();

	LTRESULT Init(const char* filename, const char* fontface, uint32 nSize);
	LTRESULT Render();
	LTRESULT Dispose();
	CUIFont *GetVectorFont(){ return m_pFont; }
    void    SetText(const char* sText);
    void    SetPos(uint16 x, uint16 y);
    void    Center();
    void    Bottom();

  private:
	CUIFormattedPolyString*	m_pFontString;
    CUIFont  *m_pFont;

};

#endif	// __CGUI_H__