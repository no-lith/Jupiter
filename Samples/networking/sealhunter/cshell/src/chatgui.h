// ----------------------------------------------------------------------- //
//
// MODULE  : CChatGui.h
//
// PURPOSE : CChatGui - Definition and constants
//
// CREATED : 07/23/2003
//
// (c) 2003 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __CCHATGUI_H__
#define __CCHATGUI_H__

#include <iltclient.h>
#include <ltbasedefs.h>
#include "clientinterfaces.h"
#include "serverdefs.h"
#include <iltfontmanager.h>

#define MAX_CHAT_LINES 4

class ILTMessage_Read;

struct CHATLINE
{
    char   szText[512];
    float  fTimeToLive;
    uint8  iNext; //We keep track of the next in the array for simple cyclical support.
    bool   bActive;
};


class CChatGui
{
public:

	// Construction / destruction
	CChatGui();
	~CChatGui();

	LTRESULT 		            Init();
	LTRESULT 		            Term();
    LTRESULT                    Update();

	LTRESULT 	            	Render();

    LTRESULT                    HandleInputDown(int key, int rep);
    LTRESULT                    HandleInputUp(int key);

    LTRESULT                    HandleMessage(ILTMessage_Read* pMessage);
    void                        SetActive(bool bActive){ m_bIsInputActive = bActive; }
    bool                        IsChatInputActive(){ return m_bIsInputActive; }

private:

    LTRESULT                    RecalcStrings();
    LTRESULT                    RecalcChatLines();
    char TranslateKey(uint32 key) const;

    HTEXTURE                    m_hBackDrop;
	CUIFont*					m_pFont;
	CUIFormattedPolyString*	    m_pChatString;
	CUIFormattedPolyString*	    m_pBufferString;
	CUIFormattedPolyString*	    m_pCursorString;
    char                        m_szBuffer[256];
    bool                        m_bIsInputActive;

    CHATLINE                    m_ChatLines[MAX_CHAT_LINES];
    uint8                       m_iCurChatLine;
};



#endif	// __CCHATGUI_H__
