// ----------------------------------------------------------------------- //
//
// MODULE  : serverinterfaces.cpp
//
// PURPOSE : Defines pointers to the server interfaces
//
// CREATED : 12/20/00
//
// (c) 2000 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

// The header for the serverinterfaces
#include "serverinterfaces.h"

// Engine includes
#include <iltserver.h>

#include <iltphysics.h>
#include <iltmodel.h>
#include <iltsoundmgr.h>
#include <iltcommon.h>

/////////////////////////////////////////////////////////////////////////////
ILTCommon* g_pLTSCommon = NULL;
ILTPhysics* g_pLTSPhysics = NULL;
ILTModel* g_pLTSModel = NULL;
ILTSoundMgr* g_pLTSSoundMgr = NULL;

/////////////////////////////////////////////////////////////////////////////
define_holder_to_instance(ILTCommon, g_pLTSCommon, Server);
define_holder_to_instance(ILTPhysics, g_pLTSPhysics, Server);
define_holder_to_instance(ILTModel, g_pLTSModel, Server);
define_holder_to_instance(ILTSoundMgr, g_pLTSSoundMgr, Server);

/////////////////////////////////////////////////////////////////////////////
static 
LTRESULT VerifyInterface(IBase *pInterface, const char *pIName)
{
	if (pInterface == NULL)
	{
		g_pLTServer->DebugOut("%s interface not properly initialized.\n", pIName);
		return LT_NOTINITIALIZED;
	}
	return LT_OK;
}


/////////////////////////////////////////////////////////////////////////////
LTRESULT VerifyServerInterfaces()
{
	LTRESULT result = LT_OK;

	result = VerifyInterface(g_pLTServer, "ILTServer");
	if (LT_OK != result)
		return result;
	
	result = VerifyInterface(g_pLTSCommon, "ILTCommon");
	if (LT_OK != result)
		return result;

	result = VerifyInterface(g_pLTSPhysics, "ILTPhysics");
	if (LT_OK != result)
		return result;
	
	result = VerifyInterface(g_pLTSModel, "ILTModel");
	if (LT_OK != result)
		return result;
	
	result = VerifyInterface(g_pLTSSoundMgr, "ILTSoundMgr");
	if (LT_OK != result)
		return result;
	
	return result;
}


