
#include "shaderNoise.hlsli"
//#include "marble.hlsli"
//#include "BaseVoronoi.hlsli"
//#include "Voronoise.hlsli"
//#include "VorocracksMarble.hlsli"
#include "MarbleT2.hlsli"

Texture2D		g_Texture : register(t0);

//// 定数バッファ
//cbuffer ConstatntBuffer : register(b0)
//{
//    matrix World;
//    matrix View;
//    matrix Projection;

//    float4 CameraPosition;
//}

//=============================================================================
// ピクセルシェーダ
//=============================================================================
void main(  in float4 inPosition		: SV_POSITION,
            in float4 inWorldPosition   : POSITION0,
			in float4 inLocalPosition   : POSITION1,
			in float4 inNormal			: NORMAL0,
			in float4 inDiffuse		    : COLOR0,
			in float2 inTexCoord		: TEXCOORD0,

			out float4 outDiffuse		: SV_Target )
{

    outDiffuse.rgb = 1.0;
    
    
    //ライティング
    //float3 lightDir = float3(1.0, -1.0, 1.0);
    //lightDir = normalize(lightDir);
    
    //float light = 0.5 - dot(inNormal.xyz, lightDir) * 0.5;
    //outDiffuse.rgb *= light;
        
    float color;
    //color = fbm2(inTexCoord * 10.0, 5);
    //color = color * 0.5 + 0.5;
    
    //color = valueNoise3(inLocalPosition.xyz*10);
    
    //color = randomf3(inLocalPosition.xyz);
    
    //outDiffuse.rgb = color;
    
    //木目
    //color = fbm3(float3(inLocalPosition.x, inLocalPosition.y, inLocalPosition.z * 0.1), 3);
    //color = saturate(sin(color * 100) * 0.5 + 0.5);
    //color = color * color;
    //outDiffuse.rgb = float3(0.5, 0.2, 0.1) * color + float3(0.9, 0.7, 0.4) * (1.0 - color);
    //outDiffuse.a = 1.0;
    
    float4 fragColor = { 0.0, 0.0, 0.0, 1.0 };
    //BaseVoronoi(fragColor, inTexCoord);
    //Voronoise(fragColor, inTexCoord);
    MarbleT2(fragColor, inTexCoord);
    outDiffuse = fragColor;
}
