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
SamplerState NormalRTSampler : register(s5);
SamplerState DepthRTSampler : register(s6);

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

half3 decodeNormal(half2 enc)
{
    half2 fenc = enc * 4 - 2;
    half f = dot(fenc, fenc);
    half g = sqrt(1 - f / 4);
    half3 n;
    n.xy = fenc * g;
    n.z = 1 - f / 2;
    return n;
}

half4 PS(PixelInput input) : SV_TARGET
{
    half3 normal = NormalRT.Sample(NormalRTSampler, input.uv).rgb;
    float4 depth = DepthRT.Sample(DepthRTSampler, input.uv);
    //return half4(abs(depth.gba), 1);

    if (depth.g < 1.001f && depth.g > 0.999f &&
        depth.b < 1.001f && depth.b > 0.999f &&
        depth.a < 0.001f && depth.a > -0.001f)
        return float4(1, 1, 1, 1);

    normal.rgb = decodeNormal(normal.xy);
    //return float4(normal, 1);
    
    //directional ¸ÕÀú
    float4 color = (float4) 0;
    
    Diffuse(color.rgb, normal);
    
    PointLightFunc(color.rgb, depth.gba, normal);
    SpotLightFunc(color.rgb, depth.gba, normal);

    return color;
}