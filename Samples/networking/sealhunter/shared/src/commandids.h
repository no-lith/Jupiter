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
    COMMAND_SHOOT                   = 15,
	COMMAND_JUMP					= 16,
    COMMAND_SHOWSTATS               = 17,
    COMMAND_ENTER                   = 18,
    COMMAND_CHAT                    = 19,
    COMMAND_DEBUG_1                 = 61,
       
    COMMAND_QUIT					= 250
};

//------------------------------------------------------------------------------
// 

#define  MOVE_FORWARD    (1<<0)
#define  MOVE_BACKWARD   (1<<1)
#define  MOVE_RIGHT      (1<<2)
#define  MOVE_LEFT       (1<<3)
#define  MOVE_UP         (1<<4)
#define  MOVE_JUMP		 (1<<5)

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

#endif	// __COMMAND_IDS_H__
