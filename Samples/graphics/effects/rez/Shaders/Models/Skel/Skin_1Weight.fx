//
// Model Inverse color in one pass.
//
float4 cColor = {1.0f, 1.0f, 1.0f, 1.0f};
texture texture0; // This texture will be loaded by the application
float4x4 worldMatrix;
float4x4 worldMatrix1;
float4x4 worldMatrix2;
float4x4 worldMatrix3;
float4x4 viewProjMatrix;
int BoneCount = 0;

sampler Sampler0 = sampler_state
{
	texture = <texture0>;
	AddressU = Wrap;
	AddressV = Wrap;
	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
};

//-----------------------------------------------------------------------------
// Vertex Definitions
//-----------------------------------------------------------------------------

struct VS_INPUT
{
	float3 position			: POSITION;
	float BlendWeights 		: BLENDWEIGHT;
	float3 normal	 		: NORMAL;
	float2 tex0 			: TEXCOORD0;

};

struct VS_OUTPUT
{
	float4 position	: POSITION;
	float2 tex0	: TEXCOORD0;
	float4 color	: COLOR0;
};

struct PS_OUTPUT
{
	float4 color : COLOR;
};

//-----------------------------------------------------------------------------
// Vertex Shader
//-----------------------------------------------------------------------------

VS_OUTPUT VS_0WEIGHTS( VS_INPUT IN )
{
    VS_OUTPUT OUT;
	
	//
	float4 p0 = mul(worldMatrix, float4(IN.position,1.0));

	// Set this is the view projection...
	float4 finalPos = mul( viewProjMatrix, p0);
	OUT.position = finalPos;

	OUT.color = float4( 1.0, 1.0, 1.0, 1.0 );
	OUT.tex0 = IN.tex0;

	return OUT;
}

VS_OUTPUT VS_1WEIGHTS( VS_INPUT IN )
{
    VS_OUTPUT OUT;
	
	//
	float4 p0 = mul(worldMatrix, float4(IN.position,1.0));
	float4 p1 = mul(worldMatrix1, float4(IN.position,1.0));

	//vBlend = (v1*w1) + (v2 * (1.0 - W1))
	float4 vBlend = (p0*IN.BlendWeights) + 
			(p1 * (1.0 - IN.BlendWeights));

	// Set this is the view projection...
	float4 blendedPos = vBlend;
	float4 finalPos = mul( viewProjMatrix, blendedPos);
	OUT.position = finalPos;

	OUT.color = float4( 1.0, 1.0, 1.0, 1.0 );
	OUT.tex0 = IN.tex0;

	return OUT;
}


//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------
PS_OUTPUT PS( VS_OUTPUT IN )
{
    PS_OUTPUT OUT;
	
	// Sampled texture color
	float4 actColor = (tex2D( Sampler0, IN.tex0 ) * IN.color);
	OUT.color = actColor;

	// Inverted color
	// float4 invColor = cColor - actColor;
	// OUT.color = invColor;
	
    return OUT;
}

VertexShader vsArray1_1[ 2 ] = { compile vs_1_1 VS_0WEIGHTS(),
                                compile vs_1_1 VS_1WEIGHTS()};

//
technique T0
{
	pass p0
	{
		CullMode=CCW;
		VertexShader = ( vsArray1_1[ BoneCount ] );
		PixelShader  = compile ps_1_1 PS();
	}
}

technique Fallback
{
    pass p0
    { 
    }
}