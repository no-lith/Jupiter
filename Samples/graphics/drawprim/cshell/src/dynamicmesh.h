#ifndef _DYNAMICMESH_H_
#define _DYNAMICMESH_H_

#include "iclientshell.h"
#include "ilttexinterface.h"
#include "iltdrawprim.h"


class DynamicMesh
{
public:
    DynamicMesh();
    ~DynamicMesh();

    void Init(const char* filename, int x, int y);
    void Reset();
    void Render();
    void Term();

    void SetCamera(HOBJECT newCamera){ m_hCamera = newCamera; }

    void LoadUSGSData(const char* filename, int x, int y);
    void UnloadUSGSData();

    void SetVert(LT_VERTGT *pVert, int x, int z);
    int  LookupGridPointY(int x, int z, int width, int *points);

    void InitPrimBuffer();
    void TermPrimBuffer();



private:

    HOBJECT       m_hCamera;

    LT_POLYGT3    *m_verts;
    int           m_nVertCount;
    HTEXTURE      m_hTerrainTex;

    int           m_nGridWidth;
    int           m_nGridLength;
    int           m_nTileTimes;

    int           m_nSize;
    int           m_nHeightScale;

    int           *m_nGridData;
    unsigned char *m_bUSGSData;
    unsigned int  m_nUSGSDataX;
    unsigned int  m_nUSGSDataY;


};

#endif