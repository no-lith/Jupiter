#include "serverutilities.h"
#include "AutoMessage.h"
#include "iltserver.h"
#include "FxDefs.h"


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	PlayClientFX
//
//	PURPOSE:	Plays a fire-and-forget clientFX at a specific location 
//				in the world.
//
//	NOTE:		(pvPos is ignored if hParent is passed in)
//
//				Since these are fire & forget, dwFlags should never
//				include looping
// ----------------------------------------------------------------------- //
void PlayClientFX(const char* szFXName, HOBJECT hParent, LTVector* pvPos, LTRotation *prRot, uint32 dwFlags)
{
	PlayClientFX( szFXName, hParent, LTNULL, pvPos, prRot, LTNULL, dwFlags );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	PlayClientFX
//
//	PURPOSE:	Plays a fire-and-forget clientFX at a specific location 
//				in the world using data from a target object.
//
//	NOTE:		(pvPos is ignored if hParent is passed in)
//
//				Since these are fire & forget, dwFlags should never
//				include looping
// ----------------------------------------------------------------------- //
void PlayClientFX(const char* szFXName, HOBJECT hParent, HOBJECT hTarget, LTVector* pvPos, LTRotation *prRot, LTVector *pvTargetPos, uint32 dwFlags)
{
	LTVector vPos;
	LTRotation rRot;
	if(hParent)
	{
		g_pLTServer->GetObjectPos(hParent, &vPos);
		g_pLTServer->GetObjectRotation(hParent, &rRot);
	}
	else if( !pvPos || !prRot )
	{
			// [KLS 05/19/02] Don't do the fx if the position is bogus...
		g_pLTServer->CPrint("ERROR in PlayClientFX()!  Invalid position specified for FX: %s!", szFXName);
		return;
	}
	else
	{
		vPos = *pvPos;
		rRot = *prRot;
	}
	

	CAutoMessage cMsg;
	cMsg.Writeuint8(SFX_CLIENTFXGROUPINSTANT);
	cMsg.WriteString(szFXName);
	cMsg.Writeuint32(dwFlags);
	cMsg.WriteCompPos(vPos);
	cMsg.WriteCompLTRotation(rRot);
	cMsg.WriteObject(hParent);

	// Write the target information if we have any...
	
	if( hTarget || pvTargetPos )
	{
		cMsg.Writeuint8( true );
		cMsg.WriteObject( hTarget );

		LTVector vTargetPos;
		if( hTarget )
		{
			g_pLTServer->GetObjectPos( hTarget, &vTargetPos );
		}
		else
		{
			vTargetPos = *pvTargetPos;
		}

		cMsg.WriteCompPos( vTargetPos );
	}
	else
	{
		cMsg.Writeuint8( false );
	}

	g_pLTServer->SendSFXMessage(cMsg.Read(), vPos, 0);
}
