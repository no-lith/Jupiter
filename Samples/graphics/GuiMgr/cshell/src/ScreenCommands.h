// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenCommands.h
//
// PURPOSE : Enumerate interface screen commands
//
// (c) 1999-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef _SCREENCOMMANDS_H
#define _SCREENCOMMANDS_H

///////////////////////////////////////
// The command IDs for the screen options

enum eScreenCmds
{
	CMD_NONE,

//	Reusable
	CMD_BACK,
	CMD_CONTINUE,
	CMD_LEFT,
	CMD_RIGHT,
	CMD_MAIN,
	CMD_OK,
	CMD_CANCEL,
	CMD_UPDATE,

	CMD_RESUME,
	CMD_EXIT, 
	CMD_QUIT,

//  from Main menu
	CMD_ONE,
	CMD_TWO,

// from one

	CMD_THREE,

	CMD_CUSTOM, //this needs to be the last cmd, so that custom commands may be defined after it
};



#endif // SCREENCOMMANDS_H
