// ----------------------------------------------------------------------- //
//
// MODULE  : ltguipolystring.cpp
//
// PURPOSE : Wraps the ilttextstring interface simulating the wide 
//				 polystring interface
//
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "ltguimgr.h"

#define GET_A(val) (((val) >> 24) & 0xFF)
#define SET_ARGB(a,r,g,b) (((uint32)(a) << 24) | (uint32)(r) << 16) | ((uint32)(g) << 8) | ((uint32)(b))


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString
//
//	PURPOSE:	Simple gui string class for wide fonts 
//
// ----------------------------------------------------------------------- //
CLTGUIPolyString::CLTGUIPolyString() :
	m_hString(NULL),
	m_nColor(0xFFFFFFFF),
	m_vDropShadow(0,0),
	m_nWidth(0),
	m_Alignment(kLeft)
{	
	m_vPos.Init();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::SetFont
//
//	PURPOSE:	Set the font type 
//
// ----------------------------------------------------------------------- //
void CLTGUIPolyString::SetFont(const CFontInfo& Font)
{
	//invalid font info
	if (!Font.m_nHeight)
		return;

	m_Font = Font;

	//if we already have a texture, rebuild it
	if (IsValid())
		CreateTexture();
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::SetText
//
//	PURPOSE:	Set the string text  
//
// ----------------------------------------------------------------------- //
void CLTGUIPolyString::SetText(const wchar_t *pText)
{
	//if we are not given text erase texture / string
	if ( !pText )
	{
		FlushTexture();
		m_sString.erase();
		return;
	}

	// Ignore if it's the same string.
	if( LTStrEquals( m_sString.c_str( ), pText ))
		return;

	// New text
	m_sString = pText;

	//if we already have a texture, rebuild it
//	if (!IsValid())
		CreateTexture();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CLTGUIPolyString::SetFontHeight(uint32 nFontHeight)
{
	m_Font.m_nHeight = nFontHeight;

	//if we already have a texture, rebuild it
	if (IsValid())
		CreateTexture();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CLTGUIPolyString::SetColor(uint32 nColor)
{
	m_nColor = nColor;

	uint32 a = GET_A(nColor);
	m_nShadowColor = SET_ARGB(a,0x00,0x00,0x00);

}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CLTGUIPolyString::SetAlignment(eTextAlign align) 
{
	m_Alignment = align;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CLTGUIPolyString::SetPos(LTVector2n vPos) 
{ 
	m_vPos.x = (float)vPos.x;
	m_vPos.y = (float)vPos.y;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CLTGUIPolyString::SetPos(LTVector2f vPos) 
{ 
	m_vPos.x = floor(vPos.x);
	m_vPos.y = floor(vPos.y);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CLTGUIPolyString::SetPos(uint32 x, uint32 y ) 
{ 
	m_vPos.x = (float)x;
	m_vPos.y = (float)y;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CLTGUIPolyString::SetPos(float x, float y ) 
{ 
	m_vPos.x = x;
	m_vPos.y = y;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CLTGUIPolyString::SetPos(LTIntPt pos) 
{ 
	m_vPos.x = (float)pos.x;
	m_vPos.y = (float)pos.y;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CLTGUIPolyString::FlushTexture()
{
	if (m_hString)
	{
		g_pLTCTextureString->ReleaseTextureString(m_hString);
		m_hString = NULL;
	}
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
float CLTGUIPolyString::GetWidth()
{
	if ( IsEmpty() || !IsValid())
		return 0.0f;

	LTRect2n nRect;

	g_pLTCTextureString->GetStringExtents( m_hString, nRect );

	return  (float) nRect.GetWidth();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
float CLTGUIPolyString::GetHeight()
{
	if ( IsEmpty() || !IsValid())
		return 0.0f;

	LTRect2n nRect;

	g_pLTCTextureString->GetStringExtents( m_hString, nRect );

	return  (float) nRect.GetHeight();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CLTGUIPolyString::GetRect( LTRect2n * nRect )
{

	if ( IsEmpty() || !IsValid())
	{
		nRect->Init();
		return;
	}
	
	g_pLTCTextureString->GetStringExtents( m_hString, *nRect );

}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: Set wrap width - zero turns of word wrap
//
// ----------------------------------------------------------------------- //
void CLTGUIPolyString::SetWrapWidth(uint32 nWidth)
{
	// don't wrap at less than height
	if (nWidth <= m_Font.m_nHeight)
		nWidth = 0;

	m_nWidth = nWidth;

	if (IsValid())
	{
		// zero width so reset wrap
		if ( !nWidth )
		{
			g_pLTCTextureString->WordWrapString(m_hString, 0xFFFFFFFF);
		}
		else
		{
			g_pLTCTextureString->WordWrapString(m_hString, nWidth);
		}
	}
}

uint32 CLTGUIPolyString::GetStringLength( )
{
	uint32 nLength = 0;

	if ( IsEmpty() || !IsValid())
		return nLength;

	g_pLTCTextureString->GetStringLength( m_hString, &nLength );

	return nLength;

}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CLTGUIPolyString::GetCharRect( uint32 nCharIndex, LTRect2n & rRect )
{
	if ( IsEmpty() || !IsValid())
		return;

	LTRect2n		rBlackBox;
	LTVector2f	vUVDims;
	LTVector2f  vUVPos;

	g_pLTCTextureString->GetCharRect(m_hString, nCharIndex, 
												rRect,
												rBlackBox,
												vUVPos,
												vUVDims);

}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CLTGUIPolyString::CreateTexture()
{
	if (IsEmpty())
		return;

	if (m_hString)
	{
		LTRESULT res = g_pLTCTextureString->RecreateTextureString(m_hString,m_sString.c_str(),m_Font);
		if (res != LT_OK)
		{
			FlushTexture();
		}
	}
	else
	{
		m_hString = g_pLTCTextureString->CreateTextureString(m_sString.c_str(),m_Font);
	}


	if (m_nWidth && m_hString)
	{
		g_pLTCTextureString->WordWrapString(m_hString, m_nWidth);
	}

}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
LTRESULT CLTGUIPolyString::Render( uint32 nFirstChar, uint32 nLastChar )
{
	if (IsEmpty())
		return LT_OK;

	if (!IsValid() )
		CreateTexture();

	
	std::wstring szSubStr = m_sString.substr( nFirstChar, nLastChar-nFirstChar );

	g_pLTCTextureString->SetupTextRendering(g_pLTCDrawPrim);

	if (m_vDropShadow != LTVector2f(0,0) )
	{
		LTVector2f vDrop = m_vPos;
		vDrop += m_vDropShadow;
		RenderSubString(szSubStr.c_str(), vDrop, m_nShadowColor );
	}


	return RenderSubString( szSubStr.c_str(), m_vPos, m_nColor );

}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
LTRESULT CLTGUIPolyString::Render()
{
	if (IsEmpty())
		return LT_OK;

	if (!IsValid() )
		CreateTexture();

	LTVector2f vAnchorScale(0.0f,0.0f);

	switch(m_Alignment) 
	{
	case kCenter:
		vAnchorScale.x = 0.5f;
		break;
	case kRight:
		vAnchorScale.x = 1.0f;
		break;
	}


	g_pLTCTextureString->SetupTextRendering(g_pLTCDrawPrim);

	if (m_vDropShadow != LTVector2f(0,0))
	{
		LTVector2f vDrop = m_vPos;
		vDrop += m_vDropShadow;
		RenderString(vDrop, m_nShadowColor, vAnchorScale );
	}

	return RenderString( m_vPos, m_nColor, vAnchorScale );
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
LTRESULT CLTGUIPolyString::RenderClipped(const LTRect2n& nRect)
{
	if (IsEmpty())
		return LT_OK;

	if (!IsValid() )
		CreateTexture();

	LTVector2f vAnchorScale(0.0f,0.0f);

	switch(m_Alignment) 
	{
	case kCenter:
		vAnchorScale.x = 0.5f;
		break;
	case kRight:
		vAnchorScale.x = 1.0f;
		break;
	}

	g_pLTCTextureString->SetupTextRendering(g_pLTCDrawPrim);

	if (m_vDropShadow != LTVector2f(0,0))
	{
		LTVector2f vDrop = m_vPos;
		vDrop += m_vDropShadow;
		RenderStringClipped(nRect,vDrop, m_nShadowColor, vAnchorScale );
	}


	return RenderStringClipped( nRect, m_vPos, m_nColor, vAnchorScale );
}




// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
LTRESULT CLTGUIPolyString::RenderString(const LTVector2f& vAnchor,
									uint32			nColor				/* = 0xFFFFFFFF            */,
									const LTVector2f& vAnchorScale	/* = LTVector2(0.0f, 0.0f) */, 
									const LTVector2f& vGround			/* = LTVector2(1.0f, 0.0f) */,
									const LTVector2f& vDown				/* = LTVector2(0.0f, 1.0f) */,
									const LTVector2f& vStretch			/* = LTVector2(1.0f, 1.0f) */)
{
	if (m_hString)
	{
		return g_pLTCTextureString->RenderString( m_hString, g_pLTCDrawPrim, vAnchor, nColor, vAnchorScale, vGround, vDown, vStretch );
	}

	return LT_ERROR;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
LTRESULT CLTGUIPolyString::RenderStringClipped(	const LTRect2n& rClipRect,
																const LTVector2f& vAnchor, 
																uint32 nColor,								/* 0xFFFFFFFF					*/
																const LTVector2f& vAnchorScale )		/* LTVector2f(0.0f, 0.0f)	*/

{
	if (m_hString)
	{
		return g_pLTCTextureString->RenderStringClipped( m_hString, g_pLTCDrawPrim, rClipRect, vAnchor, nColor, vAnchorScale );
	}

	return LT_ERROR;

}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CLTGUIPolyString::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
LTRESULT		CLTGUIPolyString::RenderSubString( const wchar_t* pszString,
															  const LTVector2f& vAnchor, 
											              uint32 nColor	)				/*	= 0xFFFFFFFF */
{

	if ( m_hString)
	{
		return g_pLTCTextureString->RenderSubString( m_hString, pszString, g_pLTCDrawPrim, vAnchor, nColor );
	}

	return LT_ERROR;
}