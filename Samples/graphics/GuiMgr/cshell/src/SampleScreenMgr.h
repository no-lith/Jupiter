// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenMgr.h
//
// PURPOSE : Interface screen manager
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
// ----------------------------------------------------------------------- //


#if !defined(_SAMPLE_SCREEN_MGR_H_)
#define _SAMPLE_SCREEN_MGR_H_

#include "ScreenMgr.h"



class CSampleScreenMgr : public CScreenMgr
{

public:

	CSampleScreenMgr();
	virtual ~CSampleScreenMgr();

   virtual LTBOOL			Init();
	virtual const char *	GetScreenName(eScreenID id);
	virtual uint16			GetScreenIDFromName(char * pName);


protected:

	void				AddScreen(eScreenID screenID);
	virtual void	SwitchToScreen(CBaseScreen *pNewScreen);

};

#endif // _SAMPLE_SCREENMGR_H_