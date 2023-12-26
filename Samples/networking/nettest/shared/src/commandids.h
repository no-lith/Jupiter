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
    COMMAND_MOVE_UP                 = 5,
    COMMAND_ENTER                   = 6,
    COMMAND_SHOOT                   = 10,
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


#endif	// __COMMAND_IDS_H__
