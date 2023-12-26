//------------------------------------------------------------------------------//
//
// MODULE   : GuiMgr.h
//
// PURPOSE  : Manages the text displays
//
// CREATED  : 09/18/2003
//
// (c) 2003 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#include "GuiMgr.h"

//---------------------------------------------------------------------------
//  Name:         GuiMgr
//
//  Purpose:	  constructor
//
//  Returns:      n/a
//---------------------------------------------------------------------------
GuiMgr::GuiMgr()
{
}



//---------------------------------------------------------------------------
//  Name:         ~GuiMgr
//
//  Purpose:	  destructor
//
//  Returns:      n/a
//---------------------------------------------------------------------------
GuiMgr::~GuiMgr()
{
}



//---------------------------------------------------------------------------
//  Name:         Init
//
//  Purpose:	  Initialize
//
//  Returns:      LTRESULT
//---------------------------------------------------------------------------
LTRESULT GuiMgr::Init()
{
	LTRESULT result = LT_OK;

	// user interface gui
	m_ControlsGui.Init("fonts/nolf.ttf", "Nolf", 24);
	m_ControlsGui.SetPos(16, 384);

	m_ControlsGui.SetText("Controls:\n"
						  "B - Toggle Background Color\n"
						  "G - Toggle This UI\n"
						  );

	m_ControlsGui.SetTextColor(0xFFF0C040);

	return result;
}



//---------------------------------------------------------------------------
//  Name:         Term
//
//  Purpose:	  Terminate
//
//  Returns:      LTRESULT
//---------------------------------------------------------------------------
LTRESULT GuiMgr::Term()
{
	m_ControlsGui.Term();

	return LT_OK;
}




//---------------------------------------------------------------------------
//  Name:         Render
//
//  Purpose:
//
//  Returns:      LTRESULT
//---------------------------------------------------------------------------
LTRESULT GuiMgr::Render()
{

	// Draw the controls gui.
	if (m_ControlsGui.IsVisible())
	{
		m_ControlsGui.Render();
	}

	return LT_OK;
}

