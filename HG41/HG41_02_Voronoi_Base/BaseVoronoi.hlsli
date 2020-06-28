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

float3 Resolution = { 1600, 900, 0 };

float2 hash(float2 p)
{
    p = float2(dot(p, float2(127.1, 311.7)), dot(p, float2(269.5, 183.3)));
    return frac(sin(p) * 18.5453);
}

// return distance, and cell id
float2 voronoi(in float2 x)
{
    float2 n = floor(x);
    float2 f = frac(x);
    float3 m = float3(8.0, 8.0, 8.0);
    for (int j = -1; j <= 1; j++)
        for (int i = -1; i <= 1; i++)
        {
            float2 g = float2(float(i), float(j));
            float2 o = hash(n + g);
            //float2  r = g - f + o;
            float2 r = g - f + (0.5 + 0.5 * sin(iTime + 6.2831 * o));
            float d = dot(r, r);
            if (d < m.x)
                m = float3(d, o);
        }
    return float2(sqrt(m.x), m.y + m.z);
}

void BaseVoronoi(out float4 fragColor, in float2 fragCoord)
{
    float2 p = fragCoord.xy / Resolution.xx;
    // computer voronoi patterm
    float2 c = voronoi((100 + 6.0 * sin(0.2 ) * p));
    // colorize
    float3 col = 0.5 + 0.5 * cos(c.y * 6.2831 + float3(0.0, 1.0, 2.0));
    col *= clamp(1.0 - 0.4 * c.x * c.x, 0.0, 1.0);
    col -= (1.0 - smoothstep(0.08, 0.09, c.x));
    fragColor = float4(col, 1.0);
}