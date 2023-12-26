float4x4 worldMatrix;
float4x4 wvpMatrix;
float4x4 worldViewMatrix;
float4x4 worldViewMatrixI;
float4x4 viewMatrix;
float4x4 viewInverseMatrix;
float4x4 viewProjMatrix;
float4   eye_position;

float bumpHeight = 0.5;
float saturate = 1.0;

texture texture0;
texture texture1;// : NORMAL;
texture texture2;// : ENVIRONMENT;

sampler2D BaseMapSampler = sampler_state
{
	Texture = <texture0>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D normalMapSampler = sampler_state
{
	Texture = <texture1>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

samplerCUBE envMapSampler = sampler_state
{
	Texture = <texture2>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

struct VERTEX_IN 
{
	float3 Position : POSITION0;
	float3 Normal 	: NORMAL0;
	float3 Tangent : TANGENT0;
	float3 Binormal : BINORMAL0;	
	float4 Color 	: COLOR0;
	float3 TexCoord : TEXCOORD0;
};

struct VERTEX_OUT 
{
	float4 Position 	: POSITION;
	float4 TexCoord 	: TEXCOORD0;
	float4 TexCoord1 	: TEXCOORD1; //first row of the 3x3 transform from tangent to cube space
	float4 TexCoord2 	: TEXCOORD2; //second row of the 3x3 transform from tangent to cube space
	float4 TexCoord3 	: TEXCOORD3; //third row of the 3x3 transform from tangent to cube space
	float4 TexCoord4 	: TEXCOORD4; //third row of the 3x3 transform from tangent to cube space
	float4 Color 		: COLOR0;
	//float4 TexCoord4	: COLOR1;
	//float4	Binormal	: BINORMAL;
};

//
// Vertex Shader
//
VERTEX_OUT BumpReflectVS(VERTEX_IN IN,
		uniform float4x4 WorldViewProj,
		uniform float4x4 World,
		uniform float4x4 ViewIT)
{
	VERTEX_OUT OUT;
	// Position in screen space.
	//OUT.Position = mul(wvpMatrix, IN.Position);


	float4 newPos = mul(worldMatrix, float4(IN.Position,1.0));
	newPos = mul( viewProjMatrix, newPos);
	OUT.Position = newPos;

	// pass texture coordinates for fetching the normal map
	OUT.TexCoord = float4(IN.TexCoord, 1.0);
	OUT.Color = IN.Color;

	//float4 renormal = mul(worldMatrix, IN.Normal);
	//renormal = normalize(renormal);	
	
	// Since we don't have real tangent information, let's just use the normal.
	float3 normal = IN.Normal.xyz;//renormal.xyz;
	float3 tangent = IN.Tangent.xyz;//IN.Normal.xyz;
	float3 binormal = IN.Binormal.xyz;//cross(normal,tangent.xyz);

	// compute the 4x4 tranform from tangent space to object space
	float3x3 TangentToObjSpace;
	// first rows are the tangent and binormal scaled by the bump scale
	TangentToObjSpace[0] = float3(tangent.x, binormal.x, normal.x);
	TangentToObjSpace[1] = float3(tangent.y, binormal.y, normal.y);
	TangentToObjSpace[2] = float3(tangent.z, binormal.z, normal.z);
	OUT.TexCoord1.x = dot(World[0].xyz, TangentToObjSpace[0]);
	OUT.TexCoord1.y = dot(World[1].xyz, TangentToObjSpace[0]);
	OUT.TexCoord1.z = dot(World[2].xyz, TangentToObjSpace[0]);
	OUT.TexCoord2.x = dot(World[0].xyz, TangentToObjSpace[1]);
	OUT.TexCoord2.y = dot(World[1].xyz, TangentToObjSpace[1]);
	OUT.TexCoord2.z = dot(World[2].xyz, TangentToObjSpace[1]);
	OUT.TexCoord3.x = dot(World[0].xyz, TangentToObjSpace[2]);
	OUT.TexCoord3.y = dot(World[1].xyz, TangentToObjSpace[2]);
	OUT.TexCoord3.z = dot(World[2].xyz, TangentToObjSpace[2]);

	float4 worldPos = mul(World, IN.Position);

	//transpose contains eye position in world space in last row.
	float4 eyeVector = worldPos - eye_position; 
	OUT.TexCoord1.w = eyeVector.x;
	OUT.TexCoord2.w = eyeVector.y;
	OUT.TexCoord3.w = eyeVector.z;
	
	OUT.TexCoord4 = float4(IN.Normal, 1.0);

	return OUT;
}

//
// Pixel Shader
//
float4 BumpReflectPS(VERTEX_OUT IN,
				uniform sampler2D NormalMap,
				uniform samplerCUBE EnvironmentMap,
                uniform float BumpScale) : COLOR
{
	// fetch the bump normal from the normal map
	float3 baseColor = tex2D(BaseMapSampler, IN.TexCoord);

	float3 normal = tex2D(NormalMap, IN.TexCoord.xy).xyz * 2.0 - 1.0;
	normal = normalize(float3(normal.x * BumpScale, normal.y * BumpScale, normal.z));

	// transform the bump normal into cube space
	// then use the transformed normal and eye vector to compute a reflection vector
	// used to fetch the cube map
	// (we multiply by 2 only to increase brightness)

	float3 eyevec = float3(IN.TexCoord1.w, IN.TexCoord2.w, IN.TexCoord3.w);
	float3 worldNorm;

	worldNorm.x = dot(IN.TexCoord1.xyz,normal);
	worldNorm.y = dot(IN.TexCoord2.xyz,normal);
	worldNorm.z = dot(IN.TexCoord3.xyz,normal);

	float3 lookup = reflect(eyevec, worldNorm);

	// Do the Bump Reflect and blend the vertex colors (use baked in world lighting)
	return float4(baseColor, 1.0) * texCUBE(EnvironmentMap, lookup) * (IN.Color*(saturate*2.0));

	//Just return the normal
	//return normalize(IN.TexCoord4); 
}

technique BumpReflect
{
	pass p0
	{
		VertexShader = compile vs_1_1 BumpReflectVS(wvpMatrix,worldMatrix,viewInverseMatrix);
		PixelShader = compile ps_2_0 BumpReflectPS(normalMapSampler,envMapSampler,bumpHeight);
	}
}

technique Fallback
{
    pass p0
    { 
    }
}
