// ----------------------------------------------------------------------- //
//
// MODULE  : LayoutMgr.h
//
// PURPOSE : Attribute file manager for interface layout info
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
// ----------------------------------------------------------------------- //

#if !defined(_LAYOUTMGR_H_)
#define _LAYOUTMGR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ltbasetypes.h"
#include "ScreenMgr.h"

struct CFontLayout
{
	wchar_t		szFontName[32]; // limit is windows based
	uint32		nHeight;
	uint8			lfCharSet;
	uint32		nStyle;
};

class CLayoutMgr
{
public:
	CLayoutMgr();
	virtual ~CLayoutMgr();

	virtual LTBOOL      Init( void );
	void		Term();

	uint8		GetBackFont();
	uint8		GetBackSize();
	LTIntPt	GetBackPos();

	LTIntPt	GetNextPos();

	uint32	GetBackColor();
	void		GetBackGroundTex(char *pBuf, int nBufLen);
	LTFLOAT	GetBackGroundScale();

	void		GetFrameTex(char *pBuf, int nBufLen);
	uint32	GetFrameColor();

	void		GetSliderTex(char *pBuf, int nBufLen);

	void		GetBackTex(char *pBuf, int nBufLen);
	void		GetBackTexH(char *pBuf, int nBufLen);
   LTIntPt  GetBackTexPos();

	void		GetNextTex(char *pBuf, int nBufLen);
	void		GetNextTexH(char *pBuf, int nBufLen);
   LTIntPt  GetNextTexPos();

   LTIntPt  GetScreenTitlePos(eScreenID screenId);
	uint8		GetScreenTitleFont(eScreenID screenId);
	uint8		GetScreenTitleSize(eScreenID screenId);
	uint8		GetScreenFontFace(eScreenID screenId);
	uint8		GetScreenFontSize(eScreenID screenId);

   LTRect   GetScreenPageRect(eScreenID screenId);
	int	   GetScreenItemSpacing(eScreenID screenId);
	int		GetScreenItemAlign(eScreenID screenId);

	void		GetScreenSliderTex(eScreenID screenId, char *pBuf, int nBufLen);
	void		GetScreenSliderTexH(eScreenID screenId, char *pBuf, int nBufLen);

	uint32	GetScreenSelectedColor(eScreenID screenId);
	uint32	GetScreenNonSelectedColor(eScreenID screenId);
	uint32	GetScreenDisabledColor(eScreenID screenId);

	LTIntPt	GetScreenCustomPoint(eScreenID screenId, char *pAttribute);
	LTRect	GetScreenCustomRect(eScreenID screenId, char *pAttribute);
	int		GetScreenCustomInt(eScreenID screenId, char *pAttribute);
	LTFLOAT	GetScreenCustomFloat(eScreenID screenId, char *pAttribute);
	void		GetScreenCustomString(eScreenID screenId, char *pAttribute, char *pBuf, int nBufLen);


	int		GetNumFonts()			{return m_nNumFonts;}
	void		GetFontName(int nFont, wchar_t * pszFontFile, int nFontFileBufLen );
	uint32	GetFontSize(int nFont);
	uint32	GetFontStyle(int nFont);
	uint8		GetFontCharSet(int nFont);

protected:

	// some hardcoded fonts normally you would load this from a file or database
	int			m_nNumFonts;

	CFontLayout m_Fonts[10];


};

#endif // !defined(_LAYOUTMGR_H_)