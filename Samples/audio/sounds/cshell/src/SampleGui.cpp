// ----------------------------------------------------------------------- //
//
// MODULE  : SampleGui.cpp
//
// PURPOSE : Gui menu for sound demo.
//
// CREATED : 2002/10/05
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "SampleGui.h"
#include "SoundSampleMgr.h"
#include "ClientUtil.h"
#include "clientinterfaces.h"

#include <iclientshell.h>



//-------------------------------------------------------------------------------------------------
// SampleGui
//-------------------------------------------------------------------------------------------------


//----------------------------------------------------------------------------
//  Name:       SampleGui
//
//  Purpose:	constructor
//
//----------------------------------------------------------------------------
SampleGui::SampleGui()
	: m_pFont(NULL),
	  m_hFontTexture(NULL),
	  m_pMenuText(NULL),
	  m_pCursorText(NULL),
	  m_pDetailText(NULL),
	  m_CursorPos(0)
{
}



//----------------------------------------------------------------------------
//  Name:       ~SampleGui
//
//  Purpose:	destructor
//
//----------------------------------------------------------------------------
SampleGui::~SampleGui()
{
	Term();
}



//----------------------------------------------------------------------------
//  Name:       Init
//
//  Purpose:	Initializes the menu
//
//----------------------------------------------------------------------------
LTRESULT SampleGui::Init()
{
	if (g_pLTCFontManager == NULL ||
		g_pLTCTexInterface == NULL)
	{
		return LT_ERROR;
	}

    uint8 ptSize = 18;
    m_pFont = g_pLTCFontManager->CreateFont("fonts/SQR721B.TTF", "Square721 BT", ptSize, 33, 255);

	if (m_pFont)
	{
		uint32 w,h;
		g_pLTCTexInterface->GetTextureDims(m_pFont->GetTexture(),w,h);
		g_pLTClient->CPrint("Created font <%s> using a %dX%d texture.","fonts/SQR721B.TTF",w,h);

        m_pFont->SetDefCharWidth((ptSize/4));
        m_pFont->SetDefColor(0xFF00FF00);
	}
    else
    {
        g_pLTClient->CPrint("Failed to create font: %s", "fonts/SQR721B.TTF");
    }

	// This font can scale: setup the screen width and height.
	const uint32 CharWidth 		= 10;
	const uint32 CharHeight 	= 15;
	m_pFont->SetDefCharWidth(CharWidth);
	m_pFont->SetDefCharHeight(CharHeight);

	// Create the menu poly string. This will contain the headings for each sound sample detail as
	// well as the navigation buttons.
	m_pMenuText = g_pLTCFontManager->CreateFormattedPolyString(m_pFont);
	if (m_pMenuText == NULL)
	{
        g_pLTClient->CPrint("Failed to create menu poly string");
		return LT_ERROR;
	}

	// Create the cursor poly string. This will contain a cursor indicating the field that it currently selected
	// in the menu.
	m_pCursorText = g_pLTCFontManager->CreateFormattedPolyString(m_pFont);
	if (m_pCursorText == NULL)
	{
        g_pLTClient->CPrint("Failed to create cursor poly string");
		return LT_ERROR;
	}

	// Create the details poly string. This will contain the sound sample details.
	m_pDetailText = g_pLTCFontManager->CreateFormattedPolyString(m_pFont);
	if (m_pDetailText == NULL)
	{
        g_pLTClient->CPrint("Failed to create detail poly string");
		return LT_ERROR;
	}

	// Set the colors for the poly strings.
	m_pMenuText->SetColor(0xFF00FF00);
	m_pCursorText->SetColor(0xFFFFFFFF);
	m_pDetailText->SetColor(0xFF00FFFF);

	return LT_OK;
}



//----------------------------------------------------------------------------
//  Name:       Term
//
//  Purpose:    Shut down
//
//----------------------------------------------------------------------------
LTRESULT SampleGui::Term()
{
	// font
	if (m_pFont != NULL)
	{
		g_pLTCFontManager->DestroyFont(m_pFont);
		m_pFont = NULL;
	}

	// menu poly string
	if (m_pMenuText != NULL)
	{
		g_pLTCFontManager->DestroyPolyString(m_pMenuText);
		m_pMenuText = NULL;
	}

	// cursor poly string
	if (m_pCursorText != NULL)
	{
		g_pLTCFontManager->DestroyPolyString(m_pCursorText);
		m_pCursorText = NULL;
	}

	// detail poly string
	if (m_pDetailText != NULL)
	{
		g_pLTCFontManager->DestroyPolyString(m_pDetailText);
		m_pDetailText = NULL;
	}

	return LT_OK;
}



//----------------------------------------------------------------------------
//  Name:       Update
//
//  Purpose:	Updates the text with the data from the given sample.
//
//----------------------------------------------------------------------------
void SampleGui::Update(SoundSample *pSample)
{
	UpdateMenuText(pSample);
	UpdateDetailText(pSample);
	UpdateCursorText();
}



//----------------------------------------------------------------------------
//  Name:       UpdateDetailText
//
//  Purpose:	Sets the text showing the parameters for the currently
//				selected sound sample.
//
//----------------------------------------------------------------------------
void SampleGui::UpdateDetailText(SoundSample *pSample)
{
	if (m_pDetailText != NULL &&
	    pSample != NULL)
	{
		// Set the sound type.
		char SoundType[50];
		if (pSample->IsLocal())
		{
			strcpy(SoundType, "local");
		}
		else if (pSample->IsAmbient())
		{
			strcpy(SoundType, "ambient");
		}
		else if (pSample->Is3D())
		{
			strcpy(SoundType, "3d");
		}

		// Set the loop type.
		char Loop[4];
		strcpy(Loop, (pSample->IsLooped() ? "yes" : "no"));

		// Set the text.
		char buf[256];
		sprintf(buf, "%s\n"
					 "%s\n"
					 "%u\n"
					 "%.2f\n"
					 "%.2f\n"
					 "%u\n"
					 "%.2f\n"
					 "(%.2f, %.2f, %.2f)\n"
					 "%s",
					 pSample->GetName(),
					 SoundType,
					 pSample->GetPriority(),
					 pSample->GetOuterRadius(),
					 pSample->GetInnerRadius(),
					 pSample->GetVolume(),
					 pSample->GetPitchShift(),
					 pSample->GetPosition().x, pSample->GetPosition().y, pSample->GetPosition().z,
					 Loop);
		m_pDetailText->SetText(buf);

		m_pDetailText->SetPosition(210.0f, 50.0f);
	}
}



//----------------------------------------------------------------------------
//  Name:       UpdateMenuText
//
//  Purpose:	Sets the text for the menu fields and navigation buttons
//
//----------------------------------------------------------------------------
void SampleGui::UpdateMenuText(SoundSample *pSample)
{
	if (m_pMenuText != NULL)
	{
		char PlayType[50];
		strcpy(PlayType, (pSample->IsPlaying() ? "Stop" : "Play"));

		char buf[256];
		sprintf(buf, "Sample         :\n"
					 "Type           :\n"
					 "Priority       :\n"
					 "Outer Radius   :\n"
					 "Inner Radius   :\n"
					 "Volume         :\n"
					 "Pitch Shift    :\n"
					 "Position       :\n"
					 "Looped         :\n"
					 "-- %s --",
					 PlayType);

		m_pMenuText->SetText(buf);

		m_pMenuText->SetPosition(50.0f, 50.0f);
	}
}



//----------------------------------------------------------------------------
//  Name:       UpdateCursorText
//
//  Purpose:	Sets the text for the cursor indicating which menu field is
//				selected.
//
//----------------------------------------------------------------------------
void SampleGui::UpdateCursorText()
{
	if (m_pCursorText != NULL)
	{
		const unsigned BufSize = (MAX_GUICOMMAND_FILEDS*2) + 1;
		char buf[BufSize];

		char *pLine = buf;
		for (int i = 0; i < MAX_GUICOMMAND_FILEDS; ++i)
		{
			if (i == m_CursorPos)
			{
				*pLine = '>';
				++pLine;
				*pLine = '\n';
				++pLine;
			}
			else
			{
				*pLine = '.';
				++pLine;
				*pLine = '\n';
				++pLine;
			}
		}
		buf[BufSize - 1] = '\0';

		m_pCursorText->SetText(buf);

		m_pCursorText->SetPosition(40.0f, 50.0f);
	}
}



//----------------------------------------------------------------------------
//  Name:       IncCursor
//
//  Purpose:	Increments the cursor position by the given offset
//
//----------------------------------------------------------------------------
void SampleGui::IncCursor(int Offset)
{
	m_CursorPos += Offset;
	ClientUtil::WrapToRange(&m_CursorPos, 0, static_cast<int>(MAX_GUICOMMAND_FILEDS - 1));
}



//----------------------------------------------------------------------------
//  Name:       Render
//
//  Purpose:	Renders the text
//
//----------------------------------------------------------------------------
LTRESULT SampleGui::Render()
{
	// menu
	if (m_pMenuText != NULL)
    {
		m_pMenuText->Render();
    }

	// cursor
	if (m_pCursorText != NULL)
    {
		m_pCursorText->Render();
    }

	// details
	if (m_pDetailText != NULL)
    {
		m_pDetailText->Render();
    }

	return LT_OK;
}
