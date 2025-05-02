// ----------------------------------------------------------------------- //
//
// MODULE  : BaseScreen.h
//
// PURPOSE : Base class for interface screens
//				 Use this class to add functionality that all screens have
//
// (c) 1999-2001 Monolith Productions, Inc.  All Rights Reserved
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
// ----------------------------------------------------------------------- //

#if !defined(_BASE_SCREEN_H_)
#define _BASE_SCREEN_H_


#include "LTGUIMgr.h"
#include "interfaceresmgr.h"
#include "LTPoly.h"


const uint32	argbWhite		= 0xFFFFFFFF;
const uint32	argbGray			= 0xFF7F7F7F;
const uint32	argbBlack		= 0xFF000000;
const uint32	argbTransBlack	= 0x00000000;
const uint32	argbBlue			= 0xFF00008F;
const uint32	argbYellow		= 0xFFFFFF00;
const uint32   argbGreen		= 0xFF009F7f;

class CScreenMgr;

const uint16 kNoSelection = 0xFFFF;

const LTIntPt kDefaultPos(-1,-1);

class CBaseScreen : public CLTGUICommandHandler
{

public:
	CBaseScreen();
	virtual ~CBaseScreen();

	// Initialization/Termination
   virtual LTBOOL	Init(int nScreenID);
	virtual void	Term();

	// Builds the screen
	virtual LTBOOL Build();
	inline  LTBOOL IsBuilt() const { return m_bBuilt; }


	virtual void Escape();

	virtual uint32 OnCommand(uint32 dwCommand, uint32 dwParam1, uint32 dwParam2);

	// Get the screen ID
	inline int GetScreenID() const	{ return m_nScreenID; }


	// Renders the screen 
	virtual LTBOOL Render();

	// Creates the title for the screen
	void SetTitlePos(LTIntPt pt);
	void SetTitleFont(uint8 nFont);
	void SetTitleSize(uint8 nFontSize);
	void SetTitleColor(uint32 titleColor);

	LTBOOL CreateTitle(int nStringID);
	LTBOOL CreateTitle(wchar_t *lpszTitle);

	//set the background for the screen
	void UseBack(LTBOOL	bBack=LTTRUE,LTBOOL	bOK=LTFALSE, LTBOOL bReturn = LTFALSE );

	// This is called when the screen gets or loses focus
	virtual void OnFocus(LTBOOL bFocus);

	// This is called when the selected item changes
	virtual void  OnSelectionChange()			{};

	// Handles a key press.  Returns FALSE if the key was not processed through this method.
	// Left, Up, Down, Right, and Enter are automatically passed through OnUp(), OnDown(), etc.
	virtual LTBOOL   HandleKeyDown(int key, int rep);
	virtual LTBOOL   HandleKeyUp(int key) { return LTFALSE; }
	virtual LTBOOL   HandleChar(wchar_t c);
	virtual LTBOOL   HandleForceUpdate() { return LTFALSE; }

	//reset selection and help display
	virtual void	ForceMouseUpdate();
//	virtual void	UpdateHelpText();

	// Mouse messages
	virtual LTBOOL   OnLButtonDown(int x, int y);
	virtual LTBOOL   OnLButtonUp(int x, int y);
	virtual LTBOOL   OnLButtonDblClick(int x, int y);
	virtual LTBOOL   OnRButtonDown(int x, int y);
	virtual LTBOOL   OnRButtonUp(int x, int y);
	virtual LTBOOL   OnRButtonDblClick(int x, int y);
	virtual LTBOOL   OnMouseMove(int x, int y);

	// Page rectangle ( could be title safe area for controls and text ) 
	inline int 		GetPageLeft() const {return m_PageRect.left;}
	inline int 		GetPageRight() const {return m_PageRect.right;}
	inline int 		GetPageTop() const {return m_PageRect.top;}
	inline int 		GetPageBottom() const {return m_PageRect.bottom;}

	// Controls
	uint16		AddControl(CLTGUICtrl* pCtrl);
	void			RemoveControl(CLTGUICtrl* pControl,LTBOOL bDelete = LTTRUE);

	// Selection ( highlighted ) control 
	uint16			SetSelection(uint16 select, LTBOOL bFindSelectable = LTFALSE);
	uint16			NextSelection();
	uint16			PreviousSelection();
	inline uint16	GetSelection() const		{return m_nSelection;}
	inline uint16	GetOldSelection() const		{return m_nOldSelection;}

	// captured control
	void				SetCapture(CLTGUICtrl *pCtrl)	{m_pCaptureCtrl = pCtrl;}
	CLTGUICtrl*		GetCapture()					{return m_pCaptureCtrl;}

	inline CLTGUICtrl*	GetSelectedControl()  {return GetControl(m_nSelection);}
	CLTGUICtrl*				GetControl(uint16 nIndex);
	uint16					GetIndex(CLTGUICtrl* pCtrl);

	// Auto spacing calls for controls
	inline void		SetItemSpacing(int space)	{m_nItemSpacing = space;}
	inline int		GetItemSpacing() const		{ return m_nItemSpacing; }
	virtual void	ScreenDimsChanged();

	//default font	for items is the large font
	//	These	AddXXX()	functions call	CreateXXX()	and then	add the control to the control list
	CLTGUIEditCtrl * AddEditItem ( uint16 nMaxLength, uint32 nCommandID, wchar_t * pStringValue = 0, LTIntPt pos = kDefaultPos, int nFont = -1 );

	CLTGUITextCtrl*		AddTextItem(int stringID, uint32	commandID, int	helpID, LTIntPt pos = kDefaultPos, LTBOOL	bFixed =	LTFALSE,	int nFont =	-1);
	CLTGUITextCtrl*		AddTextItem(wchar_t *pString,	uint32 commandID,	int helpID,	LTIntPt pos	= kDefaultPos,	LTBOOL bFixed = LTFALSE, int nFont = -1);
	CLTGUITextCtrl*		AddTextItem(int stringID,	uint32 commandID,	int helpID,	char * pPos = NULL, char * pSize = NULL, eTextAlign halign = kLeft, LTBOOL bFixed = LTFALSE, int nFont = -1);
	CLTGUITextCtrl*		AddTextItemRect(int stringID,	uint32 commandID,	int helpID,	char * pRect = NULL, char * pSize = NULL, eTextAlign halign = kLeft, LTBOOL bFixed = LTFALSE, int nFont = -1);

	CLTGUIListCtrl*		AddList(LTIntPt pos,	uint16 nHeight, LTBOOL bUseArrows =	LTFALSE,	uint16 nArrowOffset = 0);

	CLTGUICycleCtrl*		 AddCycle(int stringID,	int helpID,	int nHeaderWidth,	uint8	*pnValue=LTNULL, LTIntPt pos = kDefaultPos, LTBOOL	bFixed =	LTFALSE,	int nFont =	-1);
	CLTGUICycleCtrl*		 AddCycle(wchar_t	*pString, int helpID, int nHeaderWidth, uint8 *pnValue=LTNULL,	LTIntPt pos	= kDefaultPos,	LTBOOL bFixed = LTFALSE, int nFont = -1);

	CLTGUIToggle*		 AddToggle(int	stringID, int helpID, int nHeaderWidth, LTBOOL *pbValue=LTNULL, LTIntPt	pos =	kDefaultPos, LTBOOL bFixed	= LTFALSE, int	nFont	= -1);
	CLTGUIToggle*		 AddToggle(wchar_t *pString, int	helpID, int	nHeaderWidth, LTBOOL	*pbValue=LTNULL, LTIntPt pos = kDefaultPos, LTBOOL	bFixed =	LTFALSE,	int nFont =	-1);

	CLTGUISlider*		 AddSlider(int	stringID,  int	helpID, int	nHeaderWidth, int	nBarWidth, int	nBarHeight = -1,
		int *pnValue =	LTNULL, LTIntPt pos = kDefaultPos, LTBOOL	bFixed =	LTFALSE,	int nFont =	-1);
	CLTGUISlider*		 AddSlider(wchar_t *pString,	int helpID,	int nHeaderWidth,	int nBarWidth,	int nBarHeight	= -1,
		int *pnValue =	LTNULL, LTIntPt pos = kDefaultPos, LTBOOL	bFixed =	LTFALSE,	int nFont =	-1);

	CLTGUIColumnCtrl*	AddColumnCtrl(uint32	commandID, int	helpID, LTIntPt pos = kDefaultPos, LTBOOL	bFixed =	LTFALSE,	int nFont =	-1);

	CLTGUIButton *		AddButton(int stringID, uint32 commandID, int helpID, HTEXTURE hNormal, HTEXTURE hSelected=LTNULL, HTEXTURE hDisabled=LTNULL, LTIntPt pos = kDefaultPos, LTBOOL	bFixed =	LTFALSE,	int nFont =	-1);
	CLTGUIButton *		AddButton(wchar_t *pString, uint32 commandID, int helpID, HTEXTURE hNormal, HTEXTURE hSelected=LTNULL, HTEXTURE hDisabled=LTNULL, LTIntPt pos = kDefaultPos, LTBOOL	bFixed =	LTFALSE,	int nFont =	-1);
	CLTGUIButton *		AddButton(int stringID, uint32 commandID, int helpID, char * pNormal, char * pSelected = NULL, char * pDisabled = NULL, char * pPos = NULL, char * pSize = NULL, eTextAlign halign = kLeft, LTBOOL bFixed = LTFALSE, int nFont = -1);
	CLTGUIButton *		AddButton(int stringID, uint32 commandID, int helpID, HTEXTURE hNormal, HTEXTURE hSelected=LTNULL, HTEXTURE hDisabled=LTNULL, char * pPos = NULL, char * pSize = NULL, eTextAlign halign = kLeft, LTBOOL bFixed = LTFALSE, int nFont = -1);


	//	These	CreateXXX()	create controls but do not	add them	to	the control	list
	CLTGUIEditCtrl*		CreateEditItem( uint16 nMaxLength, uint32 nCommandID, wchar_t * pStringValue = 0, LTIntPt pos = kDefaultPos, int nFont = -1 );

	CLTGUITextCtrl*		CreateTextItem(int stringID, uint32	commandID, int	helpID, LTIntPt pos = kDefaultPos, LTBOOL	bFixed =	LTFALSE,	int nFont =	-1);
	CLTGUITextCtrl*		CreateTextItem(wchar_t *pString,	uint32 commandID,	int helpID,	LTIntPt pos	= kDefaultPos,	LTBOOL bFixed = LTFALSE, int nFont = -1);
	CLTGUITextCtrl*		CreateTextItem(int stringID, uint32	commandID, int	helpID, char * pPos = NULL, char * pSize = NULL, eTextAlign halign = kLeft,  LTBOOL	bFixed =	LTFALSE,	int nFont =	-1);
	CLTGUITextCtrl*		CreateTextItemRect(int stringID, uint32	commandID, int	helpID, char * pRect = NULL, char * pSize = NULL, eTextAlign halign = kLeft,  LTBOOL	bFixed =	LTFALSE,	int nFont =	-1);



	CLTGUIListCtrl*		CreateList(LTIntPt pos,	uint16 nHeight, LTBOOL bUseArrows =	LTFALSE,	uint16 nArrowOffset = 0);

	CLTGUICycleCtrl*		 CreateCycle(int stringID,	int helpID,	int nHeaderWidth,	uint8	*pnValue=LTNULL, LTIntPt pos = kDefaultPos, LTBOOL	bFixed =	LTFALSE,	int nFont =	-1);
	CLTGUICycleCtrl*		 CreateCycle(wchar_t	*pString, int helpID, int nHeaderWidth, uint8 *pnValue=LTNULL,	LTIntPt pos	= kDefaultPos,	LTBOOL bFixed = LTFALSE, int nFont = -1);

	CLTGUIToggle*		 CreateToggle(int	stringID, int helpID, int nHeaderWidth, LTBOOL *pbValue=LTNULL, LTIntPt	pos =	kDefaultPos, LTBOOL bFixed	= LTFALSE, int	nFont	= -1);
	CLTGUIToggle*		 CreateToggle(wchar_t *pString, int	helpID, int	nHeaderWidth, LTBOOL	*pbValue=LTNULL, LTIntPt pos = kDefaultPos, LTBOOL	bFixed =	LTFALSE,	int nFont =	-1);

	CLTGUISlider*		 CreateSlider(int	stringID,  int	helpID, int	nHeaderWidth, int	nBarWidth, int	nBarHeight = -1,
		int *pnValue =	LTNULL, LTIntPt pos = kDefaultPos, LTBOOL	bFixed =	LTFALSE,	int nFont =	-1);
	CLTGUISlider*		 CreateSlider(wchar_t *pString,	int helpID,	int nHeaderWidth,	int nBarWidth,	int nBarHeight	= -1,
		int *pnValue =	LTNULL, LTIntPt pos = kDefaultPos, LTBOOL	bFixed =	LTFALSE,	int nFont =	-1);

	CLTGUIColumnCtrl*	CreateColumnCtrl(uint32	commandID, int	helpID, LTIntPt pos = kDefaultPos, LTBOOL	bFixed =	LTFALSE,	int nFont =	-1);

	CLTGUIButton *		CreateButton(int stringID, uint32 commandID, int helpID, HTEXTURE hNormal, HTEXTURE hSelected=LTNULL, HTEXTURE hDisabled=LTNULL, LTIntPt pos = kDefaultPos, LTBOOL	bFixed =	LTFALSE,	int nFont =	-1);
	CLTGUIButton *		CreateButton(wchar_t *pString, uint32 commandID, int helpID, HTEXTURE hNormal, HTEXTURE hSelected=LTNULL, HTEXTURE hDisabled=LTNULL, LTIntPt pos	= kDefaultPos,	LTBOOL bFixed = LTFALSE, int nFont = -1);
	CLTGUIButton *		CreateButton(int stringID, uint32 commandID, int helpID, char * pNormal, char * pSelected = NULL, char * pDisabled = NULL, char * pPos = NULL, char * pSize = NULL, eTextAlign halign = kLeft, LTBOOL bFixed = LTFALSE, int nFont = -1);
	CLTGUIButton *		CreateButton(int stringID, uint32 commandID, int helpID, HTEXTURE hNormal, HTEXTURE hSelected=LTNULL, HTEXTURE hDisabled=LTNULL, char * pPos = NULL, char * pSize = NULL, eTextAlign halign = kLeft, LTBOOL bFixed = LTFALSE, int nFont = -1);


	// Calls UpdateData on each control in the screen
	virtual void            UpdateData(LTBOOL bSaveAndValidate=LTTRUE);

protected:

	// Handle input
	virtual LTBOOL   OnUp();
	virtual LTBOOL   OnDown();
	virtual LTBOOL   OnLeft();
	virtual LTBOOL   OnRight();
	virtual LTBOOL   OnEnter();

	// Gets the index of the control that is under the specific screen point.
	// Returns FALSE if there isn't one under the specified point.
	LTBOOL GetControlUnderPoint(int xPos, int yPos, uint16 *pnIndex);

protected:

	void RemoveAll(LTBOOL bDelete = LTTRUE);

	void CreateBack(LTBOOL bOK = LTFALSE, LTBOOL bReturn = LTFALSE);



	void			SetPolyRenderState();
	void			InitPoly(LTPoly_GT4* pPoly, LTIntPt pos, HTEXTURE hTex);
	void			ScalePoly(LTPoly_GT4* pPoly, LTIntPt pos, HTEXTURE hTex);

protected:

	LTBOOL			m_bInit;
   LTBOOL			m_bBuilt;
	LTBOOL			m_bVisited;
	
	CScreenMgr*		m_pScreenMgr;

	int				m_nScreenID;		// The ID of this screen
	int				m_nContinueID;		// The ID of the screen to show when continue is clicked

	//title stuff
	CLTGUIPolyString m_TitleString;	// The title string
	LTIntPt			m_TitlePos;
	uint8				m_TitleFont;
	uint8				m_TitleSize;
	uint32			m_TitleColor;

	LTBOOL			m_bHaveFocus;

	// Array of free controls that this screen owns
	ControlArray	m_controlArray;

	int				m_nItemSpacing;

	CLTGUICtrl*		m_pCaptureCtrl;

	LTRect			m_PageRect;

	int				m_nAlignment;
	uint32			m_SelectedColor;
	uint32			m_NonSelectedColor;
	uint32			m_DisabledColor;

	CLTGUITextCtrl*	m_pNext;

	static  LTBOOL      s_bReadLayout;

	LTBOOL	m_bBack;

	static CLTGUIButton	s_Back;
	static LTIntPt			s_BackPos;
	static LTIntPt			s_NextPos;

	LTIntPt			m_nextPos;

	LTFLOAT			m_fLastScale;

	uint16			m_nSelection;
	uint16			m_nOldSelection;
	uint16			m_nLMouseDownItemSel;
	uint16			m_nRMouseDownItemSel;
};




inline CLTGUIEditCtrl * CBaseScreen::AddEditItem (  uint16 nMaxLength, uint32 nCommandID, wchar_t * pStringValue, LTIntPt pos, int nFont )
{
	CLTGUIEditCtrl* pCtrl = CreateEditItem (  nMaxLength, nCommandID, pStringValue, pos, nFont );		
		
	if	(pCtrl)
		AddControl(pCtrl);

	return pCtrl;

}


inline  CLTGUITextCtrl* CBaseScreen::AddTextItem(int stringID, uint32 commandID, int helpID, LTIntPt pos, LTBOOL bFixed, int nFont)
{
	CLTGUITextCtrl* pCtrl = CreateTextItem(stringID, commandID, helpID,  pos, bFixed, nFont);
	if (pCtrl)
		AddControl(pCtrl);
	return pCtrl;
}
inline  CLTGUITextCtrl* CBaseScreen::AddTextItem(wchar_t *pString, uint32 commandID, int helpID, LTIntPt pos, LTBOOL bFixed, int nFont)
{
	CLTGUITextCtrl* pCtrl = CreateTextItem(pString, commandID, helpID,  pos, bFixed, nFont);
	if (pCtrl)
		AddControl(pCtrl);
	return pCtrl;
}

inline CLTGUIListCtrl* CBaseScreen::AddList(LTIntPt pos, uint16 nHeight, LTBOOL bUseArrows, uint16 nArrowOffset)
{
	
	CLTGUIListCtrl* pList = CreateList(pos,nHeight,bUseArrows,nArrowOffset);
    if (pList)
		AddControl(pList);	
	return pList;
}


inline  CLTGUICycleCtrl* CBaseScreen::AddCycle(int stringID, int helpID, int nHeaderWidth, uint8 *pnValue, LTIntPt pos, LTBOOL bFixed, int nFont)
{
	CLTGUICycleCtrl* pCtrl = CreateCycle(stringID, helpID, nHeaderWidth, pnValue,  pos, bFixed, nFont);
	if (pCtrl)
		AddControl(pCtrl);
	return pCtrl;
}
inline  CLTGUICycleCtrl* CBaseScreen::AddCycle(wchar_t *pString, int helpID, int nHeaderWidth, uint8 *pnValue, LTIntPt pos, LTBOOL bFixed, int nFont)
{
	CLTGUICycleCtrl* pCtrl = CreateCycle(pString, helpID, nHeaderWidth, pnValue, pos, bFixed, nFont);
	if (pCtrl)
		AddControl(pCtrl);
	return pCtrl;
}


inline  CLTGUIToggle* CBaseScreen::AddToggle(int stringID, int helpID, int nHeaderWidth, LTBOOL *pbValue, LTIntPt pos, LTBOOL bFixed, int nFont)
{
	CLTGUIToggle* pCtrl = CreateToggle(stringID, helpID, nHeaderWidth, pbValue,  pos, bFixed, nFont);
	if (pCtrl)
		AddControl(pCtrl);
	return pCtrl;
}
inline  CLTGUIToggle* CBaseScreen::AddToggle(wchar_t *pString, int helpID, int nHeaderWidth, LTBOOL *pbValue, LTIntPt pos, LTBOOL bFixed, int nFont)
{
	CLTGUIToggle* pCtrl = CreateToggle(pString, helpID, nHeaderWidth, pbValue, pos, bFixed, nFont);
	if (pCtrl)
		AddControl(pCtrl);
	return pCtrl;
}


inline  CLTGUISlider* CBaseScreen::AddSlider(int stringID, int helpID, int nHeaderWidth,  int nBarWidth, int nBarHeight, 
												int *pnValue, LTIntPt pos, LTBOOL bFixed, int nFont)
{
	CLTGUISlider* pCtrl = CreateSlider(stringID, helpID, nHeaderWidth, nBarWidth, nBarHeight, pnValue, pos, bFixed, nFont);
	if (pCtrl)
		AddControl(pCtrl);
	return pCtrl;
}
inline  CLTGUISlider* CBaseScreen::AddSlider(wchar_t *pString, int helpID, int nHeaderWidth,  int nBarWidth, int nBarHeight, 
												int *pnValue, LTIntPt pos, LTBOOL bFixed, int nFont)
{
	CLTGUISlider* pCtrl = CreateSlider(pString, helpID, nHeaderWidth, nBarWidth, nBarHeight, pnValue, pos, bFixed, nFont);
	if (pCtrl)
		AddControl(pCtrl);
	return pCtrl;
}

inline  CLTGUIColumnCtrl* CBaseScreen::AddColumnCtrl(uint32 commandID, int helpID, LTIntPt pos, LTBOOL bFixed, int nFont)
{
	
	CLTGUIColumnCtrl* pCtrl = CreateColumnCtrl(commandID, helpID, pos, bFixed, nFont);
	if (pCtrl)
		AddControl(pCtrl);
	return pCtrl;
}

inline CLTGUIButton * CBaseScreen::AddButton(wchar_t *pString, uint32 commandID, int helpID, HTEXTURE hNormal, HTEXTURE hSelected, HTEXTURE hDisabled, LTIntPt pos, LTBOOL bFixed, int nFont )
{
	CLTGUIButton *	pCtrl	= CreateButton(pString, commandID, helpID, hNormal, hSelected, hDisabled, pos, bFixed,	nFont);
	if	(pCtrl)
		AddControl(pCtrl);
	return pCtrl;

}

inline  CLTGUIButton * CBaseScreen::AddButton(int stringID, uint32 commandID, int helpID, HTEXTURE hNormal, HTEXTURE hSelected, HTEXTURE hDisabled, LTIntPt pos, LTBOOL bFixed, int nFont ) 
{
	CLTGUIButton *	pCtrl	= CreateButton(stringID, commandID, helpID, hNormal, hSelected, hDisabled, pos, bFixed,	nFont);
	if	(pCtrl)
		AddControl(pCtrl);
	return pCtrl;
}

inline	CLTGUIButton *		CBaseScreen::AddButton(int stringID, uint32 commandID, int helpID, char * pNormal, char * pSelected, char * pDisabled, char * pPos, char * pSize, eTextAlign halign, LTBOOL bFixed, int nFont)
{
	CLTGUIButton *	pCtrl	= CreateButton(stringID, commandID, helpID, pNormal, pSelected, pDisabled, pPos, pSize, halign, bFixed,	nFont);

	if	(pCtrl)
		AddControl(pCtrl);

	return pCtrl;

}


inline  CLTGUIButton * CBaseScreen::AddButton(int stringID, uint32 commandID, int helpID, HTEXTURE hNormal, HTEXTURE hSelected, HTEXTURE hDisabled, char * pPos, char * pSize, eTextAlign halign, LTBOOL bFixed, int nFont)
{
	CLTGUIButton *	pCtrl	= CreateButton(stringID, commandID, helpID, hNormal, hSelected, hDisabled, pPos, pSize, halign, bFixed,	nFont);
	if	(pCtrl)
		AddControl(pCtrl);
	return pCtrl;
}

#endif // _BASE_SCREEN_H_
