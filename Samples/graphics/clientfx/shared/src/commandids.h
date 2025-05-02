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

        /*
        COMMAND_FX_ONE                  = 10,
        COMMAND_FX_TWO                  = 11,
        COMMAND_FX_THREE                = 12,
        COMMAND_FX_FOUR                 = 13,
        COMMAND_FX_FIVE                 = 14,
        COMMAND_FX_SIX                  = 15,
        COMMAND_FX_SEVEN                = 16,
        COMMAND_FX_EIGHT                = 17,
        COMMAND_FX_NINE                 = 18,
        COMMAND_FX_TEN                  = 19,
        */
        COMMAND_FX_NEXT                 = 20,
        COMMAND_FX_PREV                 = 21,
        COMMAND_FX_TOGGLEGUI            = 22,
        COMMAND_FX_STOP                 = 23,
                
		COMMAND_QUIT					= 250
};

#endif	// __COMMAND_IDS_H__
