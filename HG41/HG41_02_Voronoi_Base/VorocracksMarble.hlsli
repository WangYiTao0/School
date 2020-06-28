// variant of Vorocracks: https://shadertoy.com/view/lsVyRy
// integrated with cracks here: https://www.shadertoy.com/view/Xd3fRN


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

#define MM 0

#define VARIANT 1             // 1: amplifies Voronoi cell jittering
#if VARIANT
float ofs = .5; // jitter Voronoi centers in -ofs ... 1.+ofs
#else
      float ofs = 0.;
#endif
    
//int FAULT = 1;                 // 0: crest 1: fault

float RATIO = 1., // stone length/width ratio
 /*   STONE_slope = .3,        // 0.  .3  .3  -.3
      STONE_height = 1.,       // 1.  1.  .6   .7
      profile = 1.,            // z = height + slope * dist ^ prof
 */   
      CRACK_depth = 3.,
      CRACK_zebra_scale = 1., // fractal shape of the fault zebra
      CRACK_zebra_amp = .67,
      CRACK_profile = 1., // fault vertical shape  1.  .2 
      CRACK_slope = 50., //                      10.  1.4
      CRACK_width = .0;
    

// std int hash, inspired from https://www.shadertoy.com/view/XlXcW4
float3 hash3(float3 x)
{
    return float3(x) / float(0xffffffffU) + 1e-30; // <- eps to fix a windows/angle bug
}

// === Voronoi =====================================================
// --- Base Voronoi. inspired by https://www.shadertoy.com/view/MslGD8

#define hash22(p)  frac( 18.5453 * sin(mul (p , float2x2(127.1,311.7,269.5,183.3))) )
#define disp(p) ( -ofs + (1.+2.*ofs) * hash22(p) )

float3 voronoi(float2 u)  // returns len + id
{
    float2 iu = floor(u), v;
    float m = 1e9, d;
#if VARIANT
    for (int k = 0; k < 25; k++)
    {
        float2 p = iu + float2(k % 5 - 2, k / 5 - 2),
#else
    for( int k=0; k < 9; k++ ) {
        float2  p = iu + float2(k%3-1,k/3-1),
#endif
            o = disp(p),
      	      r = p - u + o;
        d = dot(r, r);
        if (d < m)
            m = d, v = r;
    }

    return float3(sqrt(m), v + u);
}

// --- Voronoi distance to borders. inspired by https://www.shadertoy.com/view/ldl3W8
float3 voronoiB(float2 u)  // returns len + id
{
    float2 iu = floor(u), C, P;
    float m = 1e9, d;
#if VARIANT
    for (int k = 0; k < 25; k++)
    {
        float2 p = iu + float2(k % 5 - 2, k / 5 - 2),
#else
    for( int k=0; k < 9; k++ ) {
        float2  p = iu + float2(k%3-1,k/3-1),
#endif
              o = disp(p),
      	      r = p - u + o;
        d = dot(r, r);
        if (d < m)
            m = d, C = p - iu, P = r;
    }

    m = 1e9;
    
    for (int k = 0; k < 25; k++)
    {
        float2 p = iu + C + float2(k % 5 - 2, k / 5 - 2),
		     o = disp(p),
             r = p - u + o;

        if (dot(P - r, P - r) > 1e-5)
            m = min(m, .5 * dot((P + r), normalize(r - P)));
    }

    return float3(m, P + u);
}

// === pseudo Perlin noise =============================================
#define rot(a) float2x2(cos(a),-sin(a),sin(a),cos(a))
int MOD = 1; // type of Perlin noise
    
// --- 2D
#define hash21(p) frac(sin(dot(p,float2(127.1,311.7)))*43758.5453123)
float noise2(float2 p)
{
    float2 i = floor(p);
    float2 f = frac(p);
    f = f * f * (3. - 2. * f); // smoothstep

    float v = lerp(lerp(hash21(i+float2(0,0)), hash21(i+float2(1,0)), f.x),
                  lerp(hash21(i+float2(0,1)), hash21(i+float2(1,1)), f.x), f.y);
    return MOD == 0 ? v
	       : MOD == 1 ? 2. * v - 1.
           : MOD == 2 ? abs(2. * v - 1.)
                    : 1. - abs(2. * v - 1.);
}

float fbm2(float2 p)
{
    float v = 0., a = .5;
    float2x2 R = rot(.37);

    for (int i = 0; i < 9; i++, p *= 2., a /= 2.) 
        p = mul(p, R),
        v += a * noise2(p);

    return v;
}
#define noise22(p) float2(noise2(p),noise2(p+17.7))
float2 fbm22(float2 p)
{
    float2 v = float2(0,0);
    float a = .5;
    float2x2 R = rot(.37);

    for (int i = 0; i < 6; i++, p *= 2., a /= 2.) 
        p = mul(p, R),
        v += a * noise22(p);

    return v;
}
float2 mfbm22(float2 p)
{ // multifractal fbm 
    float2 v = float2(1,1);
    float a = .5;
    float2x2 R = rot(.37);

    for (int i = 0; i < 6; i++, p *= 2., a /= 2.) 
        p = mul(p, R),
        //v *= 1.+noise22(p);
          v += v * a * noise22(p);

    return v - 1.;
}

/*
// --- 3D 
#define hash31(p) fract(sin(dot(p,float3(127.1,311.7, 74.7)))*43758.5453123)
float noise3(float3 p) {
    float3 i = floor(p);
    float3 f = fract(p); f = f*f*(3.-2.*f); // smoothstep

    float v= lerp( lerp( lerp(hash31(i+float3(0,0,0)),hash31(i+float3(1,0,0)),f.x),
                       lerp(hash31(i+float3(0,1,0)),hash31(i+float3(1,1,0)),f.x), f.y), 
                  lerp( lerp(hash31(i+float3(0,0,1)),hash31(i+float3(1,0,1)),f.x),
                       lerp(hash31(i+float3(0,1,1)),hash31(i+float3(1,1,1)),f.x), f.y), f.z);
	return   MOD==0 ? v
	       : MOD==1 ? 2.*v-1.
           : MOD==2 ? abs(2.*v-1.)
                    : 1.-abs(2.*v-1.);
}

float fbm3(float3 p) {
    float v = 0.,  a = .5;
    float2x2 R = rot(.37);

    for (int i = 0; i < 9; i++, p*=2.,a/=2.) 
        p.xy *= R, p.yz *= R,
        v += a * noise3(p);

    return v;
}
*/
    
// ======================================================

void VorocracksMarble(out float4 O, float2 U)
{
    U *= 4. / iResolution.y;
    U.x += iTime; // for demo
 // O = float4( 1.-voronoiB(U).x,voronoi(U).x, 0,0 );   // for tests
    float2 I = floor(U / 2.);
    bool vert = fmod(I.x + I.y, 2.) == 0.; //if (vert) U = U.yx;
    float3 H0;


    for (float i = 0.; i < CRACK_depth; i++)
    {
        float2 V = U / float2(RATIO, 1), // voronoi cell shape
             D = CRACK_zebra_amp * fbm22(U / CRACK_zebra_scale) * CRACK_zebra_scale;
        float3 H = voronoiB(V + D);
        if (i == 0.)
            H0 = H;
        float d = H.x; // distance to cracks
   /*         r = voronoi(V).x,                       // distance to center
              s = STONE_height-STONE_slope*pow(r,profile);// stone interior
    */                                                // cracks
        d = min(1., CRACK_slope * pow(max(0., d - CRACK_width), CRACK_profile));
  
        O += float4(1.0 - d, 1.0 - d, 1.0 - d, 1.0 - d) / exp2(i);
        U = mul(U, 1.5 * rot(.37));
    }
    /*
    O = float4( 
        FAULT==1 ? d * s                              // fault * stone
                 : lerp(1.,s, d)                       // crest or stone
            ); */
    if (vert)
        O = 1. - O;
    O *= float4(.9, .85, .85, 1); // for demo
    
#if MM
    O.g = hash3(ufloat3(H0.yz,1)).x;
#endif
}