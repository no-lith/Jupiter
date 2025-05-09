#ifndef _SERVERUTILITIES_H_
#define _SERVERUTILITIES_H_

#include "ltbasetypes.h"
#include "ltbasedefs.h"
#include "AutoMessage.h"

// Save/Load Macros

#define SAVE_BYTE(variable) pMsg->Writeuint8(variable);
#define LOAD_BYTE(variable) variable = pMsg->Readuint8();
#define LOAD_BYTE_CAST(variable, clazz) variable = (clazz)pMsg->Readuint8();
#define SAVE_WORD(variable) pMsg->Writeuint16(variable);
#define LOAD_WORD(variable) variable = pMsg->Readuint16();
#define LOAD_WORD_CAST(variable, clazz) variable = (clazz)pMsg->Readuint16();
#define SAVE_INT(variable) pMsg->Writeint32(variable);
#define LOAD_INT(variable) variable = pMsg->Readint32();
#define LOAD_INT_CAST(variable, clazz) variable = (clazz)pMsg->Readint32();
#define SAVE_BOOL(variable) pMsg->Writebool(variable != LTFALSE);
#define LOAD_BOOL(variable) variable = (pMsg->Readbool() ? LTTRUE : LTFALSE);
#define SAVE_bool(variable) pMsg->Writebool(variable);
#define LOAD_bool(variable) variable = pMsg->Readbool();
#define SAVE_DWORD(variable) pMsg->Writeuint32(variable);
#define LOAD_DWORD(variable) variable = pMsg->Readuint32();
#define LOAD_DWORD_CAST(variable, clazz) variable = (clazz)pMsg->Readuint32();
#define SAVE_QWORD(variable) pMsg->Writeuint64( variable );
#define LOAD_QWORD(variable) variable = pMsg->Readuint64();
#define LOAD_QWORD_CAST(variable, clazz) variable = (clazz)pMsg->Readuint64();
#define SAVE_FLOAT(variable) pMsg->Writefloat(variable);
#define LOAD_FLOAT(variable) variable = pMsg->Readfloat();
#define SAVE_HOBJECT(variable) pMsg->WriteObject(variable);
#define LOAD_HOBJECT(variable) variable = pMsg->ReadObject();
#define SAVE_VECTOR(variable)   pMsg->WriteLTVector(variable);
#define LOAD_VECTOR(variable)   variable = pMsg->ReadLTVector();
#define SAVE_ROTATION(variable) pMsg->WriteLTRotation(variable);
#define LOAD_ROTATION(variable) variable = pMsg->ReadLTRotation();
#define SAVE_HSTRING(variable)  pMsg->WriteHString(variable);
#define LOAD_HSTRING(variable)  variable = pMsg->ReadHString();
#define SAVE_CHARSTRING(variable)  pMsg->WriteString(variable);
#define LOAD_CHARSTRING(variable, nsize)  pMsg->ReadString(variable, nsize);
#define SAVE_STDSTRING(variable) pMsg->WriteString((variable).c_str());
#define LOAD_STDSTRING(variable) { (variable).resize(pMsg->PeekString(0,0) + 1); pMsg->ReadString((variable).begin(), (variable).size()); }
#define SAVE_RANGE(variable) pMsg->Writefloat((float)variable.GetMin()); pMsg->Writefloat((float)variable.GetMax());
#define LOAD_RANGE(variable) variable.Set(pMsg->Readfloat(), pMsg->Readfloat());
#define LOAD_RANGE_CAST(variable, type) variable.Set((type)pMsg->Readfloat(), (type)pMsg->Readfloat());
#define SAVE_TIME(variable) pMsg->Writefloat(( variable ) - g_pLTServer->GetTime( ));
#define LOAD_TIME(variable) variable = g_pLTServer->GetTime( ) + pMsg->Readfloat( );
#define SAVE_LITEOBJECT(variable) pMsg->Writeuint32(g_pGameServerShell->GetLiteObjectMgr()->GetSerializeID((GameBaseLite*)variable));
#define LOAD_LITEOBJECT(variable, type) (variable) = (type*)g_pGameServerShell->GetLiteObjectMgr()->GetSerializeObject(pMsg->Readuint32());
#define SAVE_COBJECT_INTERNAL(variable) pMsg->WriteObject(variable ? ((ILTBaseClass*)variable)->m_hObject : LTNULL);
#define LOAD_COBJECT_INTERNAL(variable, type) { HOBJECT hObject = pMsg->ReadObject(); variable = hObject ? (type*)g_pLTServer->HandleToObject(hObject) : LTNULL;}
#define SAVE_COBJECT(variable) { if (variable && ((ILTBaseClass*)variable)->m_hObject == 0) { SAVE_bool(true); SAVE_LITEOBJECT(variable); } else { SAVE_bool(false); SAVE_COBJECT_INTERNAL(variable); } }
#define LOAD_COBJECT(variable, type) { if (pMsg->Readbool()) { LOAD_LITEOBJECT(variable, type); } else { LOAD_COBJECT_INTERNAL(variable, type); } }

//
// LINKFROM_MODULE
// LINKTO_MODULE
//
// These macros can be used to ensure a module does not get optimized out by the linker when
// linking a static lib into a dll or exe.  If a module in a static lib contains symbols
// that are never referenced by the dll or exe linking with it, then the module will not
// be used in the link.  To ensure a module gets used establish a link between the unreferenced
// module and a known referenced module.  The referenced module can be part of the static lib
// as well, but it must be somehow connected to modules in the dll or exe.  The need for
// this comes from the use of class factory like architecture, where dummy objects use
// their constructors to do work, but the objects themselves are never referenced.
//
// To use, put the LINKFROM_MODULE(linkfrom_module_name) into the unreferenced module cpp, where
// linkfrom_module_name is the name unreferenced module.  Then put the LINKTO_MODULE(linkto_module_name) into the
// known referenced module where the linkto_module_name is the name of the unreferenced module.
//
#define LINKFROM_MODULE(linkfrom_module_name)					\
	int g_n##linkfrom_module_name = 0;

#define LINKTO_MODULE(linkto_module_name)						\
	extern int g_n##linkto_module_name;							\
	static int* s_pn##linkto_module_name = &g_n##linkto_module_name;

LTBOOL MoveObjectToFloorWithDims(HOBJECT hObj, HOBJECT *pFilterList = NULL, ObjectFilterFn pFilterFn = NULL);

// Utility functions for filtering intersection queries

//this will filter out all objects in the list, which is a list of HOBJECTs, terminated
//by NULL
inline bool ObjListFilterFn(HOBJECT hTest, void *pUserData)
{
	// Filters out objects for a raycast.  pUserData is a list of HOBJECTS terminated
	// with a NULL HOBJECT.
	HOBJECT *hList = (HOBJECT*)pUserData;
	while(hList && *hList)
	{
		if(hTest == *hList)
			return false;
		++hList;
	}
	return true;
}

void PlayClientFX(char* szFXName, HOBJECT hParent, uint32 dwFlags);
void PlayClientFX(char* szFXName, HOBJECT hParent, LTVector* pvPos, LTRotation *prRot, uint32 dwFlags);
void PlayClientFX(char* szFXName, HOBJECT hParent, HOBJECT hTarget, LTVector* pvPos, LTRotation *prRot, LTVector *pvTargetPos, uint32 dwFlags);


#endif