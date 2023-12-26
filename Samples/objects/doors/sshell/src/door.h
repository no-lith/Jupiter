//------------------------------------------------------------------------------//
// 
// MODULE   : Door.h
// 
// PURPOSE  : Door - Definition
// 
// CREATED  : 10/23/2002
// 
// (c) 2002 LithTech, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//

#ifndef __DOOR_H__
#define __DOOR_H__


// Engine includes
#include <ltbasedefs.h>
#include <ltengineobjects.h>
//#include "stdafx.h"

enum DoorMode
{
    DOOR_CLOSED = 0,
    DOOR_OPENING = 1,
    DOOR_CLOSING = 2,
    DOOR_OPEN = 3
};

//-----------------------------------------------------------------------------
class Door : public BaseClass
{
	
public:
	
	// Construction / destruction
    Door()  {
        m_nMode = DOOR_CLOSED;
        m_hackInitialRot.Init( 0.0f, 0.0f, 0.0f, 1.0f );
    }
	virtual ~Door() {}
	
	// EngineMessageFn handlers
	uint32	EngineMessageFn(uint32 messageID, void *pData, float fData);
    uint32  ObjectMessageFn(HOBJECT hSender, ILTMessage_Read *pMsg);
	
private:
	
	uint32		PreCreate(void *pData, float fData);
    uint32      InitialUpdate(void *pData, float fData);
	uint32		Update();
	void		ReadProps(ObjectCreateStruct* pStruct);

    void        Open();
    void        Closed();
    void        Opening();
    void        Closing();

	void		OnMoveDone(int nState, float fAngle, LTRotation &rMode);
	void		OnMove(float fDegrees, int nOnInterrupt);

    uint8       m_nMode;

    LTRotation  m_rClosedRot; //start rot
    LTRotation  m_rOpenRot;
    LTVector    m_vClosedPos; //start pos
    LTVector    m_vOpenPos;

    float       m_fOpenRot;

    //Props
    float       m_fMaxOpenAngle;
    LTVector    m_vHingeOffset;
    float       m_fUpdateRate;
    float       m_fDegreesPerUpdate;

    //Fix worldmodel rotation
    LTRotation m_hackInitialRot;


};


#endif // __Door_H__
