// ----------------------------------------------------------------------- //
//
// MODULE  : ServerApp.cpp
//
// PURPOSE : Server application class for Linux dedicated server.
//
// CREATED : 07/09/04
//
// (c) 1996-2004 Monolith Productions, Inc.  All Rights Reserved
// (c) 2004 Touchdown Entertainment Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include <stdio.h>
#include <asm/errno.h>
#include "ServerApp.h"

enum
{
	k_nMainThreadPollMS = 100
};

extern LTGUID GAMEGUID;

CServerApp::CServerApp()
{
}

CServerApp::~CServerApp()
{
}

int CServerApp::Run(int argc, char** argv)
{
	// print startup banner
	std::string strBanner = "SealHunter Dedicated Linux Server";
	
	printf("%s\n\n", strBanner.c_str());
	
	printf("Initializing...\n");
	
	ParseCommandLine(false, NULL, argc, argv);
	
	ResourceFileList cResourceFiles;
	// add sealhunter resource directory
	cResourceFiles.push_back("rez");

	// initialize
	if (!InitializeServer(SI_VERSION, GAMEGUID, "server.so", cResourceFiles))
	{
		printf("Server failed to initialize.\n");
	
		return 1;
	}

	if (!StartServer())
	{
		return 1;
	}
	
	// initialization complete	
	printf("Ready\n");

	// poll stop event, and process any messages
	MessageList cMessageList;

	while (GetStopEvent().Block(k_nMainThreadPollMS) == BLOCK_TIMEOUT)
	{
		// process message list
		GetCriticalSection().Enter();
		cMessageList = GetShellMessages();
		ClearShellMessages();
		GetCriticalSection().Leave();
		
		for (MessageList::iterator itMessage = cMessageList.begin(); itMessage != cMessageList.end(); ++itMessage)
		{
			std::string& strMessage = *itMessage;
			printf("%s\n", strMessage.c_str());
			
			WriteToErrorLog(strMessage.c_str());
		}
	}
		
	// print exit message
	printf("SealHunter Dedicated Linux Server - Ending\n\n");
	
	return 0;
}

void CServerApp::OnServerInit()
{
}

void CServerApp::OnServerPreAddClient(CPlayerInfo& cPlayerInfo)
{
}

void CServerApp::OnServerPostAddClient(CPlayerInfo& cPlayerInfo)
{
}

void CServerApp::OnServerRemovedClient(CPlayerInfo& cPlayerInfo)
{
}

void CServerApp::OnServerUpdate()
{
}

void CServerApp::OnServerPreLoadWorld()
{
}

void CServerApp::OnServerPostLoadWorld()
{
}

void CServerApp::OnServerError(ServerErrorEnum eServerError)
{
	char szTemp[256];
	
	switch ( eServerError )
	{
		case DS_ERROR_INITFAILED:
			strcpy(szTemp, "Init Failed" );
			break;	
		case DS_ERROR_NOMAPS:
			strcpy(szTemp, "No Maps" );
			break;	
		case DS_ERROR_OUTOFMEMORY:
			strcpy(szTemp, "Out of Memory" );
			break;	
		case DS_ERROR_UNABLETOHOST:
			strcpy(szTemp, "Unable to host" );
			break;	
		case DS_ERROR_BINARIES:
			strcpy(szTemp, "Loading Binaries" );
			break;	
		case DS_ERROR_HOSTSESSION:
			strcpy(szTemp, "Hosting Session" );
			break;	
		case DS_ERROR_SELECTSERVICE:
			strcpy(szTemp, "Selecting service" );
			break;
		case DS_ERROR_LOADWORLD:
			strcpy(szTemp, "Loading world" );
			break;
		default:
			strcpy(szTemp, "Unknown" );
			break;
		
	}
	
	printf("Server Error: %s\n", szTemp);
	
}

