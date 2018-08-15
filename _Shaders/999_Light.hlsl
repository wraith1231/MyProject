#include "000_Header.hlsl"

//cbuffer PS_Value : register(b2)
//{
//    float _valueWidth;
//    float _valueHeight;
//    float _valueNear;
//    float _valueFar;
//}

cbuffer PS_LightValue : register(b3)
{
    float _lightAttenuation;
    float _specularPower;
}

Texture2D NormalRT : register(t5);
Texture2D DepthRT : register(t6);
Texture2D LightMeshRT : register(t7);

SamplerState NormalRTSampler : register(s5);
SamplerState DepthRTSampler : register(s6);
SamplerState LightMeshRTSampler : register(s7);

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 view : VIEW0;
    float2 uv : TEXCOORD0;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    float4 world = mul(input.position, _world);
    output.position = mul(world, _view);
    output.position = mul(output.position, _projection);
    
    output.view = GetViewPosition() - world.xyz;
    output.uv = input.uv;
    
    return output;
}

half4 PS(PixelInput input) : SV_TARGET
{
    //return LightMeshRT.Sample(LightMeshRTSampler, input.uv);
    float4 light = LightMeshRT.Sample(LightMeshRTSampler, input.uv);
    float3 normal = NormalRT.Sample(NormalRTSampler, input.uv).rgb;
    //half3 normal;
    //normal.rg = NormalRT.Sample(NormalRTSampler, input.uv).rg;
    //normal = NormalDecode(normal.xy);
    float4 depth = DepthRT.Sample(DepthRTSampler, input.uv);
    
    float4 color = (float4) 0;
    
    Diffuse(color.rgb, normal);
    [branch]
    if (length(light.xyz) > 0.5f)
        return color;
    else
    {
        
        PointLightFunc(color.rgb, depth.gba, normal);
        SpotLightFunc(color.rgb, depth.gba, normal);

        return color;
    }
}