float4 diffuse = {1,1,1,1};
float4 c0 = {0.299, 0.587, 0.114, 0};
float4 c1 = {0.191, -0.054, -0.221, 0};

shared float fSaturate = 1.0;

float4x4 worldMatrix;
float4x4 viewProjMatrix;
float4x4 modelviewproj;
float4x4 modelview;
float4x4 view;
float3 eyepos = {0,0,0};

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

sampler2D normalmap_sampler = sampler_state
{
	Texture = <texture0>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

samplerCUBE envmap_sampler = sampler_state
{
	Texture = <texture1>;
	AddressU=Clamp;
	AddressV=Clamp;
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
	float4 Color 	: COLOR0;
	float3 TexCoord : TEXCOORD0;
	float3 Tangent : TANGENT0;
	float3 Binormal : BINORMAL0;
};

struct VERTEX_OUT
{
    float4 Position	: POSITION;
    float4 color	: COLOR0;
    float2 txcoord	: TEXCOORD0;
    float4 txcoord2	: TEXCOORD1;
    //float3 tangent	: TEXCOORD3;
    //float3 binormal	: TEXCOORD4;
    float4 Basis0 	: TEXCOORD2; //first row of the 3x3 transform from tangent to cube space
    float4 Basis1 	: TEXCOORD3; //second row of the 3x3 transform from tangent to cube space
    float4 Basis2 	: TEXCOORD4; //third row of the 3x3 transform from tangent to cube space    
};

VERTEX_OUT view_space(VERTEX_IN IN)
{
	VERTEX_OUT OUT;

	// Transform this to world space
	float4 worldPos = mul(worldMatrix, float4(IN.Position,1.0));

	// Transform this to clip space
	float4 clipPos = mul( viewProjMatrix, worldPos);
	OUT.Position = clipPos;

	OUT.color = IN.Color;
	OUT.txcoord = IN.TexCoord;

	OUT.txcoord2 = GetScreenTexCoords(clipPos);

	// Since we don't have real tangent information, let's just use the normal.
	//float3 normal = float3(0,1,0);//IN.Normal.xyz;//renormal.xyz;
	//float3 tangent = float3(1,0,0);//IN.Tangent.xyz;//IN.Normal.xyz;
	//float3 binormal = float3(0,0,1);//IN.Binormal.xyz;//cross(normal,tangent.xyz);
	
	float3 normal = IN.Normal.xyz;//renormal.xyz;
	float3 tangent = IN.Tangent.xyz;//IN.Normal.xyz;
	float3 binormal = IN.Binormal.xyz;//cross(normal,tangent.xyz);

	// compute the 4x4 tranform from tangent space to object space
	float3x3 TangentToObjSpace;
	// first rows are the tangent and binormal scaled by the bump scale
	TangentToObjSpace[0] = float3(tangent.x, binormal.x, normal.x);
	TangentToObjSpace[1] = float3(tangent.y, binormal.y, normal.y);
	TangentToObjSpace[2] = float3(tangent.z, binormal.z, normal.z);
	OUT.Basis0.x = dot(worldMatrix[0].xyz, TangentToObjSpace[0]);
	OUT.Basis0.y = dot(worldMatrix[1].xyz, TangentToObjSpace[0]);
	OUT.Basis0.z = dot(worldMatrix[2].xyz, TangentToObjSpace[0]);
	OUT.Basis1.x = dot(worldMatrix[0].xyz, TangentToObjSpace[1]);
	OUT.Basis1.y = dot(worldMatrix[1].xyz, TangentToObjSpace[1]);
	OUT.Basis1.z = dot(worldMatrix[2].xyz, TangentToObjSpace[1]);
	OUT.Basis2.x = dot(worldMatrix[0].xyz, TangentToObjSpace[2]);
	OUT.Basis2.y = dot(worldMatrix[1].xyz, TangentToObjSpace[2]);
	OUT.Basis2.z = dot(worldMatrix[2].xyz, TangentToObjSpace[2]);
	
	OUT.Basis0.xyz = tangent;
	OUT.Basis1.xyz = binormal;
	OUT.Basis2.xyz = normal;

	//transpose contains eye position in world space in last row.
	float4 eyeVector = worldPos - float4(eyepos, 1.0);
	OUT.Basis0.w = eyeVector.x;
	OUT.Basis1.w = eyeVector.y;
	OUT.Basis2.w = eyeVector.z;

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

	float3 eyevec = float3(IN.Basis0.w, IN.Basis1.w, IN.Basis2.w);
	float3 worldNorm;

	worldNorm.x = dot(IN.Basis0.xyz,normal);
	worldNorm.y = dot(IN.Basis1.xyz,normal);
	worldNorm.z = dot(IN.Basis2.xyz,normal);

	float3 envlookup = reflect(eyevec, IN.Basis2.xyz);
	float4 env = texCUBE(envmap_sampler, envlookup);
	
	// Diffuse color
	//float4 basemap = float4(1,1,1,1);
	//float4 basemap = tex2D(basemap_sampler, IN.txcoord);

	// Standard color, Refracted, multiply blend
	return (refr) * (env) * IN.color * fSaturate;
	//return (env) * IN.color * fSaturate;
}

technique Refraction
{
    pass p0
    {
	   // fillmode=wireframe;
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


