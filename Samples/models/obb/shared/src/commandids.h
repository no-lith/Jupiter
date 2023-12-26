// ----------------------------------------------------------------------- //
//
// MODULE  : commandids.h
//
// PURPOSE : Commands IDs - Definition
//
// CREATED : 12/10/00
//
// (c) 2000 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //


#ifndef __COMMAND_IDS_H__
#define __COMMAND_IDS_H__



//-----------------------------------------------------------------------------
// These directly correspond to the AddAction values in autoexec.cfg.
enum ECommands
{
	COMMAND_MOVE_FORWARD 			= 1,
	COMMAND_MOVE_BACKWARD 			= 2,
	COMMAND_MOVE_LEFT 				= 3,
	COMMAND_MOVE_RIGHT 				= 4,
	COMMAND_MOVE_UP 				= 5,
	COMMAND_MOVE_DOWN 				= 6,
	COMMAND_TURN_LEFT 				= 7,
	COMMAND_TURN_RIGHT 				= 8,
	COMMAND_LOOK_UP 				= 9,
	COMMAND_LOOK_DOWN 				= 10,
	COMMAND_XAXIS_MOVE 				= 11,
	COMMAND_YAXIS_MOVE 				= 12,
	COMMAND_XAXIS_LOOK 				= 13,
	COMMAND_YAXIS_LOOK 				= 14,
	COMMAND_CENTER_VIEW				= 15,
	COMMAND_RESET_POS				= 16,
	COMMAND_ADD_ROW					= 20,
	COMMAND_REMOVE_ROW				= 21,
	COMMAND_ADD_COL					= 22,
	COMMAND_REMOVE_COL				= 23,
	COMMAND_REMOVE_ALL				= 24,
	COMMAND_TOGGLE_BOXES			= 25,
	COMMAND_PREV_MODEL				= 30,
	COMMAND_NEXT_MODEL				= 31,
	COMMAND_TOGGLE_COMMANDGUI		= 32,
	COMMAND_NEXT_BKGROUND_COLOR		= 35,
	COMMAND_QUIT					= 99
};


//------------------------------------------------------------------------------
// These directly correspond to the AddDevice values in autoexec.cfg
enum EDevices
{
	DEVICE_KEYBOARD			= 1,
	DEVICE_MOUSE			= 2,
	DEVICE_GAMEPAD0			= 3,
	DEVICE_GAMEPAD1			= 4,
	DEVICE_GAMEPAD2			= 5,
	DEVICE_GAMEPAD3			= 6
};



#endif // ifndef __COMMAND_IDS_H__
