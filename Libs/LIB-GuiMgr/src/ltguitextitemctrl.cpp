// ----------------------------------------------------------------------- //
//
// MODULE  : LTGUITextItemCtrl.cp
//
// PURPOSE : Simple text control which may be used as a menu item.
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "ltguimgr.h"


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CLTGUITextCtrl::CLTGUITextCtrl()
{
	m_pFont           = LTNULL;
	m_nFontSize			= 0;
	m_nBaseFontSize	= 0;
	m_pCommandHandler = LTNULL;
	m_nWidth				= 0;
	m_nHeight			= 0;
	m_nFixedWidth		= 0;
	m_nIndent			= 0;
	m_bClip				= LTFALSE;
	m_pString			= LTNULL;

}

CLTGUITextCtrl::~CLTGUITextCtrl()
{
	Destroy();
}

// Create the control
LTBOOL CLTGUITextCtrl::Create (  	const wchar_t*				pText,
                                    uint32						nCommandID,
                                    uint32						nHelpID,
												CFontInfo*					pFont,
												uint8							nFontSize,
												CLTGUICommandHandler*	pCommandHandler,
                                    uint32						nParam1,
                                    uint32						nParam2)
{
	m_pCommandHandler	= pCommandHandler;


	if (!SetFont(pFont,nFontSize))
	{
        return LTFALSE;
	}

	// Add the string
	if (pText)
	{
		SetString(pText);
	}

	CLTGUICtrl::Create(nCommandID, nHelpID, nParam1,nParam2);

    return LTTRUE;
}


// Destroys the control
void CLTGUITextCtrl::Destroy ( )
{
	if (m_pString)
	{
		debug_delete(m_pString);
		m_pString = LTNULL;
	}

}


// Add more strings to the control.  These are cycled when OnLeft() and OnRight() are called
void CLTGUITextCtrl::SetString(const wchar_t *pString)
{
	if ( !pString || !m_pFont || !m_nFontSize)
		return;

	if (!m_pString)
	{
		m_pString = debug_new( CLTGUIPolyString );
		m_pString->SetFont(*m_pFont);
		m_pString->SetPos(m_pos);
		m_pString->SetText(pString);
	}
	else
	{
		m_pString->SetText(pString);
	}

	if (m_pString)
	{
		if (m_nFixedWidth && !m_bClip)
		{
			uint16 nWidth = (uint16)(m_fScale * (float)(m_nFixedWidth-m_nIndent));
			m_pString->SetWrapWidth(nWidth);
		}

		m_pString->SetFontHeight(m_nFontSize);
		CalculateSize();

	}
}


// Set the font
LTBOOL CLTGUITextCtrl::SetFont(CFontInfo *pFont, uint8 nFontSize)
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

	if (m_pString)
	{
		m_pString->SetFont(*m_pFont);
		m_pString->SetFontHeight(m_nFontSize);
		CalculateSize();
	}


    return LTTRUE;
}


void CLTGUITextCtrl::CalculateSize()
{
	if (!m_pFont || !m_pString)
	{
		if (m_nFixedWidth)
			m_nWidth=m_nFixedWidth;
		else
			m_nWidth=0;

		m_nHeight=0;
		return;
	}

	if (m_nFixedWidth)
	{
		m_nWidth = (uint16)(m_fScale * (float)m_nFixedWidth);
	}
	else
	{
		float fi = (float)m_nIndent * m_fScale;
		m_nWidth = (uint16)m_pString->GetWidth() + (uint16)fi;
	}

	m_nHeight = (uint16)m_pString->GetHeight();
}

// Render the control
void CLTGUITextCtrl::Render()
{
	// Sanity checks...
	if (!IsVisible() || !m_pString) return;

   uint32 argbColor=GetCurrentColor();
	m_pString->SetColor(argbColor);
	if (m_nFixedWidth && m_bClip)
	{
		LTRect2n rc;
		rc.m_vMin.x = m_pos.x;
		rc.m_vMin.y = m_pos.y;
		rc.m_vMax.x = m_pos.x + m_nWidth;
		rc.m_vMax.y = m_pos.y + m_nHeight;
		m_pString->RenderClipped(rc);
	}
	else
		m_pString->Render();
}


// Enter was pressed
LTBOOL CLTGUITextCtrl::OnEnter ( )
{
	// Send the command
	if ( m_pCommandHandler && m_pCommandHandler->SendCommand(m_nCommandID, m_nParam1, m_nParam2) )
		return LTTRUE;
    return LTFALSE;
}

void CLTGUITextCtrl::SetBasePos ( LTIntPt pos )
{ 
	CLTGUICtrl::SetBasePos(pos);
	if (m_pString)
	{
		float fi = (float)m_nIndent * m_fScale;
		m_pString->SetPos((float)m_pos.x+fi,(float)m_pos.y);
	}
}

void CLTGUITextCtrl::SetScale(float fScale)
{
	CLTGUICtrl::SetScale(fScale);
	m_nFontSize = (uint8)(m_fScale * (float)m_nBaseFontSize);
	uint16 nWidth = 0;
	if (m_nFixedWidth && !m_bClip)
	{
		nWidth = (uint16)(m_fScale * (float)(m_nFixedWidth-m_nIndent));
	}
	if (m_pString)
	{
		float fi = (float)m_nIndent * m_fScale;
		m_pString->SetPos((float)m_pos.x+fi,(float)m_pos.y);
		m_pString->SetFontHeight(m_nFontSize);
		m_pString->SetWrapWidth(nWidth);
	}
	CalculateSize();

}


void CLTGUITextCtrl::SetFixedWidth(uint16 nWidth, LTBOOL bClip)
{
	uint16 nMinWidth = m_nBaseFontSize + m_nIndent + 1;
	ASSERT(nWidth == 0 || nWidth >= nMinWidth);
	if (nWidth > 0 && nWidth < nMinWidth)
		nWidth = nMinWidth;

	m_nFixedWidth = nWidth;
	m_bClip = bClip;

	uint16 nTrueWidth = 0;
	if (m_nFixedWidth && !m_bClip)
	{
		nTrueWidth = (uint16)(m_fScale * (float)(m_nFixedWidth-m_nIndent));
	}
	if (m_pString)
	{
		m_pString->SetWrapWidth(nTrueWidth);
	}
	CalculateSize();

}

void CLTGUITextCtrl::SetIndent(uint16 nIndent)
{
	if (m_nFixedWidth)
	{
		ASSERT(m_nFixedWidth > nIndent);
		if(m_nFixedWidth <= nIndent)
			return;
	}

	m_nIndent = nIndent;

	if (m_nFixedWidth && !m_bClip)
	{
		uint16 nTrueWidth = (uint16)(m_fScale * (float)(m_nFixedWidth-m_nIndent));
		if (m_pString)
		{
			m_pString->SetWrapWidth(nTrueWidth);
		}

	}

	if (m_pString)
	{
		float fi = (float)m_nIndent * m_fScale;
		m_pString->SetPos((float)m_pos.x+fi,(float)m_pos.y);
	}

	CalculateSize();

}

LTBOOL CLTGUITextCtrl::IsOnMe(int x, int y)
{
	if (!m_bVisible) return LTFALSE;

	return (x >= m_pos.x && y >= m_pos.y &&
			x <= (m_pos.x + GetWidth()) &&
			y <= (m_pos.y + GetHeight()) );
}

