#include "000_Header.hlsl"


cbuffer PS_Buffer : register(b3)
{
    uint _bufferRender;
    float3 _bufferPadding;
}

Texture2D NormalRT : register(t5);
Texture2D DepthRT : register(t6);
Texture2D DiffuseRT : register(t7);
Texture2D LightMeshRT : register(t8);

SamplerState NormalRTSampler : register(s5);
SamplerState DepthRTSampler : register(s6);
SamplerState DiffuseRTSampler : register(s7);
SamplerState LightMeshRTSampler : register(s8);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);
    
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


float4 PS(PixelInput input) : SV_TARGET
{
    [branch]
    if (_bufferRender == 0)
    {
        float depth = DepthRT.Sample(DepthRTSampler, input.uv).r;
        return float4(depth.rrrr);
    }
    else if (_bufferRender == 1)
    {
        float3 oPos = DepthRT.Sample(DepthRTSampler, input.uv).gba;
        return float4(oPos, 1);

    }
    else if (_bufferRender == 2)
    {
        float3 normal = NormalRT.Sample(NormalRTSampler, input.uv);
        return float4(normal, 1);
    }
    else if (_bufferRender == 3)
    {
        float4 diffuse = DiffuseRT.Sample(DiffuseRTSampler, input.uv);
        return diffuse;
    }
    else if (_bufferRender == 4)
    {
        return LightMeshRT.Sample(LightMeshRTSampler, input.uv);
    }
    float4 realColor = LightMeshRT.Sample(LightMeshRTSampler, input.uv);

    float nor = (3.141592f / 180.0f) * 3.0f;
    float dep = 0.005f;

    float nordot, ndot;
    float hei = 1.0f / _valueHeight;
    float wid = 1.0f / _valueWidth;

    float3 normal = NormalRT.Sample(NormalRTSampler, input.uv).rgb;
    float4 depthColor = DepthRT.Sample(DepthRTSampler, input.uv);
    float depth = depthColor.r;
    float3 oPos = depthColor.gba;

    float depth1;
    float3 normal1;
    float2 uv = input.uv;
   
    nordot = dot(normal, normal);
    
    float factor = 0;
    [branch]
    if (_fogUse == 1)
    {
        float dist = depth * _valueFar;
        factor = saturate((_fogEnd - dist) / (_fogEnd - _fogStart));
    
        if (factor < 0.4f)
            return lerp(_fogColor, realColor, factor) * (_sunColor * _sunIntensity);
    }
    
    uv.y -= hei;
    normal1 = NormalRT.Sample(NormalRTSampler, uv).rgb;
    depth1 = DepthRT.Sample(DepthRTSampler, uv).r;

    ndot = dot(normal, normal1);
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    if (abs(depth - depth1) > dep)
        return float4(0, 0, 0, 1);
    
    uv.y += hei;
    uv.x -= wid;
    normal1 = NormalRT.Sample(NormalRTSampler, uv).rgb;
    depth1 = DepthRT.Sample(DepthRTSampler, uv).r;

    ndot = dot(normal, normal1);
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    if (abs(depth - depth1) > dep)
        return float4(0, 0, 0, 1);
    
    uv.x += wid;
    uv.x += wid;
    normal1 = NormalRT.Sample(NormalRTSampler, uv).rgb;
    depth1 = DepthRT.Sample(DepthRTSampler, uv).r;

    ndot = dot(normal, normal1);
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    if (abs(depth - depth1) > dep)
        return float4(0, 0, 0, 1);
    
    uv.x -= wid;
    uv.y += hei;
    normal1 = NormalRT.Sample(NormalRTSampler, uv).rgb;
    depth1 = DepthRT.Sample(DepthRTSampler, uv).r;

    ndot = dot(normal, normal1);
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    if (abs(depth - depth1) > dep)
        return float4(0, 0, 0, 1);

    if(_fogUse == 1)
        return lerp(_fogColor, realColor, factor) * (_sunColor * _sunIntensity);
    else
        return (realColor) * (_sunColor * _sunIntensity);
}