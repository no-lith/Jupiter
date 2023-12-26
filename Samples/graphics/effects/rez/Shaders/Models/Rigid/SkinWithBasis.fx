//
// Model Inverse color in one pass.
//
float4 cColor = {1.0f, 1.0f, 1.0f, 1.0f};
texture texture0; // This texture will be loaded by the application
float4x4 worldMatrix;
float4x4 viewProjMatrix;

sampler Sampler0 = sampler_state
{
	texture = <texture0>;
	MipFilter = Linear;
	MinFilter = Linear;
	MagFilter = Linear;
};

//-----------------------------------------------------------------------------
// Vertex Definitions
//-----------------------------------------------------------------------------

struct VS_INPUT
{
	float3 Position 	: POSITION0;
	float3 Normal 	: NORMAL0;
	float2 TexCoord 	: TEXCOORD0;
	float3 Tangent 	: TANGENT0;
	float3 Binormal 	: BINORMAL0;
};

struct VS_OUTPUT 
{
	float4 Position 	: POSITION;
	float2 TexCoord 	: TEXCOORD0;
	float4 Color 		: COLOR0;
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
	//OUT.position = mul( worldViewProj, float4(IN.position, 1.0) );

	float4 newPos = mul(worldMatrix, float4(IN.Position,1.0));
	newPos = mul( viewProjMatrix, newPos);
	OUT.Position = newPos;
	
	OUT.TexCoord = IN.TexCoord;
	OUT.Color = cColor;
	
	return OUT;
};

//-----------------------------------------------------------------------------
// Pixel Shader
//-----------------------------------------------------------------------------
PS_OUTPUT PS( VS_OUTPUT IN )
{
    PS_OUTPUT OUT;
	
	// Sampled texture color
	float4 diffuseColor = (tex2D( Sampler0, IN.TexCoord ) * IN.Color);

	// Invert the color
	//OUT.color = (cColor - diffuseColor);
	
	// Just output the texture color at this pixel.
	OUT.color = diffuseColor;
	
    return OUT;
};

technique T0
{
	pass p0
	{
		CullMode=CCW;
		VertexShader = compile vs_1_1 VS();
		PixelShader = compile ps_1_1 PS();
	}
}

// Fall back incase the device does not support pixel shaders
// The geforce MX440 for example only supports VS 1.1 with no support for PS.
technique FallbackOne
{
	pass p0
	{
		CullMode=CCW;
		VertexShader = compile vs_1_1 VS();
	}
}

// Fallback in case the device does not support vertex or pixel shaders
technique FallbackTwo
{
	pass p0
	{
		CullMode=CCW;
	}
}

