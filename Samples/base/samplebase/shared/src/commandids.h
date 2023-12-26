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
		
		COMMAND_QUIT					= 250
};

#endif	// __COMMAND_IDS_H__
