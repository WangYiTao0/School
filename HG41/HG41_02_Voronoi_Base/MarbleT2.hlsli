// based on https://www.shadertoy.com/view/XdByD1

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


float2 csqr(float2 a)
{
    return float2(a.x * a.x - a.y * a.y, 2. * a.x * a.y);
}

float2 iSphere(in float3 ro, in float3 rd, in float4 sph)//from iq
{
    float3 oc = ro; // - sph.xyz;
    float b = dot(oc, rd);
    float c = dot(oc, oc) - sph.w * sph.w;
    float h = 1.0; //b*b - c;
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
    
    /////T2
    
    //float t = tminmax.x;
    ////float dt = .02;
    //float dt = .2 - .195 * cos(iTime * .05); //animated
    //float3 col = float3(0.0, 0.0, 0.0);
    //float c = 0.;
    //for (int i = 0; i < 64; i++)
    //{
    //    t += dt * exp(-2. * c);
    //    if (t > tminmax.y)
    //        break;
    //    float3 pos = ro + t * rd;
        
    //    c = map(ro + t * rd, sctime);
        
    //    //col = .99 * col + .08 * float3(c * c, c, c * c * c); //green	
    //    //col = .99 * col + .08 * float3(c * c * c, c * c, c); //blue
    //    col = .99 * col + .08 * pow(cos(.75 * c - float3(1, .75, .5)), float3(16, 16, 16));
    //}
    //return col;
}


void MarbleT2(out float4 fragColor, in float2 fragCoord)
{
    float2 Resolution = { 1600, 900 };
    
    float time = iTime;
    float2 q = fragCoord.xy ;
        float2 p = -1.0 + 2.0 * q;
        p.x *= Resolution.x / Resolution.y;
        float2 m = float2(0., 0.);

        float3 ro = float3(6., 6., 6.);
        float3 ta = float3(0.0, 0.0, 0.0);
        float3 ww = normalize(ta - ro);
        float3 uu = normalize(cross(ww, float3(0.0, 1.0, 0.0)));
        float3 vv = normalize(cross(uu, ww));
        float3 rd = normalize(p.x * uu + p.y * vv + 4.0 * ww);

    
        float2 tmm = iSphere(ro, rd, float4(0., 0., 0., 2.));

	// raymarch
        float3 col = raymarch(ro, rd, tmm, float2(sin(iTime), cos(iTime)));
    	
	// shade    
        col = .5 * (log(1. + col));
        col = clamp(col, 0., 1.);
        fragColor = float4(col, 1.0);

    }
