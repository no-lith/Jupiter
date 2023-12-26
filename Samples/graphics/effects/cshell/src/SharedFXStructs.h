// ----------------------------------------------------------------------- //
//
// MODULE  : SharedFXStructs.h
//
// PURPOSE : Shared Special FX structs
//
// CREATED : 10/21/99
//
// (c) 1999-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __SHARED_FX_STRUCTS_H__
#define __SHARED_FX_STRUCTS_H__


#include <iltmessage.h>

// Base SpecialFX struct (used by client-side SpecialFX.h, and by
// derived special fx structs (see SharedFXStructs.h))

struct SFXCREATESTRUCT
{
	SFXCREATESTRUCT();
	virtual ~SFXCREATESTRUCT() {}

	HOBJECT	hServerObj;

    virtual void Write(ILTMessage_Write *pMsg) {}
    virtual void Read(ILTMessage_Read *pMsg) {}

	// Server-side only...
#ifndef _CLIENTBUILD
    virtual void Load(ILTMessage_Read *pMsg)   { Read(pMsg); }
    virtual void Save(ILTMessage_Write *pMsg) { Write(pMsg); }
	virtual	void ReadProps() {}
#endif
};

inline SFXCREATESTRUCT::SFXCREATESTRUCT()
{
    hServerObj = LTNULL;
}


#endif  // __SHARED_FX_STRUCTS_H__