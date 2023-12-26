// *********************************************************************** //
//
// MODULE  : PlayerInfo.h
//
// PURPOSE : Stores information about each connected player.
//
// CREATED : 07/06/04
//
// (c) 2004 Monolith Productions, Inc.  All Rights Reserved
//
// *********************************************************************** //


#ifndef __PLAYERINFO_H__
#define __PLAYERINFO_H__

#include <string>

class CPlayerInfo
{
public:

	CPlayerInfo() : m_nClientId(-1), m_nKills(0), m_nScore(0), m_nDeaths(0)
	{
		::time(&m_timeOnServer);
	}
	~CPlayerInfo() {}

	uint32 GetClientId() const { return m_nClientId; }
	void   SetClientId(const uint32 nClientId) { m_nClientId = nClientId; }

	const std::string& GetPlayerHandle() const { return m_strPlayerHandle; }
	void	           SetPlayerHandle(const std::string& strPlayerHandle) { m_strPlayerHandle = strPlayerHandle; }

	uint32 GetKills() const { return m_nKills; }
	void   SetKills(const uint32 nKills) { m_nKills = nKills; }

	uint32 GetScore() const { return m_nScore; }
	void   SetScore(const uint32 nScore) { m_nScore = nScore; }

	uint32 GetDeaths() const { return m_nDeaths; }
	void   SetDeaths(const uint32 nDeaths) { m_nDeaths = nDeaths; }

	time_t GetTimeOnServer() const { return m_timeOnServer; }
	void   SetTimeOnServer(const time_t& timeOnServer) { m_timeOnServer = timeOnServer; }

private:

	int			m_nClientId;
	std::string	m_strPlayerHandle;
	int			m_nKills;
	int			m_nScore;
	int			m_nDeaths;
	time_t		m_timeOnServer;

};

// define a type for CPlayerInfo lists
typedef std::vector<CPlayerInfo> PlayerInfoList;



#endif // __PLAYERINFO_H__
