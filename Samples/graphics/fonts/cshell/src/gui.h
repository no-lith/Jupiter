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
#include <ilttexturestring.h>

#include "clientinterfaces.h"
class LTFontParams;


// Position Styles
#define PS_NONE			0
#define PS_CENTER_ON_X (1<<0)
#define PS_CENTER_ON_Y (1<<1)
#define PS_TOP		   (1<<2)
#define PS_BOTTOM      (1<<3)
#define PS_LEFT		   (1<<4)
#define PS_RIGHT	   (1<<5)


#define PIXEL_BORDER_OFFSET 10

class CGui
{
public:
	  
	// Construction / destruction
	CGui();
	virtual ~CGui();

	LTRESULT Init(const char* filename, const char* fontface, uint32 nSize, LTFontParams* fontParams = NULL);// Used for True Type (vector) fonts
	LTRESULT Init(const char* filename, uint32 charw, uint32 charh, bool monospace = true);		// Used for Monospaced fonts
	LTRESULT Render();
	LTRESULT Term();
	CUIFont *GetVectorFont(){ return m_pFont; }
    void    SetText(const char* sText);
	void	SetTextColor(uint32 color);
    void    SetPos(uint16 x, uint16 y);
 
	bool	IsVisible() const		{ return m_bVisible; }
	void	SetVisible(bool bVis)	{ m_bVisible = bVis; }

	void	ApplyPositionStyle(uint32 nPS, uint32 nXoffset = 0, uint32 nYoffset = 0);
	void    Center();
	void	Bottom(uint32 nOffset = 0);
	void	Top(uint32 nOffset = 0);

private:

	CUIFormattedPolyString*	m_pFontString;
    CUIFont				  *	m_pFont;

	// Used for bitmap fonts
	HTEXTURE				m_hFontTexture;

	// Visibility flag
	bool					m_bVisible;

	// Position data
	uint32					m_nPosStyle;

};


class CGuiWide
{

public:

	CGuiWide();
	~CGuiWide() {FlushTexture();}

	void SetFont ( const CFontInfo & Font );

	void SetText(const wchar_t *pText);

	const wchar_t* GetText() const {return m_sString.c_str(); }

	//does nothing if source string is used
	void SetFontHeight(uint32 nFontHeight);

	//returns 0 if source string is used
	uint32 GetFontHeight() const {return m_Font.m_nHeight; }

	void SetPos(LTVector2f vPos);
	void SetPos(LTVector2n vPos);
	void SetPos( uint32 x, uint32 y );

	void SetColor(uint32 nColor);

	void CreateTexture();
	void FlushTexture();	

	bool IsEmpty() const {return m_sString.empty(); }

	bool IsValid() const { return ((m_hString != NULL) ? true : false); }


	//render functions will recreate textures as needed
	LTRESULT	Render();

private:

	LTRESULT	RenderString(	const LTVector2f& vAnchor,
								uint32 nColor	= 0xFFFFFFFF,
								const LTVector2f& vAnchorScale	= LTVector2f(0.0f, 0.0f), 
								const LTVector2f& vGround		= LTVector2f(1.0f, 0.0f),
								const LTVector2f& vDown			= LTVector2f(0.0f, 1.0f),
								const LTVector2f& vStretch		= LTVector2f(1.0f, 1.0f));

	CFontInfo		m_Font;
	HTEXTURESTRING	m_hString;

	std::wstring	m_sString;

	uint32			m_nColor;

	LTVector2f		m_vPos;


};

#endif	// __CGUI_H__