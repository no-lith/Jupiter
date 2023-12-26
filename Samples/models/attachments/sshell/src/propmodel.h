//------------------------------------------------------------------------------//
// 
// MODULE   : Propmodel.h
// 
// PURPOSE  : Propmodel - Definition
// 
// CREATED  : 05/14/2002
// 
// (c) 2002 LithTech, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//

#ifndef __Propmodel_H__
#define __Propmodel_H__


// Engine includes
#include <ltbasedefs.h>
#include <ltengineobjects.h>
#include <ltobjectcreate.h>


//-----------------------------------------------------------------------------
class Propmodel : public BaseClass
{
    
public:
    
    // Construction / destruction
    Propmodel();
    ~Propmodel() {}
    
    // EngineMessageFn handlers
    uint32	EngineMessageFn(uint32 messageID, void *pData, float fData);
    
private:
    
    uint32		PreCreate(void *pData, float fData);
    uint32      InitialUpdate();
    uint32      Update();
    void		ReadProps(ObjectCreateStruct* pStruct);

    void        CreateAttachmentObject(HOBJECT &hObj, 
                                       const char* sFilename, 
                                       const char* sTexture);

    void        CreateAttachment(HATTACHMENT &hAttachment, 
                                 HOBJECT hChildObject, 
                                 const char* sSocket, 
                                 LTVector &vRotOffset, 
                                 LTVector &vPosOffset);

    void        PlayAnimation(const char* sAnimName);
    void        RecalculateBoundingBox();

    HOBJECT     m_hCrossbow;
    HOBJECT     m_hBillyclub;    

    HATTACHMENT m_hAttachment;

    uint8       m_nAnimStep;


};

#endif // __Propmodel_H__
