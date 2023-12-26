float4x4 worldMatrix;
float4x4 viewProjMatrix;
float4x4 viewMatrix;

texture texture0;

sampler2D BaseMapSampler = sampler_state
{
	Texture = <texture0>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};


struct VERTEX_IN 
{
	float3 Position 	: POSITION;
	float3 Normal 	: NORMAL;
	float2 TexCoord 	: TEXCOORD0;
};

struct VERTEX_OUT 
{
	float4 Position 	: POSITION;
	float4 TexCoord 	: TEXCOORD0;
	float4 Color 	: COLOR0;
};

//
// Vertex Shader
//
VERTEX_OUT VS(VERTEX_IN IN)
{
	VERTEX_OUT OUT;

	// Position in screen space.
	float4 newPos = mul(worldMatrix, float4(IN.Position,1.0));
	newPos = mul( viewProjMatrix, newPos);
	OUT.Position = newPos;

	// pass texture coordinates for fetching the normal map
	OUT.TexCoord = float4(IN.TexCoord,1.0, 1.0);

	OUT.Color = float4(IN.Normal, 1.0);

	return OUT;
}


technique Skin
{
	pass p0
	{
		CullMode=CCW;
		VertexShader = compile vs_1_1 VS();
	}
}

technique Fallback
{
    pass p0
    { 
    }
}
