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
	COMMAND_MOVE_FORWARD 		= 1,
	COMMAND_MOVE_BACKWARD 		= 2,
	COMMAND_MOVE_LEFT 			= 3,
	COMMAND_MOVE_RIGHT 			= 4,
	COMMAND_MOVE_UP 			= 5,
	COMMAND_MOVE_DOWN 			= 6,
	COMMAND_TURN_LEFT 			= 7,
	COMMAND_TURN_RIGHT 			= 8,
	COMMAND_LOOK_UP 			= 9,
	COMMAND_LOOK_DOWN 			= 10,
	COMMAND_XAXIS_MOVE 			= 11,
	COMMAND_YAXIS_MOVE 			= 12,
	COMMAND_XAXIS_LOOK 			= 13,
	COMMAND_YAXIS_LOOK 			= 14,
	COMMAND_CENTER_VIEW			= 15,
	COMMAND_RESET_POS			= 16,
	COMMAND_PLAY_MUSIC			= 17,
	COMMAND_STOP_MUSIC			= 18,	
	COMMAND_TOGGLE_MUSIC_PAUSE 	= 19,
	COMMAND_CHANGE_INTENSITY	= 20,
	COMMAND_PREV_INTENSITY		= 21,
	COMMAND_NEXT_INTENSITY		= 22,
	COMMAND_RESTARTDM          = 23,
	COMMAND_QUIT				= 250
};


// EGuiCommand -- menu command that is currently selected.
enum EGuiCommand
{
	GUICOMMAND_SAMPLE_NAME = 0,
	GUICOMMAND_SAMPLE_TYPE,
	GUICOMMAND_PRIORITY,
	GUICOMMAND_OUTER_RADIUS,
	GUICOMMAND_INNER_RADIUS,
	GUICOMMAND_VOLUME,
	GUICOMMAND_PITCH_SHIFT,
	GUICOMMAND_POSITION,
	GUICOMMAND_LOOPED,
	GUICOMMAND_PLAY,
	MAX_GUICOMMAND_FILEDS
};

#endif	// __COMMAND_IDS_H__
