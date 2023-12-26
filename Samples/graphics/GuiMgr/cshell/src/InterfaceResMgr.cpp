// ----------------------------------------------------------------------- //
//
// MODULE  : InterfaceResMgr.cpp
//
// PURPOSE : Manager for resources associated with the interface
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "ltclientshell.h"
#include "InterfaceResMgr.h"
#include "ilttexturestring.h"
#include "clienthelper.h"

CInterfaceResMgr*   g_pInterfaceResMgr = LTNULL;

namespace
{
	const uint8 kIndent = 8;
	const uint16 kBaseWidth = 320;
	const uint16 kBaseHeight = 200;
	const uint16 kBigWidth = 480;
	const uint16 kMinWidth = 160;
}

//---------------------------------------------------------------------------
//	 Name:		CInterfaceResMgr::CInterfaceResMgr()
//
//	 Purpose:	Constructor
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------

CInterfaceResMgr::CInterfaceResMgr()
{
	g_pInterfaceResMgr = this;

	m_dwScreenWidth = 0;
	m_dwScreenHeight = 0;

	m_fXRatio = 1.0f;
	m_fYRatio = 1.0f;

}

//---------------------------------------------------------------------------
//	 Name:		CInterfaceResMgr::~CInterfaceResMgr()
//
//	 Purpose:	Destructor
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
CInterfaceResMgr::~CInterfaceResMgr()
{
    g_pInterfaceResMgr = LTNULL;
}


//---------------------------------------------------------------------------
//	 Name:		CInterfaceResMgr::Init()
//
//	 Purpose:	Initialization
//
//	 Returns:	LTBOOL - 
//
//---------------------------------------------------------------------------
LTBOOL CInterfaceResMgr::Init()
{

	// Initialize the fonts
	if (!InitFonts())
	{
		return LTFALSE;
	}

	// set resolution dependant variables
	ScreenDimsChanged();

	return LTTRUE;
}



//---------------------------------------------------------------------------
//	 Name:		CInterfaceResMgr::Term()
//
//	 Purpose:	Termination
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void CInterfaceResMgr::Term()
{
	Clean();

	// Destroy fonts
	FontArray::iterator iter = m_FontArray.begin();
	while (iter != m_FontArray.end())
	{
		delete *iter;
		iter++;
	}

	m_FontArray.clear();

	// Release any textures that the InterfaceResMgr has.
	for (TextureSet::iterator iter = m_TextureSet.begin(); iter != m_TextureSet.end(); iter++)
	{
		g_pLTCTexInterface->ReleaseTextureHandle(*iter);
	}

	m_TextureSet.clear();

}



//---------------------------------------------------------------------------
//	 Name:		CInterfaceResMgr::Setup()
//
//	 Purpose:	Setup
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
LTBOOL CInterfaceResMgr::Setup()
{
	//preload common surfaces

    return LTTRUE;
}


//---------------------------------------------------------------------------
//	 Name:		CInterfaceResMgr::Term()
//
//	 Purpose:	Clean up
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void CInterfaceResMgr::Clean()
{
	if (g_pLTClient)
	{
	}
}



//---------------------------------------------------------------------------
//	 Name:		CInterfaceResMgr::DrawScreen()
//
//	 Purpose:	Draw the current screen to the surface
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void CInterfaceResMgr::DrawScreen()
{
	if (!g_pLTClient) return;

	if (m_dwScreenWidth == 0)
		ScreenDimsChanged();

	// Render the current screen
	g_pCShell->GetScreenMgr()->Render();

	return;
}



//---------------------------------------------------------------------------
//	 Name:		CInterfaceResMgr::InitFonts()
//
//	 Purpose:	Initialize the fonts
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
LTBOOL CInterfaceResMgr::InitFonts()
{

	int nNumFonts = g_pCShell->GetLayoutMgr()->GetNumFonts();

	wchar_t szFontName[_MAX_PATH];

	// Create the fonts charsets and size we are using
	for (int i = 0; i < nNumFonts; i++)
	{
		g_pCShell->GetLayoutMgr()->GetFontName( i, szFontName, LTARRAYSIZE(szFontName ) );
		uint32 nPtSize = g_pCShell->GetLayoutMgr()->GetFontSize(i);
		uint8 nCharSet = g_pCShell->GetLayoutMgr()->GetFontCharSet(i);
		uint32 nStyle = g_pCShell->GetLayoutMgr()->GetFontStyle(i);

		if (!nPtSize)
			nPtSize = 24;

		CFontInfo *pFont = new CFontInfo( szFontName, nPtSize, nCharSet, nStyle);

		if (pFont)
			m_FontArray.push_back(pFont);
		else
		{
			g_pLTClient->CPrint("Failed to create font <%s> at size %d.",szFontName,nPtSize);
		}
	}

	if ((int)m_FontArray.size() != nNumFonts)
	{
		return LTFALSE;
	}

    return LTTRUE;

}

#define USABLE_HEIGHT_I 480
#define USABLE_HEIGHT_F 480.0f

//---------------------------------------------------------------------------
//	 Name:		CInterfaceResMgr::ScreenDimsChanged()
//
//	 Purpose:	Screen dimensions have changed
//
//	 Returns:	n/a
//
//---------------------------------------------------------------------------
void CInterfaceResMgr::ScreenDimsChanged()
{
	if (!g_pLTClient) return;

	RMode currentMode;
	g_pLTClient->GetRenderMode(&currentMode);

	m_fXRatio = (float)currentMode.m_Width / 640.0f;
	m_fYRatio = (float)currentMode.m_Height / USABLE_HEIGHT_F;

	m_dwScreenWidth = currentMode.m_Width;
	m_dwScreenHeight = currentMode.m_Height;

}



//---------------------------------------------------------------------------
//	 Name:		CInterfaceResMgr::ConvertScreenPos()
//
//	 Purpose:	Convert screen coords to default coords ( 640x480 )
//
//	 Returns:	x, y in 640x480
//
//---------------------------------------------------------------------------
void CInterfaceResMgr::ConvertScreenPos(int &x, int &y)
{
	if (m_dwScreenWidth == 640 && m_dwScreenHeight == USABLE_HEIGHT_I)
		return;

	x = (int)( (LTFLOAT)x * m_fXRatio );
	y = (int)( (LTFLOAT)y * m_fYRatio );
}

//---------------------------------------------------------------------------
//	 Name:		CInterfaceResMgr::ConvertScreenRect()
//
//	 Purpose:	Convert screen rectangle to default coords ( 640x480 )
//
//	 Returns:	LTRect in 640x480
//
//---------------------------------------------------------------------------
void CInterfaceResMgr::ConvertScreenRect(LTRect &rect)
{
	if (m_dwScreenWidth == 640 && m_dwScreenHeight == USABLE_HEIGHT_I)
		return;

	rect.left = (int)( (LTFLOAT)rect.left * m_fXRatio );
	rect.right = (int)( (LTFLOAT)rect.right * m_fXRatio );
	rect.top = (int)( (LTFLOAT)rect.top * m_fYRatio );
	rect.bottom = (int)( (LTFLOAT)rect.bottom * m_fYRatio );

}

//---------------------------------------------------------------------------
//	 Name:		CInterfaceResMgr::GetFont()
//
//	 Purpose:	Get Font based on index 
//
//	 Returns:	font 
//
//---------------------------------------------------------------------------
CFontInfo* CInterfaceResMgr::GetFont(uint32 nIndex)
{
	if (nIndex >= m_FontArray.size())
		return LTNULL;
	return m_FontArray.at(nIndex);
}





//---------------------------------------------------------------------------
//	 Name:		CInterfaceResMgr::GetTexture()
//
//	 Purpose:	Get/create texture
//
//	 Returns:	HTEXTURE - texture handle
//
//---------------------------------------------------------------------------
HTEXTURE CInterfaceResMgr::GetTexture(const char *szTexName)
{
	HTEXTURE hTex = LTNULL;

	LTRESULT res = g_pLTCTexInterface->FindTextureFromName(hTex, szTexName);

	//if we found it, see if it's in our list
	if (res == LT_OK)
	{
		TextureSet::iterator iter = m_TextureSet.find(hTex);

		//if is not in our list, inc the ref count so no one else frees it
		if (iter == m_TextureSet.end())
		{
//			g_pLTClient->CPrint("CInterfaceResMgr::GetTexture() : increasing ref count to existing texture %s",szTexName);
			g_pLTCTexInterface->AddRefTextureHandle(hTex);
			m_TextureSet.insert(hTex);
		}
	}
	else
	{
//		g_pLTClient->CPrint("CInterfaceResMgr::GetTexture() : creating texture %s",szTexName);
		g_pLTCTexInterface->CreateTextureFromName(hTex, szTexName);
		// Note valid textures which we will need to release during termination
		if (hTex)
			m_TextureSet.insert(hTex);

	}

	return hTex;
}

