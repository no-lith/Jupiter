//
//
//
//
//


#include "huditem.h"
#include "clientinterfaces.h"
#include <iltdrawprim.h>

HudItem::HudItem():
m_hTexture(NULL)
{

}

void HudItem::Init(const char *pFilename)
{
    if (!m_hTexture)
		g_pLTCTexInterface->CreateTextureFromName(m_hTexture, pFilename);
}

void HudItem::Term()
{
    if(m_hTexture)
    {
        g_pLTCTexInterface->ReleaseTextureHandle(m_hTexture);
        m_hTexture = NULL;
    }	
}

void HudItem::Center()
{
	// Screen Width and Height
	uint32 nScreenW, nScreenH;

	g_pLTClient->GetSurfaceDims(g_pLTClient->GetScreenSurface(),
		&nScreenW, &nScreenH);

	// Watermark Width and Height
	uint32 nMarkW, nMarkH;

	// Get watermark texture dims.
	g_pLTCTexInterface->GetTextureDims(m_hTexture, nMarkW, nMarkH);

	m_vPos.x = float(nScreenW);
	m_vPos.y = float(nScreenH);
	
	top = float((m_vPos.y/2.0f) - (nMarkH/4));
	bottom = float((m_vPos.y/2.0f) + (nMarkH/4));
	left = float((m_vPos.x/2.0f)  - (nMarkW/4));
	right = float((m_vPos.x/2.0f) + (nMarkW/4));
}

void HudItem::Render()
{
	// The polygon we will draw
	LT_POLYFT4 poly;

	if (!m_hTexture)
		return;

	// Set up the verts (clockwise from upper left).
	poly.verts[0].x = left;
	poly.verts[0].y = top;
	poly.verts[0].z = SCREEN_NEAR_Z;
	poly.verts[0].u = 0.0f;
	poly.verts[0].v = 0.0f;

	poly.verts[1].x = right;
	poly.verts[1].y = top;
	poly.verts[1].z = SCREEN_NEAR_Z;
	poly.verts[1].u = 1.0f;
	poly.verts[1].v = 0.0f;

	poly.verts[2].x = right;
	poly.verts[2].y = bottom;
	poly.verts[2].z = SCREEN_NEAR_Z;
	poly.verts[2].u = 1.0f;
	poly.verts[2].v = 1.0f;

	poly.verts[3].x = left;
	poly.verts[3].y = bottom;
	poly.verts[3].z = SCREEN_NEAR_Z;
	poly.verts[3].u = 0.0f;
	poly.verts[3].v = 1.0f;

	// Set up color and alpha.
	poly.rgba.r = 255;
	poly.rgba.g = 255;
	poly.rgba.b = 255;
	poly.rgba.a = 128;

	// Set which texture to use.
	g_pLTCDrawPrim->SetTexture(m_hTexture);

	// Set up the drawprim render state.
	g_pLTCDrawPrim->SetTransformType(DRAWPRIM_TRANSFORM_SCREEN);
	g_pLTCDrawPrim->SetColorOp(DRAWPRIM_MODULATE);
	g_pLTCDrawPrim->SetAlphaBlendMode(DRAWPRIM_BLEND_MOD_SRCALPHA);
	g_pLTCDrawPrim->SetZBufferMode(DRAWPRIM_NOZ);
	g_pLTCDrawPrim->SetAlphaTestMode(DRAWPRIM_NOALPHATEST);
	g_pLTCDrawPrim->SetClipMode(DRAWPRIM_FASTCLIP);
	g_pLTCDrawPrim->SetFillMode(DRAWPRIM_FILL);
	g_pLTCDrawPrim->SetCullMode(DRAWPRIM_CULL_NONE);
	g_pLTCDrawPrim->SetCamera(NULL);

	// Draw the watermark (just 1 quad).
	g_pLTCDrawPrim->DrawPrim(&poly, 1);	
}