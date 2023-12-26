//------------------------------------------------------------------------------//
// 
// MODULE   : msgids.h
// 
// PURPOSE  : Messages IDs - Definition
// 
// CREATED  : 12/10/2002
// 
// (c) 2002 LithTech, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//


#ifndef __MSG_IDS__
#define __MSG_IDS__


//-----------------------------------------------------------------------------
enum EMessageID
{
	
		MSG_STARTPOINT_POS_ROT, 	// server->client
		MSG_WORLD_PROPS,			// server->client
		MSG_LEFT_MOUSECLICK,		// client->server
		MSG_ADD_MODEL,				// client->server
		MSG_MOVE_MODEL, 			// client->server
		MSG_DELETE_MODEL,			// client->server
		MSG_MOVE_UP,				// client->server
		MSG_STOP_MOVING,			// client->server
		MSG_CS_POSITION,			// client->server
		MSG_CS_ROTATION,			// client->server
		MSG_CS_POSANDROT,			// client->server
		MSG_PLAYEROBJ,
		MSG_GAMELIGHT_INFO,
		MSG_LAST_MESSAGE			//last message marker, do not handle
		
};

#endif	// __MSG_IDS__
