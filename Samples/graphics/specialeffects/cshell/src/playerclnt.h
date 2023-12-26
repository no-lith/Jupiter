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
	~CPlayerClnt();

	//void	    SetHObject(HLOCALOBJ hObj);
	void	    SpecialEffectMessage(ILTMessage_Read *pMessage);
    HLOCALOBJ   GetPlayerObject(){ return m_hObject; }
    void        SetClientObject(HLOCALOBJ hObj);
    HLOCALOBJ   GetClientObject(){ return m_hClientObject; }
    void        CreatePlayer();

    void        Update();
    void        UpdateMoveFlags(uint8 flags){ m_dwInputFlags = flags; }
    void        UpdateRotation(float yaw, float pitch, float roll);

    bool        Attack();

private:


    void        UpdateMovement();
    void        RecalculateBoundingBox();
    void        RecalculatePosition();
    void        UpdateAttacking(uint8 nTracker);
    void        PlayMovementAnimation(const char* sAnimName, uint8 nTracker);
    void        PlayAttackAnimation(const char* sAnimName, uint8 nTracker);  

    //This is the actual player representation
	HLOCALOBJ m_hObject;

    //This is the client object
    HLOCALOBJ m_hClientObject;

    uint8 m_dwInputFlags;

    float m_fPitch;
    float m_fYaw;
    float m_fRoll;

    //HMODELANIM m_hCurAnim;

    ANIMTRACKERID m_idUpperBodyTracker;
    ANIMTRACKERID m_idLowerBodyTracker;

    HMODELWEIGHTSET m_hWeightUpper;
    HMODELWEIGHTSET m_hWeightLower;

    bool m_bAttacking;

};


#endif // __PLAYERCLNT_H__

