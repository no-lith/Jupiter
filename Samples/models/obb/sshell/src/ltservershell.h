//*****************************************************************************
//
//  Filename:   ltservershell.h
//
//  Class:      CLTServerShell - Class definition
//
//  Purpose:    Provides an interface between the engine and the server-side
//				game code.
//
//  Created:	5/20/2002
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
//****************************************************************************/


#ifndef __LT_SERVER_SHELL_H__
#define __LT_SERVER_SHELL_H__

#include <iservershell.h>
#include <ltbasedefs.h>

#include <vector>

#include "MsgIDs.h"



class CLTServerShell : public IServerShellStub
{
public:

	CLTServerShell();

	~CLTServerShell()
	{
	}

	declare_interface(CLTServerShell);


	// Initialization
	virtual LTRESULT 		OnServerInitialized();

	//	Client Management
	virtual LPBASECLASS		OnClientEnterWorld(HCLIENT hClient);
	virtual void			OnClientExitWorld(HCLIENT hClient);

	// Messaging
	virtual void			OnMessage(HCLIENT hSender, ILTMessage_Read *pMessage);

protected:

	HCLIENT					GetHClient()	   { return m_hClient; }
	LPBASECLASS 			GetClientObj()	   { return m_pClientObj; }
	HOBJECT					GetClientHandle()  { return m_hClientHandle; }

	void					LoadModels();
	void 					LoadModel(const char *Model, const std::vector<char *> &Skins, const LTVector &Pos,
									  const LTVector &Scale, const LTRotation &Rot, const char *AnimName);

protected:

	// Our client, and pointer and handle to client object
	HCLIENT			m_hClient;
	LPBASECLASS 	m_pClientObj;
	HOBJECT			m_hClientHandle;
};



#endif //__LT_SERVER_SHELL_H__
