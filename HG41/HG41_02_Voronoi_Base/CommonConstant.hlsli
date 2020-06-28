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