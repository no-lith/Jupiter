//
// SepiaTone in one pass.
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
		
		def c0, 0.299, 0.587, 0.114, 0
		def c1, 0.191, -0.054, -0.221, 0
		tex t0
		dp3 r0, c0, t0
		add r0, r0, c1
	};
    }
}

technique Fallback
{
    pass p0
    { 
    }
}


