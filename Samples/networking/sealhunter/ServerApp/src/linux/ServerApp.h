// ----------------------------------------------------------------------- //
//
// MODULE  : ServerApp.h
//
// PURPOSE : Server application class for Linux dedicated server.
//
// CREATED : 07/09/04
//
// (c) 1996-2004 Monolith Productions, Inc.  All Rights Reserved
// (c) 2004 Touchdown Entertainment Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //
#include "DedicatedServerBase.h"

class CServerApp : public CDedicatedServerBase
{
public:

	CServerApp();
	virtual ~CServerApp();
	
	// main entry point
	int Run(int argc, char** argv);	

private:

	// DedicatedServerBase handlers
	virtual void OnServerInit();
	virtual void OnServerPreAddClient(CPlayerInfo& cPlayerInfo);	
	virtual void OnServerPostAddClient(CPlayerInfo& cPlayerInfo);
	virtual void OnServerRemovedClient(CPlayerInfo& cPlayerInfo);
	virtual void OnServerUpdate();
	virtual void OnServerPreLoadWorld();
	virtual void OnServerPostLoadWorld();
	virtual void OnServerError(ServerErrorEnum eServerError);

};

	
