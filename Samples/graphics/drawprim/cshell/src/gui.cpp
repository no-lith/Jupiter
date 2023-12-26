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
m_pFontString(NULL)
{}

CGui::~CGui(){}

LTRESULT CGui::Init(const char *filename, const char* fontface, uint32 nSize)
{

    uint8 ptSize = nSize;
    m_pFont = g_pLTCFontManager->CreateFont(filename, fontface, ptSize, 33, 255);    


    if (m_pFont)
    {
        m_pFont->SetDefCharWidth((ptSize/4));
        m_pFont->SetDefColor(0xFF00FF00);        
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

void CGui::SetText(const char* sText)
{
     if (m_pFontString)
     {
        m_pFontString->SetText(sText);
     }
}

void CGui::SetPos(uint16 x, uint16 y)
{
     if (m_pFontString)
     {
         m_pFontString->SetPosition(x, y);
     }
}

void CGui::Center()
{
   uint32 nFontWidth = (uint32)m_pFontString->GetWidth();
   uint32 nWidth, nHeight;
   g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
   m_pFontString->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2)), static_cast<float>(nHeight/2));
}

void CGui::Bottom()
{
   uint32 nFontHeight = (uint32)m_pFontString->GetHeight();
   uint32 nWidth, nHeight;
   g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
   m_pFontString->SetPosition(10, static_cast<float>(nHeight - nFontHeight - 10));
}

LTRESULT CGui::Render()
{
    
	if(m_pFontString)
    {
		m_pFontString->Render();
    }

	return LT_OK;
}

LTRESULT CGui::Dispose()
{
	if (m_pFont) 
	{		
	  g_pLTCFontManager->DestroyFont(m_pFont);	
	  if (m_pFontString)	g_pLTCFontManager->DestroyPolyString(m_pFontString);
	}

	return LT_OK;
}