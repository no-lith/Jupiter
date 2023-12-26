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
        MSG_OBJ_PINGOBJECT,         // object->object
		MSG_TRG_PLAYMUSIC,			// server->client
        MSG_CS_CHANGEFX,            // client->server
        MSG_SC_REMOVEOBJECT,        // server->client
        MSG_CS_STOPEFFECT,          // client->server
        MSG_OBJ_STARTFX,			// object->object
        MSG_OBJ_STOPFX,			// object->object
		SFX_CLIENTFXSTOPFX,	
		MSG_LAST_MESSAGE			//last message marker, do not handle
		
};

#endif	// __MSG_IDS__
