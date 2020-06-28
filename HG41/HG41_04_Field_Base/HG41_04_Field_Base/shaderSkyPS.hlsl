

#include "shaderNoise.hlsl"

Texture2D		g_Texture : register(t0);
SamplerState	g_SamplerState : register(s0);

// 定数バッファ
cbuffer ConstatntBuffer : register(b0)
{
    matrix World;
    matrix View;
    matrix Projection;

    float4 CameraPosition;
    float4 Parameter;

}



//=============================================================================
// ピクセルシェーダ
//=============================================================================
void main( in  float4 inPosition		: SV_POSITION,
            in float4 inWorldPosition   : POSITION0,
			in  float4 inNormal			: NORMAL0,
			in  float4 inDiffuse		: COLOR0,
			in  float2 inTexCoord		: TEXCOORD0,

			out float4 outDiffuse		: SV_Target )
{


    float cloud = fbm2(inTexCoord, 10, Parameter.x);
    
    cloud = cloud * 5.0 + 0.9;
    cloud = saturate(cloud);
    cloud = cloud * 0.9 + 0.05;
    
    cloud = 1.0 - voronoi2(inTexCoord * 5.0) * 0.2;
    
    //outDiffuse.rgb = float3(1.0, 1.0, 1.0)*cloud  +float3(0.0, 0.3, 0.8) * (1.0-cloud);
    outDiffuse.rgb = g_Texture.Sample(g_SamplerState, float2(cloud, 0.0));
    outDiffuse.a = 1.0;
    
    
}
