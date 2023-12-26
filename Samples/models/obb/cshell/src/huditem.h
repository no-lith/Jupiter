#ifndef _HUDITEM_H_
#define _HUDITEM_H_

#include <ltbasetypes.h>
#include <ltbasedefs.h>
#include <iltclient.h>
#include <ilttexinterface.h>

class HudItem
{
public:
	HudItem();
	~HudItem(){}

	void Init(const char *pFilename);
	void Term();
	void Render();
	void Center();

private:
	HTEXTURE			    m_hTexture;
	LTVector				m_vPos;
	// Dimensions of the poly
	float					top, left, bottom, right;
};

#endif
