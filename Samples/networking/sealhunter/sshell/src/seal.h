//------------------------------------------------------------------------------//
//
// MODULE   : Seal.h
//
// PURPOSE  : Seal - Definition
//
// CREATED  : 05/14/2002
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#ifndef __Seal_H__
#define __Seal_H__


// Engine includes
#include <ltbasedefs.h>
#include <ltengineobjects.h>
#include <ltobjectcreate.h>
#include <iobjectplugin.h>

enum ESealState
{
  ACTION_SPAWNED, // just spawned
  ACTION_WARPING, // seals fall from the sky, don't they?
  ACTION_NORMAL,  // idle
  ACTION_WANDER,  // wander mode
  ACTION_FLEE,    // run away
  ACTION_HIT,     // ouch
  ACTION_DYING,   // in the process of being dead
  ACTION_DEAD,    // dead
};

//-----------------------------------------------------------------------------
class Seal : public BaseClass
{

public:

    Seal();

	~Seal()
	{
	}

    // EngineMessageFn handlers
    uint32	    EngineMessageFn(uint32 messageID, void *pData, float fData);
    uint32      ObjectMessageFn(HOBJECT hSender, ILTMessage_Read *pMsg);

private:

    uint32		PreCreate(void *pData, float fData);
    uint32      Update();
    uint32      TouchNotify(void *pData, float fData);
    void		ReadProps(ObjectCreateStruct* pStruct);
    void        PlaySound();
    void        ChangeAnimation(const char* sAnimName, bool bLooping, bool bInterable);

	void 		GetPosRot(LTVector *pPos, LTRotation *pRot);
	bool 		RayCastToGround(const LTVector &Pos, const LTRotation &Rot, LTVector *pIntersectPt);

    void        SendKillStealChatMessage(HOBJECT hFirst, HOBJECT hKiller);

	void		Spawn();
	void		Warp();
	void 		StartMoving(float Speed);
	void		StopMoving();
	void		FlopAround(int &Counter, float Speed);
	void		Die();

private:

    uint8       m_nSealType;
    float       m_fSealValue;
    int8        m_iHitPoints;

    HMODELANIM  m_hAnim;
    bool        m_bAnimInterable;
    HOBJECT     m_hAssasin;
    HOBJECT     m_hFirstAttacker;
    float       m_fFirstAttackerDmg;

    uint8       m_iAction;
	LTVector	m_LastPos;
	int			m_HitTime;			// counter for fleeing
	int			m_WanderTime;		// counter for wandering
	int			m_NextWanderTime;	// counter for next wander
	float		m_TurnFactor;
	float		m_fTimeToDie;		// time until object is to be removed

	float		m_fYaw;
	float		m_fPitch;
	float		m_fRoll;

    //
    float       m_fDistanceTraveled;
    LTVector    m_vLastPos;
	float 		m_FallingDist;
};

#endif // __Seal_H__
