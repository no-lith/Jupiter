// ----------------------------------------------------------------------- //
//
// MODULE  : GuiUtils.cpp
//
// PURPOSE : Helper functions for user interfaces.
//
// CREATED : 2004/7/30
//
// (c) 2004 Touchdown Entertainment, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "GuiUtils.h"
#include <iltclient.h>
#include <ltbasedefs.h>
#include <iltfontmanager.h>
#include "clientinterfaces.h"



namespace
{

	const float fScreenSafeArea	= 0.95f;

} // unnamed namespace




// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GuiUtils::SetPosSafeArea
//
//	PURPOSE:	Set the position of a formatted poly string, and make
//				sure it is inside the title safe area of the screen.
//
// ----------------------------------------------------------------------- //

void GuiUtils::SetPosSafeArea(CUIFormattedPolyString *pPolyString, unsigned x, unsigned y)
{
	if (LTNULL != pPolyString)
	{
		uint32 nWidth, nHeight;
        g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);

		float fWidth	= static_cast<float>(nWidth);
		float fHeight	= static_cast<float>(nHeight);
		float xMin		= (fWidth - (fWidth * fScreenSafeArea)) / 2.0f;
		float yMin 		= (fHeight - (fHeight * fScreenSafeArea)) / 2.0f;
		float xMax		= fWidth - xMin;
		float yMax		= fHeight - yMin;
		float xPos		= static_cast<float>(x);
		float yPos		= static_cast<float>(y);
		uint32 fDimsX	= static_cast<uint32>(pPolyString->GetWidth());
		uint32 fDimsY	= static_cast<uint32>(pPolyString->GetHeight());

		if (xPos < xMin)
		{
			xPos = xMin;
		}
		else if ((xPos+fDimsX) > xMax)
		{
			xPos = xMax-fDimsX;

			// The best we can do
			if ( xPos < xMin )
				xPos = xMin;
		}

		if (yPos < yMin)
		{
			yPos = yMin;
		}
		else if ((yPos+fDimsY) > yMax)
		{
			yPos = yMax-fDimsY;

			// The best we can do
			if ( yPos < yMin )
				yPos = yMin;

		}

		pPolyString->SetPosition(xPos, yPos);
	}
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GuiUtils::SetPosSafeArea
//
//	PURPOSE:	Moves the given coordinates to make sure that they exist
//				within the title safe area of the screen.
//
// ----------------------------------------------------------------------- //

void GuiUtils::SetPosSafeArea(unsigned &x, unsigned &y)
{
	float xPos		= static_cast<float>(x);
	float yPos		= static_cast<float>(y);

	SetPosSafeArea(xPos, yPos);

	x = static_cast<unsigned>(xPos);
	y = static_cast<unsigned>(yPos);
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	GuiUtils::SetPosSafeArea
//
//	PURPOSE:	Moves the given coordinates to make sure that they exist
//				within the title safe area of the screen.
//
// ----------------------------------------------------------------------- //

void GuiUtils::SetPosSafeArea(float &x, float &y)
{
	uint32 nWidth, nHeight;
    g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(), &nWidth, &nHeight);

	float fWidth	= static_cast<float>(nWidth);
	float fHeight	= static_cast<float>(nHeight);
	float xMin		= (fWidth - (fWidth * fScreenSafeArea)) / 2.0f;
	float yMin 		= (fHeight - (fHeight * fScreenSafeArea)) / 2.0f;
	float xMax		= fWidth - xMin;
	float yMax		= fHeight - yMin;
	float xPos		= static_cast<float>(x);
	float yPos		= static_cast<float>(y);

	if (xPos < xMin)
	{
		xPos = xMin;
	}
	else if (xPos > xMax)
	{
		xPos = xMax;
	}

	if (yPos < yMin)
	{
		yPos = yMin;
	}
	else if (yPos > yMax)
	{
		yPos = yMax;
	}

	x = xPos;
	y = yPos;
}
