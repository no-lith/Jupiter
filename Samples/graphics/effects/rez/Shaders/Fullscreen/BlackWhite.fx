//
// Black and White in one pass.
//

//-----------------------------------------------------------------------------
// Effect File Variables
//-----------------------------------------------------------------------------
texture g_txSrcTexture; // This texture will be loaded by the application

sampler2D texmap_sampler = sampler_state
{
	Texture = <g_txSrcTexture>;
	MinFilter = Point;
	MagFilter = Point;
	MipFilter = Point;
};

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

technique ProcessScreen
{
    pass p0
    {
	Sampler[0] = <texmap_sampler>;
	MipFilter[0] = POINT;
	MinFilter[0] = POINT;
	MagFilter[0] = POINT;

	PixelShader =
	asm
	{
		ps_1_1 
		def c0, 0.30, 0.59, 0.11, 1.0
		tex t0
		mov r0, t0
		dp3 r0, r0, c0
	}; 
    }
}

technique Fallback
{
    pass p0
    { 
    }
}

