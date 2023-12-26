// ----------------------------------------------------------------------- //
//
// MODULE  : LTGUIMgr.h
//
// PURPOSE : Shared header for the library of LTGUI controls
//
// (c) 1997-2001 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //


#if !defined(_LTGUIMGR_H_)
#define _LTGUIMGR_H_


#include "lithtech.h"
#include "iltdrawprim.h"
//#include "iltfontmanager.h"

//Engine specific
#include "DebugNew.h"

#pragma warning( disable : 4786 )
#include <vector>

#include "ltguipolystring.h"

typedef std::vector<CLTGUIPolyString*> PStringArray;
typedef std::vector<CLTGUIPolyString*> FPStringArray;

//base classes
#include "ltguicommandhandler.h"
#include "ltguictrl.h"
typedef std::vector<CLTGUICtrl*> ControlArray;

//basic control classes
#include "ltguitextitemctrl.h"
#include "ltguibutton.h"
#include "ltguicyclectrl.h"
#include "ltguitoggle.h"
#include "ltguislider.h"
#include "ltguicolumnctrl.h"
#include "ltguiframe.h"
#include "ltguieditctrl.h"
#include "ltguilargetext.h"

//container control classes
#include "ltguiwindow.h"
#include "ltguilistctrl.h"


//these are defined in the module that links
extern ILTDrawPrim*		g_pLTCDrawPrim;
//extern ILTFontManager*	g_pLTCFontManager;
extern ILTTexInterface*	g_pLTCTexInterface;
//extern ILTTextureString*	g_pLTCTextureString;
extern ILTTextureString* g_pLTCTextureString;

//utility functions for handling UV coordinates
#include "ltquaduvutils.h"



#endif // _LTGUIMGR_H_
