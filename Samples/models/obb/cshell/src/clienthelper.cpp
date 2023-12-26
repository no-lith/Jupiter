// ----------------------------------------------------------------------- //
//
// MODULE  : ltclientshell.cpp
//
// PURPOSE : Client-side helper functions - Implementation
//
// CREATED : 03/22/01
//
// (c) 2001 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //


// Definition of helper functions
#include "clienthelper.h"
#include "helper.h"
#include "clientinterfaces.h"

// Engine includes
#include <iltclient.h>



//-----------------------------------------------------------------------------
// Print an error message and ask the client to shutdown
LTRESULT ShutdownIfFailed(LTRESULT result, const char *pErrStr)
{
	if (LT_OK != result)
	{
		if (!pErrStr)
		{
			pErrStr = "Unknown error, shutting down ...";
		}
		g_pLTClient->DebugOut(LTRESULT_TO_STRING(result));
		g_pLTClient->DebugOut(": ");
		g_pLTClient->DebugOut(pErrStr);
		g_pLTClient->ShutdownWithMessage(pErrStr);
	}

	return result;
}


//-----------------------------------------------------------------------------
// Print an error message
LTRESULT DebugPrintIfFailed(LTRESULT result, const char *pErrStr)
{
	if (LT_OK != result)
	{
		if (!pErrStr)
		{
			pErrStr = "Unknown error, shutting down ...";
		}
		g_pLTClient->DebugOut(LTRESULT_TO_STRING(result));
		g_pLTClient->DebugOut(": ");
		g_pLTClient->DebugOut(pErrStr);
	}

	return result;
}

