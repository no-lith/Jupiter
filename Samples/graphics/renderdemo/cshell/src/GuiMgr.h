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

#ifndef __GUIMGR_H__
#define __GUIMGR_H__


#include <ltbasetypes.h>
#include <ltbasedefs.h>

#include "gui.h"



class GuiMgr
{
public:

	GuiMgr();
	~GuiMgr();

	// initialize
	LTRESULT				Init();

	// terminate
	LTRESULT				Term();

	// gui access
	CGui*					GetModelGui()					{ return &m_ModelGui; }
    CGui*					GetFrameRateGui()				{ return &m_FrameRateGui; }
    CGui*					GetRenderStatsGui() 			{ return &m_RendererStatsGui; }
	CGui*					GetControlsGui()				{ return &m_ControlsGui; }

	// update
	void					UpdateModelGui();
	void					ToggleControlsGui()				{ m_ControlsGui.SetVisible(!m_ControlsGui.IsVisible()); }

	// render
	LTRESULT				Render(float fFrameTime);

private:


private:

	// gui
	CGui					m_ModelGui;
    CGui                    m_FrameRateGui;
    CGui                    m_RendererStatsGui;
	CGui					m_ControlsGui;

	// render stats
	LTRendererStats         m_RendererStats;

    // frame rate
    unsigned          		m_nNumFrames;
    float           		m_fFrameRate;
	float					m_fFrameRateRefresh;
};


#endif // __GUIMGR_H__

