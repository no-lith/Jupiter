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

	CPlayerClnt();
	~CPlayerClnt();

	void	    		SpecialEffectMessage(ILTMessage_Read *pMessage);
    HLOCALOBJ   		GetPlayerObject()										{ return m_hObject; }
    void        		SetClientObject(HLOCALOBJ hObj);
    HLOCALOBJ   		GetClientObject()										{ return m_hClientObject; }
    void        		CreatePlayer();

    void        		Update();
    void        		UpdateMoveFlags(uint8 flags)							{ m_dwInputFlags |= flags; }
    void        		UpdateRotation(float yaw, float pitch, float roll);

	bool        		Attack();
    void        		SetClub(HOBJECT hObj);
    void        		SetScore(uint32 iScore) 								{ m_iScore = iScore; }
    uint32      		GetScore() 												{ return m_iScore; }
    void        		SetMoney(float fMoney) 									{ m_fMoney = fMoney; }
    float       		GetMoney() 												{ return m_fMoney; }

private:

    void        		UpdateMovement();
	void				UpdateJump();
    void        		RecalculateBoundingBox();
    void        		RecalculatePosition();
    void        		UpdateAttacking(uint8 nTracker);
    void        		PlayMovementAnimation(const char* sAnimName, uint8 nTracker);
    void        		PlayAttackAnimation(const char* sAnimName, uint8 nTracker);

	void 				ShowAttachClientFX();
	void 				HideAttachClientFX();
	void 				CreateAttachClientFX();
	void 				RemoveAttachClientFX();

private:

	HLOCALOBJ       	m_hObject; //This is the actual player representation
    HLOCALOBJ       	m_hClientObject; //This is the client object

    uint8           	m_dwInputFlags;

    float           	m_fPitch;
    float           	m_fYaw;
    float           	m_fRoll;

    ANIMTRACKERID   	m_idUpperBodyTracker;
    ANIMTRACKERID   	m_idLowerBodyTracker;

    HMODELWEIGHTSET 	m_hWeightUpper;
    HMODELWEIGHTSET 	m_hWeightLower;

    bool            	m_bAttacking;

    HOBJECT         	m_hClubObject;

    uint32          	m_iScore;
    float           	m_fMoney;

	//Jump vars
	bool				m_bIsJumping;
	float				m_fCurrentJumpRadians;
	float				m_fJumpTimeRemaining;

	HOBJECT				m_DebugSphere;
	ModelOBB			m_WeaponOBB;

    //
    bool                m_bUseLeashing;
    float               m_fLeashingDelay;

	CLIENTFX_LINK_NODE	m_AttachClientFX;
};


#endif // __PLAYERCLNT_H__

