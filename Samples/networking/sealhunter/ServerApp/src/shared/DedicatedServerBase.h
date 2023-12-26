// *********************************************************************** //
//
// MODULE  : DedicatedServerBase.h
//
// PURPOSE : Base class for dedicated server applications.
//
// CREATED : 07/06/04
//
// (c) 2004 Monolith Productions, Inc.  All Rights Reserved
//
// *********************************************************************** //


#ifndef __DEDICATEDSERVERBASE_H__
#define __DEDICATEDSERVERBASE_H__

#include <vector>
#include <string>
#include <set>
//#include "platform.h"
#include "server_interface.h"
//#include "ltthreads.h"
#include "PlayerInfo.h"
//#include "ScmdConsole.h"
#include "systhread.h"

// forward declarations
class ServerGameOptions;

// type definitions
typedef std::vector<std::string> MessageList;
typedef std::vector<std::string> MapNameList;
typedef std::vector<std::string> ResourceFileList;

class CDedicatedServerBase : public ServerAppHandler //, public ScmdConsoleDriver
{
public:

	// enums
	enum ServerErrorEnum
	{
		DS_ERROR_INITFAILED,
		DS_ERROR_NOMAPS,
		DS_ERROR_OUTOFMEMORY,
		DS_ERROR_UNABLETOHOST,
		DS_ERROR_BINARIES,
		DS_ERROR_HOSTSESSION,
		DS_ERROR_SELECTSERVICE,
		DS_ERROR_LOADWORLD
	};

	CDedicatedServerBase();
	virtual ~CDedicatedServerBase();

	// properties
	ServerInterface* GetServerInterface() const { return m_pServerMgr; }

protected:

	// structure for a client connection
	struct ClientIP
	{
		// Used for sorting in std::set comparisons.
		bool operator<( ClientIP const& y ) const
		{
			for( int i = 0; i < 4; i++ )
			{
				if( m_nPart[i] < y.m_nPart[i] )
					return true;
			}

			return false;
		}

		uint8	m_nPart[4];
	};

	typedef std::set<ClientIP> ClientIPSet; 

	// properties
	const PlayerInfoList& GetPlayers() const { return m_Players; }
	const ClientIPSet&	  GetUniqueClients() const {return m_UniqueClients; }
	uint32				  GetCurrentMap() const { return m_nCurrentMap; }
	const std::string&    GetCurrentMapName() const { return m_strCurrentMapName; }
	const MapNameList&    GetMapNames() const { return m_MapNames; }
	const MessageList&	  GetShellMessages() const { return m_ShellMessages; }
	CLTCriticalSection&   GetCriticalSection() { return m_CriticalSection; }
	CLTThreadEventManual& GetStopEvent() { return m_cStopEvent; }
	bool				  IsErrorLogEnabled() const { return m_bWriteErrorLog; }
	const std::string&	  GetErrorLogFileName() const { return m_strErrorLogFileName; }
	bool				  IsErrorLogAlwaysFlushEnabled() const { return m_bAlwaysFlushLog; }

	// methods
	void ParseCommandLine(bool bCheckForProfile, ServerGameOptions* pServerGameOptions, int argc, char** argv);
	bool InitializeServer(const int nSIVersion, LTGUID cAppGUID, const std::string& strEngineModuleName, ResourceFileList& cResourceFiles);
//	bool StartServer(ServerGameOptions& cServerGameOptions);
	bool StartServer();
	bool ShutdownServer();
	void AddShellMessage(const std::string& strMessage) { m_ShellMessages.push_back(strMessage); }
	void ClearShellMessages() { m_ShellMessages.erase(m_ShellMessages.begin(), m_ShellMessages.end()); }
	bool IsLoaded() { return (m_pServerMgr != NULL); }
	bool IsRunning() { return (m_pServerMgr != NULL && m_cServerUpdateThread.IsCreated()); }	
	bool InitErrorLog();
	void TermErrorLog();
	bool WriteToErrorLog(char const* pszMsg);

	// derived class hooks
	virtual void OnServerInit() = 0;
	virtual void OnServerPreAddClient(CPlayerInfo& cPlayerInfo) = 0;
	virtual void OnServerPostAddClient(CPlayerInfo& cPlayerInfo) = 0;
	virtual void OnServerRemovedClient(CPlayerInfo& cPlayerInfo) = 0;
	virtual void OnServerUpdate() = 0;
	virtual void OnServerPreLoadWorld() = 0;
	virtual void OnServerPostLoadWorld() = 0;
	virtual void OnServerError(ServerErrorEnum eServerError) = 0;
//	virtual wchar_t const* OnServerLoadString(const char* szResId) = 0;

private:

	// ServerAppHandler interface methods
	LTRESULT ShellMessageFn( ILTMessage_Read& msg );
	LTRESULT ConsoleOutputFn( const char *pMsg );
	LTRESULT OutOfMemory();

	// shell message handling methods
	void OnShellMessageInit(ILTMessage_Read& msg);
	void OnShellMessageAddClient(ILTMessage_Read& msg);
	void OnShellMessageRemoveClient(ILTMessage_Read& msg);
	void OnShellMessageUpdate(ILTMessage_Read& msg);
	void OnShellMessagePreLoadWorld(ILTMessage_Read& msg);
	void OnShellMessagePostLoadWorld(ILTMessage_Read& msg);

	// ScmdConsoleDriver overrides
	virtual bool SendToServer( ILTMessage_Read& msg );
	virtual bool WriteMessage( char const* pszMessge );
//	virtual char const * LoadStringResId( const char* szResId );

	// engine server interface
	ServerInterface* m_pServerMgr;

	// thread function, thread object, and stop event
	static uint32 ServerUpdateThreadFunction(void *pParam);

	CLTThread			 m_cServerUpdateThread;
	CLTThreadEventManual m_cStopEvent;

	// critical section for update thread
	CLTCriticalSection m_CriticalSection;

	// additional resource files from command line
	ResourceFileList m_cAdditionalResourceFiles;

	// list of map names
	MapNameList m_MapNames;

	// list of text messages from the shell
	MessageList m_ShellMessages;

	// list of players and associated information
	PlayerInfoList m_Players;

	// list of unique client connections
	ClientIPSet m_UniqueClients;

	// current map number
	enum { k_nInvalidMap = -1 };
	int m_nCurrentMap;

	// current map name
	std::string m_strCurrentMapName;

	// error log options and file
	bool		m_bWriteErrorLog;
	std::string m_strErrorLogFileName;
	bool		m_bAlwaysFlushLog;
	FILE*		m_pErrorLogFile;
	int			m_nPort;
	std::string m_sServerName;
	int			m_nMaxPlayers;

};

#endif // __DEDICATEDSERVERBASE_H__

