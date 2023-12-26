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

// Position Styles
#define PS_NONE			0
#define PS_CENTER_ON_X (1<<0)
#define PS_CENTER_ON_Y (1<<1)
#define PS_TOP		   (1<<2)
#define PS_BOTTOM      (1<<3)
#define PS_LEFT		   (1<<4)
#define PS_RIGHT	   (1<<5)



class CGui
{
public:

	CGui();
	~CGui();

	// initialize using a bitmap font
	LTRESULT					Init(const char* pFilename, uint32 nCharHeight);

	// initialize using a bitmap font
	LTRESULT					Init(const char* pFilename, uint32 nCharWidth, uint32 nCharHeight, bool monospaced = true );

	// initialize using the console font
	LTRESULT 					Init(uint32 nCharHeight = 18)				{ return Init(LTNULL, nCharHeight); }

	LTRESULT 					Term();

	CUIFont*					GetVectorFont()								{ return m_pFont; }

	void    					SetText(const char* sText);
    void    					SetPos(uint16 x, uint16 y);
	void						SetColor(uint32 argb);

	bool						IsVisible() const							{ return m_bVisible; }
	void						SetVisible(bool bVis)						{ m_bVisible = bVis; }

	void						ApplyPositionStyle(uint32 nPS, uint32 nXoffset = 0, uint32 nYoffset = 0);
    void    					Center();
    void    					Bottom(uint32 nOffset = 0);
    void    					Top(uint32 nOffset = 0);

	LTRESULT 					Render();

private:
	CUIFormattedPolyString*		m_pFontString;
    CUIFont*					m_pFont;
	HTEXTURE 					m_hFontTexture;
	uint32						m_nPosStyle;
	bool						m_bVisible;
};



#endif	// __CGUI_H__
