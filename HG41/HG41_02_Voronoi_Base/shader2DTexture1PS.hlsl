
#include "shaderNoise.hlsli"
//#include "marble.hlsli"
//#include "BaseVoronoi.hlsli"
//#include "Voronoise.hlsli"
//#include "VorocracksMarble.hlsli"
//#include "MarbleT2.hlsli"
#include "MarbleT3.hlsli"



Texture2D g_Texture : register(t0);
//=============================================================================
// ピクセルシェーダ
//=============================================================================
void main(in float4 inPosition : SV_POSITION,
			in float4 inNormal : NORMAL0,
			in float4 inDiffuse : COLOR0,
			in float2 inTexCoord : TEXCOORD0,

			out float4 outDiffuse : SV_Target)
{

    //outDiffuse.rgb = fmod(floor(inTexCoord.x  * 2.0),2);     
	//斜めストライプ
	//outDiffuse.rgb = fmod(floor(inTexCoord.x * 2.0 + inTexCoord.y * 2.0), 2);
	 

	//float sx = sin(1 * 3.14 * inTexCoord.x) / 2 * 0.5;
	//float sy = sin(1 * 3.14 * inTexCoord.y) / 2 * 0.5;

	//outDiffuse.rgb = (sx + sy)/2;

	//outDiffuse.rgb = sin(voronoi2(inTexCoord * 0.5f) * 30.0f);

	//チエック模様

	//outDiffuse.rgb = perlinNoise2(inTexCoord * 0.5) * 0.5f + 0.5f;
 //   float color = fbm2(inTexCoord * 0.5, 3);

 //   color = sin(color * 20);
 //   outDiffuse.rgb = color;
	
   // 木目

    //float color = fbm2(inTexCoord * 0.2, 2);
    //color = saturate(sin(color * 100) * 0.5 + 0.5);
    //color = color * color;
    //outDiffuse.rgb = float3(0.5, 0.2, 0.1) * color +
				//	float3(0.9, 0.7, 0.4) * (1.0 - color);
	
    //outDiffuse.a = 1.0;
	
	//BaseVoronoi
	
    float4 fragColor = { 0.0, 0.0, 0.0, 1.0 };
    //BaseVoronoi(fragColor, inTexCoord);
    //Voronoise(fragColor, inTexCoord);
    //MarbleT2(fragColor, inTexCoord);
    MarbleT3(fragColor, inTexCoord);
    outDiffuse = fragColor;
}
