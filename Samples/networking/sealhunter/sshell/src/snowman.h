//------------------------------------------------------------------------------//
//
// MODULE   : Snowman.h
//
// PURPOSE  : Snowman - Definition
//
// CREATED  : 07/25/2003
//
// (c) 2003 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#ifndef __Snowman_H__
#define __Snowman_H__


// Engine includes
#include <ltbasedefs.h>
#include <ltengineobjects.h>
#include <ltobjectcreate.h>
#include <iobjectplugin.h>

enum ESnowmanState
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
class Snowman : public BaseClass
{

public:

    Snowman();

	~Snowman()
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
    void        SendKillChatMessage(HOBJECT hAssassin);
	void		Die();

private:

    uint8       m_nSnowmanType;
    float       m_fSnowmanValue;
    int8        m_iHitPoints;

    HMODELANIM  m_hAnim;
    bool        m_bAnimInterable;
    HOBJECT     m_hAssasin;
    HOBJECT     m_hFirstAttacker;
    float       m_fFirstAttackerDmg;

    uint8       m_iAction;
	float		m_fTimeToDie;		// time until object is to be removed
};

#endif // __Snowman_H__
