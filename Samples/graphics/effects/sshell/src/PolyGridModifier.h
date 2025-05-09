// ----------------------------------------------------------------------- //
//
// MODULE  : PolyGridModifier.h
//
// PURPOSE : Defines the object PolyGridModifier which exists entirely for
//			 creating modifiers for a polygrid so that they can be controlled
//			 easier by artists. Their data will be taken by the polygrids
//			 and used when updating the surface
//
// CREATED : 1/11/02
//
// ----------------------------------------------------------------------- //

#ifndef __POLYGRIDMODIFIER_H__
#define __POLYGRIDMODIFIER_H__

#include "ltengineobjects.h"
#include "GameBase.h"

class PolyGridModifier : public GameBase
{
	public :

		PolyGridModifier();
		~PolyGridModifier();

		//the dimensions of this modifier
		LTVector	m_vDims;

		//each time we accelerate, we need to accelerate N samples in the area
		uint32		m_nNumAccelPoints;

		//for each point we accelerate, we need to accelerate it this much
		float		m_fAccelAmount;

		//whether or not this modifier starts out enabled or not
		LTBOOL		m_bStartEnabled;

	protected :

        uint32      EngineMessageFn(uint32 messageID, void *pData, LTFLOAT fData);

	private :

        LTBOOL ReadProp(ObjectCreateStruct *pData);
        LTBOOL InitialUpdate(LTVector *pMovement);
};

#endif // __POLYGRIDMODIFIER_H__