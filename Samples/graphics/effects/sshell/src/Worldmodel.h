//------------------------------------------------------------------------------//
// 
// MODULE   : Worldmodel.h
// 
// PURPOSE  : Worldmodel - Definition
// 
// CREATED  : 07/24/2003
// 
// (c) 2003 LithTech, Inc.  All Rights Reserved
// 
//------------------------------------------------------------------------------//

#ifndef __Worldmodel_H__
#define __Worldmodel_H__


// Engine includes
#include <ltbasedefs.h>
#include <ltengineobjects.h>
#include <ltobjectcreate.h>

//-----------------------------------------------------------------------------
class Worldmodel : public BaseClass
{
    
public:
    
    // Construction / destruction
    Worldmodel()  {}
    virtual ~Worldmodel() {}
    
    // EngineMessageFn handlers
    uint32	EngineMessageFn(uint32 messageID, void *pData, float fData);
    
private:
    
    uint32		PreCreate(void *pData, float fData);
    uint32      AllObjectsCreated(void *pData, float fData);
    void		ReadProps(ObjectCreateStruct* pStruct);


};


#endif // __Worldmodel_H__
