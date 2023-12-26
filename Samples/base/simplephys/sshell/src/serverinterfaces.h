// ----------------------------------------------------------------------- //
//
// MODULE  : serverinterfaces.h
//
// PURPOSE : Declares extern pointers to the server interfaces
//
// CREATED : 12/20/00
//
// (c) 2000 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __SERVER_INTERFACES_H__
#define __SERVER_INTERFACES_H__


// Engine includes
#include <ltbasedefs.h>


/////////////////////////////////////////////////////////////////////////////
// Must include sdk/inc/iltserver.h to use
class ILTServer;
extern ILTServer *g_pLTServer;

// Must include sdk/inc/iltcommon.h to use
class ILTCommon;
extern ILTCommon *g_pLTSCommon;

// Must include sdk/inc/iltphysics.h to use
class ILTPhysics;
extern ILTPhysics *g_pLTSPhysics;

// Must include sdk/inc/iltmodel.h to use
class ILTModel;
extern ILTModel *g_pLTSModel;

// Must include sdk/inc/iltsoundmgr.h to use
class ILTSoundMgr;
extern ILTSoundMgr* g_pLTSSoundMgr;

/////////////////////////////////////////////////////////////////////////////
// This function verifies that all interfaces have been properly
//  initialized (non-NULL)
//  It should be called sometime after construction of all interfaces.
LTRESULT VerifyServerInterfaces();


#endif	// __SERVER_INTERFACES_H__
