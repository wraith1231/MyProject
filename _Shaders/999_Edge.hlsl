#include "000_Header.hlsl"

cbuffer PS_TargetBuffer : register(b3)
{
    uint _bufferRender;
    uint _bufferSSAOSwitch;
    float2 _bufferPadding;
}

Texture2D NormalRT : register(t0);
Texture2D DepthRT : register(t1);
Texture2D DiffuseRT : register(t2);
Texture2D LightMeshRT : register(t3);
Texture2D ShadowMap : register(t4);
Texture2D SSAORT : register(t5);

SamplerState NormalRTSampler : register(s0);
SamplerState DepthRTSampler : register(s1);
SamplerState DiffuseRTSampler : register(s2);
SamplerState LightMeshRTSampler : register(s3);
SamplerState ShadowMapSampler : register(s4);
SamplerState SSAORTSampler : register(s5);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

static const float2 arrBasePos[4] =
{
    float2(-1.0f, 1.0f),
    float2(1.0f, 1.0f),
    float2(-1.0f, -1.0f),
    float2(1.0f, -1.0f),
};

PixelInput VS(uint VertexID : SV_VERTEXID)//VertexTextureNormalinput)
{
    PixelInput output;

    output.position = float4(arrBasePos[VertexID], 0.0f, 1.0f);
    output.uv = saturate(output.position.xy);
    output.uv.y = 1.0f - output.uv.y;
    
    return output;
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
    else if (_bufferRender == 5)
    {
        //float2 shadowUv = input.uv * 4.0f;
        //float shadowMapDepth = ShadowMap.Sample(ShadowMapSampler, shadowUv).r;
        float shadowMapDepth = ShadowMap.Sample(ShadowMapSampler, input.uv).r;

        return shadowMapDepth.rrrr;
    }
    else if (_bufferRender == 6)
    {
        return SSAORT.Sample(SSAORTSampler, input.uv);
    }

    float4 realColor = LightMeshRT.Sample(LightMeshRTSampler, input.uv);
    
    float nor = (3.141592f / 180.0f) * 3.0f;
    float dep = 0.005f;

    float nordot, ndot;
    float hei = 1.0f / _valueHeight;
    float wid = 1.0f / _valueWidth;

    float3 normal = NormalRT.Sample(NormalRTSampler, input.uv).rgb;
    normal = NormalDecode3to3(normal);
    float4 depthColor = DepthRT.Sample(DepthRTSampler, input.uv);

    if (_bufferSSAOSwitch == 1)
    {
        float ssao = SSAORT.Sample(SSAORTSampler, input.uv).r;
        realColor *= ssao;
    }

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
    normal1 = NormalDecode3to3(normal1);
    depth1 = DepthRT.Sample(DepthRTSampler, uv).r;

    ndot = dot(normal, normal1);
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    if (abs(depth - depth1) > dep)
        return float4(0, 0, 0, 1);
    
    uv.y += hei;
    uv.x -= wid;
    normal1 = NormalRT.Sample(NormalRTSampler, uv).rgb;
    normal1 = NormalDecode3to3(normal1);
    depth1 = DepthRT.Sample(DepthRTSampler, uv).r;

    ndot = dot(normal, normal1);
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    if (abs(depth - depth1) > dep)
        return float4(0, 0, 0, 1);
    
    uv.x += wid;
    uv.x += wid;
    normal1 = NormalRT.Sample(NormalRTSampler, uv).rgb;
    normal1 = NormalDecode3to3(normal1);
    depth1 = DepthRT.Sample(DepthRTSampler, uv).r;

    ndot = dot(normal, normal1);
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    if (abs(depth - depth1) > dep)
        return float4(0, 0, 0, 1);
    
    uv.x -= wid;
    uv.y += hei;
    normal1 = NormalRT.Sample(NormalRTSampler, uv).rgb;
    normal1 = NormalDecode3to3(normal1);
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