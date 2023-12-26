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



LTRESULT CGui::Init(uint32 nCharWidth, uint32 nCharHeight)
{
	uint8 ptSize = 18;
    m_pFont = g_pLTCFontManager->CreateFont("fonts/nolf.ttf", "Nolf", ptSize, 33, 255);    

    if (m_pFont)
    {
        m_pFont->SetDefCharWidth((ptSize/4));
        m_pFont->SetDefColor(0xFFFFFFFF);           
            
    }

	if (m_pFont)
	{
		uint32 w,h;
		g_pLTCTexInterface->GetTextureDims(m_pFont->GetTexture(),w,h);
		g_pLTClient->CPrint("Created font <%s> using a %dX%d texture.","fonts/nolf.ttf",w,h);
	}    
    else
    {
        g_pLTClient->CPrint("Failed to create font: ", "fonts/nolf.ttf");        
    }

	if (NULL == m_pFontString)
	{
		m_pFontString = g_pLTCFontManager->CreateFormattedPolyString(m_pFont);
	}

	if (NULL != m_pFontString)
    {
        char buf[256];
        sprintf(buf, "This is the console font !!!");
        m_pFontString->SetText(buf);

        uint32 nFontWidth = (uint32)m_pFontString->GetWidth();

        uint32 nWidth, nHeight;
        g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
        m_pFontString->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2)), static_cast<float>(nHeight/2));

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
		m_pFontString->SetPosition(x, y);
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



void CGui::Center()
{
   uint32 nFontWidth = (uint32)m_pFontString->GetWidth();
   uint32 nWidth, nHeight;
   g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
   m_pFontString->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2)), static_cast<float>(nHeight/2));
}



void CGui::Bottom(uint32 nOffset)
{
   uint32 nFontHeight = (uint32)m_pFontString->GetHeight();
   uint32 nWidth, nHeight;
   g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
   m_pFontString->SetPosition(PIXEL_BORDER_OFFSET, static_cast<float>(nHeight - nFontHeight - PIXEL_BORDER_OFFSET - nOffset));
}



void CGui::Top(uint32 nOffset)
{
   m_pFontString->SetPosition(PIXEL_BORDER_OFFSET, static_cast<float>(PIXEL_BORDER_OFFSET + nOffset));
}



LTRESULT CGui::Render()
{
	if (NULL != m_pFontString)
    {
		m_pFontString->Render();
    }

	return LT_OK;
}
