// ----------------------------------------------------------------------- //
//
// MODULE  : GuiUtils.h
//
// PURPOSE : Helper functions for user interfaces.
//
// CREATED : 2004/7/30
//
// (c) 2004 Touchdown Entertainment, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //


// forwards:
class CUIFormattedPolyString;


class GuiUtils
{
public:

	// Set the position of a formatted poly string, and make sure it is inside the
	// title safe area of the screen.
	static void 		SetPosSafeArea(CUIFormattedPolyString *pPolyString, unsigned x, unsigned y);

	// Moves the given coordinates to make sure that they exist within the title
	// safe area of the screen.
	static void			SetPosSafeArea(unsigned &x, unsigned &y);

	// Moves the given coordinates to make sure that they exist within the title
	// safe area of the screen.
	static void			SetPosSafeArea(float &x, float &y);

};
