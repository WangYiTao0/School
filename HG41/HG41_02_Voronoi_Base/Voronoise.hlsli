
cbuffer ConstatntBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;

    float4 CameraPosition;
    float4 CameraDirection;
    
    float3 iResolution;
    float iTime; //second
    float4 iMouse;
}

float3 hash3(float2 p)
{
    float3 q = float3(dot(p, float2(127.1, 311.7)),
				   dot(p, float2(269.5, 183.3)),
				   dot(p, float2(419.2, 371.9)));
    return frac(sin(q) * 43758.5453);
}

float iqnoise(in float2 x, float u, float v)
{
    float2 p = floor(x);
    float2 f = frac(x);
		
    float k = 1.0 + 63.0 * pow(1.0 - v, 6.0);
	
    float va = 0.0;
    float wt = 0.0;
    for (int j = -2; j <= 2; j++)
        for (int i = -2; i <= 2; i++)
        {
            float2 g = float2(float(i), float(j));
            float3 o = hash3(p + g) * float3(u, u, 1.0);
            float2 r = g - f + o.xy;
            float d = dot(r, r);
            float ww = pow(1.0 - smoothstep(0.0, 1.414, sqrt(d)), k);
            va += o.z * ww;
            wt += ww;
        }
	
    return va / wt;
}

void Voronoise(out float4 fragColor, in float2 fragCoord)
{
    float2 uv = fragCoord.xy / iResolution.xx;

    float2 p = 0.5 - 0.5 * cos(iTime * float2(1.0, 0.5));
    
    if (iMouse.w > 0.001)
        p = float2(0.0, 1.0) + float2(1.0, -1.0) * iMouse.xy / iResolution.xy;
	
    p = p * p * (3.0 - 2.0 * p);
    p = p * p * (3.0 - 2.0 * p);
    p = p * p * (3.0 - 2.0 * p);
    
    if (cos(iTime * 0.5) > 0.5)
    {
        p = uv * float2(1.0, iResolution.x / iResolution.y);
    }
	
    float f = iqnoise(2400 * uv, p.x, p.y);
	
    fragColor = float4(f, f, f, 1.0);
}