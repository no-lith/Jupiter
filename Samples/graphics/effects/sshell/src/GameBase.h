//------------------------------------------------------------------------------//
// 
// MODULE	: GameBase.h
// 
// PURPOSE	: Base of Object - Definition
// 
// CREATED	: 03/19/2004
// 
//------------------------------------------------------------------------------//

#ifndef __GAME_BASE_H__
#define __GAME_BASE_H__

#define DECLARE_CLASS( className, baseClassName ) \
	typedef baseClassName SuperClass; \
	typedef className ThisClass;

#define	UPDATE_NEXT_FRAME			(0.001f)
#define UPDATE_NEVER				(0.0f)

#include <ltbasedefs.h>
#include <ltengineobjects.h>
#include <ltobjectcreate.h>
#include <iobjectplugin.h>
#include "serverinterfaces.h"
#include "ParsedMsg.h"
#include "MsgIDs.h"


class GameBase : public BaseClass
{

public:

	GameBase(uint8 nType = OT_NORMAL);
	virtual ~GameBase();

    uint32 ObjectMessageFn(HOBJECT hSender, ILTMessage_Read *pMsg);

protected:

	uint32 EngineMessageFn(uint32 messageID, void *pData, float fData);

	void SetNextUpdate(HOBJECT hobj, LTFLOAT fDelta);

	virtual bool OnTrigger(HOBJECT hSender, const CParsedMsg &cMsg);

	void Update();

	bool IsFirstUpdate() { return m_bFirstUpdate; }

    uint32 m_dwOriginalFlags;

private:

	void TriggerMsg(HOBJECT hSender, const char* pMsg);
	bool m_bFirstUpdate;
	uint32 m_dUpdateCount;

};

#endif // __GAME_BASE_H__