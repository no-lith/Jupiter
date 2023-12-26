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
#include "commandids.h"
#include "ltclientshell.h"

enum
{
    SCREEN_MAIN = 0,
    SCREEN_JOIN = 1
};


//------------------------------------------------------------------------------
//	CGui::CGui()
//
//------------------------------------------------------------------------------
CGui::CGui():
m_pFont(NULL),
m_pFontString(NULL),
m_pCursorString(NULL),
m_pJoinString(NULL),
m_nCurScreen(SCREEN_MAIN),
m_nMainCurPos(0),
m_nJoinCurPos(0)
{}


//------------------------------------------------------------------------------
//	CGui::~CGui()
//
//------------------------------------------------------------------------------
CGui::~CGui()
{
}


//------------------------------------------------------------------------------
//	CGui::Init(const char *filename, const char* fontface)
//
//------------------------------------------------------------------------------
LTRESULT CGui::Init(const char *filename, const char* fontface)
{

    uint8 ptSize = 18;
    m_pFont = g_pLTCFontManager->CreateFont(filename, fontface, ptSize, 33, 255);    

	if (m_pFont)
	{
		uint32 w,h;
		g_pLTCTexInterface->GetTextureDims(m_pFont->GetTexture(),w,h);
		g_pLTClient->CPrint("Created font <%s> using a %dX%d texture.",filename,w,h);

        m_pFont->SetDefCharWidth((ptSize/4));
        m_pFont->SetDefColor(0xFF00FF00); 
	}    
    else
    {
        g_pLTClient->CPrint("Failed to create font: ", filename);        
    }

	if (!m_pFontString && !m_pCursorString)
	{
		m_pFontString = g_pLTCFontManager->CreateFormattedPolyString(m_pFont,"");
		m_pCursorString = g_pLTCFontManager->CreateFormattedPolyString(m_pFont,"");
	}

    if(m_pFontString && m_pCursorString)
    {

        char buf[256];
        sprintf(buf, "Start Normal Game\nFind Game\nHost Game\n");
        m_pFontString->SetText(buf);

        char buf2[256];
        sprintf(buf2, ">\n.\n.\n");
        m_pCursorString->SetText(buf2);

        uint32 nFontWidth = (uint32)m_pFontString->GetWidth();
        uint32 nFontHeight = (uint32)m_pFontString->GetHeight();
        uint32 nWidth, nHeight;
        g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
        m_pFontString->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2)), 
                                   static_cast<float>((nHeight/2) - (nFontHeight/2)));
        m_pCursorString->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2) - 25), 
                                   static_cast<float>((nHeight/2) - (nFontHeight/2)));
        g_pLTClient->CPrint("Font String Created!");
    }
      
     return LT_OK;

}


//------------------------------------------------------------------------------
//	CGui::SetText(const char* sText)
//
//------------------------------------------------------------------------------
void CGui::SetText(const char* sText)
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


//------------------------------------------------------------------------------
//	CGui::Render()
//
//------------------------------------------------------------------------------
LTRESULT CGui::Render()
{
    switch(m_nCurScreen)
    {
    case SCREEN_MAIN:
        {            
            if(m_pFontString && m_pCursorString)
            {
                m_pFontString->Render();
                m_pCursorString->Render();
            }
        }
        break;
    case SCREEN_JOIN:
        {
            if(m_pJoinString && m_pCursorString)
            {
                m_pJoinString->Render();
                m_pCursorString->Render();
            }
        }
        break;
    default:
        break;
    }

	return LT_OK;
}


//------------------------------------------------------------------------------
//	CGui::HandleInput(int nCommand)
//
//------------------------------------------------------------------------------
void CGui::HandleInput(int nCommand)
{
    switch(m_nCurScreen)
    {
    case SCREEN_MAIN:
        {
            //g_pLTClient->CPrint("Handle: %d", nCommand);

            if(COMMAND_MOVE_FORWARD == nCommand) 
            {
                if(m_nMainCurPos > 0)
                {
                    m_nMainCurPos--;
                    ReCalcCursor(m_nMainCurPos, 3, m_pFontString);
                }
            }
            else
            if(COMMAND_MOVE_BACKWARD == nCommand) 
            {
                if(m_nMainCurPos < 2)
                {
                    m_nMainCurPos++;
                    ReCalcCursor(m_nMainCurPos, 3, m_pFontString);
                }
            }
            else
            if(COMMAND_ENTER == nCommand)
            {
                switch(m_nMainCurPos)
                {
                case 0:
                    g_pCShell->StartNormalGame();
                    break;
                case 1:
                    {
                        g_pLTClient->CPrint("Finding servers...");

                        // Render next frame before checking for servers
                        g_pCShell->Render();
                        uint nServers = g_pCShell->FindServers(m_ServerList);
                        g_pLTClient->CPrint("Active Servers: %d", nServers);
                        m_nCurScreen = SCREEN_JOIN;
                        m_pCursorString->SetText(">\n.\n.\n.\n.\n");
                        CreateJoinString();
                        ReCalcCursor(m_nJoinCurPos, 5, m_pJoinString);
                    }
                    break;
                case 2:
                    g_pCShell->StartMultiplayerServer();
                    break;
                default:
                    break;
                }
            }
                            
            //g_pLTClient->CPrint("Gui::HandleInput: %d %d", nCommand, m_nMainCurPos);
        }
        break;
    case SCREEN_JOIN:
        {
            if(COMMAND_MOVE_FORWARD == nCommand) 
            {
                if(m_nJoinCurPos > 0)
                {
                    m_nJoinCurPos--;
                    ReCalcCursor(m_nJoinCurPos, 5, m_pJoinString);
                }
            }
            else
            if(COMMAND_MOVE_BACKWARD == nCommand) 
            {
                if(m_nJoinCurPos < 4)
                {
                    m_nJoinCurPos++;
                    ReCalcCursor(m_nJoinCurPos, 5, m_pJoinString);
                }
            }
            if(COMMAND_ENTER == nCommand)
            {
                switch(m_nJoinCurPos)
                {
                case 0:
                    {
                        m_nCurScreen = SCREEN_MAIN;
                        ReCalcCursor(m_nMainCurPos, 3, m_pFontString);
                        DestroyJoinString();
                    }
                    break;
                default:
                    {
                        if(m_ServerList[ m_nJoinCurPos -1 ].IpAddress[0] != '\0')
                        {
                          g_pCShell->JoinMultiplayerGame(m_ServerList[ m_nJoinCurPos -1 ].IpAddress);
                        }
                    }
                    break;
                }
            }
        }
        break;
    default:
        break;
    }
}


//------------------------------------------------------------------------------
//	CGui::ReCalcCursor(uint8 nPos, uint8 nMax, CUIFormattedPolyString* pString)
//
//------------------------------------------------------------------------------
void CGui::ReCalcCursor(uint8 nPos, uint8 nMax, CUIFormattedPolyString* pString)
{    
    char buf[256];
    memset(buf, 0, 255);
    for(uint8 i = 0; i < nMax; i++)
    {
        if(nPos == i)
        {
            strcat(buf,">\n");
        }
        else
        {
            strcat(buf,".\n");
        }
    }
    
    m_pCursorString->SetText(buf);

    uint32 nFontWidth = (uint32)pString->GetWidth();
    uint32 nFontHeight = (uint32)pString->GetHeight();
    uint32 nWidth, nHeight;
    g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
    m_pCursorString->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2) - 25), 
                                    static_cast<float>((nHeight/2) - (nFontHeight/2)));
    
}


//------------------------------------------------------------------------------
//	CGui::CreateJoinString()
//
//------------------------------------------------------------------------------
void CGui::CreateJoinString()
{
    if(!m_pJoinString)
    {
       m_pJoinString = g_pLTCFontManager->CreateFormattedPolyString(m_pFont,"");
       //m_pJoinString->SetText("Back\nLithTech Test Server\n----\n----\n----");
       char buf[4096];
       memset(buf, 0, 4095);
       sprintf(buf, "Back\n");

       for(int i = 0; i < 4; i++)
       {
           /*
           char tmpBuf[256];
           sprintf(tmpBuf, );
           */
           strcat(buf, m_ServerList[i].Name);
           strcat(buf, " ");
           strcat(buf, m_ServerList[i].IpAddress);
           strcat(buf, "\n");

       }
       m_pJoinString->SetText(buf);

       uint32 nFontWidth = (uint32)m_pJoinString->GetWidth();
       uint32 nFontHeight = (uint32)m_pJoinString->GetHeight();
       uint32 nWidth, nHeight;
       g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
       m_pJoinString->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2)), 
                                  static_cast<float>((nHeight/2) - (nFontHeight/2)));
       m_pCursorString->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2) - 25), 
                                    static_cast<float>((nHeight/2) - (nFontHeight/2)));
    }
}


//------------------------------------------------------------------------------
//	CGui::DestroyJoinString()
//
//------------------------------------------------------------------------------
void CGui::DestroyJoinString()
{
	  if (m_pJoinString)
      {
          g_pLTCFontManager->DestroyPolyString(m_pJoinString);
          m_pJoinString = NULL;
      }
      
}


//------------------------------------------------------------------------------
//	CGui::Dispose()
//
//------------------------------------------------------------------------------
LTRESULT CGui::Dispose()
{
	if (m_pFont) 
	{		
	  g_pLTCFontManager->DestroyFont(m_pFont);	
	  if (m_pFontString)	g_pLTCFontManager->DestroyPolyString(m_pFontString);
	  if (m_pCursorString)	g_pLTCFontManager->DestroyPolyString(m_pCursorString);
	}

	return LT_OK;
}