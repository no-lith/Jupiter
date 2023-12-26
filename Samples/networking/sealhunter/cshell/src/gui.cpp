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
#include <windows.h>

#include "gui.h"
#include "commandids.h"
#include "ltclientshell.h"

#define OFFSET_X 5
#define OFFSET_Y 5


enum
{
    SCREEN_MAIN   = 0,
    SCREEN_JOIN   = 1,
    SCREEN_INGAME = 2
};

// Position Styles
#define PS_CENTER_ON_X (1<<0)
#define PS_CENTER_ON_Y (1<<1)
#define PS_TOP		   (1<<2)
#define PS_BOTTOM      (1<<3)
#define PS_LEFT		   (1<<4)
#define PS_RIGHT	   (1<<5)

#define SEAL_HUD_WIDTH		75
#define SEAL_HUD_HEIGHT		75




//------------------------------------------------------------------------------
//	CGui::CGui()
//
//------------------------------------------------------------------------------
CGui::CGui():
m_hSplashScreen(NULL),
m_hSealHUD(NULL),
m_hFontTexture(NULL),
m_pFont(NULL),
m_pFontString(NULL),
m_pCursorString(NULL),
m_pJoinString(NULL),
m_nCurScreen(SCREEN_MAIN),
m_nMainCurPos(0),
m_nJoinCurPos(0),
m_Score(0),
m_Money(0.0f)
{
}



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
LTRESULT CGui::Init(uint32 nCharWidth, uint32 nCharHeight)
{
	// Load the title screen.
	if (!LoadTexture("tex\\seal\\splash.dtx", m_hSplashScreen))
	{
		return LT_ERROR;
	}

	// Load the HUD.
	if (!LoadTexture("tex\\seal\\sealgui.dtx", m_hSealHUD))
	{
		return LT_ERROR;
	}

	//
	// Create the gui stuff.
	//

	const char *pFontFilename = "fonts/SQR721B.TTF";
	const char *pFontFace = "Square721 BT";
	uint8 ptSize = 18;
    m_pFont = g_pLTCFontManager->CreateFont(pFontFilename, pFontFace, ptSize, 33, 255);

	if (m_pFont)
	{
		uint32 w, h;
		g_pLTCTexInterface->GetTextureDims(m_pFont->GetTexture(), w, h);
		g_pLTClient->CPrint("Created font <%s> using a %dX%d texture.", pFontFilename, w, h);

        m_pFont->SetDefCharWidth((ptSize/4));
        m_pFont->SetDefColor(0xFF0081DF);
	}
    else
    {
        g_pLTClient->CPrint("Failed to create font: ", pFontFilename);
    }

	if (LTNULL == m_pFontString)
	{
		m_pFontString = g_pLTCFontManager->CreateFormattedPolyString(m_pFont, "");
		if (LTNULL == m_pFontString)
		{
        	g_pLTClient->CPrint("Error creating font string!");
			return LT_ERROR;
		}
	}
	if (LTNULL == m_pCursorString)
	{
		m_pCursorString = g_pLTCFontManager->CreateFormattedPolyString(m_pFont, "");
		if (LTNULL == m_pCursorString)
		{
        	g_pLTClient->CPrint("Error creating cursor string!");
			return LT_ERROR;
		}
	}
	if (LTNULL == m_pScoreString)
	{
		m_pScoreString = g_pLTCFontManager->CreateFormattedPolyString(m_pFont, "");
		if (LTNULL == m_pScoreString)
		{
        	g_pLTClient->CPrint("Error creating score string!");
			return LT_ERROR;
		}
	}
	if (LTNULL == m_pMoneyString)
	{
		m_pMoneyString = g_pLTCFontManager->CreateFormattedPolyString(m_pFont, "");
		if (LTNULL == m_pMoneyString)
		{
        	g_pLTClient->CPrint("Error creating money string!");
			return LT_ERROR;
		}
	}

    char buf[256];
    sprintf(buf, "Start Normal Game\nFind Game\nHost Game\nQuit\n");
    m_pFontString->SetText(buf);

    char buf2[256];
    sprintf(buf2, ">\n.\n.\n");
    m_pCursorString->SetText(buf2);

    uint32 nFontWidth = static_cast<uint32>(m_pFontString->GetWidth());
    uint32 nFontHeight = static_cast<uint32>(m_pFontString->GetHeight());
    uint32 nWidth, nHeight;
    g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
    m_pFontString->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2)),
                               static_cast<float>((nHeight/2) - (nFontHeight/2)));
    m_pCursorString->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2) - 25),
                               static_cast<float>((nHeight/2) - (nFontHeight/2)));
    g_pLTClient->CPrint("Font String Created!");

	return LT_OK;
}



//------------------------------------------------------------------------------
//	CGui::Term()
//
//------------------------------------------------------------------------------
LTRESULT CGui::Term()
{
	if (m_pFont)
	{
		g_pLTCFontManager->DestroyFont(m_pFont);

		if (LTNULL != m_pFontString)
		{
			g_pLTCFontManager->DestroyPolyString(m_pFontString);
		}

		if (LTNULL != m_pCursorString)
		{
			g_pLTCFontManager->DestroyPolyString(m_pCursorString);
		}

		if (LTNULL != m_pScoreString)
		{
			g_pLTCFontManager->DestroyPolyString(m_pScoreString);
		}

		if (LTNULL != m_pMoneyString)
		{
			g_pLTCFontManager->DestroyPolyString(m_pMoneyString);
		}
	}

	ReleaseTexture(m_hSplashScreen);
	ReleaseTexture(m_hSealHUD);
	ReleaseTexture(m_hFontTexture);

	return LT_OK;
}



//-----------------------------------------------------------------------------
//  CGui::LoadTexture()
//
//-----------------------------------------------------------------------------
bool CGui::LoadTexture(const char* sFilename, HTEXTURE &hTexture)
{
    if (LTNULL == hTexture)
	{
		LTRESULT result = g_pLTCTexInterface->CreateTextureFromName(hTexture, sFilename);

		if (LT_OK != result)
		{
			g_pLTClient->CPrint("Error!!! Failed to load %s", sFilename);
			return false;
		}
	}

	return true;
}



//-----------------------------------------------------------------------------
//  CGui::ReleaseTexture
//
//-----------------------------------------------------------------------------
void CGui::ReleaseTexture(HTEXTURE &hTexture)
{
    if (hTexture != LTNULL)
    {
        g_pLTCTexInterface->ReleaseTextureHandle(hTexture);
        hTexture = NULL;
    }
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
//	CGui::SetScoreText()
//
//------------------------------------------------------------------------------
void CGui::SetScoreText()
{
	char buf[255];

	uint32 nScreenWidth, nScreenHeight;
    g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nScreenWidth, &nScreenHeight);

	unsigned xOffset = 5;

	unsigned x = SEAL_HUD_WIDTH + xOffset;
	unsigned y = nScreenHeight - SEAL_HUD_HEIGHT;

	if (m_pScoreString)
	{
        sprintf(buf, "%d", m_Score);

        m_pScoreString->SetText(buf);
        uint32 nFontHeight = (uint32)m_pScoreString->GetHeight();

		y += 10;

		m_pScoreString->SetPosition(static_cast<float>(x),  static_cast<float>(y));
	}

	if (m_pMoneyString)
	{
        sprintf(buf, "$%.2f", m_Money);

        m_pMoneyString->SetText(buf);
        uint32 nFontHeight = (uint32)m_pMoneyString->GetHeight();

		y += nFontHeight + 10;

		m_pMoneyString->SetPosition(static_cast<float>(x),  static_cast<float>(y));
	}
}



//-----------------------------------------------------------------------------
//	CGui::Draw()
//
//-----------------------------------------------------------------------------
void CGui::Draw(HTEXTURE hTexture, uint8 nPosStyle, uint8 nAlpha, uint8 nOffsetX, uint nOffsetY, uint nWidth, uint nHeight)
{
	// If we haven't created this texture yet, then return.
	if (!hTexture)
	{
		g_pLTClient->CPrint("Error: HTEXTURE is invalid!!!");
		return;
	}

	// Get screen dims.
	uint32 nScreenW, nScreenH;
	g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nScreenW, &nScreenH);

	// Get texture dims.
	uint32 nTexW, nTexH;
	g_pLTCTexInterface->GetTextureDims(hTexture, nTexW, nTexH);
	if (nWidth > 0)
	{
		nTexW = nWidth;
	}
	if (nHeight > 0)
	{
		nTexH = nHeight;
	}

	// Calculate destination rect on the screen.
	float top 		= 0.0f;
	float bottom 	= float(nTexH);
	float left 		= 0.0f;
	float right 	= float(nTexW);

	// Apply Position styles
	if( nPosStyle & PS_CENTER_ON_Y)
	{
		top = float(nScreenH/2 - nTexH/2);
		bottom = float(top + nTexH);
	}
	else if( nPosStyle & PS_TOP)
	{
		top = float(nOffsetY);
		bottom = float(nTexH + nOffsetY);
	}
	else if( nPosStyle & PS_BOTTOM)
	{
		top = float(nScreenH - nTexH - nOffsetY);
		bottom = float(nScreenH - nOffsetY);
	}

	if( nPosStyle & PS_CENTER_ON_X)
	{
		left = float(nScreenW/2 - nTexW/2);
		right = float(left + nTexW);
	}
	else if( nPosStyle & PS_LEFT)
	{
		left = float(nOffsetX);
		right = float(nTexW + nOffsetX);
	}
	else if( nPosStyle & PS_RIGHT)
	{
		left = float(nScreenW - nTexW - nOffsetX);
		right = float(nScreenW - nOffsetX);
	}

	// Set up the verts (clockwise from upper left).
	LT_POLYFT4 poly;
	poly.verts[0].x = left;
	poly.verts[0].y = top;
	poly.verts[0].z = SCREEN_NEAR_Z;
	poly.verts[0].u = 0.0f;
	poly.verts[0].v = 0.0f;

	poly.verts[1].x = right;
	poly.verts[1].y = top;
	poly.verts[1].z = SCREEN_NEAR_Z;
	poly.verts[1].u = 1.0f;
	poly.verts[1].v = 0.0f;

	poly.verts[2].x = right;
	poly.verts[2].y = bottom;
	poly.verts[2].z = SCREEN_NEAR_Z;
	poly.verts[2].u = 1.0f;
	poly.verts[2].v = 1.0f;

	poly.verts[3].x = left;
	poly.verts[3].y = bottom;
	poly.verts[3].z = SCREEN_NEAR_Z;
	poly.verts[3].u = 0.0f;
	poly.verts[3].v = 1.0f;

	// Set up color and alpha.
	poly.rgba.r = 255;
	poly.rgba.g = 255;
	poly.rgba.b = 255;
	poly.rgba.a = nAlpha;

	// Set which texture to use.
	g_pLTCDrawPrim->SetTexture(hTexture);

	// Set up the drawprim render state.
	g_pLTCDrawPrim->SetTransformType(DRAWPRIM_TRANSFORM_SCREEN);
	g_pLTCDrawPrim->SetColorOp(DRAWPRIM_MODULATE);
	g_pLTCDrawPrim->SetAlphaBlendMode(DRAWPRIM_BLEND_MOD_SRCALPHA);
	g_pLTCDrawPrim->SetZBufferMode(DRAWPRIM_NOZ);
	g_pLTCDrawPrim->SetAlphaTestMode(DRAWPRIM_NOALPHATEST);
	g_pLTCDrawPrim->SetClipMode(DRAWPRIM_FASTCLIP);
	g_pLTCDrawPrim->SetFillMode(DRAWPRIM_FILL);
	g_pLTCDrawPrim->SetCullMode(DRAWPRIM_CULL_NONE);
	g_pLTCDrawPrim->SetCamera(NULL);

	// Draw the Image (just 1 quad).
	g_pLTCDrawPrim->DrawPrim(&poly, 1);
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
            if(COMMAND_MOVE_FORWARD == nCommand)
            {
                if(m_nMainCurPos > 0)
                {
                    m_nMainCurPos--;
                    ReCalcCursor(m_nMainCurPos, 4, m_pFontString);
                }
            }
            else if(COMMAND_MOVE_BACKWARD == nCommand)
            {
                if(m_nMainCurPos < 3)
                {
                    m_nMainCurPos++;
                    ReCalcCursor(m_nMainCurPos, 4, m_pFontString);
                }
            }
            else if((COMMAND_SHOOT == nCommand) || (COMMAND_ENTER == nCommand))
            {
                switch(m_nMainCurPos)
                {
                case 0:
                    {
                        g_pCShell->StartNormalGame();
                        m_nCurScreen = SCREEN_INGAME;
						m_Score = 0;
						m_Money = 0.0f;
                    }
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
                    {
                        g_pCShell->StartMultiplayerServer();
                        m_nCurScreen = SCREEN_INGAME;
						m_Score = 0;
						m_Money = 0.0f;
                    }
                    break;
                case 3:
                    {
                        g_pLTClient->Shutdown();
                    }
                    break;
                default:
                    break;
                }
            }
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
            else if(COMMAND_MOVE_BACKWARD == nCommand)
            {
                if(m_nJoinCurPos < 4)
                {
                    m_nJoinCurPos++;
                    ReCalcCursor(m_nJoinCurPos, 5, m_pJoinString);
                }
            }
			else if((COMMAND_SHOOT == nCommand)  || (COMMAND_ENTER == nCommand))
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
                          m_nCurScreen = SCREEN_INGAME;
						  m_Score = 0;
						  m_Money = 0.0f;
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
//	CGui::Render()
//
//------------------------------------------------------------------------------
LTRESULT CGui::Render()
{
	switch (m_nCurScreen)
    {
    case SCREEN_MAIN:
        {
			Draw(m_hSplashScreen, (PS_CENTER_ON_X | PS_CENTER_ON_Y), 255, 0, 0, 450, 450);

			if (NULL != m_pFontString &&
				NULL != m_pCursorString)
            {
                m_pFontString->Render();
                m_pCursorString->Render();
            }
        }
        break;
    case SCREEN_JOIN:
        {
			Draw(m_hSplashScreen, (PS_CENTER_ON_X | PS_CENTER_ON_Y), 255, 0, 0, 450, 450);

			if (NULL != m_pJoinString &&
				NULL != m_pCursorString)
            {
                m_pJoinString->Render();
                m_pCursorString->Render();
            }
        }
        break;
    case SCREEN_INGAME:
        {
			Draw(m_hSealHUD, (PS_BOTTOM | PS_LEFT), 255, 0, 0, SEAL_HUD_WIDTH, SEAL_HUD_HEIGHT);

			if (NULL != m_pScoreString &&
				NULL != m_pMoneyString)
            {
 				SetScoreText();
                m_pScoreString->Render();
                m_pMoneyString->Render();
            }
        }
        break;
    default:
        break;
    }

	return LT_OK;
}
