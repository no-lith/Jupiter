//------------------------------------------------------------------------------//
//
// MODULE	: ltservershell.h
//
// PURPOSE	: CLTServerShell - Definition
//
// CREATED	: 02/03/2002
//
// (c) 2002 LithTech, Inc.	All Rights Reserved
//
//------------------------------------------------------------------------------//

#ifndef __LT_SERVER_SHELL_H__
#define __LT_SERVER_SHELL_H__

// Definitions of members classes
#include "worldproperties.h"

// Base class for server shell
#include <iservershell.h>

//-----------------------------------------------------------------------------
class CLTServerShell : public IServerShellStub
{

public:
	CLTServerShell();
	~CLTServerShell() {}

	declare_interface(CLTServerShell);

	// Overridden functions, to receive callbacks from the engine
	//	These are all overridden here to make modification easier.
	//	Documentation exists in engine/sdk/inc/iservershell.h in the
	//	 IServerShell class
	LTRESULT	OnServerInitialized();
	void		OnServerTerm();

	void		SRand(unsigned int uiRand);

	void		PreStartWorld(bool bSwitchingWorlds);
	void		PostStartWorld();

	void		OnAddClient(HCLIENT hClient);
	void		OnRemoveClient(HCLIENT hClient);

	LPBASECLASS OnClientEnterWorld(HCLIENT hClient);
	void		OnClientExitWorld(HCLIENT hClient);

	void		Update(LTFLOAT timeElapsed);

	void		OnMessage(HCLIENT hSender, ILTMessage_Read *pMessage);
	void		OnObjectMessage(LPBASECLASS pSender, uint32 messageID,
					ILTMessage_Read* pMessage);

	void		OnPlaybackFinish();

	void		CacheFiles();
	LTRESULT	FileLoadNotify(const char *pFilename, LTRESULT status);

	LTRESULT	ServerAppMessageFn(const char *pMsg);

	LTRESULT	ProcessPacket(ILTMessage_Read *pMsg, uint8 senderAddr[4],
					uint16 senderPort);

	// Accessors
//	HCLIENT 	GetHClient()	   { return m_hClient; }
//	LPBASECLASS GetClientObj()	   { return m_pClientObj; }
//	HOBJECT 	GetClientHandle()  { return m_hClientHandle; }
	void		RegisterObject(WorldProperties *pWorldProps);
	void		UnRegisterObject(WorldProperties *pWorldProps);

private:

	LTRESULT	FindStartPoint(LTVector &vStartPtPos, LTRotation &rStartPtRot);

	// Our client, and pointer and handle to client object
//	HCLIENT 	m_hClient;
//	LPBASECLASS m_pClientObj;
//	HOBJECT 	m_hClientHandle;

	WorldProperties *m_pWorldProperties;

};


#endif //__LT_SERVER_SHELL_H__
