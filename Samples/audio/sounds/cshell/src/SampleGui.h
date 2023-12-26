// ----------------------------------------------------------------------- //
//
// MODULE  : SampleGui.h
//
// PURPOSE : Gui menu for sound demo.
//
// CREATED : 2002/10/05
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __SAMPLEGUI_H__
#define __SAMPLEGUI_H__


#include <iltclient.h>
#include <ltbasedefs.h>
#include <iltfontmanager.h>

#include "commandids.h"



// forwards:
class SoundSample;



class SampleGui
{
public:

	SampleGui();
	~SampleGui();

	LTRESULT 		Init();
	LTRESULT 		Term();

	LTRESULT 		Render();

	void    		Update(SoundSample *pSample);

	EGuiCommand		GetCurrentCommand()	const				{ return static_cast<EGuiCommand>(m_CursorPos); }

	void			MoveCursorUp()							{ IncCursor(-1); }
	void			MoveCursorDown()						{ IncCursor(1); }

private:

	void			UpdateMenuText(SoundSample *pSample);
	void			UpdateDetailText(SoundSample *pSample);
	void			UpdateCursorText();
	void			IncCursor(int Offset);

private:

    CUIFont*					m_pFont;			// font
	HTEXTURE					m_hFontTexture;		// font bitmap texture
	CUIFormattedPolyString*		m_pMenuText;		// menu text and navigation buttons
	CUIFormattedPolyString*		m_pCursorText;		// selection cursor
	CUIFormattedPolyString*		m_pDetailText;		// details
	int							m_CursorPos;		// location of cursor
};



#endif	// __SAMPLEGUI_H__
