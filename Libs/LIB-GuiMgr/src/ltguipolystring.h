// ----------------------------------------------------------------------- //
//
// MODULE  : PolyWideString.h
//
// PURPOSE : PolyString simulation for wchar using the ilttextstring interface 
//
//
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved//
// ----------------------------------------------------------------------- //

#ifndef _LTGUIPOLYSTRING_H_
#define _LTGUIPOLYSTRING_H_

#include <iltclient.h>
#include <ltbasedefs.h>
#include <ilttexturestring.h>
#include <string>

//#include "clientinterfaces.h"

class LTFontParams;


enum eTextAlign
{
	kLeft,
	kCenter,
	kRight
};


class CLTGUIPolyString
{

public:

	CLTGUIPolyString();
	~CLTGUIPolyString() {FlushTexture();}

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
	void SetPos(float x, float y );
	void SetPos(LTIntPt pos);

	void SetColor(uint32 nColor);

	void SetAlignment(eTextAlign align);

	void SetDropShadow(LTVector2f drop) {m_vDropShadow = drop;}

	void CreateTexture();
	void FlushTexture();	

	float GetWidth();

	float GetHeight();

	void GetRect( LTRect2n * nRect );

	LTVector2f GetPos()	const  { return m_vPos;}
	uint32 GetColor() const {return m_nColor;}
	eTextAlign GetAlignment() const {return m_Alignment;}


	void SetWrapWidth(uint32 nWidth);

	void GetPos( float *x, float *y ) { *x =	m_vPos.x; *y = m_vPos.y; }

	bool IsEmpty() const {return m_sString.empty(); }

	bool IsValid() const { return ((m_hString != NULL) ? true : false); }

	void GetCharRect( uint32 nCharIndex, LTRect2n & rRect );

	uint32 GetStringLength( );

	//render functions will recreate textures as needed
	LTRESULT	Render();
	LTRESULT Render( uint32 nFirstChar, uint32 nLastChar );
	LTRESULT RenderClipped( const LTRect2n & nRect );

private:

	LTRESULT	RenderString(	const LTVector2f& vAnchor,
								uint32 nColor	= 0xFFFFFFFF,
								const LTVector2f& vAnchorScale	= LTVector2f(0.0f, 0.0f), 
								const LTVector2f& vGround		= LTVector2f(1.0f, 0.0f),
								const LTVector2f& vDown			= LTVector2f(0.0f, 1.0f),
								const LTVector2f& vStretch		= LTVector2f(1.0f, 1.0f));

	LTRESULT RenderStringClipped(	const LTRect2n& rClipRect,
											const LTVector2f& vAnchor, 
											uint32 nColor= 0xFFFFFFFF,
											const LTVector2f& vAnchorScale = LTVector2f(0.0f, 0.0f));

	LTRESULT RenderSubString( const wchar_t* pszString,
									  const LTVector2f& vAnchor, 
									  uint32 nColor = 0xFFFFFFFF );


	CFontInfo		m_Font;
	HTEXTURESTRING	m_hString;

	std::wstring	m_sString;

	eTextAlign		m_Alignment;

	uint32			m_nColor;

	LTVector2f		m_vPos;

	// future
	LTVector2f		m_vDropShadow;
	uint32			m_nShadowColor;

	// used for word wrap - zero is no wrap
	uint32			m_nWidth;

};

#endif  // endif _LTGUIPOLYSTRING_H_