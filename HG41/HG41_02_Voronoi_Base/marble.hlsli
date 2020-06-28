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

SamplerState g_SamplerState : register(s0);

float zoom = 10.;

#define MAP_OCTAVE 10

float2 cmul(float2 a, float2 b)
{
    return float2(a.x * b.x - a.y * b.y, a.x * b.y + a.y * b.x);
}
float2 csqr(float2 a)
{
    return float2(a.x * a.x - a.y * a.y, 2. * a.x * a.y);
}
float2x2 rot(float a)
{
    return float2x2(cos(a), sin(a), -sin(a), cos(a));
}

float2 iSphere(in float3 ro, in float3 rd, in float4 sph)//from iq
{
    //ro  ray origin
    //rd  ray dir
    //sph.xyz sphere center
    //sph.w sph radius
    // return intersection point distance
    
    float3 oc = ro - sph.xyz;
    float b = dot(rd, oc);
    float c = dot(oc, oc);
    float h = b * b - c + sph.w * sph.w;
    if (h < 0.0)
        return float2(-1.0,-1.0);
    h = sqrt(h);
    return float2(-b - h, -b + h);
    
    
    //float3 oc = ro - sph.xyz;
    //float a = dot(rd, rd);
    //float b = dot(oc, rd);
    //float c = dot(oc, oc) - sph.w * sph.w;
    //float discriminant = b * b - 4*a*c;
    //if (discriminant < 0.0)
    //    return float2(-1.0,-1.0);
    //discriminant = sqrt(discriminant);
    //return float2(-b - discriminant, -b + discriminant);
}
float map(in float3 p)
{
    float res = 0.0;
	
    float3 c = p;
    for (int i = 0; i < MAP_OCTAVE; ++i)
    {
        p = 0.7 * abs(p) / dot(p, p) - 0.7;
        p.yz = csqr(p.yz);
        p = p.zxy;
        res += exp(-25.0 * abs(dot(p, c)));
    }
    return res;
}
float3 raymarch(in float3 ro, float3 rd, float2 tminmax)
{
    float t = tminmax.x;
    float dt = 0.02;
    //loat dt = 0.2 - 0.195 * cos((iTime + 32.0) * 0.05); //animated
    float3 col = float3(0.0,0.0,0.0);
    float c = 0.0;
    for (int i = 0; i < 64; i++)
    {
        t += dt * exp(-2.0 * c);
        if (t > tminmax.y)
            break;
        float3 pos = ro + t * rd;
        
        c =0.45* map(ro + t * rd);
        
        col = 0.98 * col + 0.08 * float3(c * c, c, c * c * c); // green
        col = 0.99 * col + 0.08 * float3(c * c * c, c * c, c); // blue
        col = 0.99 * col + 0.08 * float3(c, c * c * c, c * c); // red
        col = .99 * col + 0.08 * pow(cos(0.75 * c - float3(1, 0.75, 0.5)), float3(16, 16, 16));
    }
    return col;
}

void PlayingMarble(out float4 fragColor, in float2 fragCoord)
{
    float time = iTime;
    float2 q = fragCoord.xy / iResolution.xy;
    float2 p = -1.0 + 2.0 * q;
    p.x *= iResolution.x / iResolution.y;

    // camera
    float3 ro = zoom * float3(4.0,4.0,4.0);
    //ray origin

    //float3 ro = float3(CameraPosition.xyz);
    
    //float3 rd = CameraDirection;
    float3 ta = float3(0.0, 0.0, 0.0);
    float3 ww = normalize(ta - ro);
    float3 uu = normalize(cross(ww, float3(0.0, 1.0, 0.0)));
    float3 vv = normalize(cross(uu, ww));
    float3 rd = normalize(p.x * uu + p.y * vv + 4.0 * ww);

    
    float2 tmm = iSphere(ro, rd, float4(0.0, 0.0, 0.0, 2.));

	// raymarch
    float3 col = raymarch(ro, rd, tmm);

	// shade
  
    col = 0.75 * (log(1.0 + col));
    col = clamp(col, 0., 1.);
    fragColor = float4(col, 1.0);

}
