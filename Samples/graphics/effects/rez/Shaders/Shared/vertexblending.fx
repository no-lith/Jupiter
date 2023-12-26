float4 Blend0Weights(float3 Position, float3 BlendWeights)
{
	float4 p0 = mul(worldMatrix, float4(Position,1.0));
			
	return p0;
}

float4 Blend1Weights(float3 Position, float3 BlendWeights)
{
	//
	float4 p0 = mul(worldMatrix, float4(Position,1.0));
	float4 p1 = mul(worldMatrix1, float4(Position,1.0));

	//vBlend = (v1*w1) + (v2 * (1.0 - W1))
	float4 vBlend = (p0*BlendWeights.x) + 
			(p1 * (1.0 - BlendWeights.x));
			
	return vBlend;
}

float4 Blend2Weights(float3 Position, float3 BlendWeights)
{
	float4 p0 = mul(worldMatrix, float4(Position,1.0));
	float4 p1 = mul(worldMatrix1, float4(Position,1.0));
	float4 p2 = mul(worldMatrix2, float4(Position,1.0));

	// vBlend = (v1*w1) + (v2*w2) + (v3 * (1.0 -(w1 + w2)) )
	float4 vBlend = (p0*BlendWeights.x) + 
			(p1*BlendWeights.y) + 
			(p2 * (1.0 - (BlendWeights.x + BlendWeights.y)));
			
	return vBlend;
}

float4 Blend3Weights(float3 Position, float3 BlendWeights)
{
	float4 p0 = mul(worldMatrix, float4(Position,1.0));
	float4 p1 = mul(worldMatrix1, float4(Position,1.0));
	float4 p2 = mul(worldMatrix2, float4(Position,1.0));
	float4 p3 = mul(worldMatrix3, float4(Position,1.0));
	
	//vBlend = (v1*w1) + (v2*W2) + (v3*W3) + (v4*(1.0 - (w1+w2+w3)))
	float4 vBlend = (p0*BlendWeights.x) + 
			(p1*BlendWeights.y) + 
			(p2*BlendWeights.z) + 
			(p3 * (1.0 - (BlendWeights.x + BlendWeights.y + BlendWeights.z)));
			
	return vBlend;
}

float3 Blend3WeightsVector(float3 Vector, float3 BlendWeights)
{
	float3 v0 = mul((float3x3)worldMatrix, Vector);
	float3 v1 = mul((float3x3)worldMatrix1, Vector);
	float3 v2 = mul((float3x3)worldMatrix2, Vector);
	float3 v3 = mul((float3x3)worldMatrix3, Vector);
	
	//vBlend = (v1*w1) + (v2*W2) + (v3*W3) + (v4*(1.0 - (w1+w2+w3)))
	float3 vBlend = (v0*BlendWeights.x) + 
			(v1*BlendWeights.y) + 
			(v2*BlendWeights.z) + 
			(v3 * (1.0 - (BlendWeights.x + BlendWeights.y + BlendWeights.z)));
			
	return vBlend;
}
