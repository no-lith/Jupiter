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
#include "ModelMgr.h"



//---------------------------------------------------------------------------
//  Name:         GuiMgr
//
//  Purpose:	  constructor
//
//  Returns:      n/a
//---------------------------------------------------------------------------
GuiMgr::GuiMgr()
	: m_nNumFrames(0),
      m_fFrameRate(0.0f),
	  m_fFrameRateRefresh(0.25f)
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

	// model gui
	m_ModelGui.Init();
	m_ModelGui.Bottom();
	m_ModelGui.SetText("Models: 1");

	// Initialize the "Node hit" information display
	m_HitStatusGui.Init();
	m_HitStatusGui.ApplyPositionStyle(PS_LEFT | PS_TOP);
	m_HitStatusGui.SetText("No Hit");
	m_HitStatusGui.SetColor(0xFF55B4FF);

	// frame rate gui
    m_FrameRateGui.Init();
    m_FrameRateGui.Bottom(50);
    m_FrameRateGui.SetText("FPS: 0");

	// renderer stats gui
    m_RendererStatsGui.Init();
	m_RendererStatsGui.ApplyPositionStyle(PS_RIGHT | PS_BOTTOM);
    m_RendererStatsGui.SetText("Model Tris: 0");

	// user interface gui
	m_ControlsGui.Init();
	m_ControlsGui.ApplyPositionStyle(PS_LEFT | PS_CENTER_ON_Y);

	m_ControlsGui.SetText("Controls:\n"
						  "W - Move Forward\n"
						  "A - Move Left\n"
						  "S - Move Back\n"
						  "D - Move Right\n"
						  "1 - Add Row\n"
						  "2 - Remove Row\n"
						  "3 - Add Column\n"
						  "4 - Remove Column\n"
						  "5 - Remove All\n"
						  "6 - Previous Model\n"
						  "7 - Next Model\n"
						  "B - Toggle Background Color\n"
						  "C - Toggle This UI\n"
						  "T - Toggle OBB Draw\n"
						  );

	m_ControlsGui.SetColor(0xFFF0C040);

	return result;
}



//---------------------------------------------------------------------------
//  Name:		  Term
//
//  Purpose:	  Terminate
//
//  Returns:      LTRESULT
//---------------------------------------------------------------------------
LTRESULT GuiMgr::Term()
{
	m_ModelGui.Term();
    m_FrameRateGui.Term();
    m_RendererStatsGui.Term();
	m_ControlsGui.Term();
	m_HitStatusGui.Term();

	return LT_OK;
}



//---------------------------------------------------------------------------
//  Name:         UpdateModelGui
//
//  Purpose:	  Update the number of models
//
//  Returns:      void
//---------------------------------------------------------------------------
void GuiMgr::UpdateModelGui()
{
	char buf[256];
	sprintf(buf, "Models: %u", g_ModelMgr.GetNumModels());
    m_ModelGui.SetText(buf);
}



//---------------------------------------------------------------------------
//  Name:         UpdateHitGui
//
//  Purpose:      Update the hit message.
//
//  Returns:      void
//---------------------------------------------------------------------------
void GuiMgr::UpdateHitGui(const char *pText)
{
	if (NULL != pText)
	{
		m_HitStatusGui.SetText(pText);
	}
}



//---------------------------------------------------------------------------
//  Name:         Render
//
//  Purpose:
//
//  Returns:      LTRESULT
//---------------------------------------------------------------------------
LTRESULT GuiMgr::Render(float fFrameTime)
{
	// Draw the model gui.
	m_ModelGui.Render();

	// Draw the hit status gui.
	m_HitStatusGui.Render();

	// Draw the controls gui.
	if (m_ControlsGui.IsVisible())
	{
		m_ControlsGui.Render();
	}

    // Calc and render our frame rate
    char buf[256];
    if (m_fFrameRateRefresh > 0.0f)
    {
        ++m_nNumFrames;
        m_fFrameRate += fFrameTime;
		m_fFrameRateRefresh -= fFrameTime;
    }
    else
    {
		m_fFrameRate /= (float)m_nNumFrames;
        sprintf(buf, "FPS: %.3f", (1.0f/(m_fFrameRate)));
        m_FrameRateGui.SetText(buf);

        // Reset frame rate counters
        m_fFrameRateRefresh = 0.25f;

        // Since we are using the last frame rate average
        // for the first element of the next average
        // Set the num frames to 1.
        m_nNumFrames = 1;
    }

	// Draw the frame rate gui.
	m_FrameRateGui.Render();

    // Draw the renderer stats.
    memset(buf, 0, 255);
    if (g_pLTClient->GetRendererStats(m_RendererStats) == LT_OK)
    {
        sprintf(buf, "Model Tris: %d\n World Tris: %d", m_RendererStats.ModelTriangles, m_RendererStats.WorldTriangles);
        m_RendererStatsGui.SetText(buf);
        m_RendererStatsGui.Render();
    }

	return LT_OK;
}

