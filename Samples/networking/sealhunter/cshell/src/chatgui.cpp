// ----------------------------------------------------------------------- //
//
// MODULE  : CChatGui.cpp
//
// PURPOSE : CChatGui - Implementation
//
// CREATED : 07/18/2003
//
// (c) 2003 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include <windows.h> // For keyboard opertations
#include <iltdrawprim.h>
#include <iltmessage.h>
#include <iltcommon.h>

#include "chatgui.h"
#include "clientinterfaces.h"
#include "msgids.h"

#define MAX_CHAT_LINE_TTL 10.0f

//------------------------------------------------------------------------------
//	CChatGui::CChatGui()
//
//------------------------------------------------------------------------------
CChatGui::CChatGui():
m_hBackDrop(NULL),
m_pFont(NULL),
m_pChatString(NULL),
m_pBufferString(NULL),
m_pCursorString(NULL),
m_bIsInputActive(false),
m_iCurChatLine(0)
{
    m_szBuffer[0] = '\0';

    // Set default values for chat lines
    for(int i = 0; i < MAX_CHAT_LINES; i++)
    {
        m_ChatLines[i].fTimeToLive = 0.0f;
        m_ChatLines[i].szText[0] = '\0';
        m_ChatLines[i].bActive = false;

        // Handle the last element differently
        if(i == MAX_CHAT_LINES-1)
        {
            m_ChatLines[i].iNext = 0;
        }
        else
        {
            m_ChatLines[i].iNext = i+1;
        }

    }
}


//------------------------------------------------------------------------------
//	CChatGui::~CChatGui()
//
//------------------------------------------------------------------------------
CChatGui::~CChatGui()
{

}


//------------------------------------------------------------------------------
//	CChatGui::Init()
//
//------------------------------------------------------------------------------
LTRESULT CChatGui::Init()
{

    if (NULL == m_hBackDrop)
	{
		LTRESULT result = g_pLTCTexInterface->CreateTextureFromName(m_hBackDrop, "tex/ui/chatdrop.dtx");

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

	if (LTNULL == m_pChatString)
	{
		m_pChatString = g_pLTCFontManager->CreateFormattedPolyString(m_pFont, "");
		if (LTNULL == m_pChatString)
		{
        	g_pLTClient->CPrint("Error creating m_pChatString!");
			return LT_ERROR;
		}
	}
    m_pChatString->SetColor(0xFFFFAA55);
    m_pChatString->SetText("");
    m_pChatString->SetPosition(5.0f, 5.0f);

	if (LTNULL == m_pBufferString)
	{
		m_pBufferString = g_pLTCFontManager->CreateFormattedPolyString(m_pFont, "");
		if (LTNULL == m_pBufferString)
		{
        	g_pLTClient->CPrint("Error creating m_pBufferString!");
			return LT_ERROR;
		}
	}
	m_pBufferString->SetText("");

	if (LTNULL == m_pCursorString)
	{
		m_pCursorString = g_pLTCFontManager->CreateFormattedPolyString(m_pFont, "");
		if (LTNULL == m_pCursorString)
		{
        	g_pLTClient->CPrint("Error creating m_pBufferString!");
			return LT_ERROR;
		}
	}
	m_pCursorString->SetText("|");

    /*
    uint32 nFontWidth = static_cast<uint32>(m_pStatsString_Playername->GetWidth());
    uint32 nFontHeight = static_cast<uint32>(m_pStatsString_Playername->GetHeight());
    uint32 nWidth, nHeight;
    g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
    m_pStatsString_Playername->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2)),
                               static_cast<float>((nHeight/2) - (nFontHeight/2)));
    */
    RecalcStrings();

    return LT_OK;
}


//------------------------------------------------------------------------------
//	CChatGui::Term()
//
//------------------------------------------------------------------------------
LTRESULT CChatGui::Term()
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

	if (LTNULL != m_pChatString)
	{
		g_pLTCFontManager->DestroyPolyString(m_pChatString);
	}

	if (LTNULL != m_pBufferString)
	{
		g_pLTCFontManager->DestroyPolyString(m_pBufferString);
	}

	if (LTNULL != m_pCursorString)
	{
		g_pLTCFontManager->DestroyPolyString(m_pCursorString);
	}


    return LT_OK;
}

//------------------------------------------------------------------------------
//	CChatGui::Update()
//
//------------------------------------------------------------------------------
LTRESULT CChatGui::Update()
{
    float fFrameTime = g_pLTClient->GetFrameTime();

    for(int i = 0; i < MAX_CHAT_LINES; i++)
    {
        if(m_ChatLines[i].fTimeToLive > 0.0f)
        {
            m_ChatLines[i].fTimeToLive -= fFrameTime;
        }
        else
        {
            if(m_ChatLines[i].bActive)
            {
                // Remove it.
                m_iCurChatLine = m_ChatLines[i].iNext;
                m_ChatLines[i].bActive = false;

                //recalc chat lines
                RecalcChatLines();
            }
        }
    }

    return LT_OK;
}


//------------------------------------------------------------------------------
//	CChatGui::Render()
//
//------------------------------------------------------------------------------
LTRESULT CChatGui::Render()
{
	if (NULL == m_pChatString ||
		NULL == m_pBufferString)
	{
		return LT_ERROR;
	}

    if (m_bIsInputActive)
    {
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

        // Draw the input buffer
        m_pBufferString->Render();

        // Draw the input cursor
        m_pCursorString->Render();
    }

    // Draw the current chat output now.
    m_pChatString->Render();

    return LT_OK;
}


//------------------------------------------------------------------------------
//	CChatGui::HandleInput(int key, int rep)
//
//------------------------------------------------------------------------------
LTRESULT CChatGui::HandleInputDown(int key, int rep)
{
	if (NULL == m_pBufferString)
	{
		return LT_ERROR;
	}

	if(m_bIsInputActive)
    {
        switch(key)
        {
        case VK_BACK: // Char: Backspace
            {
                //Delete the last char. (not the NULL char)
                int len = strlen(m_szBuffer);
                if(len > 0)
                {
					m_szBuffer[len - 1] = '\0';
					m_pBufferString->SetText(m_szBuffer);
					RecalcStrings();
                }
            }
            break;
        case VK_RETURN: // Char: Enter
            {
                // Validate it
                if(m_szBuffer[0] == '\0')
                {
                    m_bIsInputActive = false;
                    break;
                }

                if(strlen(m_szBuffer) > 255)
                {
                    m_bIsInputActive = false;
                    break;
                }

                // Send it

	            ILTMessage_Write *pMsg;
	            LTRESULT nResult = g_pLTCCommon->CreateMessage(pMsg);

	            if( LT_OK == nResult)
	            {
		            pMsg->IncRef();
		            pMsg->Writeuint8(MSG_CS_CHAT);
                    pMsg->WriteString(m_szBuffer);
		            g_pLTClient->SendToServer(pMsg->Read(), MESSAGE_GUARANTEED);
		            pMsg->DecRef();
                }

                // Clear the temp chat input buffer
                m_szBuffer[0] = '\0';
                m_pBufferString->SetText(m_szBuffer);
                RecalcStrings();
                m_bIsInputActive = false;
            }
            break;
        case VK_SHIFT: // Char: Shift
            {
                //Ignore
            }
            break;
        case VK_ESCAPE: // Char: Esc
            {
                m_bIsInputActive = false;
            }
            break;
        default:
            {
                float fWidth = m_pBufferString->GetWidth();

                // Is the text too large to fit in the chat box?
                if(fWidth > 480)
                {
                    break;
                }

                // Is the string too long for the buffer?
                if(strlen(m_szBuffer) > 254)
                {
                    break;
                }

                char buf[1];
                buf[0] = TranslateKey(key);//key;
                strncat(m_szBuffer, buf, 1);
                m_pBufferString->SetText(m_szBuffer);
                RecalcStrings();
            }

            break;
        }
    }

    return LT_OK;
}


//------------------------------------------------------------------------------
//	CChatGui::HandleInputUp(int key, int rep)
//
//------------------------------------------------------------------------------
LTRESULT CChatGui::HandleInputUp(int key)
{
    return LT_OK;
}


//------------------------------------------------------------------------------
//	CChatGui::HandleMessage(ILTMessage_Read* pMessage)
//
//------------------------------------------------------------------------------
LTRESULT CChatGui::HandleMessage(ILTMessage_Read* pMessage)
{
    assert(pMessage);

    char szChatString[256];
    szChatString[255] = '\0';

    pMessage->ReadString(szChatString, 255);

    // Validate the string
    // TODO:

    // Save it
    //m_pChatString->SetText(szChatString);

    // Store the string
    g_pLTClient->CPrint("%s", szChatString);

    m_ChatLines[m_iCurChatLine].fTimeToLive = MAX_CHAT_LINE_TTL;
    sprintf(m_ChatLines[m_iCurChatLine].szText, "%s\n", szChatString);
    m_ChatLines[m_iCurChatLine].bActive = true;

    m_iCurChatLine = m_ChatLines[m_iCurChatLine].iNext;

    RecalcChatLines();

    return LT_OK;
}


//------------------------------------------------------------------------------
//	LTRESULT CChatGui::RecalcStatsString()
//
//------------------------------------------------------------------------------
LTRESULT CChatGui::RecalcStrings()
{
	if (NULL == m_pBufferString ||
		NULL == m_pCursorString)
	{
		return LT_ERROR;
	}

	uint32 nWidth, nHeight, nFontWidth, nFontHeight;

    g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);
    nFontWidth = static_cast<uint32>(m_pBufferString->GetWidth());
    nFontHeight = static_cast<uint32>(m_pBufferString->GetHeight());

    m_pBufferString->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2)),
                                 static_cast<float>((nHeight/2) - (nFontHeight/2)));

    m_pCursorString->SetPosition(static_cast<float>((nWidth/2) - (nFontWidth/2)) + m_pBufferString->GetWidth(),
                                 static_cast<float>((nHeight/2) - (nFontHeight/2)));

    return LT_OK;
}


//------------------------------------------------------------------------------
//	LTRESULT CChatGui::RecalcChatLines()
//
//------------------------------------------------------------------------------
LTRESULT CChatGui::RecalcChatLines()
{
	if (NULL == m_pChatString)
	{
		return LT_ERROR;
	}

	char szChatBuffer[MAX_CHAT_LINES * 512];
    szChatBuffer[0] = '\0';

    uint8 iCurElement = m_iCurChatLine; // m_ChatLines[m_iCurChatLine].iNext;
    for(int i = 0; i < MAX_CHAT_LINES; i++)
    {
        if(m_ChatLines[iCurElement].bActive)//m_ChatLines[iCurElement].fTimeToLive > 0.0f)
        {
            //Add it
            strncat(szChatBuffer, m_ChatLines[iCurElement].szText, 511);
        }

        iCurElement = m_ChatLines[iCurElement].iNext;
    }

    m_pChatString->SetText(szChatBuffer);

    return LT_OK;
}


//------------------------------------------------------------------------------
//	char CChatGui::TranslateKey(uint32 key) const
//
//------------------------------------------------------------------------------
char CChatGui::TranslateKey(uint32 key) const
{
	char aResult[2];
	uint8 aKeyState[256];
	if ( !GetKeyboardState( aKeyState ) )
		return 0;

	switch ( ToAscii( key, 0, aKeyState, (LPWORD)aResult, 0 ) )
	{
		// No translation available
		case 0 :
			return 0;
		// 1 character translated
		case 1 :
			return aResult[0];
		// 2 characters required for translation
		case 2 :
			return aResult[1];
		// Documentation says this should never happen...
		default :
			return 0;
	}
}
