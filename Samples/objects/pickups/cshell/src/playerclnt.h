//------------------------------------------------------------------------------//
// 
// MODULE   : playerclnt.h
// 
// PURPOSE  : CPlayerClnt - Definition
// 
// CREATED  : 07/15/2002
// 
// (c) 2002 LithTech, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//


#ifndef __PLAYERCLNT_H__
#define __PLAYERCLNT_H__

#include "ltclientshell.h"

// Engine includes
#include <ltbasedefs.h>


//-----------------------------------------------------------------------------
class CPlayerClnt 
{

public:

	// Construction / destruction
	CPlayerClnt();
	~CPlayerClnt() {}

	//void	    SetHObject(HLOCALOBJ hObj);
	void	    SpecialEffectMessage(ILTMessage_Read *pMessage);
    HLOCALOBJ   GetPlayerObject(){ return m_hObject; }
    void        SetClientObject(HLOCALOBJ hObj);
    HLOCALOBJ   GetClientObject(){ return m_hClientObject; }
    void        CreatePlayer();

    void        UpdateMoveFlags(uint8 flags){ m_dwInputFlags = flags; }
    void        UpdateRotation(float yaw, float pitch, float roll);

    void        Update();
    void        UpdateMovement();
    void        RecalculatePosition();

private:

    //This is the actual player representation
	HLOCALOBJ m_hObject;

    //This is the client object
    HLOCALOBJ m_hClientObject;

    uint8 m_dwInputFlags;

    float m_fPitch;
    float m_fYaw;
    float m_fRoll;
};


#endif // __PLAYERCLNT_H__
