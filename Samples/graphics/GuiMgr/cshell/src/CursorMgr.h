// ----------------------------------------------------------------------- //
//
// MODULE  : Cursormgr.h
//
// PURPOSE : Cursor manager 
//
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
// ----------------------------------------------------------------------- //


#if !defined(_CURSORMGR_H_)
#define _CURSORMGR_H_

#include <iclientshell.h>
#include "iltcursor.h"
#include "ilttexinterface.h"
#include "iltdrawprim.h"


class CCursorMgr;
extern CCursorMgr* g_pCursorMgr;

class CCursorMgr
{

public:

	CCursorMgr();
	~CCursorMgr();

   LTBOOL			Init();
	LTBOOL			Term();
	LTBOOL			Render();


	LTIntPt GetCursorCenter () { return m_CursorCenter; }

	void		UseHardwareCursor(LTBOOL bUseHardwareCursor, bool bForce = false);
   void		UseCursor(LTBOOL bUseCursor, LTBOOL bLockCursorToCenter = LTFALSE);

	void		SetCursorPos ( int x, int y ) { m_CursorPos.x=x; m_CursorPos.y=y; }
	LTIntPt  GetCursorPos() { return  m_CursorPos; }
protected:

	LTBOOL		m_bInitialized;
	LTBOOL      m_bUseCursor;
	LTBOOL      m_bUseHardwareCursor;

	LTIntPt		m_CursorCenter;
	LTIntPt		m_CursorPos;

	// windows hardware cursor
	HLTCURSOR	m_hCursor;

	HTEXTURE		m_hCursorTex;

	LT_POLYGT4	m_Poly;

	float			m_fWidth;
	float			m_fHeight;

};

#endif // _CURSORMGR_H_