// ----------------------------------------------------------------------- //
//
// MODULE  : msgids.h
//
// PURPOSE : Messages IDs - Definition
//
// CREATED : 12/10/00
//
// (c) 2000 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //


#ifndef __MSG_IDS__
#define __MSG_IDS__


enum EMessageID
{

	MSG_STARTPOINT_POS_ROT      = 1,	// server->client
	MSG_WORLD_PROPS				= 2,	// server->client

	MSG_LEFT_MOUSECLICK         = 4,    // client->server

	MSG_ADD_MODEL				= 5,	// client->server
	MSG_MOVE_MODEL				= 6,	// client->server
	MSG_DELETE_MODEL			= 7,	// client->server

	MSG_MOVE_UP					= 8,	// client->server
	MSG_STOP_MOVING				= 9,	// client->server

	MSG_CS_POSITION				= 11,	// client->server
	MSG_CS_ROTATION				= 12,	// client->server
	MSG_CS_POSANDROT			= 13,	// client->server

	MSG_LAST_MESSAGE			= 14	//last message marker, do not handle
};


#endif	// __MSG_IDS__
