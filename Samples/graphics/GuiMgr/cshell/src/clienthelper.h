// ----------------------------------------------------------------------- //
//
// MODULE  : clienthelper.h
//
// PURPOSE : Client-side helper functions - Definition
//
// CREATED : 03/22/01
//
// (c) 2001 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __CLIENT_HELPER_H__
#define __CLIENT_HELPER_H__


// Engine includes
#include <ltcodes.h>
#include "ClientRes.h"

/////////////////////////////////////////////////////////////////////////////
LTRESULT	ShutdownIfFailed(LTRESULT result, const char *pErrStr);
LTRESULT	DebugPrintIfFailed(LTRESULT result, const char *pErrStr);
int			DebugWrite(const char *pStr, ...);
wchar_t * LoadTempString(int messageCode);
int GetConsoleInt(char const* sKey, int nDefault);

#endif // __CLIENT_HELPER_H__
