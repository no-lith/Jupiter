//
// Fullscreen inverse color
//

//-----------------------------------------------------------------------------
// Effect File Variables
//-----------------------------------------------------------------------------
float4x4 worldMatrix;
float4x4 viewProjMatrix;
texture g_txSrcTexture; // This texture will be loaded by the application
float4 cColor = {1.0f,1.0f,1.0f,2.0f};

sampler Sampler = sampler_state
{
    Texture   = (g_txSrcTexture);
    MipFilter = POINT;
    MinFilter = POINT;
    MagFilter = POINT;
};

//-----------------------------------------------------------------------------
// Vertex Definitions
//-----------------------------------------------------------------------------

struct VS_INPUT
{
	float4 position	: POSITION;
	float4 color	: COLOR0;
	float2 texture0	: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position	: POSITION;
	float4 color	: COLOR;
	float2 texture0	: TEXCOORD0;
};

struct PS_OUTPUT
{
	float4 color : COLOR;
};

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------
// Not really needed by this effect. Added as example.
VS_OUTPUT VS( VS_INPUT IN )
{
    VS_OUTPUT OUT;

    	float4 newPos = mul(worldMatrix, IN.position);
	newPos = mul( viewProjMatrix, newPos);

	OUT.position = newPos;
	OUT.color = float4( 1.0, 1.0, 1.0, 1.0 );
	OUT.texture0 = IN.texture0;

	return OUT;
}

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------

PS_OUTPUT PS( VS_OUTPUT IN )
{
    PS_OUTPUT OUT;
	
	// Sampled texture color
	float4 actColor = (tex2D( Sampler, IN.texture0 ) * IN.color);

	OUT.color = cColor - actColor;
	
    return OUT;
}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

technique ProcessScreen
{
    pass p0
    {
	VertexShader  = compile vs_1_1 VS();
	PixelShader  = compile ps_1_1 PS();
    }
}

technique Fallback
{
    pass p0
    { 
    }
}


