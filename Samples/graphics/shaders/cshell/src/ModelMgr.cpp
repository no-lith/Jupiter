//------------------------------------------------------------------------------//
//
// MODULE   : ModelMgr.cpp
//
// PURPOSE  : Loads models in rows and columns
//
// CREATED  : 09/18/2003
//
// (c) 2003 LithTech, Inc.  All Rights Reserved
//
//------------------------------------------------------------------------------//

#include "ModelMgr.h"
#include "clientinterfaces.h"
#include "clienthelper.h"
#include "ShaderMgr.h"

#include <iltclient.h>
#include <iltmodel.h>
#include <ltobjectcreate.h>



// Models
enum SampleModels
{
	SAMPLEMODEL_TDSPHERE = 0,
	SAMPLEMODEL_SEAL,
	SAMPLEMODEL_SUPERJ,
	SAMPLEMODEL_TORELLO,
	SAMPLEMODEL_CATE,
	SAMPLEMODEL_MAX_MODELS
};



//---------------------------------------------------------------------------
//  Name:         ModelMgr
//
//  Purpose:	  constructor
//
//  Returns:      n/a
//---------------------------------------------------------------------------
ModelMgr::ModelMgr()
	: m_nCols(0),
	  m_nRows(0),
	  m_nCurModel(0)
{
	// Initialize our array of models.
	for (unsigned i = 0; i < MAX_ROWCOL; ++i)
	{
		for (unsigned j = 0; j < MAX_ROWCOL; ++j)
		{
			m_hModels[i][j] = NULL;
		}
	}
}



//---------------------------------------------------------------------------
//  Name:         ~ModelMgr
//
//  Purpose:	  destructor
//
//  Returns:      n/a
//---------------------------------------------------------------------------
ModelMgr::~ModelMgr()
{
	Term();
}



//---------------------------------------------------------------------------
//  Name:         Init
//
//  Purpose:	  Initialize
//
//  Returns:      LTRESULT
//---------------------------------------------------------------------------
LTRESULT ModelMgr::Init()
{
	LTRESULT result = LT_OK;


	return result;
}


//---------------------------------------------------------------------------
//  Name:         Term
//
//  Purpose:	  Terminate
//
//  Returns:      LTRESULT
//---------------------------------------------------------------------------
LTRESULT ModelMgr::Term()
{
	LTRESULT result = LT_OK;

	RemoveAllModels();

	return result;
}


//---------------------------------------------------------------------------
//  Name:         GetSingleton
//
//  Purpose:	  ModelMgr is a singleton
//
//  Returns:	  ModelMgr&
//
//---------------------------------------------------------------------------
ModelMgr& ModelMgr::GetSingleton()
{
	static ModelMgr s_Singleton;
	return s_Singleton;
}



//---------------------------------------------------------------------------
//  Name:         AddRow
//
//  Purpose:	  Adds a row of models
//
//  Returns:      void
//---------------------------------------------------------------------------
void ModelMgr::AddRow()
{
    if (m_nRows > MAX_ROWCOL)
	{
        return;
	}

    if (m_nCols == 0)
	{
       ++m_nCols;
	}

    for (unsigned i = 0; i < m_nCols; ++i)
    {
		AddModel(m_nRows, i);
    }

    ++m_nRows;
}



//---------------------------------------------------------------------------
//  Name:         SubtractRow
//
//  Purpose:	  Subtracts a row of models
//
//  Returns:      void
//---------------------------------------------------------------------------
void ModelMgr::SubtractRow()
{
    if (m_nRows <= 0)
	{
        return;
	}

    for (unsigned i = 0; i < m_nCols; ++i)
    {
        g_pLTClient->RemoveObject(m_hModels[m_nRows - 1][i]);
        m_hModels[m_nRows - 1][i] = NULL;
    }

    --m_nRows;

	if (GetNumModels() == 0)
	{
		RemoveAllModels();
	}
}



//---------------------------------------------------------------------------
//  Name:         AddCol
//
//  Purpose:	  Adds a column of models
//
//  Returns:      void
//---------------------------------------------------------------------------
void ModelMgr::AddCol()
{
    if (m_nCols > MAX_ROWCOL)
	{
        return;
	}

    if (m_nRows == 0)
	{
        ++m_nRows;
	}

    for (unsigned i = 0; i < m_nRows; ++i)
    {
		AddModel(i, m_nCols);
    }

    ++m_nCols;
}



//---------------------------------------------------------------------------
//  Name:         SubtractCol
//
//  Purpose:	  Subtracts a column of models
//
//  Returns:      void
//---------------------------------------------------------------------------
void ModelMgr::SubtractCol()
{
    if (m_nCols <= 0)
	{
        return;
	}

    for (unsigned i = 0; i < m_nRows; ++i)
    {
        g_pLTClient->RemoveObject(m_hModels[i][m_nCols - 1]);
        m_hModels[i][m_nCols - 1] = NULL;
    }

    --m_nCols;

	if (GetNumModels() == 0)
	{
		RemoveAllModels();
	}
}




//---------------------------------------------------------------------------
//  Name:         PrevModel
//
//  Purpose:	  decrements the current model index
//
//  Returns:	  void
//
//---------------------------------------------------------------------------
void ModelMgr::PrevModel()
{
	int CurrIndex = m_nCurModel - 1;
	if (CurrIndex < 0)
	{
		CurrIndex = SAMPLEMODEL_MAX_MODELS - 1;
	}
	m_nCurModel = CurrIndex;

	ReloadModels();
}



//---------------------------------------------------------------------------
//  Name:         NextModel
//
//  Purpose:	  increments the current model index
//
//  Returns:	  void
//
//---------------------------------------------------------------------------
void ModelMgr::NextModel()
{
	int CurrIndex = m_nCurModel + 1;
	if (CurrIndex >= SAMPLEMODEL_MAX_MODELS)
	{
		CurrIndex = 0;
	}
	m_nCurModel = CurrIndex;

	ReloadModels();
}



//---------------------------------------------------------------------------
//  Name:         ReloadModels
//
//  Purpose:	  Reloads the array of models
//
//  Returns:	  void
//
//---------------------------------------------------------------------------
void ModelMgr::ReloadModels()
{
	if (m_nRows == 0 &&
		m_nCols == 0)
	{
		return;
	}

	uint32 nTmpRows = m_nRows;
	uint32 nTmpCols = m_nCols - 1;

	RemoveAllModels();

	for (uint32 i = 0; i < nTmpRows; ++i)
	{
		AddRow();
	}

	for (uint32 j = 0; j < nTmpCols; ++j)
	{
		AddCol();
	}
}



//---------------------------------------------------------------------------
//  Name:         RemoveAllModels
//
//  Purpose:	  Removes the the array of models
//
//  Returns:	  void
//
//---------------------------------------------------------------------------
void ModelMgr::RemoveAllModels()
{
	for (unsigned i = 0; i < m_nRows; ++i)
	{
		for (unsigned j = 0; j < m_nCols; ++j)
		{
			g_pLTClient->RemoveObject(m_hModels[i][j]);
			m_hModels[i][j] = NULL;
		}
	}

	m_nRows = 0;
	m_nCols = 0;
}



//---------------------------------------------------------------------------
//  Name:         AddModel
//
//  Purpose:	  Add a model to the given row and column
//
//  Returns:      n/a
//---------------------------------------------------------------------------
void ModelMgr::AddModel(unsigned Row, unsigned Col)
{
	if (Row >= MAX_ROWCOL ||
		Col >= MAX_ROWCOL)
	{
		return;
	}

	ObjectCreateStruct ocs;
    ocs.Clear();

    ocs.m_ObjectType = OT_MODEL;
    ocs.m_Flags = FLAG_VISIBLE | FLAG_NOLIGHT;

	// Create the model object.
	switch (m_nCurModel)
	{
		case SAMPLEMODEL_TDSPHERE:
		{
			// renderstyles
			switch (g_ShaderMgr.GetCurrentShader())
			{
				case SHADERTYPE_FIXEDFUNCTION:
				{
					// model
					strcpy(ocs.m_Filename, "Models\\tdsphere.ltb");

					// textures
					strcpy(ocs.m_SkinNames[0], "ModelTextures\\TDlogo.dtx");

					break;
				}

				case SHADERTYPE_TOON:
				{
					// model
					strcpy(ocs.m_Filename, "Models\\TDSphereToon.ltb");

					// textures
					strcpy(ocs.m_SkinNames[0], "ModelTextures\\toon.dtx");
					strcpy(ocs.m_SkinNames[1], "ModelTextures\\TDlogo.dtx");

					// renderstyle
					strcpy(ocs.m_RenderStyleNames[0], "RS\\SkelZeroToon.ltb");

					break;
				}

				default:
				{
 					g_pLTClient->DebugOut("Error: Unknown shader index!\n");
					break;
				}
			}

			// position
			ocs.m_Pos.Init(120.0f * (float)Col, 0.0f, 120.0f * (float)Row);

			// Create the object.
			HLOCALOBJ hObj = g_pLTClient->CreateObject(&ocs);
			m_hModels[Row][Col] = hObj;

			break;
		}

		case SAMPLEMODEL_SEAL:
		{
			// renderstyles
			switch (g_ShaderMgr.GetCurrentShader())
			{
				case SHADERTYPE_FIXEDFUNCTION:
				{
					// model
					strcpy(ocs.m_Filename, "Models\\seal.ltb");

					// textures
					strcpy(ocs.m_SkinNames[0], "ModelTextures\\seal.dtx");

					break;
				}

				case SHADERTYPE_TOON:
				{
					// model
					strcpy(ocs.m_Filename, "Models\\SealToon.ltb");

					// textures
					strcpy(ocs.m_SkinNames[0], "ModelTextures\\toon.dtx");
					strcpy(ocs.m_SkinNames[1], "ModelTextures\\seal.dtx");

					// renderstyle
					strcpy(ocs.m_RenderStyleNames[0], "RS\\SkelThreeToon.ltb");

					break;
				}

				default:
				{
 					g_pLTClient->DebugOut("Error: Unknown shader index!\n");
					break;
				}
			}

			// position
			ocs.m_Pos.Init(120.0f * (float)Col, 0.0f, 120.0f * (float)Row);

			// Create the object.
			HLOCALOBJ hObj = g_pLTClient->CreateObject(&ocs);
			m_hModels[Row][Col] = hObj;

			// animation
			HMODELANIM hAnim = g_pLTClient->GetAnimIndex(hObj, "idleAnim");
			g_pLTClient->SetModelAnimation(hObj, hAnim);
			g_pLTClient->SetModelLooping(hObj, LTTRUE);

			break;
		}

		case SAMPLEMODEL_SUPERJ:
		{
			// renderstyles
			switch (g_ShaderMgr.GetCurrentShader())
			{
				case SHADERTYPE_FIXEDFUNCTION:
				{
					// model
					strcpy(ocs.m_Filename, "Models\\SuperJ.ltb");

					// textures
					strcpy(ocs.m_SkinNames[0], "ModelTextures\\superj.dtx");

					break;
				}

				case SHADERTYPE_TOON:
				{
					// model
					strcpy(ocs.m_Filename, "Models\\SuperJToon.ltb");

					// textures
					strcpy(ocs.m_SkinNames[0], "ModelTextures\\toon.dtx");
					strcpy(ocs.m_SkinNames[1], "ModelTextures\\superj.dtx");

					// renderstyle
					strcpy(ocs.m_RenderStyleNames[0], "RS\\SkelZeroToon.ltb");
					strcpy(ocs.m_RenderStyleNames[1], "RS\\SkelTwoToon.ltb");
					strcpy(ocs.m_RenderStyleNames[2], "RS\\SkelTwoToon.ltb");

					break;
				}

				default:
				{
 					g_pLTClient->DebugOut("Error: Unknown shader index!\n");
					break;
				}
			}

			// position
			ocs.m_Pos.Init(120.0f * (float)Col, 0.0f, 120.0f * (float)Row);

			// Create the object.
			HLOCALOBJ hObj = g_pLTClient->CreateObject(&ocs);
			m_hModels[Row][Col] = hObj;

			// animation
			HMODELANIM hAnim = g_pLTClient->GetAnimIndex(hObj, "Idle");
			g_pLTClient->SetModelAnimation(hObj, hAnim);
			g_pLTClient->SetModelLooping(hObj, LTTRUE);

			break;
		}

		case SAMPLEMODEL_TORELLO:
		{
			// renderstyles
			switch (g_ShaderMgr.GetCurrentShader())
			{
				case SHADERTYPE_FIXEDFUNCTION:
				{
					// model
					strcpy(ocs.m_Filename, "Models\\JohnnyTorrello.ltb");

					// textures
					strcpy(ocs.m_SkinNames[0], "ModelTextures\\JohnnyTorrello_face.dtx");
					strcpy(ocs.m_SkinNames[1], "ModelTextures\\JohnnyTorrello_body.dtx");

					break;
				}

				case SHADERTYPE_TOON:
				{
					// model
					strcpy(ocs.m_Filename, "Models\\JohnnyTorrelloToon.ltb");

					// textures
					strcpy(ocs.m_SkinNames[0], "ModelTextures\\toon.dtx");
					strcpy(ocs.m_SkinNames[1], "ModelTextures\\JohnnyTorrello_face.dtx");
					strcpy(ocs.m_SkinNames[2], "ModelTextures\\JohnnyTorrello_body.dtx");

					// renderstyle
					strcpy(ocs.m_RenderStyleNames[0], "RS\\SkelOneToon.ltb");
					strcpy(ocs.m_RenderStyleNames[1], "RS\\SkelZeroToon.ltb");
					strcpy(ocs.m_RenderStyleNames[2], "RS\\SkelThreeToon.ltb");

					break;
				}

				default:
				{
 					g_pLTClient->DebugOut("Error: Unknown shader index!\n");
					break;
				}
			}

			// position
			ocs.m_Pos.Init(120.0f * (float)Col, 0.0f, 120.0f * (float)Row);

			// Create the object.
			HLOCALOBJ hObj = g_pLTClient->CreateObject(&ocs);
			m_hModels[Row][Col] = hObj;

			// Play an animation.
			HMODELANIM hAnim = g_pLTClient->GetAnimIndex(hObj, "3StWaGd3");
			g_pLTClient->SetModelAnimation(hObj, hAnim);
			g_pLTClient->SetModelLooping(hObj, LTTRUE);

			break;
		}

		case SAMPLEMODEL_CATE:
		{
			// renderstyles
			switch (g_ShaderMgr.GetCurrentShader())
			{
				case SHADERTYPE_FIXEDFUNCTION:
				{
					// model
					strcpy(ocs.m_Filename, "Models\\player_action.ltb");

					// textures
					strcpy(ocs.m_SkinNames[0], "ModelTextures\\CateAction.dtx");
					strcpy(ocs.m_SkinNames[1], "ModelTextures\\CateActionHead.dtx");
					strcpy(ocs.m_SkinNames[2], "ModelTextures\\CateActionHair.dtx");
					strcpy(ocs.m_SkinNames[3], "ModelTextures\\CateActionLash.dtx");

					break;
				}

				case SHADERTYPE_TOON:
				{
					// model
					strcpy(ocs.m_Filename, "Models\\player_action_Toon.ltb");

					// textures
					strcpy(ocs.m_SkinNames[0], "ModelTextures\\toon.dtx");
					strcpy(ocs.m_SkinNames[1], "ModelTextures\\CateAction.dtx");
					strcpy(ocs.m_SkinNames[2], "ModelTextures\\CateActionHead.dtx");
					strcpy(ocs.m_SkinNames[3], "ModelTextures\\CateActionHair.dtx");
					strcpy(ocs.m_SkinNames[4], "ModelTextures\\CateActionLash.dtx");

					// renderstyle
					strcpy(ocs.m_RenderStyleNames[0], "RS\\SkelZeroToon.ltb");
					strcpy(ocs.m_RenderStyleNames[1], "RS\\SkelThreeToon.ltb");
					strcpy(ocs.m_RenderStyleNames[2], "RS\\SkelThreeToon.ltb");

					break;
				}

				default:
				{
 					g_pLTClient->DebugOut("Error: Unknown shader index!\n");
					break;
				}
			}

			// position
			ocs.m_Pos.Init(120.0f * (float)Col, 0.0f, 120.0f * (float)Row);

			// Create the object.
			HLOCALOBJ hObj = g_pLTClient->CreateObject(&ocs);
			m_hModels[Row][Col] = hObj;

			// Play an animation.
			HMODELANIM hAnim = g_pLTClient->GetAnimIndex(hObj, "UUR");
			g_pLTClient->SetModelAnimation(hObj, hAnim);
			g_pLTClient->SetModelLooping(hObj, LTTRUE);

			break;
		}

		default:
		{
			break;
		}
	}
}
