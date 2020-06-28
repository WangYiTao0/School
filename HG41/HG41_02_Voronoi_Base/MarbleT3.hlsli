// License Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License.
// Created by S. Guillitte 2015

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

float zoom = 1.;

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
    float3 oc = ro - sph.xyz;
    float b = dot(oc, rd);
    float c = dot(oc, oc) - sph.w * sph.w;
    float h = b * b - c;
    if (h < 0.0)
        return float2(-1.0,-1.0);
    h = sqrt(h);
    return float2(-b - h, -b + h);
}

float map(in float3 p, float2 sctime)
{
	
    float res = 0.;
	
    float3 c = p;
    c.xy = c.xy * sctime.x + float2(c.y, c.x) * sctime.y;
    for (int i = 0; i < 10; ++i)
    {
        p = .7 * abs(p) / dot(p, p) - .7;
        p.yz = csqr(p.yz);
        p = p.zxy;
        res += exp(-19. * abs(dot(p, c)));
        
    }
    return res / 2.;
}



float3 raymarch(in float3 ro, float3 rd, float2 tminmax, float2 sctime)
{
    //tminmax += float2(1.,1.) * sin( iTime * 1.3)*3.0;
    float3 one3 = float3(1., 1., 1.);
    float3 t = one3 * tminmax.x;
    
    float3 dt = float3(.07, 0.02, 0.05);
    float3 col = float3(0., 0., 0.);
    float3 c = one3 * 0.;
    for (int i = 0; i < 64; i++)
    {
        float3 s = float3(2.0, 3.0, 4.0);
        t += dt * exp(-s * c);
        float3 a = step(t, one3 * tminmax.y);
        float3 pos = ro + t * rd;
        
        c.x = map(ro + t.x * rd, sctime);
        c.y = map(ro + t.y * rd, sctime);
        c.z = map(ro + t.z * rd, sctime);
        
        col = lerp(col, .99 * col + .08 * c * c * c, a);
    
    }

    
    float3 c0 = float3(0.4, 0.3, 0.99);
    float3 c1 = float3(0.9, 0.7, 0.0);
    float3 c2 = float3(0.9, 0.1, 0.2);
    return c0 * col.x + c1 * col.y + c2 * col.z;
    
    //float t = tminmax.x;
    ////float dt = .02;
    //float dt = .2 - .195*cos(iTime*.05);//animated
    //float3 col = float3(0.0, 0.0, 0.0);
    //float c = 0.;
    //for (int i = 0; i < 64; i++)
    //{
    //    t += dt * exp(-2. * c);
    //    if (t > tminmax.y)
    //        break;
    //    float3 pos = ro + t * rd;
        
    //    c = map(ro + t * rd,sctime);
        
    //   //col = .99 * col + .08 * float3(c * c, c, c * c * c); //green	
    //    //col = .99 * col + .08 * float3(c * c * c, c * c, c); //blue
    //    col = .99 * col + .08 * pow(cos(.75 * c - float3(1, .75, .5)), float3(16,16,16));
    //}
    //return col;

}


void MarbleT3(out float4 fragColor, in float2 fragCoord)
{
    float time = iTime;
    float2 q = fragCoord.xy * 5 -2;
  
    float2 p = -1.0 + 2.0 * q;
    p.x *= iResolution.x / iResolution.y;
    float2 m = float2(0.,0.);
    if (iMouse.z > 0.0)
        m = iMouse.xy / iResolution.xy * 3.14;
    m -= .5;

    // camera

    float3 ro = zoom * float3(4.0,4.0,4.0);
    ro.yz = mul(ro.yz, rot(m.y));
    ro.xz = mul(ro.xz, rot(m.x + 0.1 * time));
    float3 ta = float3(0.0, 0.0, 0.0);
    float3 ww = normalize(ta - ro);
    float3 uu = normalize(cross(ww, float3(0.0, 1.0, 0.0)));
    float3 vv = normalize(cross(uu, ww));
    float3 rd = normalize(p.x * uu + p.y * vv + 4.0 * ww) ;

    
    float2 tmm = iSphere(ro, rd, float4(0., 0., 0., 2.));

	// raymarch
    float3 col = raymarch(ro, rd, tmm, float2(sin(iTime), cos(iTime)));
    
    float4 bgC = float4(0.0f, 0.5f, 0.5f, 1.0f);
    
    if (tmm.x < 0.)
        col = bgC;
    else
    {
        float nor = (ro + tmm.x * rd) / 2.;
        nor = reflect(rd, nor);
        float fre = pow(.5 + clamp(dot(nor, rd), 0.0, 1.0), 3.) * 1.3;
        col += bgC.rgb * fre;
    
    }
	// shade
    
        col = .5 * (log(1. + col));
        col = clamp(col, 0., 1.);
        fragColor = float4(col, 1.0);

    }