// ----------------------------------------------------------------------- //
//
// MODULE  : CStatsGui.cpp
//
// PURPOSE : CStatsGui - Implementation
//
// CREATED : 07/18/2003
//
// (c) 2003 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //
#include <windows.h>

#include "statsgui.h"
#include "clientinterfaces.h"
#include <iltdrawprim.h>
#include <iltmessage.h>

//------------------------------------------------------------------------------
//	CStatsGui::CStatsGui()
//
//------------------------------------------------------------------------------
CStatsGui::CStatsGui():
m_hBackDrop(NULL),
m_pFont(NULL),
m_pStatsString_Title(NULL),
m_pStatsString_Playername(NULL),
m_pStatsString_Sealswhacked(NULL),
m_pStatsString_Moneyearned(NULL),
m_pScores(NULL),
m_iNumPlayers(0)
{

}


//------------------------------------------------------------------------------
//	CStatsGui::~CStatsGui()
//
//------------------------------------------------------------------------------
CStatsGui::~CStatsGui()
{

}


//------------------------------------------------------------------------------
//	CStatsGui::Init()
//
//------------------------------------------------------------------------------
LTRESULT CStatsGui::Init()
{
    if (NULL == m_hBackDrop)
	{
		LTRESULT result = g_pLTCTexInterface->CreateTextureFromName(m_hBackDrop, "tex/ui/backdrop.dtx");

		if (LT_OK != result)
		{
			g_pLTClient->CPrint("Error!!! Failed to load %s", "tex/ui/backdrop.dtx");
			return false;
		}
	}

    //Init the font
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

	if (LTNULL == m_pStatsString_Title)
	{
		m_pStatsString_Title = g_pLTCFontManager->CreateFormattedPolyString(m_pFont, "");
		if (LTNULL == m_pStatsString_Title)
		{
        	g_pLTClient->CPrint("Error creating m_pStatsString_Playername!");
			return LT_ERROR;
		}
	}
    m_pStatsString_Title->SetText("");


	if (LTNULL == m_pStatsString_Playername)
	{
		m_pStatsString_Playername = g_pLTCFontManager->CreateFormattedPolyString(m_pFont, "");
		if (LTNULL == m_pStatsString_Playername)
		{
        	g_pLTClient->CPrint("Error creating m_pStatsString_Playername!");
			return LT_ERROR;
		}
	}
    m_pStatsString_Playername->SetText("");


	if (LTNULL == m_pStatsString_Sealswhacked)
	{
		m_pStatsString_Sealswhacked = g_pLTCFontManager->CreateFormattedPolyString(m_pFont, "");
		if (LTNULL == m_pStatsString_Sealswhacked)
		{
        	g_pLTClient->CPrint("Error creating m_pStatsString_Playername!");
			return LT_ERROR;
		}
	}
    m_pStatsString_Sealswhacked->SetText("");


	if (LTNULL == m_pStatsString_Moneyearned)
	{
		m_pStatsString_Moneyearned = g_pLTCFontManager->CreateFormattedPolyString(m_pFont, "");
		if (LTNULL == m_pStatsString_Moneyearned)
		{
        	g_pLTClient->CPrint("Error creating m_pStatsString_Playername!");
			return LT_ERROR;
		}
	}
    m_pStatsString_Moneyearned->SetText("");


    /*
    uint32 nFontWidth = static_cast<uint32>(m_pStatsString_Playername->GetWidth());
    uint32 nFontHeight = static_cast<uint32>(m_pStatsString_Playername->GetHeight());
    uint32 nWidth, nHeight;
    g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
    m_pStatsString_Playername->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2)),
                               static_cast<float>((nHeight/2) - (nFontHeight/2)));
    */

    return LT_OK;
}


//------------------------------------------------------------------------------
//	CStatsGui::Term()
//
//------------------------------------------------------------------------------
LTRESULT CStatsGui::Term()
{

    if(LTNULL != m_hBackDrop)
    {
        g_pLTCTexInterface->ReleaseTextureHandle(m_hBackDrop);
        m_hBackDrop = NULL;
    }

    if(LTNULL != m_pFont)
    {

        g_pLTCFontManager->DestroyFont(m_pFont);
    }

	if (LTNULL != m_pStatsString_Title)
	{
		g_pLTCFontManager->DestroyPolyString(m_pStatsString_Title);
	}

	if (LTNULL != m_pStatsString_Playername)
	{
		g_pLTCFontManager->DestroyPolyString(m_pStatsString_Playername);
	}

	if (LTNULL != m_pStatsString_Sealswhacked)
	{
		g_pLTCFontManager->DestroyPolyString(m_pStatsString_Sealswhacked);
	}

	if (LTNULL != m_pStatsString_Moneyearned)
	{
		g_pLTCFontManager->DestroyPolyString(m_pStatsString_Moneyearned);
	}

	if (LTNULL != m_pScores)
	{
		delete[] m_pScores;
        m_pScores = NULL;
	}


    return LT_OK;
}


//------------------------------------------------------------------------------
//	CStatsGui::Render()
//
//------------------------------------------------------------------------------
LTRESULT CStatsGui::Render()
{
    if (NULL == m_pStatsString_Title ||
    	NULL == m_pStatsString_Playername ||
		NULL == m_pStatsString_Sealswhacked ||
		NULL == m_pStatsString_Moneyearned)
	{
		return LT_ERROR;
	}

    // If we haven't created this texture yet, then return.
	if (!m_hBackDrop)
	{
		g_pLTClient->CPrint("Error: m_hBackDrop is invalid!!!");
		return LT_ERROR;
	}

	// Get screen dims.
	uint32 nScreenW, nScreenH;
	g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nScreenW, &nScreenH);

	// Get texture dims.
	uint32 nTexW, nTexH;
	g_pLTCTexInterface->GetTextureDims(m_hBackDrop, nTexW, nTexH);
	/*
    if (nWidth > 0)
	{
		nTexW = nWidth;
	}
	if (nHeight > 0)
	{
		nTexH = nHeight;
	}
    */

	// Calculate destination rect on the screen.
	float top 		= 0.0f;
	float bottom 	= float(nTexH);
	float left 		= 0.0f;
	float right 	= float(nTexW);

    // Center on the Y axis
	top = float(nScreenH/2 - nTexH/2);
	bottom = float(top + nTexH);

    // Center on the X axis
	left = float(nScreenW/2 - nTexW/2);
	right = float(left + nTexW);

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
	poly.rgba.a = 128;//nAlpha;

	// Set which texture to use.
	g_pLTCDrawPrim->SetTexture(m_hBackDrop);

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

    // Draw the stats now.
    m_pStatsString_Title->Render();
    m_pStatsString_Playername->Render();
    m_pStatsString_Sealswhacked->Render();
    m_pStatsString_Moneyearned->Render();

    return LT_OK;
}


//------------------------------------------------------------------------------
//	CStatsGui::HandleMessage(ILTMessage_Read* pMessage)
//
//------------------------------------------------------------------------------
LTRESULT CStatsGui::HandleMessage(ILTMessage_Read* pMessage)
{
    m_iNumPlayers = pMessage->Readuint8();

    if(m_iNumPlayers > 0)
    {
        if(LTNULL != m_pScores)
        {
            delete m_pScores;
            m_pScores = NULL;
        }

        m_pScores = new SCORESTRUCT[m_iNumPlayers];
        assert(m_pScores);

        //g_pLTClient->CPrint("Server Scores:");

        for(int i = 0; i < m_iNumPlayers; i++)
        {
            m_pScores[i].iClientID = pMessage->Readuint32();
            pMessage->ReadString(m_pScores[i].sPlayerName , 32);
            m_pScores[i].iScore = pMessage->Readuint32();
            m_pScores[i].fMoney = pMessage->Readfloat();
            //g_pLTClient->CPrint("(%d) %d - %s - %d - $%.2f", i, iClientID, sName, iScore, fMoney);
        }

        // Sort the stats from most to least money
        SortStats();

        // Recalculate the poly string
        RecalcStatsString();
    }

    return LT_OK;
}


//------------------------------------------------------------------------------
//	LTRESULT CStatsGui::SortStats()
//
//------------------------------------------------------------------------------
LTRESULT CStatsGui::SortStats()
{
    SCORESTRUCT largest;
    int largestElement = 0;
    SCORESTRUCT temp;
    largest.fMoney = -1.0f;

    if(m_iNumPlayers < 2)
    {
        return LT_OK;
    }

    //Sort this stuff (Using Selection Sort algorithm)
    for(int i = 0; i < m_iNumPlayers; i++)
    {
        for(int j = 0; j < (m_iNumPlayers - i); j++)
        {
            if(m_pScores[j+i].fMoney > largest.fMoney)
            {
                largest.fMoney = m_pScores[j+i].fMoney;
                largestElement = (j+i);
            }
        }

        //do the move.
        int nSize = sizeof(SCORESTRUCT);
        temp = m_pScores[i];
        m_pScores[i] = m_pScores[largestElement];
        m_pScores[largestElement] = temp;

        // Invalidate the largest element now.
        largest.fMoney = -1.0f;
    }

    return LT_OK;
}


//------------------------------------------------------------------------------
//	LTRESULT CStatsGui::RecalcStatsString()
//
//------------------------------------------------------------------------------
LTRESULT CStatsGui::RecalcStatsString()
{
    if (NULL == m_pStatsString_Title ||
    	NULL == m_pStatsString_Playername ||
		NULL == m_pStatsString_Sealswhacked ||
		NULL == m_pStatsString_Moneyearned)
	{
		return LT_ERROR;
	}

    char sStatText[1024];
    char sStatText1[1024];
    char sStatText2[1024];
    char sStatText3[1024];

    // Set the NULL char so that strncat will work properly. (cheaper than ZeroMemory or memset)
    sStatText[0] = '\0';
    sStatText1[0] = '\0';
    sStatText2[0] = '\0';
    sStatText3[0] = '\0';

    // This is not exactly an optimal solution, but is sufficiant for our purposes.
    sprintf(sStatText, "(Name)                -               (Seals Whacked)               -               (Money)\n\n");

	for (int i = 0; i < m_iNumPlayers; i++)
    {
        char sBuf[256];
        char sBuf2[32];
        char sBuf3[64];

        sprintf(sBuf, "%s\n", m_pScores[i].sPlayerName);
        strncat(sStatText1, sBuf, 128);

        sprintf(sBuf2, "%d\n", m_pScores[i].iScore);
        strncat(sStatText2, sBuf2, 16);

        sprintf(sBuf3, "$%.2f\n", m_pScores[i].fMoney);
        strncat(sStatText3, sBuf3, 32);     
    }



    uint32 nWidth, nHeight, nFontWidth, nFontHeight;

    // Set up the title
    m_pStatsString_Title->SetText(sStatText);
    nFontWidth = static_cast<uint32>(m_pStatsString_Title->GetWidth());
    nFontHeight = static_cast<uint32>(m_pStatsString_Title->GetHeight());

    g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
    m_pStatsString_Title->SetPosition(
        static_cast<float>(((nWidth/2) - (nFontWidth/2)) - (254 - nFontWidth/2) ),
        static_cast<float>((nHeight/2) - (nFontHeight/2)) - (128 - nFontHeight/2) );

    // Set up the Player's name
    m_pStatsString_Playername->SetText(sStatText1);
    nFontWidth = static_cast<uint32>(m_pStatsString_Playername->GetWidth());
    nFontHeight = static_cast<uint32>(m_pStatsString_Playername->GetHeight());

    g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
    m_pStatsString_Playername->SetPosition(
        static_cast<float>(((nWidth/2) - (nFontWidth/2)) - (254 - nFontWidth/2) ),
        static_cast<float>(((nHeight/2) - (nFontHeight/2)) - (96 - nFontHeight/2)) );

    // Set up the Seals whacked
    m_pStatsString_Sealswhacked->SetText(sStatText2);
    nFontWidth = static_cast<uint32>(m_pStatsString_Sealswhacked->GetWidth());
    nFontHeight = static_cast<uint32>(m_pStatsString_Sealswhacked->GetHeight());

    g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
    m_pStatsString_Sealswhacked->SetPosition(
        static_cast<float>((nWidth/2) - (nFontWidth/2)),
        static_cast<float>( ((nHeight/2) - (nFontHeight/2)) - (96 - nFontHeight/2) )  );

    // Set up the Money
    m_pStatsString_Moneyearned->SetText(sStatText3);
    nFontWidth = static_cast<uint32>(m_pStatsString_Moneyearned->GetWidth());
    nFontHeight = static_cast<uint32>(m_pStatsString_Moneyearned->GetHeight());

    g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
    m_pStatsString_Moneyearned->SetPosition(
        static_cast<float>(((nWidth/2) - (nFontWidth/2)) + (254 - nFontWidth/2) ),
        static_cast<float>(((nHeight/2) - (nFontHeight/2)) - (96 - nFontHeight/2)) );

    return LT_OK;
}
