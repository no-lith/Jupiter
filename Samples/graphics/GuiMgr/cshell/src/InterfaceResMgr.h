// ----------------------------------------------------------------------- //
//
// MODULE  : InterfaceResMgr.h
//
// PURPOSE : Manager for resources associated with the interface
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
// ----------------------------------------------------------------------- //

#if !defined(_INTERFACERESMGR_H_)
#define _INTERFACERESMGR_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LTGUIMgr.h"
#include "ilttexturestring.h"
#include <vector>
#include <set>

class CInterfaceResMgr;
extern CInterfaceResMgr* g_pInterfaceResMgr;

typedef std::vector<CFontInfo*> FontArray;
typedef std::set<HTEXTURE> TextureSet;

const int kMaxSoundFilename = 64;

class CInterfaceResMgr
{
public:
	CInterfaceResMgr();
	virtual ~CInterfaceResMgr();

	LTBOOL			Init();
	void				Term();

	HTEXTURE			GetTexture(const char *szTexName);

	CFontInfo *		GetFont(uint32 nIndex);

	void				ScreenDimsChanged();

	void				DrawScreen();

	void				ConvertScreenRect(LTRect &rect);
	void				ConvertScreenPos(LTIntPt &pos) {ConvertScreenPos(pos.x,pos.y);}
	void				ConvertScreenPos(int &x, int &y);
	LTFLOAT        GetXRatio()                         {return m_fXRatio;}
	LTFLOAT        GetYRatio()                         {return m_fYRatio;}

	uint32         GetScreenWidth();
	uint32         GetScreenHeight();

	//call Setup() before entering a 2-d state (screen)
	//call Clean() before returning to the game
	LTBOOL			Setup();
	void				Clean();

protected:

	// More initialization

	LTBOOL			InitFonts();

protected:


	TextureSet			m_TextureSet;			// list of used textures

	FontArray			m_FontArray;

	int					m_nYesVKeyCode;		// The virtual key code for "yes" responses
	int					m_nNoVKeyCode;			// The virtual key code for "no" responses

	LTFLOAT              m_fXRatio;
	LTFLOAT              m_fYRatio;

	uint32              m_dwScreenWidth;
	uint32              m_dwScreenHeight;

};


inline uint32 CInterfaceResMgr::GetScreenWidth()
{
	if (m_dwScreenWidth < 0)
		ScreenDimsChanged();
	return m_dwScreenWidth;
}
inline uint32 CInterfaceResMgr::GetScreenHeight()
{
	if (m_dwScreenHeight < 0)
		ScreenDimsChanged();
	return m_dwScreenHeight;
}


#endif // !defined(_INTERFACERESMGR_H_)