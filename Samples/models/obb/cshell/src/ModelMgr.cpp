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

#include <iltclient.h>
#include <iltmodel.h>
#include <ltobjectcreate.h>
#include <iltphysics.h>
#include <iltcommon.h>



// Models
enum SampleModels
{
	SAMPLEMODEL_ORDOG = 0,
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
	  m_nCurModel(0),
	  m_hSphere(NULL),
	  m_hSquare(NULL),
	  m_bShowBoxes(true) // this assumes the autoexec.cfg set the boxes to true.
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

	// Intialize the Sphere and Square debugging models.
	CreateDebugGeometry();

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

	// debug geometry
	RemoveDebugGeometry();

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
	ocs.m_Flags2 = FLAG2_USEMODELOBBS;

	// Create the model object.
	switch (m_nCurModel)
	{
		case SAMPLEMODEL_ORDOG:
		{
			// model
			strcpy(ocs.m_Filename, "Models\\ordog.ltb");

			// textures
			strcpy(ocs.m_SkinNames[1], "modeltextures/ordog/fourpack.dtx");			//Arms
			strcpy(ocs.m_SkinNames[2], "modeltextures/ordog/ordogarm1.dtx");		//Lower back fins/upper arms
			strcpy(ocs.m_SkinNames[3], "modeltextures/ordog/ordogchest2.dtx");		//Chest
			strcpy(ocs.m_SkinNames[4], "modeltextures/ordog/ordoghead1.dtx");		//head
			strcpy(ocs.m_SkinNames[5], "modeltextures/ordog/ordogleg2.dtx");		//legs
			strcpy(ocs.m_SkinNames[6], "modeltextures/ordog/ordogleg3.dtx");		//legs 2
			strcpy(ocs.m_SkinNames[7], "modeltextures/ordog/ordogparts1.dtx");		//crotch
			strcpy(ocs.m_SkinNames[8], "modeltextures/ordog/ordogparts2.dtx");		//under the chest
			strcpy(ocs.m_SkinNames[9], "modeltextures/ordog/ordogshoulder1.dtx");	//Thrusters
			strcpy(ocs.m_SkinNames[10], "modeltextures/ordog/ordogshoulder2.dtx");	//Shoulder

			ocs.m_Pos.Init();
			HLOCALOBJ hObj = g_pLTClient->CreateObject(&ocs);
			if (NULL == hObj)
			{
				return;
			}

			// Store our handle
			m_hModels[Row][Col] = hObj;

			// Calculate the starting position for this object.
			LTVector vDims;
			HMODELANIM hCurAnim;
			g_pLTCModel->GetCurAnim(hObj, MAIN_TRACKER, hCurAnim);
			g_pLTCCommon->GetModelAnimUserDims(hObj, &vDims, hCurAnim);
			ocs.m_Pos.Init(vDims.x * 3 * (float)Col, 0.0f, vDims.z * 3 * (float)Row);
			g_pLTClient->SetObjectPos(hObj, &ocs.m_Pos);

			break;
		}

		case SAMPLEMODEL_CATE:
		{
			// model
			strcpy(ocs.m_Filename, "Models\\player_action.ltb");

			// textures
			strcpy(ocs.m_SkinNames[0], "ModelTextures\\CateAction.dtx");
			strcpy(ocs.m_SkinNames[1], "ModelTextures\\CateActionHead.dtx");
			strcpy(ocs.m_SkinNames[2], "ModelTextures\\CateActionHair.dtx");
			strcpy(ocs.m_SkinNames[3], "ModelTextures\\CateActionLash.dtx");

			ocs.m_Pos.Init();
			HLOCALOBJ hObj = g_pLTClient->CreateObject(&ocs);
			if (NULL == hObj)
			{
				return;
			}

			// Store our handle
			m_hModels[Row][Col] = hObj;

			// Calculate the starting position for this object.
			LTVector vDims;
			HMODELANIM hCurAnim;
			g_pLTCModel->GetCurAnim(hObj, MAIN_TRACKER, hCurAnim);
			g_pLTCCommon->GetModelAnimUserDims(hObj, &vDims, hCurAnim);
			ocs.m_Pos.Init(vDims.x * 3 * (float)Col, 0.0f, vDims.z * 3 * (float)Row);
			g_pLTClient->SetObjectPos(hObj, &ocs.m_Pos);

			// Play an animation.
			HMODELANIM hAnim = g_pLTClient->GetAnimIndex(hObj, "LWF");
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



//---------------------------------------------------------------------------
//  Name:         CheckModelObbs
//
//  Purpose:	  Check for ray intersection on Model OBBs.
//
//  Returns:      n/a
//---------------------------------------------------------------------------
void ModelMgr::CheckModelObbs(LTVector &vPos, LTRotation &rRot, char *pHitText)
{
	// Check for line intersection
    IntersectQuery qInfo;
	IntersectInfo iInfo;

    LTVector vF = rRot.Forward();

	qInfo.m_From = vPos + vF;
	qInfo.m_To   = vPos + (vF * 10000.0f);

	qInfo.m_Flags =  INTERSECT_OBJECTS | INTERSECT_MODELOBBS;

    if (g_pLTClient->IntersectSegment(&qInfo, &iInfo))
	{
		if (iInfo.m_hObject)
		{
			if (g_pLTCPhysics->IsWorldObject( iInfo.m_hObject ) == LT_YES)
			{
				strcpy(pHitText, "No hit");

				// Make sure the debug objects are NOT visible
				SetDebugObjectsVisible(false);
			}
			else
			{
				char buf[1024];
				if (iInfo.m_hNode != INVALID_MODEL_NODE)
				{
					// Setup a few useful variables
					uint32 nNumOBBS = 0;
					ModelOBB *pObbs = NULL;
                    float radius = 0;
                    uint32 iObb = 0;
					char tmp[256];
					tmp[0] = '\0';

					// Get the number of OBBs in our model
					g_pLTCModel->GetNumModelOBBs(iInfo.m_hObject, nNumOBBS);

					if (nNumOBBS > 0)
					{
						pObbs = new ModelOBB[nNumOBBS];
						g_pLTCModel->GetModelOBBCopy(iInfo.m_hObject, pObbs);
						for (iObb = 0; iObb < nNumOBBS; ++iObb)
						{
							if (pObbs[iObb].m_iNode == iInfo.m_hNode)
							{
								// Get our OBBs "best fit" radius
                                radius = pObbs[iObb].m_Radius;

								// Get info about our OBB
								sprintf(tmp, "\nOBB Dims: %f %f %f\nOBB Radius: %f\n",
										VEC_EXPAND(pObbs[iObb].m_Size), pObbs[iObb].m_Radius);
                                break;
							}
						}
					}

                    // Store our hit Node's name.
					char buf2[512];
					g_pLTCModel->GetNodeName(iInfo.m_hObject, iInfo.m_hNode, buf2, 128);
					sprintf(buf, "Node Hit: %s", buf2);
					strncat(buf, tmp, 128);

					// Store our point of intersection
					memset(buf2, 0, 512);
					sprintf(buf2, "Point of Intersection: %f %f %f", VEC_EXPAND(iInfo.m_Point));
					strncat(buf, buf2, 512);

					// Get our Node position/rotation information
                    LTransform tr;
                    g_pLTCModel->GetNodeTransform(iInfo.m_hObject, iInfo.m_hNode, tr, true);

                    // Translate our OBB position
                    LTMatrix obb_mat;
                    obb_mat.Identity();
					const LTVector
						vRight = tr.m_Rot.Right(),
						vUp = tr.m_Rot.Up(),
						vForward = tr.m_Rot.Forward();

                    obb_mat.SetBasisVectors( &vRight, &vUp, &vForward );
                    obb_mat.SetTranslation( tr.m_Pos );
					obb_mat.Apply(pObbs[iObb].m_Pos);

                    // Set the position of our debugging sphere
					g_pLTClient->SetObjectPos(m_hSphere, &pObbs[iObb].m_Pos);
                    LTVector vScale(radius, radius, radius);
                    g_pLTClient->SetObjectScale(m_hSphere, &vScale);
					LTVector vDims(0.0f, 0.0f, 0.0f);
                    g_pLTCPhysics->SetObjectDims(m_hSphere, &vDims, 0);

                    // Setup OBB rot
                    LTMatrix mObbMat;
                    mObbMat.SetBasisVectors(&pObbs[iObb].m_Basis[0], &pObbs[iObb].m_Basis[1], &pObbs[iObb].m_Basis[2]);

                    // Get matrix of our Node's rotation
                    LTMatrix mTrMat;
                    tr.m_Rot.ConvertToMatrix(mTrMat);

                    // Apply our node rotation to our obb rotation
                    mTrMat.Apply(mObbMat);

                    // Convert back to an LTRotation
                    LTRotation rRot;
                    rRot.ConvertFromMatrix(mObbMat);

                    // Set the pos/rot of our debugging square
					g_pLTClient->SetObjectPos(m_hSquare, &pObbs[iObb].m_Pos);
                    g_pLTClient->SetObjectRotation(m_hSquare, &rRot);

					// Set the scale of our debugging square
                    LTVector vScale2(pObbs[iObb].m_Size.x , pObbs[iObb].m_Size.y, pObbs[iObb].m_Size.z);
                    g_pLTClient->SetObjectScale(m_hSquare, &vScale2);
                    g_pLTCPhysics->SetObjectDims(m_hSquare, &vDims, 0);

					// Set our recently stored "hit" information. (including point of intersection etc...)
                    strcpy(pHitText, buf);

					// Make sure the debug objects are visible
					SetDebugObjectsVisible(true);

					// Free our copy of the modelobbs
					if (NULL != pObbs)
					{
						delete[] pObbs;
						pObbs = NULL;
					}
				}
				else
				{
    				sprintf(pHitText, "Hit! (No OBBs)\nPos: %f %f %f", VEC_EXPAND(iInfo.m_Point));
				}
			}
		}
		else
		{
			strcpy(pHitText, "No hit");

			// Make sure the debug objects are NOT visible
			SetDebugObjectsVisible(false);
		}
	}
	else
	{
        strcpy(pHitText, "No hit");

		// Make sure the debug objects are NOT visible
		SetDebugObjectsVisible(false);
	}
}



//---------------------------------------------------------------------------
//  Name:         CreateDebugGeometry
//
//  Purpose:	  Create the debug OBBs and spheres.
//
//  Returns:      n/a
//---------------------------------------------------------------------------
void ModelMgr::CreateDebugGeometry()
{
	ObjectCreateStruct ocs;

    // Create Debugging Sphere
	ocs.Clear();
	ocs.m_ObjectType = OT_MODEL;
	ocs.m_Flags = FLAG_VISIBLE | FLAG_NOLIGHT;
    ocs.m_Flags2 |= FLAG2_FORCETRANSLUCENT;
    strncpy(ocs.m_Filename, "models/2x2sphere.ltb", 127);
    strncpy(ocs.m_RenderStyleName , "RenderStyles/Translucent.ltb", 127);
	m_hSphere = g_pLTClient->CreateObject(&ocs);
    float r = 0.2f;
    float g = 0.2f;
    float b = 0.8f;
    float a = 0.35f;
    g_pLTClient->SetObjectColor(m_hSphere, r, g, b, a);

    // Create Debugging Square
	ocs.Clear();
	ocs.m_ObjectType = OT_MODEL;
	ocs.m_Flags = FLAG_VISIBLE | FLAG_NOLIGHT;
    ocs.m_Flags2 |= FLAG2_FORCETRANSLUCENT;
    strncpy(ocs.m_Filename, "models/1x1square.ltb", 127);
    strncpy(ocs.m_RenderStyleName , "RenderStyles/Translucent.ltb", 127);
	m_hSquare = g_pLTClient->CreateObject(&ocs);
    r = 0.8f;
    g = 0.2f;
    b = 0.2f;
    a = 0.9f;
	g_pLTClient->SetObjectColor(m_hSquare, r, g, b, a);
}



//---------------------------------------------------------------------------
//  Name:         RemoveDebugGeometry
//
//  Purpose:	  Remove the debug OBBs and spheres.
//
//  Returns:      n/a
//---------------------------------------------------------------------------
void ModelMgr::RemoveDebugGeometry()
{
    if (NULL != m_hSphere)
	{
        g_pLTClient->RemoveObject(m_hSphere);
	}

    if (NULL != m_hSquare)
	{
        g_pLTClient->RemoveObject(m_hSquare);
	}
}



//---------------------------------------------------------------------------
//  Name:         SetDebugObjectsVisible
//
//  Purpose:      Draw the debug geometry or not.
//
//  Returns:      n/a
//---------------------------------------------------------------------------
void ModelMgr::SetDebugObjectsVisible(bool bSet)
{
	g_pLTCCommon->SetObjectFlags(m_hSphere, OFT_Flags, (bSet ? FLAG_VISIBLE : 0), FLAG_VISIBLE);
	g_pLTCCommon->SetObjectFlags(m_hSquare, OFT_Flags, (bSet ? FLAG_VISIBLE : 0), FLAG_VISIBLE);
}



//---------------------------------------------------------------------------
//  Name:         ToggleDebugBoxes
//
//  Purpose:      Toggle the boxes on and off.
//
//  Returns:      n/a
//---------------------------------------------------------------------------
void ModelMgr::ToggleDebugBoxes()
{
	m_bShowBoxes = !m_bShowBoxes;

	if (m_bShowBoxes)
	{
		g_pLTClient->RunConsoleString("ModelDebug_DrawBoxes 1");
		g_pLTClient->RunConsoleString("ModelDebug_DrawOBBS 1");
	}
	else
	{
		g_pLTClient->RunConsoleString("ModelDebug_DrawBoxes 0");
		g_pLTClient->RunConsoleString("ModelDebug_DrawOBBS 0");
	}
}

