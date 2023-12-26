// ----------------------------------------------------------------------- //
//
// MODULE  : CGui.cpp
//
// PURPOSE : CGui - Definition and constants
//
// CREATED : 2002/05/24
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "gui.h"


CGui::CGui():
m_pFont(NULL),
m_pFontString(NULL),
m_hFontTexture(NULL),
m_nPosStyle(PS_NONE),
m_bVisible(true)
{}

CGui::~CGui(){}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGui::Init
//
//	PURPOSE:	Creates a Gui object that uses true type fonts 
//				for displaying text.
//
// ----------------------------------------------------------------------- //
LTRESULT CGui::Init(const char *filename, const char* fontface, uint32 nSize, LTFontParams* fontParams)
{

    uint8 ptSize = nSize;
    m_pFont = g_pLTCFontManager->CreateFont(filename, fontface, ptSize, 33, 255, fontParams);    


    if (m_pFont)
    {
        m_pFont->SetDefCharWidth((ptSize/4));
        m_pFont->SetDefColor(0xFFFF0000);        
    }

	if (m_pFont)
	{
		uint32 w,h;
		g_pLTCTexInterface->GetTextureDims(m_pFont->GetTexture(),w,h);
		g_pLTClient->CPrint("Created font <%s> using a %dX%d texture.",filename,w,h);
	}    
    else
    {
        g_pLTClient->CPrint("Failed to create font: ", filename);        
    }

	if (!m_pFontString)
	{
		m_pFontString = g_pLTCFontManager->CreateFormattedPolyString(m_pFont,"");
	}

    if (m_pFontString)
    {

        char buf[256];
        sprintf(buf, "This is the %s font !!!", fontface);
        m_pFontString->SetText(buf);

        uint32 nFontWidth = (uint32)m_pFontString->GetWidth();

        uint32 nWidth, nHeight;
        g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
        m_pFontString->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2)), static_cast<float>(nHeight/2));

        g_pLTClient->CPrint("Font String Created!");
    }
      

     return LT_OK;

}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGui::Init
//
//	PURPOSE:	Creates a Gui object that uses bitmap fonts.  The 
//				'monospace' parameter is used to determine if the font 
//				is monospace or proportional.
//
// ----------------------------------------------------------------------- //
LTRESULT CGui::Init(const char* filename, uint32 charw, uint32 charh, bool monospace)
{

	g_pLTCTexInterface->CreateTextureFromName(m_hFontTexture, filename);
	
	if (NULL == m_hFontTexture)
	{
        g_pLTClient->CPrint("Failed to load font texture");
		return LT_ERROR;
	}

	if (monospace)
	{
		// Create a monospace font
        m_pFont = g_pLTCFontManager->CreateFont(m_hFontTexture, 9, 9);
		m_pFont->SetDefColor(0xFF00FF00);
	}
	else
	{
		// Create a proportional font based only upon a texture (formatted with transparent(no alpha) green pixels)
		m_pFont = g_pLTCFontManager->CreateFont(m_hFontTexture);	
		m_pFont->SetDefColor(0xFF0000FF);
	}

    if (NULL != m_pFont)
    {
		const uint32 CharWidth 		= charw;
		const uint32 CharHeight 	= charh;
		m_pFont->SetDefCharWidth(CharWidth);
		m_pFont->SetDefCharHeight(CharHeight);
    }
	else
	{
        g_pLTClient->CPrint("Failed to create font from texture");
		return LT_ERROR;
	}


	if (NULL == m_pFontString)
	{
		m_pFontString = g_pLTCFontManager->CreateFormattedPolyString(m_pFont);
	}

	if (NULL != m_pFontString)
    {
        char buf[256];
        sprintf(buf, "This is the PC font !!!");
        m_pFontString->SetText(buf);

        uint32 nFontWidth = (uint32)m_pFontString->GetWidth();

        uint32 nWidth, nHeight;
        g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
        m_pFontString->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2)), static_cast<float>(nHeight/2));

        g_pLTClient->CPrint("Font String Created!");
    }
	m_pFont->SetDefBold(30);
	m_pFont->SetAttributes(CUI_FONT_BOLD);
	return LT_OK;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGui::SetText
//
//	PURPOSE:	Replaces the current text contained within the Gui with 
//				new text from sText parameter
//
// ----------------------------------------------------------------------- //
void CGui::SetText(const char* sText)
{
     if (m_pFontString)
     {
        m_pFontString->SetText(sText);
     }
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGui::SetTextColor
//
//	PURPOSE:	Changes the current text color to the color specified by
//				the color parameter.
//
// ----------------------------------------------------------------------- //
void CGui::SetTextColor(uint32 color)
{
	if (m_pFont)
	{
		m_pFont->SetDefColor(color);
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGui::SetPos
//
//	PURPOSE:	Sets the screen position of the Gui object
//
// ----------------------------------------------------------------------- //
void CGui::SetPos(uint16 x, uint16 y)
{
     if (m_pFontString)
     {
         m_pFontString->SetPosition(x, y);
     }
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGui::ApplyPositionStyle
//
//	PURPOSE:	Sets the screen position of the Gui object based off of
//				flags and offsets
//
// ----------------------------------------------------------------------- //
void CGui::ApplyPositionStyle(uint32 nPS, uint32 nXoffset, uint32 nYoffset)
{
	if (NULL == m_pFontString)
	{
		return;
	}

	m_nPosStyle = nPS;

	uint16 x = 0;
	uint16 y = 0;
	uint32 nScreenX = 0;
	uint32 nScreenY = 0;
	uint32 fDimsX = static_cast<uint32>(m_pFontString->GetWidth());
	uint32 fDimsY = static_cast<uint32>(m_pFontString->GetHeight());

	g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nScreenX, &nScreenY);

	if (nPS & PS_CENTER_ON_Y)
	{
		y = (nScreenY/2 - fDimsY/2) + nYoffset;
	}
	else if (nPS & PS_TOP)
	{
		y = nYoffset + PIXEL_BORDER_OFFSET;
	}
	else if (nPS & PS_BOTTOM)
	{
		y = (nScreenY - fDimsY) - PIXEL_BORDER_OFFSET;
	}

	if (nPS & PS_CENTER_ON_X)
	{
		x = (nScreenX/2 - fDimsX/2) + nXoffset;
	}
	else if (nPS & PS_LEFT)
	{
		x = nXoffset + PIXEL_BORDER_OFFSET;
	}
	else if (nPS & PS_RIGHT)
	{
		x = (nScreenX - fDimsX) - PIXEL_BORDER_OFFSET;
	}

	m_pFontString->SetPosition(x, y);
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGui::Center
//
//	PURPOSE:	Places the Gui object at the top of the screen
//
// ----------------------------------------------------------------------- //
void CGui::Top(uint32 nOffset)
{
	m_pFontString->SetPosition(PIXEL_BORDER_OFFSET, static_cast<float>(PIXEL_BORDER_OFFSET + nOffset));
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGui::Center
//
//	PURPOSE:	Places the Gui object in the center of the screen
//
// ----------------------------------------------------------------------- //
void CGui::Center()
{
   uint32 nFontWidth = (uint32)m_pFontString->GetWidth();
   uint32 nWidth, nHeight;
   g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
   m_pFontString->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2)), static_cast<float>(nHeight/2));
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGui::Center
//
//	PURPOSE:	Places the Gui object at the bottom of the screen
//
// ----------------------------------------------------------------------- //
void CGui::Bottom(uint32 nOffset)
{
	uint32 nFontHeight = (uint32)m_pFontString->GetHeight();
	uint32 nWidth, nHeight;
	g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
	m_pFontString->SetPosition(PIXEL_BORDER_OFFSET, static_cast<float>(nHeight - nFontHeight - PIXEL_BORDER_OFFSET - nOffset));
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGui::Render
//
//	PURPOSE:	Draws the contents (text) of the Gui object to the screen
//
// ----------------------------------------------------------------------- //
LTRESULT CGui::Render()
{
    
	if(m_pFontString)
    {
		m_pFontString->Render();
    }

	return LT_OK;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGui::Term
//
//	PURPOSE:	Deallocates the members of the Gui object
//
// ----------------------------------------------------------------------- //
LTRESULT CGui::Term()
{
	if (m_pFont) 
	{		
	  g_pLTCFontManager->DestroyFont(m_pFont);
	  if (m_pFontString)
	  {
		  g_pLTCFontManager->DestroyPolyString(m_pFontString);
	  }
	}

	// Release the texture handle if it was used for bitmap fonts
	if(m_hFontTexture)
	{
		if(g_pLTCTexInterface->ReleaseTextureHandle(m_hFontTexture))
		{
			m_hFontTexture = NULL;
		}
		else
		{
			g_pLTClient->CPrint("CGui::Term() - Could Not release font texture.");
		}
		
	}

	return LT_OK;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGuiWide
//
//	PURPOSE:	Simple gui string class for wide fonts 
//
// ----------------------------------------------------------------------- //
CGuiWide::CGuiWide() :
	m_hString(NULL),
	m_nColor(0xFFFFFFFF)
{	
	m_vPos.Init();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGuiWide::SetFont
//
//	PURPOSE:	Set the font type 
//
// ----------------------------------------------------------------------- //
void CGuiWide::SetFont(const CFontInfo& Font)
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
//	ROUTINE:	CGuiWide::SetText
//
//	PURPOSE:	Set the string text  
//
// ----------------------------------------------------------------------- //
void CGuiWide::SetText(const wchar_t *pText)
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
	if (IsValid())
		CreateTexture();
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGuiWide::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CGuiWide::SetFontHeight(uint32 nFontHeight)
{
	m_Font.m_nHeight = nFontHeight;

	//if we already have a texture, rebuild it
	if (IsValid())
		CreateTexture();
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGuiWide::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CGuiWide::SetColor(uint32 nColor)
{
	m_nColor = nColor;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGuiWide::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CGuiWide::SetPos(LTVector2n vPos) 
{ 
	m_vPos.x = (float)vPos.x;
	m_vPos.y = (float)vPos.y;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGuiWide::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CGuiWide::SetPos(LTVector2f vPos) 
{ 
	m_vPos.x = floor(vPos.x);
	m_vPos.y = floor(vPos.y);
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGuiWide::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CGuiWide::SetPos(uint32 x, uint32 y ) 
{ 
	m_vPos.x = (float)x;
	m_vPos.y = (float)y;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGuiWide::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CGuiWide::FlushTexture()
{
	if (m_hString)
	{

#ifdef _DEBUG
		g_pLTClient->CPrint("CLTGUIString flushing");
#endif

		g_pLTCTextureString->ReleaseTextureString(m_hString);
		m_hString = NULL;
	}
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGuiWide::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
void CGuiWide::CreateTexture()
{
	if (IsEmpty())
		return;

#ifdef _DEBUG
	if (m_hString)
	{
		g_pLTClient->CPrint("CLTGUIString re-creating Texture");
	}
	else
	{
		g_pLTClient->CPrint("CLTGUIString creating Texture");
	}
#endif

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

}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGuiWide::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
LTRESULT CGuiWide::Render()
{
	if (IsEmpty())
		return LT_OK;

	if (!IsValid() )
		CreateTexture();

	LTVector2f vAnchorScale(0.0f,0.0f);

/*	switch(m_Alignment) 
	{
	case kCenter:
		vAnchorScale.x = 0.5f;
		break;
	case kRight:
		vAnchorScale.x = 1.0f;
		break;
	}

*/

	g_pLTCTextureString->SetupTextRendering(g_pLTCDrawPrim);

	return RenderString( m_vPos, m_nColor, vAnchorScale );
}




// ----------------------------------------------------------------------- //
//
//	ROUTINE:	CGuiWide::
//
//	PURPOSE: 
//
// ----------------------------------------------------------------------- //
LTRESULT CGuiWide::RenderString(const LTVector2f& vAnchor,
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
