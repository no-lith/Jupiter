// ----------------------------------------------------------------------- //
//
// MODULE  : CStatsGui.h
//
// PURPOSE : CStatsGui - Definition and constants
//
// CREATED : 07/18/2003
//
// (c) 2003 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __CSTATSGUI_H__
#define __CSTATSGUI_H__

#include <iltclient.h>
#include <ltbasedefs.h>
#include "clientinterfaces.h"
#include "serverdefs.h"
#include <iltfontmanager.h>
#include "scoredefs.h"

class ILTMessage_Read;

class CStatsGui
{
public:

	// Construction / destruction
	CStatsGui();
	~CStatsGui();

	LTRESULT 		            Init();
	LTRESULT 		            Term();

	LTRESULT 	            	Render();

    LTRESULT                    HandleMessage(ILTMessage_Read* pMessage);
    LTRESULT                    SortStats();
    LTRESULT                    RecalcStatsString();

private:
    HTEXTURE                    m_hBackDrop;
	CUIFont*					m_pFont;
	CUIFormattedPolyString*	    m_pStatsString_Title;
	CUIFormattedPolyString*	    m_pStatsString_Playername;
	CUIFormattedPolyString*	    m_pStatsString_Sealswhacked;
	CUIFormattedPolyString*	    m_pStatsString_Moneyearned;
    SCORESTRUCT*                m_pScores;
    uint8                       m_iNumPlayers;
};



#endif	// __CGUI_H__
