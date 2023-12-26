//------------------------------------------------------------------------------//
// 
// MODULE	: Door.cpp
// 
// PURPOSE	: Door - Implementation
// 
// CREATED	: 10/23/2002
// 
// (c) 2002 LithTech, Inc.	All Rights Reserved
// 
//------------------------------------------------------------------------------//

#include "Door.h"

#include <ltobjectcreate.h>
#include <iltmessage.h>
#include <iltcommon.h>
#include <iltphysics.h>
#include "msgids.h"
#include "serverinterfaces.h"

//-----------------------------------------------------------------------------
BEGIN_CLASS(Door)
ADD_REALPROP(MaxOpenAngle, 90.0f)
ADD_VECTORPROP(HingeOffset)
ADD_REALPROP(UpdateRate, 0.0333f)
ADD_REALPROP(DegreesPerUpdate, 2.5f)
END_CLASS_DEFAULT_FLAGS(Door, BaseClass, LTNULL, LTNULL, CF_ALWAYSLOAD | CF_WORLDMODEL)


//-----------------------------------------------------------------------------
//	Door::EngineMessageFn(uint32 messageID, void *pData, float fData)
//
//-----------------------------------------------------------------------------  
uint32 Door::EngineMessageFn(uint32 messageID, void *pData, float fData)
{
    
    switch (messageID)
    {
    case MID_PRECREATE:
        return PreCreate(pData, fData);
    case MID_INITIALUPDATE:
        return InitialUpdate(pData, fData);
    case MID_TOUCHNOTIFY:
        {
			//If the door is touched, start to open
            g_pLTServer->CPrint("(Door) TOUCH");
            m_nMode = DOOR_OPENING;
            g_pLTServer->SetNextUpdate(m_hObject, m_fUpdateRate);
        }
        break;
    case MID_UPDATE:
        return Update();
    default:
        break;
    }
    
    // Pass the message along to parent class.
    return BaseClass::EngineMessageFn(messageID, pData, fData);
}

//-----------------------------------------------------------------------------
//	Door::ObjectMessageFn(HOBJECT hSender, ILTMessage_Read *pMsg)
//
//-----------------------------------------------------------------------------
uint32  Door::ObjectMessageFn(HOBJECT hSender, ILTMessage_Read *pMsg)
{
    uint8 messageID = pMsg->Readuint8();
    
    switch(messageID)
    {
    case MSG_OBJ_PINGOBJECT:
        {
            g_pLTServer->CPrint("(Door) PING");            
            m_nMode = DOOR_OPENING;          
            g_pLTServer->SetNextUpdate(m_hObject, m_fUpdateRate);
            
        }
        break;
        
    default:
        break;
    }
    
    return 1;
}


//-----------------------------------------------------------------------------
//	Door::PreCreate(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Door::PreCreate(void *pData, float fData)
{
    // Let parent class handle it first
    BaseClass::EngineMessageFn(MID_PRECREATE, pData, fData);
    
    // Cast pData to a ObjectCreateStruct* for convenience
    ObjectCreateStruct* pStruct = (ObjectCreateStruct*)pData;
    
    // Set the object type to OT_NORMAL
    pStruct->m_ObjectType = OT_WORLDMODEL;
    
    pStruct->m_Flags |= (FLAG_SOLID 
					   | FLAG_VISIBLE 
					   | FLAG_GOTHRUWORLD 
					   | FLAG_TOUCH_NOTIFY 
                       | FLAG_FULLPOSITIONRES);

    pStruct->m_Flags2 |= FLAG2_DISABLEPREDICTION;
    
    // Check to see if this is coming from a world file
    if(fData == PRECREATE_WORLDFILE)	  
    {
        m_hackInitialRot = pStruct->m_Rotation;
        pStruct->m_Rotation.Identity();
        ReadProps(pStruct);
    }
    
    // Return default of 1
    return 1;
}


//-----------------------------------------------------------------------------
//	Door::IntialUpdate(void *pData, float fData)
//
//-----------------------------------------------------------------------------
uint32 Door::InitialUpdate(void *pData, float fData)
{
    // Get the starting rotation
    g_pLTServer->GetObjectRotation(m_hObject, &m_rClosedRot);
  
	// Get the starting position
    g_pLTServer->GetObjectPos(m_hObject, &m_vClosedPos);	

    // Calculate rotation of the door, fully open.
    m_rOpenRot = m_rClosedRot;
    float fAngle = DegreesToRadians(m_fMaxOpenAngle);
    m_rOpenRot.Rotate(m_rOpenRot.Up(), fAngle);  

	// Pre-set initial open angle offset, in degrees.
	m_fOpenRot = m_fMaxOpenAngle;

	// Don't ignore small collisions. Listen for ALL collisions.
    g_pLTSPhysics->SetForceIgnoreLimit(m_hObject, 0.0f);

    g_pLTServer->SetNextUpdate(m_hObject, m_fUpdateRate);	

    return 1;
}


//-----------------------------------------------------------------------------
//	Door::Update()
//
//-----------------------------------------------------------------------------
uint32 Door::Update()
{
    
    switch(m_nMode)
    {
    case DOOR_OPENING:
        {
            m_fOpenRot -= m_fDegreesPerUpdate;
            if((m_fOpenRot - m_fDegreesPerUpdate) < 0.0f)
            {
				// Place door in fully opened position
				OnMoveDone(DOOR_OPEN, 0.0f, m_rOpenRot);
                break;
            }
            
			// Continue opening door. If the door is touched during this, 
			// then close the door
			OnMove(m_fDegreesPerUpdate, DOOR_CLOSING);
        }
        break;
    case DOOR_CLOSING:
        {            
            m_fOpenRot += m_fDegreesPerUpdate;
            if((m_fOpenRot + m_fDegreesPerUpdate) > m_fMaxOpenAngle)
            {
				// Place door in fully closed position
				OnMoveDone(DOOR_CLOSED, m_fMaxOpenAngle, m_rClosedRot);
                break;
            }         
            
			// Continue closing door. If the door is touched during this, 
			// then open the door
			OnMove(-m_fDegreesPerUpdate, DOOR_OPENING);
        }
        break;
    case DOOR_OPEN:
        {
            m_nMode = DOOR_CLOSING; 
            g_pLTServer->SetNextUpdate(m_hObject, m_fUpdateRate);
        }
        break;
    default:
        g_pLTServer->SetNextUpdate(m_hObject, 0.0f);
        break;
    }
    
    
    return 1;
}


//-----------------------------------------------------------------------------
//	Door::OnMoveDone
//
//-----------------------------------------------------------------------------
void Door::OnMoveDone(int nState, float fAngle, LTRotation &rMode)
{
    m_nMode = nState;
    m_fOpenRot = fAngle;
    LTRotation rRot;
    LTVector vPos;
    g_pLTServer->GetObjectRotation(m_hObject, &rRot);
    
	// Calculate our final position
	LTMatrix mRotation;	
	rRot = m_hackInitialRot * rMode * ~m_hackInitialRot;
	rRot.ConvertToMatrix( mRotation );
	vPos = (mRotation * m_vHingeOffset) + (m_vClosedPos - m_vHingeOffset);
    
	// Set our position and rotation
    g_pLTServer->SetObjectPos(m_hObject, &vPos);   
    g_pLTServer->SetObjectRotation(m_hObject, &rMode);                
    g_pLTServer->SetNextUpdate(m_hObject, 1.0f);    
}

//-----------------------------------------------------------------------------
//	Door::OnMove()
//
//-----------------------------------------------------------------------------
void Door::OnMove(float fDegrees, int nOnInterrupt)
{
	// Get the current rotation
    LTRotation rRot;
    g_pLTServer->GetObjectRotation(m_hObject, &rRot);
    
	// Set our new rotation
    rRot.Rotate(rRot.Up(), DegreesToRadians(fDegrees));
    g_pLTServer->RotateObject(m_hObject, &rRot);
    
	// Get our current possition
    LTVector vPos;
    g_pLTServer->GetObjectPos(m_hObject, &vPos);

	// Save our current position.
	LTVector vOldPos;	
	vOldPos = vPos;

	// Calculate the position to move to.
	LTMatrix	mRotation;	
	rRot = m_hackInitialRot * rRot * ~m_hackInitialRot;
	rRot.ConvertToMatrix( mRotation );
	vPos = (mRotation * m_vHingeOffset) + (m_vClosedPos - m_vHingeOffset);
  
	// Try to move the object
    g_pLTServer->MoveObject(m_hObject, &vPos);
    
	// Check to see if anything is in our way.
	// If it is, set the mode opposite of the current mode,
	// and reset the position.
	LTVector	vCurPos;
	g_pLTServer->GetObjectPos( m_hObject, &vCurPos );
	if( !vCurPos.NearlyEquals( vPos, MATH_EPSILON ) )
	{
            m_nMode = nOnInterrupt; 
			g_pLTServer->SetObjectPos(m_hObject, &vOldPos);
    }

    g_pLTServer->SetNextUpdate(m_hObject, m_fUpdateRate);
}


//-----------------------------------------------------------------------------
//	Door::ReadProps(ObjectCreateStruct* pStruct)
//
//-----------------------------------------------------------------------------
void Door::ReadProps(ObjectCreateStruct* pStruct)
{
    g_pLTServer->GetPropString("Name", pStruct->m_Filename, MAX_CS_FILENAME_LEN);
    g_pLTServer->GetPropReal("MaxOpenAngle", &m_fMaxOpenAngle);
    g_pLTServer->GetPropVector("HingeOffset", &m_vHingeOffset);
    g_pLTServer->GetPropReal("UpdateRate", &m_fUpdateRate);    
    g_pLTServer->GetPropReal("DegreesPerUpdate", &m_fDegreesPerUpdate);
}

