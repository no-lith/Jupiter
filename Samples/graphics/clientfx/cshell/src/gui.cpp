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
m_pSidePanel(NULL)
{}

CGui::~CGui(){}

LTRESULT CGui::Init(const char *filename, const char* fontface)
{

    uint8 ptSize = 18;
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

    if(!m_pSidePanel)
	{
		m_pSidePanel = g_pLTCFontManager->CreateFormattedPolyString(m_pFont,"");
	}


     if (m_pFontString && m_pSidePanel)
    {

        char buf[256];
        sprintf(buf, "Current FX: DemoEffect");
        m_pFontString->SetText(buf);

        char buf2[256];
        sprintf(buf2, "0\n1\n2\n3\n4\n5\n6\n7\n8\n9\n] Next\n[ Prev\nT Toggle Gui");
        m_pSidePanel->SetText(buf2);

        uint32 nFontWidth = (uint32)m_pFontString->GetWidth();

        uint32 nWidth, nHeight;
        g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
        m_pFontString->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2)), 0);//
        m_pSidePanel->SetPosition(2, 0);
        g_pLTClient->CPrint("Font String Created!");
    }
      

     return LT_OK;

}

void CGui::SetTitleText(const char* sText)
{
     if (m_pFontString)
     {
        m_pFontString->SetText(sText);
        uint32 nFontWidth = (uint32)m_pFontString->GetWidth();
        uint32 nWidth, nHeight;
        g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
        m_pFontString->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2)), 0.0f);
     }
}

void CGui::SetSideText(const char* sText)
{
     if (m_pSidePanel)
     {
        m_pSidePanel->SetText(sText);
        //uint32 nFontWidth = (uint32)m_pSidePanel->GetWidth();
        //uint32 nWidth, nHeight;
        //g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
        m_pSidePanel->SetPosition(2.0f, 0.0f);
     }
}

LTRESULT CGui::Render()
{
    
	if(m_pFontString)
    {
		m_pFontString->Render();
    }

	if(m_pSidePanel)
    {
		m_pSidePanel->Render();
    }

	return LT_OK;
}

LTRESULT CGui::Dispose()
{
	if (m_pFont) 
	{		
	  g_pLTCFontManager->DestroyFont(m_pFont);	
	  if (m_pFontString)	g_pLTCFontManager->DestroyPolyString(m_pFontString);
	  if (m_pSidePanel)	g_pLTCFontManager->DestroyPolyString(m_pSidePanel);
	}

	return LT_OK;
}