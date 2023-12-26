//------------------------------------------------------------------------------//
//
// MODULE	: GameBase.h
// 
// PURPOSE	: Base of Object - Implementation
// 
// CREATED	: 03/19/2004
//
//------------------------------------------------------------------------------//

#include "GameBase.h"
#include <ltobjectcreate.h>
#include <iltcommon.h>

BEGIN_CLASS(GameBase)
END_CLASS_DEFAULT_FLAGS(GameBase, BaseClass, NULL, NULL, CF_HIDDEN)



GameBase::GameBase(uint8 nType) : BaseClass(nType), m_dwOriginalFlags(0), m_bFirstUpdate(true), m_dUpdateCount(0)
{
}

GameBase::~GameBase()
{
}


uint32 GameBase::EngineMessageFn(uint32 messageID, void *pData, float fData)
{

	switch (messageID)
	{
		case MID_PRECREATE:
		break;

		case MID_UPDATE:
		{
			GameBase::Update();
		}
		break;

		default:
		break;
	}

	// Pass the message along to parent class.
	return BaseClass::EngineMessageFn(messageID, pData, fData);
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GameBase::ObjectMessageFn
//
//	PURPOSE:	Handle object-to-object messages
//
// ----------------------------------------------------------------------- //

uint32 GameBase::ObjectMessageFn(HOBJECT hSender, ILTMessage_Read *pMsg)
{
    if (!g_pLTServer) return 0;

	return BaseClass::ObjectMessageFn(hSender, pMsg);
}


/////////////////////////////////////////////////////////////////////////////
void GameBase::Update()
{
	if(m_bFirstUpdate)
	{
		m_bFirstUpdate = false;
	}

	++m_dUpdateCount;
}


void GameBase::SetNextUpdate(HOBJECT hObj, LTFLOAT fDelta)
{
	if (!hObj) return;

	fDelta = fDelta <= 0.0f ? 0.0f : fDelta;
    g_pLTServer->SetNextUpdate(hObj, fDelta);

	if (fDelta == 0.0f)
	{
		g_pLTServer->SetObjectState(hObj, OBJSTATE_INACTIVE);
	}
	else
	{
		g_pLTServer->SetObjectState(hObj, OBJSTATE_ACTIVE);
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	GameBase::TriggerMsg()
//
//	PURPOSE:	Route trigger messages through the standard processing method
//
// --------------------------------------------------------------------------- //

void GameBase::TriggerMsg(HOBJECT hSender, const char* szMsg)
{
	if (!szMsg) return;

	// ConParse does not destroy szMsg, so this is safe
	ConParse parse;
	parse.Init((char*)szMsg);

	while (g_pLTSCommon->Parse(&parse) == LT_OK)
	{
		// Don't parse empty messages
		if (!parse.m_nArgs || !parse.m_Args[0])
			continue;

		CParsedMsg cCurMsg(parse.m_nArgs, parse.m_Args);
		GameBase::OnTrigger(hSender, cCurMsg);
	}
}

// --------------------------------------------------------------------------- //
//
//	ROUTINE:	GameBase::OnTrigger()
//
//	PURPOSE:	Process GameBase trigger messages 
//
// --------------------------------------------------------------------------- //

bool GameBase::OnTrigger(HOBJECT hSender, const CParsedMsg &cMsg)
{
	static CParsedMsg::CToken s_cTok_1("1");
	static CParsedMsg::CToken s_cTok_True("TRUE");
	static CParsedMsg::CToken s_cTok_0("0");
	static CParsedMsg::CToken s_cTok_False("FALSE");
	static CParsedMsg::CToken s_cTok_Visible("VISIBLE");
	static CParsedMsg::CToken s_cTok_Solid("SOLID");
	static CParsedMsg::CToken s_cTok_Hidden("HIDDEN");
	static CParsedMsg::CToken s_cTok_Remove("REMOVE");

    uint32 dwFlags;
	g_pLTSCommon->GetObjectFlags(m_hObject, OFT_Flags, dwFlags);
	if (!m_dwOriginalFlags)
	{
		m_dwOriginalFlags = dwFlags;
	}

	if (cMsg.GetArg(0) == s_cTok_Visible)
	{
		if (cMsg.GetArgCount() > 1)
		{
			if ((cMsg.GetArg(1) == s_cTok_1) ||
				(cMsg.GetArg(1) == s_cTok_True))
			{
				dwFlags |= FLAG_VISIBLE;
			}
			else
			{
				if ((cMsg.GetArg(1) == s_cTok_0) ||
					(cMsg.GetArg(1) == s_cTok_False))
				{
					dwFlags &= ~FLAG_VISIBLE;
				}
			}

			g_pLTSCommon->SetObjectFlags(m_hObject, OFT_Flags, dwFlags, FLAGMASK_ALL);
		}
	}
	else if (cMsg.GetArg(0) == s_cTok_Solid)
	{
		if (cMsg.GetArgCount() > 1)
		{
			if ((cMsg.GetArg(1) == s_cTok_1) ||
				(cMsg.GetArg(1) == s_cTok_True))
			{
				dwFlags |= FLAG_SOLID;

				if (m_dwOriginalFlags & FLAG_RAYHIT)
				{
					dwFlags |= FLAG_RAYHIT;
				}
			}
			else
			{
				if ((cMsg.GetArg(1) == s_cTok_0) ||
					(cMsg.GetArg(1) == s_cTok_False))
				{
					dwFlags &= ~FLAG_SOLID;
					dwFlags &= ~FLAG_RAYHIT;
				}
			}

			g_pLTSCommon->SetObjectFlags(m_hObject, OFT_Flags, dwFlags, FLAGMASK_ALL);
		}
	}
	else if (cMsg.GetArg(0) == s_cTok_Hidden)
	{
		if (cMsg.GetArgCount() > 1)
		{
			if ((cMsg.GetArg(1) == s_cTok_1) ||
				(cMsg.GetArg(1) == s_cTok_True))
			{
				dwFlags &= ~FLAG_SOLID;
				dwFlags &= ~FLAG_RAYHIT;
				dwFlags &= ~FLAG_VISIBLE;
			}
			else
			{
				if ((cMsg.GetArg(1) == s_cTok_0) ||
					(cMsg.GetArg(1) == s_cTok_False))
				{
					// Not all objects should have solid set to true.
					// (e.g. AIs should never set solid true)

					if( m_dwOriginalFlags & FLAG_SOLID )
					{
						dwFlags |= FLAG_SOLID;
					}

					if( m_dwOriginalFlags & FLAG_VISIBLE )
					{
						dwFlags |= FLAG_VISIBLE;
					}

					if (m_dwOriginalFlags & FLAG_RAYHIT)
					{
						dwFlags |= FLAG_RAYHIT;
					}
				}
			}

			g_pLTSCommon->SetObjectFlags(m_hObject, OFT_Flags, dwFlags, FLAGMASK_ALL);
		}
	}
	else if ( cMsg.GetArg(0) == s_cTok_Remove )
	{
		g_pLTServer->RemoveObject( m_hObject );
	}
	else
	{
		return false;
	}

	return true;
}