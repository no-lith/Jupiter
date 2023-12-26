// LTGUIEditCtrl.cpp: implementation of the CLTGUIEditCtrl class.
//
//////////////////////////////////////////////////////////////////////

#include "ltguimgr.h"
#include "ltguieditctrl.h"
#include "vkdefs.h"
#include "ctype.h"
#include <locale.h>


const uint16 CLTGUIEditCtrl::kMaxLength = 64;

namespace
{
	wchar_t szString[CLTGUIEditCtrl::kMaxLength+1];
	
// Probably need to modify this for locales ?? 
	LTBOOL IsFileFriendly(wchar_t c, bool isFirst)
	{
		static const wchar_t * pszOKChars = L" _";
		if (isalnum(c)) return LTTRUE;
		if (wcschr(pszOKChars,c) && !isFirst) return LTTRUE;
		return LTFALSE;
	}
}


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLTGUIEditCtrl::CLTGUIEditCtrl()
{
	m_pLTClient     = LTNULL;
	m_pFont = LTNULL;
	m_nFontSize = 0;
	m_nBaseFontSize = 0;

	m_pText = LTNULL;

	m_nMaxLength = 0;
	m_nCaretPos = 0;
	m_pStringValue = LTNULL;

	m_bCaretEnabled = LTFALSE;
	m_fCaretTime = 0.0f;

	m_nWidth			= 0;
	m_nHeight			= 0;
	m_nFixedWidth		= 0;
	m_bUseFrame			= LTFALSE;

	m_eInputMode		= kInputAll;

	memset(&m_Caret,0,sizeof(m_Caret));
	memset(m_Frame,0,sizeof(m_Frame));

	szString[0] = 0;
}

CLTGUIEditCtrl::~CLTGUIEditCtrl()
{
	Destroy();
}

// Create the control
// pLTClient	   - Pointer to the client interface.
// nCommandID	   - The command ID which is sent when "enter" is pressed.
// pFont		   - The font to use for rendering the strings.
// nMaxLength	   - The size of the buffer to store the string in.
// pMessageCtrl    - control which receives messages when the "enter" key is pressed.
// pStringValue	   - pointer to the string to be modified in UpdateData.
LTBOOL CLTGUIEditCtrl::Create (  ILTClient *pLTClient, uint32 nCommandID, CFontInfo *pFont, uint8 nFontSize, 
								uint16 nMaxLength, CLTGUICommandHandler *pCommandHandler, wchar_t * pStringValue, 
								uint32 nParam1, uint32 nParam2 )
{
	if (!pLTClient || !pFont || nMaxLength < 0)
        return LTFALSE;

	m_pLTClient			= pLTClient;
	m_pCommandHandler	= pCommandHandler;
	m_pStringValue		= pStringValue;

	SetMaxLength(nMaxLength);

	if (!SetFont(pFont,nFontSize))
	{
        return LTFALSE;
	}


	// Add the string
	if (pStringValue)
	{
		SetText(pStringValue);
	}

	g_pLTCDrawPrim->SetRGBA(&m_Caret,0xFFFFFFFF);
	g_pLTCDrawPrim->SetRGBA(&m_Frame[0],0x7F000000);
	g_pLTCDrawPrim->SetRGBA(&m_Frame[1],0x7FFFFFFF);
	g_pLTCDrawPrim->SetRGBA(&m_Frame[2],0x7FFFFFFF);
	g_pLTCDrawPrim->SetRGBA(&m_Frame[3],0x7F000000);


	CLTGUICtrl::Create(nCommandID,nParam1,nParam2);
    return LTTRUE;
}

// Destroys the control
void CLTGUIEditCtrl::Destroy ( )
{
	if (m_pText)
	{
		debug_delete(m_pText);
		m_pText = LTNULL;
	}

}

// Update data
void CLTGUIEditCtrl::UpdateData(LTBOOL bSaveAndValidate)
{
	if (!m_pStringValue)
		return;

	// Save the string out
	if (bSaveAndValidate)
	{
		if (m_pText)
			wcscpy(m_pStringValue, m_pText->GetText() );
	}
	else
		SetText(m_pStringValue);
}

// Render the control
void CLTGUIEditCtrl::Render ()
{
	if ( !m_pText || !IsVisible())
		return;

    uint32 color = GetCurrentColor();

	// Render the text

	if (m_pText)
	{
		m_pText->SetColor(color);
		m_pText->Render();

		// draw the caret
		g_pLTCDrawPrim->SetTransformType(DRAWPRIM_TRANSFORM_SCREEN);
		g_pLTCDrawPrim->SetZBufferMode(DRAWPRIM_NOZ); 
		g_pLTCDrawPrim->SetClipMode(DRAWPRIM_NOCLIP);
		g_pLTCDrawPrim->SetFillMode(DRAWPRIM_FILL);
		g_pLTCDrawPrim->SetColorOp(DRAWPRIM_NOCOLOROP);
		g_pLTCDrawPrim->SetAlphaTestMode(DRAWPRIM_NOALPHATEST);
		g_pLTCDrawPrim->SetAlphaBlendMode(DRAWPRIM_NOBLEND);

		if (IsCaretOn() && IsEnabled())
		{
			
			float x = (float)m_pos.x;
			float y = (float)m_pos.y;
			float w = 2.0f;
			float h = m_nFontSize; //m_pText->GetHeight();

			if (m_nCaretPos > 0 && m_nCaretPos <= m_pText->GetStringLength())
			{
				LTRect2n rRect;
				m_pText->GetCharRect( (m_nCaretPos-1), rRect );
				x += (float) rRect.Right();
			}
			g_pLTCDrawPrim->SetXYWH(&m_Caret,x,y,w,h);



			g_pLTCDrawPrim->DrawPrim(&m_Caret);

		}

		// draw our frame
		if (m_bUseFrame)
			g_pLTCDrawPrim->DrawPrim(m_Frame,4);

	}
}

// Check for caret use
LTBOOL CLTGUIEditCtrl::IsCaretOn()
{
    if (!m_bCaretEnabled) return(LTFALSE);

	static float fCaretTimer = 0.0f;
	static float fLastTime    = 0.0f;
    static LTBOOL bCaretOn    = LTTRUE;

	float fCurTime   = m_pLTClient->GetTime();
	float fDeltaTime = fCurTime - fLastTime;

	if (fDeltaTime >= fCaretTimer)
	{
		fCaretTimer  = m_fCaretTime;
		bCaretOn    ^= 1;
	}
	else
		fCaretTimer -= fDeltaTime;

	fLastTime = fCurTime;

	return(bCaretOn);
}

// Enable the caret
void CLTGUIEditCtrl::EnableCaret(LTBOOL bUseCaret, float fToggleTime, uint32 argbColor)
{
	m_bCaretEnabled = bUseCaret;
	m_fCaretTime    = fToggleTime;

	g_pLTCDrawPrim->SetRGBA(&m_Caret,argbColor);

}


// Set the font
LTBOOL CLTGUIEditCtrl::SetFont(CFontInfo *pFont, uint8 nFontSize)
{
	if (!pFont && !nFontSize)
	{
        return LTFALSE;
	}

	if (pFont)
		m_pFont = pFont;
	if (nFontSize)
		m_nBaseFontSize = nFontSize;

	m_nFontSize = (uint8)(m_fScale * (float)m_nBaseFontSize);

	if (m_pText)
	{
		m_pText->SetFont(*m_pFont);
		m_pText->SetFontHeight(m_nFontSize);
		CalculateSize();
	}
	return LTTRUE;
}

// Set the text
void CLTGUIEditCtrl::SetText(const wchar_t *pString)
{
	if ( !pString || !m_pLTClient || !m_pFont || !m_nFontSize)
		return;

	if (!m_pText)
	{
		m_pText = debug_new( CLTGUIPolyString );
		m_pText->SetFont(*m_pFont);
		m_pText->SetText(pString);
		m_pText->SetPos(m_pos);

	}
	else
	{
		m_pText->SetText(pString);
	}


	if (m_pText)
	{
		m_pText->SetFontHeight(m_nFontSize);
		CalculateSize();
	}

	m_nCaretPos = wcslen(pString);
}
const wchar_t * CLTGUIEditCtrl::GetText()
{
	if (m_pText)
		return m_pText->GetText();
	else 
		return LTNULL;
}

void CLTGUIEditCtrl::SetBasePos ( LTIntPt pos )
{ 
	CLTGUICtrl::SetBasePos(pos);
	if (m_pText)
	{
		m_pText->SetPos((float)m_pos.x,(float)m_pos.y);
	}
}

void CLTGUIEditCtrl::SetScale(float fScale)
{
	CLTGUICtrl::SetScale(fScale);
	m_nFontSize = (uint8)(m_fScale * (float)m_nBaseFontSize);
	if (m_pText)
	{
		m_pText->SetPos((float)m_pos.x,(float)m_pos.y);
		m_pText->SetFontHeight(m_nFontSize);
	}
	CalculateSize();

}


// Add a character to the end
void CLTGUIEditCtrl::AddCharacter(wchar_t c)
{
	if (!m_pText)
		return;

	// Check to see are at our buffer limit
	if (m_pText->GetStringLength() >= m_nMaxLength)
		return;

	// If the string is blank
	if ( m_pText->GetStringLength() )
	{
		LTStrCpy(szString, m_pText->GetText(), m_pText->GetStringLength()+1);
	}

	int nIndex = wcslen(szString);

	while (nIndex > m_nCaretPos)
	{
		szString[nIndex] = szString[nIndex-1];
		nIndex--;
	}

	szString[nIndex]	= c;
	m_nCaretPos++;
	m_pText->SetText(szString);

	if (m_nFixedWidth)
	{
		float testWidth = m_fScale * m_nFixedWidth;
		if (m_pText->GetWidth() > testWidth )
		{
			m_nCaretPos--;
			RemoveCharacter();
		}
	}

	CalculateSize();

}

// Remove a character from the end
void CLTGUIEditCtrl::RemoveCharacter()
{
	if (!m_pText)
		return;

	// Check to see have any chars
	if (m_pText->GetStringLength() < 1)
		return;

	LTStrCpy(szString, m_pText->GetText(), m_pText->GetStringLength()+1);

	int nEnd=wcslen(szString);
	if (nEnd > m_nCaretPos)
	{
		int nIndex = m_nCaretPos;

		while (nIndex < nEnd)
		{
			szString[nIndex] = szString[nIndex+1];
			nIndex++;
		}

		szString[nIndex]='\0';
		m_pText->SetText(szString);
	}

	CalculateSize();

}

// Handles a key press
LTBOOL CLTGUIEditCtrl::HandleKeyDown(int key, int rep)
{
	if (CLTGUICtrl::HandleKeyDown(key, rep))
        return LTTRUE;

	switch (key)
	{
	case VK_BACK:
		if (m_nCaretPos > 0)
		{
			m_nCaretPos--;
			RemoveCharacter();
		}
		break;
	case VK_DELETE:
		RemoveCharacter();
		break;
	case VK_HOME:
		m_nCaretPos = 0;
		break;
	case VK_END:
		if (m_pText)
			m_nCaretPos = m_pText->GetStringLength();
		break;
	case VK_LEFT:
		if (m_nCaretPos > 0) m_nCaretPos--;
		break;
	case VK_RIGHT:
		if (m_nCaretPos < m_pText->GetStringLength()) m_nCaretPos++;
		break;
	default:
		return LTFALSE;
	}

    return LTTRUE;
}

// Handles a key press
LTBOOL CLTGUIEditCtrl::HandleChar(wchar_t c)
{
    if (CLTGUICtrl::HandleChar(c)) return LTTRUE;
    if (c < ' ') return LTFALSE;

	switch	(m_eInputMode)
	{
	case kInputAlphaNumeric:
		if (!isalnum(c)) return LTFALSE;
		break;
	case kInputAlphaOnly:
		if (!isalpha(c)) return LTFALSE;
		break;
	case kInputNumberOnly:
		if (!isdigit(c)) return LTFALSE;
		break;
	case kInputFileFriendly:
		if (!IsFileFriendly(c,(m_nCaretPos == 0))) return LTFALSE;
		break;
	};


	AddCharacter(c);
    return LTTRUE;
}

// Handle the Enter key being pressed
LTBOOL CLTGUIEditCtrl::OnEnter ( )
{
	// Send the command
	if ( m_pCommandHandler )
	{
		if	(m_pCommandHandler->SendCommand(m_nCommandID, m_nParam1, m_nParam2))
            return LTTRUE;
	}
    return LTFALSE;
}


void CLTGUIEditCtrl::CalculateSize()
{

	if (!m_pFont || !m_pText)
	{
		m_nWidth=0;
		m_nHeight=0;
	}
	else
	{
		if (m_nFixedWidth)
		{
			m_nWidth = (uint16)((float)m_nFixedWidth * m_fScale);
		}
		else
			m_nWidth = (uint16)m_pText->GetWidth();
		m_nHeight = m_nFontSize;
	}


	if (m_bUseFrame)
	{
		float fx = (float)m_pos.x - 1.0f;
		float fy = (float)m_pos.y - 1.0f;
		float fw = (float)m_nWidth + 1.0f;
		float fh = 1.0f;
		g_pLTCDrawPrim->SetXYWH(&m_Frame[0],fx,fy,fw,fh);

		fx = (float)(m_pos.x + m_nWidth);
		fy = (float)m_pos.y - 1.0f;
		fw = 1.0f;
		fh = (float)GetHeight() + 1.0f;
		g_pLTCDrawPrim->SetXYWH(&m_Frame[1],fx,fy,fw,fh);

		fx = (float)m_pos.x;
		fy = (float)(m_pos.y + GetHeight());
		fw = (float)m_nWidth + 1.0f;
		fh = 1.0f;
		g_pLTCDrawPrim->SetXYWH(&m_Frame[2],fx,fy,fw,fh);

		fx = (float)m_pos.x - 1.0f;
		fy = (float)m_pos.y;
		fw = 1.0f;
		fh = (float)GetHeight() + 1.0f;
		g_pLTCDrawPrim->SetXYWH(&m_Frame[3],fx,fy,fw,fh);
	}

}


void CLTGUIEditCtrl::SetFixedWidth(uint16 nWidth, LTBOOL bUseFrame)
{
	uint16 nMinWidth = m_nBaseFontSize + 1;
	ASSERT(nWidth >= nMinWidth);
	if (nWidth < nMinWidth)
		nWidth = nMinWidth;
	m_nFixedWidth = nWidth;
	CalculateSize();

	if (m_nFixedWidth)
	{
		m_bUseFrame = bUseFrame;
		if (m_pText)
		{
			float testWidth = m_fScale * m_nFixedWidth;
			uint32 nOldCaret = m_nCaretPos;
			m_nCaretPos = m_pText->GetStringLength();
			while (m_pText->GetStringLength() && m_pText->GetWidth() > testWidth )
			{
				m_nCaretPos--;
				RemoveCharacter();				
			}
			if (nOldCaret > m_pText->GetStringLength())
				m_nCaretPos = m_pText->GetStringLength();
			else
				m_nCaretPos = nOldCaret;
		}
	}
	else
		m_bUseFrame = LTFALSE;
}

void CLTGUIEditCtrl::SetMaxLength(uint16 nMaxLength)
{
	if (nMaxLength > kMaxLength)
		nMaxLength = kMaxLength;
	m_nMaxLength = nMaxLength;

	if (m_pText && m_nMaxLength < m_pText->GetStringLength())
	{
		LTStrCpy(szString,m_pText->GetText(), m_pText->GetStringLength());
		szString[m_nMaxLength] = '\0';
		m_pText->SetText(szString);
	}
}


LTBOOL CLTGUIEditCtrl::OnLButtonUp(int x, int y)
{
	if (!IsOnMe(x,y)) return LTFALSE;

	uint16 nMax = m_pText->GetStringLength();

	uint16 nIndex = 0;

	while (nIndex < nMax)
	{
		LTRect2n rRect;

		m_pText->GetCharRect( nIndex, rRect );

		// we found where we want the cursor
		if ( x <= (m_pos.x + rRect.Right()))
			break;

		nIndex++;
	}

	m_nCaretPos = nIndex;

	return LTTRUE;
}

