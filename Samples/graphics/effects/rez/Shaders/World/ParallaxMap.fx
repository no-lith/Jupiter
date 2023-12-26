
float tile = 1;
float depth = 0.1;
float3 ambient = {0.2,0.2,0.2};
float3 diffuse = {1,1,1};
float3 specular = {0.75,0.75,0.75};

float shine = 128.0;
float3 lightpos = { -150.0, 200.0, -125.0 };

shared float fSaturate = 1.0;

float4x4 worldMatrix;
float4x4 viewProjMatrix;
float4x4 modelviewproj;
float4x4 modelview;
float4x4 view;

texture texture0;
texture texture1;

sampler2D texmap_sampler = sampler_state
{
	Texture = <texture0>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

sampler2D reliefmap_sampler = sampler_state
{
	Texture = <texture1>;
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
    float4 hpos		: POSITION;
    float4 color	: COLOR0;
    float2 txcoord	: TEXCOORD0;
    float3 vpos		: TEXCOORD1;
    float3 tangent	: TEXCOORD2;
    float3 binormal	: TEXCOORD3;
    float3 normal	: TEXCOORD4;
    float4 lightpos	: TEXCOORD5;
    float depth		: TEXCOORD6;
};

VERTEX_OUT view_space(VERTEX_IN IN)
{
	VERTEX_OUT OUT;

	// vertex position in object space
	float4 pos = float4(IN.Position.x, IN.Position.y, IN.Position.z, 1.0);

	// compute modelview rotation only part
	float3x3 modelviewrot;
	modelviewrot[0] = modelview[0].xyz;
	modelviewrot[1] = modelview[1].xyz;
	modelviewrot[2] = modelview[2].xyz;

	float4 newPos = mul(worldMatrix, float4(IN.Position,1.0));
	newPos = mul( viewProjMatrix, newPos);
	OUT.hpos = newPos;
	
	OUT.depth = newPos.z;

	// vertex position in view space (with model transformations)
	OUT.vpos = mul(modelview, pos).xyz;

	// light position in view space
	float4 lp = float4(lightpos.x, lightpos.y, lightpos.z, 1);
	OUT.lightpos = mul(view, lp);

	// tangent space vectors in view space (with model transformations)
	OUT.tangent	=	mul(modelviewrot, IN.Tangent.xyz);
	OUT.binormal	=	mul(modelviewrot, IN.Binormal.xyz);
	OUT.normal	=	mul(modelviewrot, IN.Normal.xyz);

	// copy color and texture coordinates
	OUT.color = IN.Color;
	OUT.txcoord = IN.TexCoord.xy;

	return OUT;
}

float4 parallax_map( VERTEX_OUT IN ) : COLOR
{
   	// view and light directions
	float3 v = normalize(IN.vpos);
	float3 l = normalize(IN.lightpos.xyz - IN.vpos);

	float2 uv = IN.txcoord * tile;

	// parallax code
	
	float3x3 tbn = float3x3(IN.tangent, IN.binormal, IN.normal);
	float height = tex2D(reliefmap_sampler, uv).w * 0.06 - 0.03;
	uv += height * mul(v, tbn);
	
	// normal map
	float4 normal = tex2D(reliefmap_sampler, uv);
	normal.xy = normal.xy * 2.0 - 1.0; // trafsform to [-1,1] range
	normal.z = sqrt(1.0 - dot(normal.xy, normal.xy)); // compute z component

	// transform normal to world space
	normal.xyz = normalize(normal.x * IN.tangent - normal.y * IN.binormal + normal.z * IN.normal);

	// color map
	float4 color = tex2D(texmap_sampler, uv);

	// compute diffuse and specular terms
	float att = saturate(dot(l,IN.normal.xyz));
	float diff = saturate(dot(l,normal.xyz));
	float spec = saturate(dot(normalize(l-v),normal.xyz));

	//test for depath
	//float4 fDepth = normalize(float4(255, 255, IN.depth, 1.0));
	
	//fDepth.x = 0;
	//fDepth.y = 0;
	
	// compute final color
	float4 finalcolor;// = fDepth;// = normal; //This is a test.. just output the normal for now
	finalcolor.xyz = (ambient*color.xyz)+att*(color.xyz*diffuse.xyz*diff+specular.xyz*pow(spec,shine)) * fSaturate;
	finalcolor.w = 1.0;

	return finalcolor;
}

technique parallax_mapping
{
    pass p0
    {
    	CullMode = CCW;
	VertexShader = compile vs_1_1 view_space();
	PixelShader  = compile ps_2_0 parallax_map();
    }
}

technique Fallback
{
    pass p0
    { 
    }
}
