// ----------------------------------------------------------------------- //
//
// MODULE  : ClientUtil.h
//
// PURPOSE : Helper functions
//
// CREATED : 2002/10/11
//
// (c) 2002 LithTech, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __CLIENTUTIL_H__
#define __CLIENTUTIL_H__


namespace ClientUtil
{

	// WrapToRange -- forces "pType" into the given range, wrapping the value around if necessary.
	template< typename Type >
	void WrapToRange(Type *pType, Type MinVal, Type MaxVal)
	{
		Type Diff = MaxVal - MinVal + static_cast<Type>(1);
		if (*pType < MinVal)
		{
			while (*pType < MinVal)
			{
				*pType += Diff;
			}
		}
		if (*pType > MaxVal)
		{
			while (*pType > MaxVal)
			{
				*pType -= Diff;
			}
		}
	}

}


#endif // __CLIENTUTIL_H__
