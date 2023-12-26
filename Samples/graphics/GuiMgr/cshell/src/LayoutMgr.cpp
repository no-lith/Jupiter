// ----------------------------------------------------------------------- //
//
// MODULE  : LayoutMgr.cpp
//
// PURPOSE : Attribute file manager for interface layout info
//				 Normally this would use a file or database 
//				 Hardcoded for this sample application
//
// (c) 1999-2002 Monolith Productions, Inc.  All Rights Reserved
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "LayoutMgr.h"
#include "BaseScreen.h"
#include "ilttexturestring.h"

//---------------------------------------------------------------------------
//	 Name:		CLayoutMgr::CLayoutMgr()
//
//	 Purpose:	Constructor
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------

CLayoutMgr::CLayoutMgr()
{
	m_nNumFonts = 0;

}

//---------------------------------------------------------------------------
//	 Name:		CLayoutMgr::~CLayoutMgr()
//
//	 Purpose:	Destructor
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
CLayoutMgr::~CLayoutMgr()
{
	Term();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLayoutMgr::Init()
//
//	PURPOSE:	Initialize the layout manager
//
// ----------------------------------------------------------------------- //

LTBOOL CLayoutMgr::Init(void)
{

	m_nNumFonts = 2;

	LTStrCpy(m_Fonts[0].szFontName, L"fonts\\nolf.ttf", LTARRAYSIZE(m_Fonts[0].szFontName));
	m_Fonts[0].lfCharSet = LTDEFAULT_CHARSET;
	m_Fonts[0].nHeight = 24;
	m_Fonts[0].nStyle = 0;

	LTStrCpy(m_Fonts[1].szFontName, L"fonts\\msmincho.ttc", LTARRAYSIZE(m_Fonts[0].szFontName));
	m_Fonts[1].lfCharSet = LTGB2312_CHARSET;
	m_Fonts[1].nHeight = 24;
	m_Fonts[1].nStyle = 0;

	return LTTRUE;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLayoutMgr::Term()
//
//	PURPOSE:	Clean up.
//
// ----------------------------------------------------------------------- //

void CLayoutMgr::Term()
{

}


// ------------------------------------------------------------------------//
//
//	Basic Screen Layout
//
// ------------------------------------------------------------------------//

uint8 CLayoutMgr::GetBackFont()
{
	return 0;
}

uint8 CLayoutMgr::GetBackSize()
{
	return 16;
}

LTIntPt  CLayoutMgr::GetBackPos()
{
	return LTIntPt(40,400);
}

LTIntPt CLayoutMgr::GetNextPos()
{
	return LTIntPt(600,400);
}

uint32 CLayoutMgr::GetBackColor()
{
	return argbBlack;
}

void CLayoutMgr::GetBackGroundTex(char *pBuf, int nBufLen)
{

	LTStrCpy( pBuf, "interface\\TouchdownBG.dtx", nBufLen );

}

LTFLOAT CLayoutMgr::GetBackGroundScale()
{
	return 1.0f;
}

void CLayoutMgr::GetFrameTex(char *pBuf, int nBufLen)
{
	LTStrCpy( pBuf, "interface\\frame.dtx", nBufLen );
}

uint32 CLayoutMgr::GetFrameColor()
{
	return argbYellow;
}



void CLayoutMgr::GetSliderTex(char *pBuf, int nBufLen)
{
	LTStrCpy( pBuf, "interface\\slider.dtx", nBufLen );
}

void CLayoutMgr::GetBackTex(char *pBuf, int nBufLen)
{
	LTStrCpy( pBuf, "interface\\arrowback.dtx", nBufLen );
}
void CLayoutMgr::GetBackTexH(char *pBuf, int nBufLen)
{
	LTStrCpy( pBuf, "interface\\arrowback_h.dtx", nBufLen );
}

LTIntPt  CLayoutMgr::GetBackTexPos()
{
	return LTIntPt(40,400);
}

void CLayoutMgr::GetNextTex(char *pBuf, int nBufLen)
{
	LTStrCpy( pBuf, "interface\\arrownext.dtx", nBufLen );
}
void CLayoutMgr::GetNextTexH(char *pBuf, int nBufLen)
{
	LTStrCpy( pBuf, "interface\\arrownext_h.dtx", nBufLen );
}
LTIntPt  CLayoutMgr::GetNextTexPos()
{
	return LTIntPt(600,400);
}





// ------------------------------------------------------------------------//
//
//	Specific Screen Layouts
//
// ------------------------------------------------------------------------//

LTIntPt  CLayoutMgr::GetScreenTitlePos(eScreenID screenId)
{
	return LTIntPt ( 350,0 );
}


uint8 CLayoutMgr::GetScreenTitleFont(eScreenID screenId)
{
	return 1;
}


uint8 CLayoutMgr::GetScreenTitleSize(eScreenID screenId)
{
	return 42;
}

uint8 CLayoutMgr::GetScreenFontFace(eScreenID screenId)
{
	return 1;
}

uint8 CLayoutMgr::GetScreenFontSize(eScreenID screenId)
{
	return 32;
}

LTRect   CLayoutMgr::GetScreenPageRect(eScreenID screenId)
{
	return LTRect( 40,70,600,390);
}

int		CLayoutMgr::GetScreenItemSpacing(eScreenID screenId)
{
	return 10;
}

int		CLayoutMgr::GetScreenItemAlign(eScreenID screenId)
{
	return 50;
}

uint32 CLayoutMgr::GetScreenSelectedColor(eScreenID screenId)
{
	return argbBlue;
}

uint32 CLayoutMgr::GetScreenNonSelectedColor(eScreenID screenId)
{
	return argbGreen;
}

uint32 CLayoutMgr::GetScreenDisabledColor(eScreenID screenId)
{
	return argbBlack;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLayoutMgr::GetScreenSliderTex
//
//	PURPOSE:
//
// ----------------------------------------------------------------------- //
void CLayoutMgr::GetScreenSliderTex(eScreenID screenId, char *pBuf, int nBufLen)
{
	LTStrCpy ( pBuf, "interface\\slider.dtx", nBufLen );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLayoutMgr::GetScreenSliderTex
//
//	PURPOSE:
//
// ----------------------------------------------------------------------- //
void CLayoutMgr::GetScreenSliderTexH(eScreenID screenId, char *pBuf, int nBufLen)
{
	pBuf[0] = 0;
}



LTIntPt  CLayoutMgr::GetScreenCustomPoint(eScreenID screenId, char *pAttribute)
{
	// These would normally be retrieved using a database call
	switch (screenId)
	{
		case SCREEN_ID_MAIN:
			break;
		case SCREEN_ID_ONE:
			break;
		case SCREEN_ID_TWO:
			break;
		case SCREEN_ID_THREE:
			break;
		default:
			break;
	}

	return LTIntPt(0,0);
}

LTRect   CLayoutMgr::GetScreenCustomRect(eScreenID screenId, char *pAttribute)
{

	switch (screenId)
	{
		case SCREEN_ID_MAIN:
			break;
		case SCREEN_ID_ONE:
			break;
		case SCREEN_ID_TWO:
			break;
		case SCREEN_ID_THREE:
			break;
		default:
			break;
	}

	return LTRect(0,0,0,0);
}

int		CLayoutMgr::GetScreenCustomInt(eScreenID screenId, char *pAttribute)
{
	switch (screenId)
	{
		case SCREEN_ID_MAIN:
			break;
		case SCREEN_ID_ONE:
			break;
		case SCREEN_ID_TWO:
			break;
		case SCREEN_ID_THREE:
			break;
		default:
			break;
	}

	return 0;

}


LTFLOAT CLayoutMgr::GetScreenCustomFloat(eScreenID screenId, char *pAttribute)
{
	switch (screenId)
	{
		case SCREEN_ID_MAIN:
			break;
		case SCREEN_ID_ONE:
			break;
		case SCREEN_ID_TWO:
			break;
		case SCREEN_ID_THREE:
			break;
		default:
			break;
	}

	return 0.0f;
}

void	CLayoutMgr::GetScreenCustomString(eScreenID screenId, char *pAttribute, char *pBuf, int nBufLen)
{
	switch (screenId)
	{
		case SCREEN_ID_MAIN:
			break;
		case SCREEN_ID_ONE:
			break;
		case SCREEN_ID_TWO:
			break;
		case SCREEN_ID_THREE:
			break;
		default:
			break;
	}

	*pBuf = 0;

}


// ------------------------------------------------------------------------//
//
//	Fonts
//
// ------------------------------------------------------------------------//

void CLayoutMgr::GetFontName(int nFont, wchar_t * pszFontFile, int nFontFileBufLen )
{
	if ( nFont < m_nNumFonts )
		LTStrCpy(pszFontFile, m_Fonts[nFont].szFontName, nFontFileBufLen);
}


uint32 CLayoutMgr::GetFontSize(int nFont)
{

	if ( nFont < m_nNumFonts )
		return m_Fonts[nFont].nHeight;

	return 0;

}

uint32 CLayoutMgr::GetFontStyle(int nFont)
{

	if ( nFont < m_nNumFonts )
		return m_Fonts[nFont].nStyle;

	return 0;
}


uint8 CLayoutMgr::GetFontCharSet(int nFont)
{
	if ( nFont < m_nNumFonts )
		return m_Fonts[nFont].lfCharSet;

	return 0;
}