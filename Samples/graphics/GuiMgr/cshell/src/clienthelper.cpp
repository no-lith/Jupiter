// ----------------------------------------------------------------------- //
//
// MODULE  : ltclientshell.cpp
//
// PURPOSE : Client-side helper functions - Implementation
//
// CREATED : 03/22/01
//
// (c) 2001 LithTech, Inc.  All Rights Reserved
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
// ----------------------------------------------------------------------- //


// Definition of helper functions
#include "clienthelper.h"


#include <windows.h>

// Shared includes
#include "helper.h"

// Engine includes
#include <iltclient.h>

//-----------------------------------------------------------------------------
extern ILTClient* g_pLTClient;


static wchar_t s_szStringBuffer[2048];

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


wchar_t * LoadTempString(int messageCode)
{
	void* pModule;
	g_pLTClient->GetEngineHook("cres_hinstance",&pModule);
	HMODULE hModule = (HINSTANCE)pModule;

	s_szStringBuffer[0] = '\0';

    if (hModule)
    {
		uint32 nBytes = LoadStringW(hModule, messageCode, s_szStringBuffer, sizeof(s_szStringBuffer));
    }

	return s_szStringBuffer;
}

int GetConsoleInt(char const* sKey, int nDefault)
{
    if (g_pLTClient)
	{
        HCONSOLEVAR hVar = g_pLTClient->GetConsoleVar(( char* )sKey);
		if (hVar)
		{
            float fValue = g_pLTClient->GetVarValueFloat(hVar);
			return((int)fValue);
		}
	}

	return(nDefault);
}

