// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenMgr.cpp
//
// PURPOSE : Interface/UI screen manager
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
// ----------------------------------------------------------------------- //


//#include "stdafx.h"
#include "SampleScreenMgr.h"

//screens
#include "BaseScreen.h"

// Main interface screen
#include "ScreenMain.h"

// under main screen
#include "ScreenOne.h"
#include "ScreenTwo.h"

//under screen main
#include "ScreenThree.h"


#include "ltclientshell.h"

// List of human readable screens 
static char s_aScreenName[SCREEN_ID_UNASSIGNED+1][32] =
{
	#define INCLUDE_AS_STRING
	#include "ScreenEnum.h"
	#undef INCLUDE_AS_STRING
};


//---------------------------------------------------------------------------
//	 Name:		CSampleScreenMgr::CSampleScreenMgr()
//
//	 Purpose:	Constructor
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
CSampleScreenMgr::CSampleScreenMgr()
{
	m_eCurrentScreenID = SCREEN_ID_NONE;
	m_eLastScreenID = SCREEN_ID_NONE;
}


//---------------------------------------------------------------------------
//	 Name:		CSampleScreenMgr::~CSampleScreenMgr()
//
//	 Purpose:	destructor
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
CSampleScreenMgr::~CSampleScreenMgr()
{

}


//---------------------------------------------------------------------------
//	 Name:		CSampleScreenMgr::Init()
//
//	 Purpose:	Initialization 
//
//	 Returns:	LTBOOL - success or failure 
//
//---------------------------------------------------------------------------
LTBOOL CSampleScreenMgr::Init()
{
	// build screen array
	m_screenArray.reserve(SCREEN_ID_UNASSIGNED);

	for (int nID = SCREEN_ID_MAIN; nID < SCREEN_ID_UNASSIGNED; ++nID)
	{
		AddScreen((eScreenID)nID);
	}

    return CScreenMgr::Init();
}


//---------------------------------------------------------------------------
//	 Name:		CSampleScreenMgr::SwitchToScreen()
//
//	 Purpose:	Switch to a given screen 
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void CSampleScreenMgr::SwitchToScreen(CBaseScreen *pNewScreen)
{

	CScreenMgr::SwitchToScreen(pNewScreen);

	// Do any special case work for each screen
	if (m_eCurrentScreenID == SCREEN_ID_MAIN)
	{
		m_nHistoryLen = 0;
		m_eScreenHistory[0] = SCREEN_ID_NONE;
	}

}



//---------------------------------------------------------------------------
//	 Name:		CSampleScreenMgr::AddScreen()
//
//	 Purpose:	Create a screen given an Id
//
//					This is one place you need would change to add a new screen
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void CSampleScreenMgr::AddScreen(eScreenID screenID)
{
	CBaseScreen* pScreen = LTNULL;

	switch (screenID)
	{
	case SCREEN_ID_MAIN:
		pScreen = debug_new(CScreenMain);
		break;
	case SCREEN_ID_ONE:
		pScreen = debug_new(CScreenOne);
		break;
	case SCREEN_ID_TWO:
		pScreen = debug_new(CScreenTwo);
		break;
	case SCREEN_ID_THREE:
		pScreen = debug_new(CScreenThree);
		break;
	}

	if (pScreen)
	{
		// Call the base screen class initializer
		pScreen->Init(screenID);

		// Call the generic screenmgr add proc
		CScreenMgr::AddScreen(pScreen);
	}

}


//---------------------------------------------------------------------------
//	 Name:		CSampleScreenMgr::GetScreenName()
//
//	 Purpose:	Get human readable screen name given the id
//
//	 Returns:	const char * - screen name
//
//---------------------------------------------------------------------------
const char* CSampleScreenMgr::GetScreenName(eScreenID id)
{
	return s_aScreenName[id];
}

//---------------------------------------------------------------------------
//	 Name:		CSampleScreenMgr::GetScreenIDFromName()
//
//	 Purpose:	Get a screen id give the humand readable name
//
//	 Returns:	Screen ID on success or 999 for failure 
//
//---------------------------------------------------------------------------
uint16 CSampleScreenMgr::GetScreenIDFromName(char * pName)
{
	for (uint16 i=0; i < SCREEN_ID_UNASSIGNED; ++i)
	{
		if (!strcmpi(pName, s_aScreenName[i]))
			return (i);
	}

	return 999;
}
