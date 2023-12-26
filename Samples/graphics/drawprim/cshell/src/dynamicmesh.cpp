

#include "DynamicMesh.h"
#include "clientinterfaces.h"
#include "iltclient.h"
#include "iltdrawprim.h"


DynamicMesh::DynamicMesh():
m_bUSGSData(NULL),
m_verts(NULL)
{

}

DynamicMesh::~DynamicMesh()
{
    Term();
}


void DynamicMesh::Init(const char* filename, int x, int y)
{

    if ( (NULL != m_verts) || ( NULL != m_bUSGSData) )
    {
        // If there's still active data, remove it.
        Term();
    }

    if (NULL != filename)
    {
        
    }


    LoadUSGSData(filename, x, y);

    InitPrimBuffer();
    
}

void DynamicMesh::Reset()
{
    Term();
}

void DynamicMesh::Render()
{
    if (NULL == m_verts)
    {
        return;
    }


	//g_pDrawPrimLT->DrawPrim(verts, nVert);

	// set up the render state
	g_pLTCDrawPrim->SetTransformType(DRAWPRIM_TRANSFORM_WORLD);
	g_pLTCDrawPrim->SetColorOp(DRAWPRIM_MODULATE);
	g_pLTCDrawPrim->SetAlphaBlendMode(DRAWPRIM_NOBLEND);
	g_pLTCDrawPrim->SetZBufferMode(DRAWPRIM_ZRW);
    g_pLTCDrawPrim->SetAlphaTestMode(DRAWPRIM_NOALPHATEST);
	g_pLTCDrawPrim->SetClipMode(DRAWPRIM_FULLCLIP);
	g_pLTCDrawPrim->SetFillMode(DRAWPRIM_FILL);
	g_pLTCDrawPrim->SetCullMode(DRAWPRIM_CULL_NONE);
    g_pLTCDrawPrim->SetCamera(m_hCamera);

    g_pLTCDrawPrim->SetTexture(m_hTerrainTex);

    int nCount = 1;
    int i;
    for (i = 0; i < nCount; i++)
    {
    	g_pLTCDrawPrim->DrawPrim(m_verts, m_nVertCount);
    }
}

void DynamicMesh::Term()
{
    UnloadUSGSData();
    TermPrimBuffer();
}


void DynamicMesh::LoadUSGSData(const char* filename, int x, int y)
{
    uint32 lSize;

    m_nUSGSDataX = x;
    m_nUSGSDataY = y;

    ILTStream *pStream = NULL;
    LTRESULT result = g_pLTClient->OpenFile(filename, &pStream);

    if (LT_OK != result)
    {
        g_pLTClient->CPrint("Error! cannot open: %s", filename);
        return;
    }
    
    // Obtain the file size
    pStream->GetLen(&lSize);
    
    // allocate memory to contain the whole file.
    m_bUSGSData = new unsigned char[lSize];
    if (m_bUSGSData == NULL)
    { 
        // If memory is not allocated, shut down.
        g_pLTClient->Shutdown();
    }
    
    // copy the file data into the buffer.
    pStream->Read(m_bUSGSData, lSize);
    
    // We're done with the stream
    pStream->Release();
}


void DynamicMesh::UnloadUSGSData()
{
    if (NULL != m_bUSGSData)
    {
        delete[] m_bUSGSData;
    }
}


//-----------------------------------------------------------------------------
//  CLTClientShell::SetVert(LT_VERTGT *pVert, int x, int z)
//
//-----------------------------------------------------------------------------
void DynamicMesh::SetVert(LT_VERTGT *pVert, int x, int z)
{

    if( NULL == m_bUSGSData )
    {
        return;
    }

    int GridY = LookupGridPointY(x, z, m_nGridWidth, m_nGridData);
    //g_pLTClient->CPrint("GridY: %d", GridY);

    pVert->x       = (float)x * m_nSize;
    pVert->y       = (float)GridY*2; // * 2 to amplify the effect
    pVert->z       = (float)z * m_nSize;
    pVert->rgba.r  = GridY / 2;
    pVert->rgba.g  = GridY;
    pVert->rgba.b  = 255/2 - GridY; //this currently loops around, being only a byte long..
    pVert->rgba.a  = 255;
    pVert->u       = (float)x / m_nGridWidth * m_nTileTimes;
    pVert->v       = (float)z / m_nGridLength * m_nTileTimes;
}



//Grab a point from our m_bUSGSData 
int DynamicMesh::LookupGridPointY(int x, int z, int width, int *points)
{
    //legacy randomized data
    //return points[x + ( z * width )]; 

    if( NULL == m_bUSGSData )
    {
        return 0;
    }    

    int _image_width = m_nUSGSDataX;
    int _image_height = m_nUSGSDataY;

    int pixel_width_ratio =   _image_width/64;
    int pixel_height_ratio = _image_height/64;


    //HACK: TODO: figure out why the grid is flipped horizontal or begin with.
    //flip the image horizontal
    int num = _image_width - (x * pixel_width_ratio);

    return (int)(m_bUSGSData[(num) + (z * pixel_height_ratio * _image_width)]);
    //return (int)(m_bUSGSData[(x * pixel_width_ratio) + (z * pixel_height_ratio * _image_width)]);


}


void DynamicMesh::InitPrimBuffer()
{
    m_nGridWidth = 64;
    m_nGridLength = 64;
  
    m_nSize = 150;

    m_verts = new LT_POLYGT3[2*m_nGridWidth*m_nGridLength];
    memset(m_verts, 0, sizeof(LT_POLYGT3)*2*m_nGridWidth*m_nGridLength);
 
    // set up the render state
	g_pLTCDrawPrim->SetTransformType(DRAWPRIM_TRANSFORM_WORLD);
	g_pLTCDrawPrim->SetColorOp(DRAWPRIM_MODULATE);
	g_pLTCDrawPrim->SetAlphaBlendMode(DRAWPRIM_NOBLEND);
	g_pLTCDrawPrim->SetZBufferMode(DRAWPRIM_ZRW);
    g_pLTCDrawPrim->SetAlphaTestMode(DRAWPRIM_NOALPHATEST);
	g_pLTCDrawPrim->SetClipMode(DRAWPRIM_FULLCLIP);
	g_pLTCDrawPrim->SetFillMode(DRAWPRIM_FILL);
	g_pLTCDrawPrim->SetCullMode(DRAWPRIM_CULL_NONE);
    g_pLTCDrawPrim->SetCamera(m_hCamera);

    m_hTerrainTex = NULL;
    const char *cTexture = "Tex/stone/st17/st0873.dtx";
    g_pLTCTexInterface->FindTextureFromName(m_hTerrainTex, cTexture);
    if (NULL == m_hTerrainTex)
    {
        g_pLTCTexInterface->CreateTextureFromName(m_hTerrainTex, cTexture);
    }

	g_pLTCDrawPrim->SetTexture(m_hTerrainTex);


    int nVert = 0;
    int nTriVert = 0;

    m_nTileTimes = 5;

    for(int i = 0; i < m_nGridLength; i++)
    {
        for(int j = 0; j < m_nGridWidth; j++)
        {
            nTriVert = 0;
            SetVert(&(m_verts[nVert].verts[nTriVert]), i, j);
            nTriVert++;

            SetVert(&(m_verts[nVert].verts[nTriVert]), i, j+1);
            nTriVert++;

            SetVert(&(m_verts[nVert].verts[nTriVert]), i+1, j+1);
            nTriVert++;

            nVert++;

            //
            nTriVert = 0;
            SetVert(&(m_verts[nVert].verts[nTriVert]), i, j);
            nTriVert++;

            SetVert(&(m_verts[nVert].verts[nTriVert]), i+1, j+1);
            nTriVert++;

            SetVert(&(m_verts[nVert].verts[nTriVert]), i+1, j);
            nTriVert++;
            nVert++;
        }
    }
    m_nVertCount = nVert;
}





void DynamicMesh::TermPrimBuffer()
{
    if ( NULL != m_verts)
    {
        delete[] m_verts;
    }

}