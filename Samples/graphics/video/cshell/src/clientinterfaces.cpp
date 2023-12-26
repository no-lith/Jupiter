// ----------------------------------------------------------------------- //
//
// MODULE  : clientinterfaces.cpp
//
// PURPOSE : Defines pointers to the client interfaces
//
// CREATED : 12/20/00
//
// (c) 2000 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

// The header for the clientinterfaces
#include "clientinterfaces.h"

// Engine includes
#include <iltclient.h>
#include <iclientshell.h>

#include <iltphysics.h>
#include <iltmodel.h>
#include <iltdrawprim.h>
#include <iltsoundmgr.h>
#include <iltcommon.h>
#include <iltfontmanager.h>
#include <iltwidgetmanager.h>
#include <iltvideomgr.h>

//-----------------------------------------------------------------------------

ILTCommon* g_pLTCCommon = NULL;
ILTPhysics* g_pLTCPhysics = NULL;
ILTModelClient* g_pLTCModel = NULL;
ILTDrawPrim* g_pLTCDrawPrim = NULL;
ILTTexInterface* g_pLTCTexInterface = NULL;
ILTClientSoundMgr* g_pLTCSoundMgr = NULL;
ILTFontManager* g_pLTCFontManager = NULL;
ILTWidgetManager* g_pLTCWidgetManager = NULL;
ILTVideoMgr * g_pLTCVideoMgr = NULL;

//-----------------------------------------------------------------------------
define_holder_to_instance(ILTCommon, g_pLTCCommon, Client);
define_holder_to_instance(ILTPhysics, g_pLTCPhysics, Client);
define_holder(ILTModelClient, g_pLTCModel);
define_holder(ILTDrawPrim, g_pLTCDrawPrim);
define_holder(ILTTexInterface, g_pLTCTexInterface);
define_holder(ILTClientSoundMgr, g_pLTCSoundMgr);
define_holder(ILTFontManager, g_pLTCFontManager);
define_holder(ILTWidgetManager, g_pLTCWidgetManager);
define_holder(ILTVideoMgr, g_pLTCVideoMgr);

//-----------------------------------------------------------------------------
static 
LTRESULT VerifyInterface(IBase *pInterface, const char *pIName)
{
	if (pInterface == NULL)
	{
		g_pLTClient->DebugOut("%s interface not properly initialized.\n", pIName);
		return LT_NOTINITIALIZED;
	}
	return LT_OK;
}


//-----------------------------------------------------------------------------
LTRESULT VerifyClientInterfaces()
{
	LTRESULT result = LT_OK;
	
	result = VerifyInterface(g_pLTClient, "ILTClient");
	if (LT_OK != result)
		return result;
	
	result = VerifyInterface(g_pLTCCommon, "ILTCommon");
	if (LT_OK != result)
		return result;
	
	result = VerifyInterface(g_pLTCPhysics, "ILTPhysics");
	if (LT_OK != result)
		return result;
	
	result = VerifyInterface(g_pLTCModel, "ILTModelClient");
	if (LT_OK != result)
		return result;
	
	result = VerifyInterface(g_pLTCSoundMgr, "ILTSoundMgr");
	if (LT_OK != result)
		return result;

	result = VerifyInterface(g_pLTCFontManager, "ILTFontManager");
	if (LT_OK != result)
		return result;

	result = VerifyInterface(g_pLTCWidgetManager, "ILTWidgetManager");
	if (LT_OK != result)
		return result;

	result = VerifyInterface(g_pLTCVideoMgr, "ILTVideoMgr");
	if (LT_OK != result)
		return result;


	return result;
}


