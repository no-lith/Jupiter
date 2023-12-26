//
// Sobel Edge Filter.
//
// Source: http://www.neatware.com/lbstudio/web/hlsl.html

//-----------------------------------------------------------------------------
// Effect File Variables
//-----------------------------------------------------------------------------
texture g_txSrcTexture; // This texture will be loaded by the application
float Brightness = 1.0;

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

PS_OUTPUT PS( VS_OUTPUT IN )
{
	PS_OUTPUT OUT;
	
	//const specifies the constants. The c[NUM] is a float2 constant array. 
	//Notes its initialization is convenience like C language. col[NUM] is a variable array
	//of type float3 with NUM elements. int i declares the i as integer. These usage is
	//effective for pixel shader 2.0 or later.
	
	const int NUM = 9;
	const float threshold = 0.05;

	const float2 c[NUM] = 
	{
		float2(-0.0078125, 0.0078125),
		float2( 0.00 ,     0.0078125),
		float2( 0.0078125, 0.0078125),
		float2(-0.0078125, 0.00 ),
		float2( 0.0,       0.0),
		float2( 0.0078125, 0.007 ),
		float2(-0.0078125,-0.0078125),
		float2( 0.00 ,    -0.0078125),
		float2( 0.0078125,-0.0078125),
	};

	float3 col[NUM];   
	int i;


	//store the samples of texture to col array.
	for (i=0; i < NUM; i++) 
	{
		col[i] = tex2D(Sampler, IN.texture0.xy + c[i]);
	}

	//now we start to compute the luminance with dot product and store them in lum array.
	float3 rgb2lum = float3(0.30, 0.59, 0.11);

	float lum[NUM];
	for (i = 0; i < NUM; i++)
	{
		lum[i] = dot(col[i].xyz, rgb2lum);
	}

	//Sobel filter computes new value at the central position by sum the weighted neighbors.
	float x = lum[2]+  lum[8]+2*lum[5]-lum[0]-2*lum[3]-lum[6];
	float y = lum[6]+2*lum[7]+  lum[8]-lum[0]-2*lum[1]-lum[2];

	//show the points which values are over the threshold and hide others. 
	//Final result is the product of col[5] and edge detector value. Brightness adjusts the brightness of the image.
	float edge =(x*x + y*y < threshold)? 1.0:0.0;

	//final output
	OUT.color.xyz = Brightness * col[5].xyz * edge;
	OUT.color.w = 1.0;

	return OUT;
}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

technique ProcessScreen
{
    pass p0
    {
		MipFilter[0] = POINT;
		MinFilter[0] = POINT;
		MagFilter[0] = POINT;

		PixelShader = compile ps_2_0 PS();
    }
}

technique FallBack
{
    pass p0
    {
    }
}
