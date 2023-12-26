// ----------------------------------------------------------------------- //
//
// MODULE  : clientinterfaces.h
//
// PURPOSE : Declares extern pointers to the client interfaces
//
// CREATED : 12/20/00
//
// (c) 2000 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __CLIENT_INTERFACES_H__
#define __CLIENT_INTERFACES_H__


// Engine includes
#include <ltbasetypes.h>


/////////////////////////////////////////////////////////////////////////////
// Must include sdk/inc/iltclient.h to use
class ILTClient;
extern ILTClient *g_pLTClient;

// Must include sdk/inc/iltcommon.h to use
class ILTCommon;
extern ILTCommon *g_pLTCCommon;

// Must include sdk/inc/iltphysics.h to use
class ILTPhysics;
extern ILTPhysics *g_pLTCPhysics;

// Must include sdk/inc/iltmodel.h to use
class ILTModelClient;
extern ILTModelClient *g_pLTCModel;

// Must include sdk/inc/iltdrawprim.h to use
class ILTDrawPrim;
extern ILTDrawPrim *g_pLTCDrawPrim;

// Must include sdk/inc/ilttexinterface.h to use
class ILTTexInterface;
extern ILTTexInterface *g_pLTCTexInterface;

// Must include sdk/inc/iltsoundmgr.h to use
class ILTClientSoundMgr;
extern ILTClientSoundMgr* g_pLTCSoundMgr;

// Must include sdk/inc/iltfontmanager.h to use
class ILTFontManager;
extern ILTFontManager* g_pLTCFontManager;

// Must include sdk/inc/iltwidgetmanager.h to use
class ILTWidgetManager;
extern ILTWidgetManager* g_pLTCWidgetManager;


class ILTDirectMusicMgr;
extern ILTDirectMusicMgr* g_pLTCDirectMusicMgr;


/////////////////////////////////////////////////////////////////////////////
// This function verifies that all interfaces have been properly
//  initialized (non-NULL)
//  It should be called sometime after construction of all interfaces.
LTRESULT VerifyClientInterfaces();


#endif	// __CLIENT_INTERFACES_H__
