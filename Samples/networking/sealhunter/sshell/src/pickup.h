//------------------------------------------------------------------------------//
//
// MODULE   : PickUp.h
//
// PURPOSE  : PickUp - Definition
//
// CREATED  : 05/14/2002
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#ifndef __PickUp_H__
#define __PickUp_H__


// Engine includes
#include <ltbasedefs.h>
#include <ltengineobjects.h>
#include <ltobjectcreate.h>
#include <iobjectplugin.h>

//-----------------------------------------------------------------------------
enum EPickupType
{
	PT_HEALTH = 0,
	PT_ARMOR,

	PT_NUM_TYPES
};

const char g_aszPickupTypes[PT_NUM_TYPES][16] =
{
	"Health",
	"Armor",
};


//-----------------------------------------------------------------------------
class PickUp : public BaseClass
{

public:

    PickUp();

    ~PickUp()
	{
	}

    // EngineMessageFn handlers
    uint32		EngineMessageFn(uint32 messageID, void *pData, float fData);

private:

    uint32		PreCreate(void *pData, float fData);
    uint32      Update();
    uint32      TouchNotify(void *pData, float fData);
    void		ReadProps(ObjectCreateStruct* pStruct);
    void        PlaySound();

private:

    uint8       m_nPickupType;
    float       m_fPickupValue;
};




class CPickupPlugin : public IObjectPlugin
{
public:

	virtual LTRESULT	PreHook_EditStringList(const char* szRezPath, const char* szPropName, char** aszStrings,
											   uint32* pcStrings, const uint32 cMaxStrings, const uint32 cMaxStringLength);
};



#endif // __PickUp_H__
