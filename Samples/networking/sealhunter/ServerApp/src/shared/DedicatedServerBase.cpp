// *********************************************************************** //
//
// MODULE  : DedicatedServerBase.cpp
//
// PURPOSE : Base class for dedicated server applications.
//
// CREATED : 07/06/04
//
// (c) 2004 Monolith Productions, Inc.  All Rights Reserved
// (c) 2004 Touchdown Entertainment Inc.  All Rights Reserved
//
// *********************************************************************** //

#include "StdAfx.h"

#if defined(__LINUX)
#include <asm/errno.h>


#else
#include "mmsystem.h"
#endif

// Engine includes
#include <iltserver.h>

// Unused Engine objects 
//#include <iltphysics.h>
//#include <iltmodel.h>
//#include <iltcommon.h>

// Engine objects 
/////////////////////////////////////////////////////////////////////////////
//ILTCommon*	g_pCommonLT = NULL;
//ILTPhysics*	g_pPhysicsLT = NULL;
//ILTModel*		g_pModelLT = NULL;
//ILTCSBase*	g_pLTBase = NULL;


// {00000000-0000-0000-0000-0000000000}
LTGUID GAMEGUID = { 0x00000000, 0x0000, 0x0000, { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 } };


#include "DedicatedServerBase.h"
#include "ltlibraryloader.h"
#include "iservershell.h"

/////////////////////////////////////////////////////////////////////////////

// Get our engine interface pointers.
SETUP_GPLTSERVER();

// maximum number of resource files and directories
enum { k_nMaxRezFiles = 32 };

// At least 33 ms between server updates = cap at 30fps
#define MIN_SERVER_UPDATE 33 

#ifdef __LINUX
// Linux version of timeGetTime()

uint32 timeGetTime()
{
	timeval curTimeval;
	gettimeofday(&curTimeval, NULL);
	return (curTimeval.tv_sec * 1000) + (curTimeval.tv_usec / 1000);
}

#endif

CDedicatedServerBase::CDedicatedServerBase()
	: m_pServerMgr(NULL),
	  m_bWriteErrorLog(false),
	  m_strErrorLogFileName("server.log"),
	  m_bAlwaysFlushLog(false),
	  m_pErrorLogFile(NULL),
	  m_nPort(27889),
	  m_sServerName("SealHunter Server"),
	  m_nMaxPlayers(12)
{
}

CDedicatedServerBase::~CDedicatedServerBase()
{
}

void CDedicatedServerBase::ParseCommandLine(bool bCheckForProfile, ServerGameOptions* pServerGameOptions, int argc, char** argv)
{
	// Search each command and find setting names with matching setting values.
	for(int nArg = 1; nArg < argc; nArg++)
	{
		// Make sure there's another argument after this one.  You can't just have
		// the setting name without the setting value.
		if( nArg + 1 >= argc )
			break;

		// Check if it's a port setting.
		if( stricmp( argv[nArg], "-port" ) == 0 )
		{
			// Set the port.
			m_nPort = (uint16)LTMIN(atoi( argv[nArg + 1] ),65535);

			// Skip the value.
			nArg++;
		}
		// Check if it's the server name.
		else if( stricmp( argv[nArg], "-name" ) == 0 )
		{
			// Set the server name.
			m_sServerName = argv[nArg + 1]; // ).c_str();

			// Skip the value.
			nArg++;
		}
		// Check if they want to write out an error log.
		else if( stricmp( argv[nArg], "+errorlog" ) == 0 )
		{
			m_bWriteErrorLog = !!atoi( argv[nArg + 1] );

			// Skip the value.
			nArg++;
		}
		// Check if they want to set the error log filename.
		else if( stricmp( argv[nArg], "+errorlogfile" ) == 0 )
		{
			// Set the errorlog name.
			m_strErrorLogFileName = argv[nArg + 1];

			// Skip the value.
			nArg++;
		}
		// Check if they want to always flush the log after every write.
		else if( stricmp( argv[nArg], "+alwaysflushlog" ) == 0 )
		{
			m_bAlwaysFlushLog = !!atoi( argv[nArg + 1] );

			// Skip the value.
			nArg++;
		}
		else if( stricmp( argv[nArg], "-rez" ) == 0)
		{
			m_cAdditionalResourceFiles.push_back(argv[nArg + 1] );

			// Skip the value.
			nArg++;
		}
	}
}



bool CDedicatedServerBase::InitializeServer(const int nSIVersion, LTGUID cAppGUID, const std::string& strEngineModuleName, ResourceFileList& cResourceFiles)
{
	// Create the server
	SI_CREATESTATUS status = CreateServer(nSIVersion, cAppGUID, &m_pServerMgr);
	if (status != 0 || !m_pServerMgr)
	{
		OnServerError(DS_ERROR_INITFAILED);
		return false;
	}


	// Merge our interface database with the database in the DLL we just loaded.
	HLTMODULE hModule = LTLibraryLoader::GetLibraryHandle(strEngineModuleName.c_str());
	TSetMasterFn pSetMasterFn = (TSetMasterFn) LTLibraryLoader::GetProcAddress(hModule, "SetMasterDatabase");

	// Check if the function existed.
	if (pSetMasterFn != NULL) 
	{
		// Merge our database with theirs
		pSetMasterFn(GetMasterDatabase());
	}


	// Fill in the csbase pointer so other utilities can call into it.
//	g_pLTBase = g_pLTServer;


	// Set our app handler.
	if (m_pServerMgr->SetAppHandler(this) != LT_OK )
	{
		OnServerError(DS_ERROR_INITFAILED);
		return false;
	}


	// convert resource file lists into the array required by the engine
	ResourceFileList cAllResourceFiles;
	cAllResourceFiles.resize(cResourceFiles.size() + m_cAdditionalResourceFiles.size());

	cAllResourceFiles.insert(cAllResourceFiles.begin(), cResourceFiles.begin(), cResourceFiles.end());
	cAllResourceFiles.insert(cAllResourceFiles.end(), m_cAdditionalResourceFiles.begin(), m_cAdditionalResourceFiles.end());

	if (cAllResourceFiles.size() > k_nMaxRezFiles)
	{
		WriteToErrorLog( "Too many resource files" );
		OnServerError(DS_ERROR_INITFAILED);
		return false;
	}
		
	char* pResourceFilesArray[k_nMaxRezFiles];
	::memset(pResourceFilesArray, 0, k_nMaxRezFiles * sizeof(char*));
	int nResourceFilesIndex = 0;
	
	for (ResourceFileList::iterator itResourceFile = cAllResourceFiles.begin(); itResourceFile != cAllResourceFiles.end(); ++itResourceFile)
	{
		std::string& strResourceFile = *itResourceFile;
		int nBufferSize = (int) strResourceFile.length() + 1;
		pResourceFilesArray[nResourceFilesIndex] = new char[nBufferSize];
		LTStrCpy(pResourceFilesArray[nResourceFilesIndex], strResourceFile.c_str(), nBufferSize);
		
		nResourceFilesIndex++;
	}
	
	// add resources
	if (!m_pServerMgr->AddResources((const char**)&pResourceFilesArray, nResourceFilesIndex))
	{
		WriteToErrorLog( "No valid resource files at startup" );
	}
	
	// free the array
	for (int nIndex = 0; nIndex < k_nMaxRezFiles; ++nIndex)
	{
		delete [] pResourceFilesArray[nIndex];
	}	

	// clear state
	ClearShellMessages();

	// hardcoded sealhunter map 
	m_strCurrentMapName = "Worlds/World";

	// get engine pointers
//	g_pModelLT   = g_pLTServer->GetModelLT();
//	g_pPhysicsLT = g_pLTServer->Physics();
//	g_pLTBase    = static_cast<ILTCSBase*>(g_pLTServer);
//	g_pCommonLT  = g_pLTServer->Common();

	// Initialize the networking.
	if (!m_pServerMgr->InitNetworking(NULL, 0))
	{
		return false;
	}

	// Get the comm services the server is supporting.
	NetService* pServiceListHead = NULL;
	if(!m_pServerMgr->GetServiceList(pServiceListHead) || !pServiceListHead)
	{
		return false;
	}

	// Find the TCPIP service.
	HNETSERVICE hNetService = NULL;
	NetService* pCurrentService = pServiceListHead;
	while (pCurrentService)
	{
		if (pCurrentService->m_dwFlags & NETSERVICE_TCPIP)
		{
			hNetService = pCurrentService->m_handle;
			break;
		}

		pCurrentService = pCurrentService->m_pNext;
	}

	// Free the service list.
	m_pServerMgr->FreeServiceList(pServiceListHead);

	// Check if tcp not found.
	if (!hNetService)
	{
		return false;
	}

	// Select it.
	if (!m_pServerMgr->SelectService(hNetService))
	{
		return false;
	}

	return true;
}



bool CDedicatedServerBase::StartServer()
{
	if (!m_pServerMgr)
	{
		return false;
	}

	// start error log if necessary
	InitErrorLog();

	// Setup the host info.
	NetHost netHost;
	netHost.m_Port = m_nPort;
	netHost.m_dwMaxConnections = m_nMaxPlayers;
	netHost.m_dwFlags = 0;
    strcpy ( netHost.m_sName, m_sServerName.c_str() );
    netHost.m_bHasPassword = true;
	netHost.m_nGameType = 0;

	// Start hosting.
	if(!m_pServerMgr->HostGame(&netHost))
	{
		OnServerError(DS_ERROR_UNABLETOHOST);
		return false;
	}
	

	// Load the server shell
	if( !m_pServerMgr->LoadBinaries())
	{
		OnServerError(DS_ERROR_BINARIES);
		return false;
	}


	// Make sure the stop event is clear
	m_cStopEvent.Clear();


	// start the thread
	m_cServerUpdateThread.Create(&ServerUpdateThreadFunction, (void*)this);


	// Load the world 
	StartGameRequest sgr;
	sgr.m_Type = STARTGAME_HOSTTCP;
	strcpy ( sgr.m_WorldName, "Worlds/World" );

    if ( !m_pServerMgr->StartWorld(&sgr ) )
	{
		OnServerError(DS_ERROR_LOADWORLD);
		return false;
	}

	return true;

}



bool CDedicatedServerBase::ShutdownServer()
{
	// Kill the thread.
	if (m_cServerUpdateThread.IsCreated())
	{
		m_cStopEvent.Set();
		m_cServerUpdateThread.WaitForExit();
	}

	// Delete the server interface.
	if (m_pServerMgr)
	{
		m_pServerMgr->SetAppHandler( NULL );
		DeleteServer();
		m_pServerMgr = NULL;
	}

	// Clear out variables.
	ClearShellMessages();
	m_nCurrentMap = k_nInvalidMap;
	m_strCurrentMapName = "";

	// stop error log
	TermErrorLog();

	return true;
}



LTRESULT CDedicatedServerBase::ShellMessageFn(ILTMessage_Read& msg)
{
	uint8 nMessageId = msg.Readuint8();
	return LT_OK;
}

/*

void CDedicatedServerBase::OnShellMessageInit(ILTMessage_Read& msg)
{
	// notify derived class
	OnServerInit();
}


void CDedicatedServerBase::OnShellMessageAddClient(ILTMessage_Read& msg)
{
	// Get the client id for the player.
	int nClientId = msg.Readuint16();

	// Remove any old players that might be in the player list.
	PlayerInfoList::iterator itPlayer = m_Players.begin();
	
	while (itPlayer != m_Players.end())
	{
		CPlayerInfo& cPlayer = *itPlayer;

		if (cPlayer.GetClientId() == nClientId)
		{
			// existing player has the same Id as the new player - remove it
			WriteToErrorLog( "duplicate client ID encountered" );

			itPlayer = m_Players.erase(itPlayer);
		}
		else
		{
			// move to the next player
			++itPlayer;
		}
	}

	// create the new player object
	CPlayerInfo cPlayerInfo;
	cPlayerInfo.SetClientId(nClientId);

	// Add the client to our map of unique clients.
	uint16 nPort;
	ClientIP clientIP;
	if( m_pServerMgr->GetClientAddr( nClientId, clientIP.m_nPart, &nPort ))
	{
		m_UniqueClients.insert(clientIP);
	}

	// set time of connection
	time_t currentTime;
	::time(&currentTime);
	cPlayerInfo.SetTimeOnServer(currentTime);

	// notify derived class we're about to add a new client
	OnServerPreAddClient(cPlayerInfo);

	// store the new player
	m_Players.push_back(cPlayerInfo);

	// notify derived class we added the client
	OnServerPostAddClient(cPlayerInfo);
}

void CDedicatedServerBase::OnShellMessageRemoveClient(ILTMessage_Read& msg )
{
	// Get the client id for the player.
	int nClientId = msg.Readuint16();

	// Remove the player from the list.
	CPlayerInfo cPlayerInfo;

	for (PlayerInfoList::iterator itPlayer = m_Players.begin(); itPlayer != m_Players.end( ); ++itPlayer)
	{
		cPlayerInfo = *itPlayer;
		if( cPlayerInfo.GetClientId() == nClientId )
		{
			m_Players.erase(itPlayer);
			break;
		}
	}

	// notify derived class
	OnServerRemovedClient(cPlayerInfo);		
}

void CDedicatedServerBase::OnShellMessageUpdate(ILTMessage_Read& msg)
{

	// Keep reading player updates until invalid clientid given.
	while (true)
	{
		uint16 nClientId = msg.Readuint16( );

		// end of list is -1
		if (nClientId == (uint16)-1)
		{
			break;
		}

		// Lookup client Id in known clients.
		CPlayerInfo* pPlayerInfo = NULL;
		PlayerInfoList::iterator itPlayer = m_Players.begin();
		
		for( ; itPlayer != m_Players.end( ); ++itPlayer)
		{
			CPlayerInfo& cPlayerInfo = *itPlayer;

			if (cPlayerInfo.GetClientId() != nClientId)
			{
				continue;
			}

			// found the player
			pPlayerInfo = &cPlayerInfo;
			break;
		}

		if (!pPlayerInfo)
		{
			// could not locate this player
			WriteToErrorLog( "update specified client id that has not corresponding player" );
			
			break;
		}

		// Update the playerinfo.
		char szHandle[256];
		msg.ReadString(szHandle, sizeof(szHandle));
		pPlayerInfo->SetPlayerHandle(szHandle);
		pPlayerInfo->SetKills(msg.Readint16());
		pPlayerInfo->SetDeaths(msg.Readint16());
		pPlayerInfo->SetScore(msg.Readint16());
	}

	// inform the derived class
	OnServerUpdate();
}

void CDedicatedServerBase::OnShellMessagePreLoadWorld(ILTMessage_Read& msg)
{

	// about to change maps - set map number to invalid map
	m_nCurrentMap = k_nInvalidMap;

	// inform derived class
	OnServerPreLoadWorld();
}

void CDedicatedServerBase::OnShellMessagePostLoadWorld(ILTMessage_Read& msg)
{

	// map change complete
	m_nCurrentMap = msg.Readuint16();
	char szCurLevel[MAX_PATH*2] = "";
	msg.ReadString(szCurLevel, ARRAY_LEN(szCurLevel));
	m_strCurrentMapName = szCurLevel;

	// inform derived class
	OnServerPostLoadWorld();
}

*/


LTRESULT CDedicatedServerBase::ConsoleOutputFn( const char *pMsg )
{
	// add it to the list of messages
	m_ShellMessages.push_back(pMsg);

	return LT_OK;
}

LTRESULT CDedicatedServerBase::OutOfMemory()
{
	// notify derived class
	OnServerError(DS_ERROR_OUTOFMEMORY);

	return LT_OK;
}

uint32 CDedicatedServerBase::ServerUpdateThreadFunction(void* pParam)
{
	CDedicatedServerBase* pServerBase = (CDedicatedServerBase*)pParam;

	uint32 nLastTime = timeGetTime();

	CLTCriticalSection&   cCriticalSection = pServerBase->GetCriticalSection();
	CLTThreadEventManual& cStopEvent       = pServerBase->GetStopEvent();

	// Call the update until we are done...
	while (cStopEvent.Block(0) == BLOCK_TIMEOUT)
	{
		cCriticalSection.Enter();
		ServerInterface* pServerMgr = pServerBase->GetServerInterface();
		if (pServerMgr)
		{
			if (!pServerMgr->Update(0))
			{		
				char szError[256];

				// Something went wrong.  Put the error on the console.
				pServerMgr->GetErrorString(szError, sizeof(szError)-1);
				pServerBase->AddShellMessage(szError);

				// set stop event to indicate we need to shutdown
				cStopEvent.Set();
			}
		}
		cCriticalSection.Leave();

		// Restrict the server's framerate
		uint32 nCurTime = timeGetTime();
		uint32 nDelay = 0;
		if (((nLastTime + MIN_SERVER_UPDATE) > nCurTime) && (nCurTime > nLastTime))
		{
			nDelay = (nLastTime + MIN_SERVER_UPDATE) - nCurTime;
		}

		// If we're running too fast, slow it down..
		if (nDelay)
		{
			// Allow our sleeping to be interrupted by a terminate request
			cStopEvent.Block(nDelay);
		}

		// Get the time for the next loop
		nLastTime = timeGetTime();

		// Adjust the time by however much we overslept
		if (((nLastTime - nCurTime) > nDelay) && (nDelay > 0))
		{
			nLastTime -= (nLastTime - (nCurTime + nDelay));
		}	
	}

	return 0;
}

bool CDedicatedServerBase::InitErrorLog()
{
	// Make sure they want an errorlog.
	if (!m_bWriteErrorLog)
	{
		return false;
	}

	// Make sure we start fresh.
	TermErrorLog( );

	// open the file
	if ((m_pErrorLogFile = fopen(m_strErrorLogFileName.c_str(), "wtc")) == NULL)
	{
		return false;
	}

	return true;
}

void CDedicatedServerBase::TermErrorLog()
{
	if (!m_pErrorLogFile)
	{
		return;
	}

	::fflush(m_pErrorLogFile);
	::fclose(m_pErrorLogFile);
	m_pErrorLogFile = NULL;
}

bool CDedicatedServerBase::WriteToErrorLog(char const* pszMsg)
{
	if (!pszMsg || !m_pErrorLogFile)
	{
		return false;
	}

	::fprintf(m_pErrorLogFile, pszMsg);
	if (pszMsg[strlen(pszMsg) - 1] != '\n')
	{
		::fprintf(m_pErrorLogFile, "\n");
	}

	if (m_bAlwaysFlushLog)
	{
		::fflush(m_pErrorLogFile);
	}

	return true;
}

bool CDedicatedServerBase::SendToServer(ILTMessage_Read& msg)
{
	if (m_pServerMgr->SendToServerShell(msg) != LT_OK)
	{
		return false;
	}

	return true;
}


bool CDedicatedServerBase::WriteMessage(char const* pszMessage)
{
	// Check inputs.
	if (!pszMessage || !pszMessage[0])
	{
		WriteToErrorLog( "Invalid inputs" );
		return false;
	}
	
	// put on the message list
	m_ShellMessages.push_back( pszMessage );
	
	return true;
}


