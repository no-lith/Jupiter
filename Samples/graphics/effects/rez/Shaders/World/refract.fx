float4 diffuse = {1,1,1,1};
float4 c0 = {0.299, 0.587, 0.114, 0};
float4 c1 = {0.191, -0.054, -0.221, 0};

shared float fSaturate = 1.0;

float4x4 worldMatrix;
float4x4 viewProjMatrix;
float4x4 modelviewproj;
float4x4 modelview;
float4x4 view;

texture texture0;
texture texture1;
texture backbuffer;

// Get tex coords from our clip space transformed vertex position
float4 GetScreenTexCoords(float4 vPos)
{
	float4 vResult;
	vResult.x = ( vPos.x * 0.5 + vPos.w * 0.5 );
	vResult.y = ( vPos.w * 0.5 - vPos.y * 0.5 );
	vResult.z = vPos.w;
	vResult.w = vPos.w;
	return vResult;
}

sampler2D basemap_sampler = sampler_state
{
	Texture = <texture0>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D normalmap_sampler = sampler_state
{
	Texture = <texture1>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D backbuffer_sampler = sampler_state
{
	Texture = <backbuffer>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

struct VERTEX_IN 
{
	float3 Position : POSITION0;
	float3 Normal 	: NORMAL0;
	float3 Tangent 	: TANGENT0;
	float3 Binormal : BINORMAL0;
	float4 Color 	: COLOR0;
	float3 TexCoord : TEXCOORD0;
};

struct VERTEX_OUT
{
    float4 Position	: POSITION;
    float4 color	: COLOR0;
    float2 txcoord	: TEXCOORD0;
    float4 txcoord2	: TEXCOORD1;
    //float3 normal	: TEXCOORD2;
    //float3 tangent	: TEXCOORD3;
    //float3 binormal	: TEXCOORD4;
};

VERTEX_OUT view_space(VERTEX_IN IN)
{
	VERTEX_OUT OUT;

	// Transform this to world space
	float4 newPos = mul(worldMatrix, float4(IN.Position,1.0));

	// Transform this to clip space
	newPos = mul( viewProjMatrix, newPos);
	OUT.Position = newPos;

	OUT.color = IN.Color;
	OUT.txcoord = IN.TexCoord;

	OUT.txcoord2 = GetScreenTexCoords(newPos);

	//OUT.normal = IN.Normal;
	//OUT.tangent = IN.Tangent;
	//OUT.binormal = IN.Binormal;

	return OUT;
}

float4 pixel_shader( VERTEX_OUT IN ) : COLOR
{
	//Sample the normal map
	// normal map
	float4 normal = tex2D(normalmap_sampler, IN.txcoord);
	normal.xy = normal.xy * 2.0 - 1.0; // transform to [-1,1] range
	normal.z = sqrt(1.0 - dot(normal.xy, normal.xy)); // compute z component

	// transform normal to world space
	normal.xyz = normalize(normal.xyz);
	//normal.xyz = normalize(normal.x * IN.tangent - normal.y * IN.binormal + normal.z * IN.normal);

	float4 refr = tex2D(backbuffer_sampler, (IN.txcoord2.xy / IN.txcoord2.w) + (0.02 * normal.xy));

	// Diffuse color
	//float4 basemap = float4(1,1,1,1);
	float4 basemap = tex2D(basemap_sampler, IN.txcoord);

	// Standard color, Refracted, multiply blend
	return (refr * basemap) * fSaturate;
}

technique Refraction
{
    pass p0
    {
    	CullMode = CCW;
	AlphaBlendEnable = false;
	VertexShader = compile vs_1_1 view_space();
	PixelShader  = compile ps_2_0 pixel_shader();
    }
}

// If our device does not support PS 2.0, then we need to fall back to a simple multiply blend.
technique MultiplyBlend
{
    pass p0
    {
    	CullMode = CCW;
	AlphaBlendEnable = true;
	SrcBlend=ZERO;
	DestBlend=SRCCOLOR;
    }
}


