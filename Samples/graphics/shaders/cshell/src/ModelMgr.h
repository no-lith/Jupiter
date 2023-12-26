//------------------------------------------------------------------------------//
//
// MODULE   : ModelMgr.h
//
// PURPOSE  : Loads models in rows and columns
//
// CREATED  : 09/18/2003
//
// (c) 2003 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#ifndef __MODELMGR_H__
#define __MODELMGR_H__


#include <iclientshell.h>
#include <ltbasetypes.h>
#include <ltbasedefs.h>



class ModelMgr
{
public:

	ModelMgr();
	~ModelMgr();

	// initialize
	LTRESULT				Init();
	
	// terminate
	LTRESULT				Term();

	// singleton access
	static ModelMgr&		GetSingleton();

	// counts
	unsigned				GetNumRows() const				{ return m_nRows; }
	unsigned				GetNumCols() const				{ return m_nCols; }
	unsigned				GetNumModels() const			{ return m_nRows * m_nCols; }
	unsigned				GetCurrentModel() const			{ return m_nCurModel; }

	// row
	void					AddRow();
    void					SubtractRow();

	// column
	void					AddCol();
    void					SubtractCol();

	// current model
	void					PrevModel();
	void					NextModel();

	// reload the current model array
	void					ReloadModels();

	// remove all the models
	void					RemoveAllModels();

private:

	void					AddModel(unsigned Row, unsigned Col);

private:

	enum
	{
		MAX_ROWCOL = 256
	};

	// models
    unsigned          		m_nCols;
    unsigned          		m_nRows;
	unsigned				m_nCurModel;
    HOBJECT         		m_hModels[MAX_ROWCOL][MAX_ROWCOL];
};


// global
#define g_ModelMgr			ModelMgr::GetSingleton()



#endif // __MODELMGR_H__
