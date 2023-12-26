// ----------------------------------------------------------------------- //
//
// MODULE  : commandids.h
//
// PURPOSE : Commands IDs - Definition
//
// CREATED : 12/10/2002
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //


#ifndef __COMMAND_IDS_H__
#define __COMMAND_IDS_H__


//-----------------------------------------------------------------------------
// These directly correspond to the AddAction values in autoexec.cfg.
enum ECommands
{
		COMMAND_MOVE_FORWARD			= 1,
		COMMAND_MOVE_BACKWARD			= 2,
		COMMAND_MOVE_LEFT				= 3,
		COMMAND_MOVE_RIGHT				= 4,

        COMMAND_ADD_MODELS              = 5,
        COMMAND_REMOVE_MODELS           = 6,
        COMMAND_ADD_COL                 = 7,
        COMMAND_REMOVE_COL              = 8,
		COMMAND_REMOVE_ALL				= 9,
		COMMAND_PREV_MODEL				= 10,
		COMMAND_NEXT_MODEL				= 11,

		COMMAND_QUIT					= 250
};



//------------------------------------------------------------------------------
// Vertex shaders
enum EVertexShaders
{
	VERTEXSHADER_RIGID					= 1,
	VERTEXSHADER_RIGID_NORMAL			= 2,

	VERTEXSHADER_SKEL1					= 3,
	VERTEXSHADER_SKEL2					= 4,
	VERTEXSHADER_SKEL3					= 5,

	VERTEXSHADER_SKEL3_NORMAL			= 6
};


//------------------------------------------------------------------------------
// Pixel shaders
enum EPixelShaders
{
	PIXELSHADER_SIMPLE					= 1,
	PIXELSHADER_COLORBUMP				= 2,
	PIXELSHADER_COLORLERP				= 3,
	PIXELSHADER_COLORBUMP2				= 4,
};



#endif	// __COMMAND_IDS_H__
