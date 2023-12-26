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
#include "GuiUtils.h"



CGui::CGui()
	: m_pFontString(NULL),
	  m_pFont(NULL),
	  m_hFontTexture(NULL),
	  m_nPosStyle(PS_NONE),
	  m_bVisible(true)
{
}



CGui::~CGui()
{
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

LTRESULT CGui::Init(const char* pFilename, uint32 nCharHeight)
{
	char Filename[260];
	Filename[0] = '\0';

	if (LTNULL == pFilename)
	{
		strcpy(Filename, "console\\console_font.dtx");
		Filename[sizeof(Filename) - 1] = '\0';
	}
	else
	{
		strcpy(Filename, pFilename);
		Filename[sizeof(Filename) - 1] = '\0';
	}

	g_pLTCTexInterface->CreateTextureFromName(m_hFontTexture, Filename);
	if (NULL == m_hFontTexture)
	{
        g_pLTClient->CPrint("Failed to load font texture");
		return LT_ERROR;
	}

	m_pFont = g_pLTCFontManager->CreateFont(m_hFontTexture, 9,9 );
    if (NULL != m_pFont)
    {
        m_pFont->SetDefColor(0xFF00FF00);
    }

	if (NULL == m_pFontString)
	{
		m_pFontString = g_pLTCFontManager->CreateFormattedPolyString(m_pFont);
		m_pFontString->SetCharScreenHeight(nCharHeight);
	}

	if (NULL != m_pFontString)
    {
        char buf[256];
        sprintf(buf, "This is the console font !!!");
        m_pFontString->SetText(buf);

        uint32 nFontWidth = (uint32)m_pFontString->GetWidth();

        uint32 nWidth, nHeight;
        g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
		GuiUtils::SetPosSafeArea(m_pFontString, (nWidth/2) - (nFontWidth/2), nHeight/2);

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
 	}
 	else
 	{
		// Create a proportional font based only upon a texture (formatted with transparent(no alpha) green pixels)
 		m_pFont = g_pLTCFontManager->CreateFont(m_hFontTexture);	
 	}
 
 	if (NULL != m_pFont)
 	{
 		const uint32 CharWidth 		= charw;
 		const uint32 CharHeight 	= charh;
 		m_pFont->SetDefCharWidth(CharWidth);
 		m_pFont->SetDefCharHeight(CharHeight);
 		if(monospace)
 		{
 			m_pFont->SetDefColor(0xFF66FF00);
 		}
 		else
 		{
 			m_pFont->SetDefColor(0xFF0077FF);
 		}
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
 		sprintf(buf, "This is a font !!!");
 		m_pFontString->SetText(buf);
   
 		uint32 nFontWidth = (uint32)m_pFontString->GetWidth();
   
 		uint32 nWidth, nHeight;
 		g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
		GuiUtils::SetPosSafeArea(m_pFontString, (nWidth/2) - (nFontWidth/2), nHeight/2);

 		g_pLTClient->CPrint("Font String Created!");
 	}
 	return LT_OK;
}



LTRESULT CGui::Term()
{
	if (NULL != m_pFont)
	{
		g_pLTCFontManager->DestroyFont(m_pFont);

		if (NULL != m_pFontString)
		{
			g_pLTCFontManager->DestroyPolyString(m_pFontString);
		}
	}

	return LT_OK;
}



void CGui::SetText(const char* sText)
{
	if (NULL != m_pFontString)
	{
		m_pFontString->SetText(sText);
		if (m_nPosStyle != PS_NONE)
		{
			ApplyPositionStyle(m_nPosStyle);
		}
	}
}



void CGui::SetPos(uint16 x, uint16 y)
{
	if (NULL != m_pFontString)
	{
		GuiUtils::SetPosSafeArea(m_pFontString, x, y);
	}
}



void CGui::SetColor(uint32 argb)
{
	if (NULL != m_pFontString)
	{
		m_pFontString->SetColor(argb);
	}
}



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
		y = nYoffset;
	}
	else if (nPS & PS_BOTTOM)
	{
		y = (nScreenY - fDimsY);
	}

	if (nPS & PS_CENTER_ON_X)
	{
		x = (nScreenX/2 - fDimsX/2) + nXoffset;
	}
	else if (nPS & PS_LEFT)
	{
		x = nXoffset;
	}
	else if (nPS & PS_RIGHT)
	{
	    x = (nScreenX - fDimsX);
	}

	GuiUtils::SetPosSafeArea(m_pFontString, x, y);
}



void CGui::Center()
{
	if (NULL != m_pFontString)
	{
		uint32 nFontWidth = (uint32)m_pFontString->GetWidth();
		uint32 nWidth, nHeight;
		g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);

		GuiUtils::SetPosSafeArea(m_pFontString, (nWidth/2) - (nFontWidth/2), nHeight/2);
	}
}



void CGui::Bottom(uint32 nOffset)
{
	if (NULL != m_pFontString)
	{
		uint32 nFontHeight = (uint32)m_pFontString->GetHeight();
		uint32 nWidth, nHeight;
		g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);

		GuiUtils::SetPosSafeArea(m_pFontString, 0, nHeight - nFontHeight - nOffset);
	}
}



void CGui::Top(uint32 nOffset)
{
	GuiUtils::SetPosSafeArea(m_pFontString, 0, nOffset);
}



LTRESULT CGui::Render()
{
	if (NULL != m_pFontString)
    {
		m_pFontString->Render();
    }

	return LT_OK;
}
