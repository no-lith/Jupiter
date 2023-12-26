// ----------------------------------------------------------------------- //
//
// MODULE  : ScreenEnum.h
//
// PURPOSE : Enums and string constants for screens.
//
// (c) 2001 Monolith Productions, Inc.  All Rights Reserved
// (c) 2005 Touchdown Entertainment Inc. All Rights Reserved
// ----------------------------------------------------------------------- //

//
// The following macros allow the enum entries to be included as the 
// body of an enum, or the body of a const char* string list.
//

#undef ADD_SCREEN

#if defined(INCLUDE_AS_ENUM)
	#define ADD_SCREEN(id,name) SCREEN_ID_##id,
#elif defined(INCLUDE_AS_STRING)
	#define ADD_SCREEN(id,name) #name,
#else
	#error	To use this include file, first define either INCLUDE_AS_ENUM or INCLUDE_AS_STRING, to include the screens as enums, or string constants.
#endif


ADD_SCREEN(NONE,GenericScreen)
ADD_SCREEN(MAIN,ScreenMain)

ADD_SCREEN(ONE,ScreenOne)
ADD_SCREEN(TWO,ScreenTwo)
ADD_SCREEN(THREE,ScreenThree)

//this must be the last id
ADD_SCREEN(UNASSIGNED,GenericScreen)



