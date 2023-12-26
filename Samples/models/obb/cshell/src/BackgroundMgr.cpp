//------------------------------------------------------------------------------//
//
// MODULE   : BackgroundMgr.cpp
//
// PURPOSE  : Sets the background color.
//
// CREATED  : 09/17/2003
//
// (c) 2003 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#include "BackgroundMgr.h"



//---------------------------------------------------------------------------
//  Name:         BackgroundMgr
//
//  Purpose:	  constructor
//
//  Returns:	  n/a
//
//---------------------------------------------------------------------------
BackgroundMgr::BackgroundMgr()
	: m_Index(2)
{
	SetColor();
}



//---------------------------------------------------------------------------
//  Name:         ~BackgroundMgr
//
//  Purpose:	  destructor
//
//  Returns:	  n/a
//
//---------------------------------------------------------------------------
BackgroundMgr::~BackgroundMgr()
{
}



//---------------------------------------------------------------------------
//  Name:         IncColor
//
//  Purpose:	  increment the current background color
//
//  Returns:	  void
//
//---------------------------------------------------------------------------
void BackgroundMgr::IncColor()
{
	int CurrIndex = m_Index + 1;
	if (CurrIndex >= MAX_COLORS)
	{
		CurrIndex = 0;
	}
	m_Index = CurrIndex;

	SetColor();
}



//---------------------------------------------------------------------------
//  Name:         DecColor
//
//  Purpose:	  decrement the current background color
//
//  Returns:	  void
//
//---------------------------------------------------------------------------
void BackgroundMgr::DecColor()
{
	int CurrIndex = m_Index - 1;
	if (CurrIndex < 0)
	{
		CurrIndex = MAX_COLORS - 1;
	}
	m_Index = CurrIndex;

	SetColor();
}



//---------------------------------------------------------------------------
//  Name:         SetColor
//
//  Purpose:	  update the current background color
//
//  Returns:	  void
//
//---------------------------------------------------------------------------
void BackgroundMgr::SetColor()
{
	switch (m_Index)
	{
		case 0:
			m_BackgroundColor.r = 220;
			m_BackgroundColor.g = 218;
			m_BackgroundColor.b = 229;
			m_BackgroundColor.a = 255;
			break;
		case 1:
			m_BackgroundColor.r = 245;
			m_BackgroundColor.g = 244;
			m_BackgroundColor.b = 247;
			m_BackgroundColor.a = 255;
			break;
		case 2:
			m_BackgroundColor.r = 57;
			m_BackgroundColor.g = 48;
			m_BackgroundColor.b = 106;
			m_BackgroundColor.a = 255;
			break;
		case 3:
			m_BackgroundColor.r = 147;
			m_BackgroundColor.g = 142;
			m_BackgroundColor.b = 174;
			m_BackgroundColor.a = 255;
			break;
		case 4:
			m_BackgroundColor.r = 12;
			m_BackgroundColor.g = 1;
			m_BackgroundColor.b = 73;
			m_BackgroundColor.a = 255;
			break;
		case 5:
			m_BackgroundColor.r = 65;
			m_BackgroundColor.g = 57;
			m_BackgroundColor.b = 113;
			m_BackgroundColor.a = 255;
			break;
		case 6:
			m_BackgroundColor.r = 255;
			m_BackgroundColor.g = 249;
			m_BackgroundColor.b = 231;
			m_BackgroundColor.a = 255;
			break;
		case 7:
			m_BackgroundColor.r = 235;
			m_BackgroundColor.g = 188;
			m_BackgroundColor.b = 137;
			m_BackgroundColor.a = 255;
			break;
		case 8:
			m_BackgroundColor.r = 213;
			m_BackgroundColor.g = 126;
			m_BackgroundColor.b = 42;
			m_BackgroundColor.a = 255;
			break;
		case 9:
			m_BackgroundColor.r = 249;
			m_BackgroundColor.g = 234;
			m_BackgroundColor.b = 202;
			m_BackgroundColor.a = 255;
			break;
		case 10:
			m_BackgroundColor.r = 242;
			m_BackgroundColor.g = 204;
			m_BackgroundColor.b = 135;
			m_BackgroundColor.a = 255;
			break;
		case 11:
			m_BackgroundColor.r = 216;
			m_BackgroundColor.g = 206;
			m_BackgroundColor.b = 188;
			m_BackgroundColor.a = 255;
			break;
		case 12:
			m_BackgroundColor.r = 0;
			m_BackgroundColor.g = 0;
			m_BackgroundColor.b = 0;
			m_BackgroundColor.a = 255;
			break;
		case 13:
			m_BackgroundColor.r = 255;
			m_BackgroundColor.g = 255;
			m_BackgroundColor.b = 255;
			m_BackgroundColor.a = 255;
			break;
	}
}
