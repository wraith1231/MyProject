#include "000_Header.hlsl"

Texture2D NormalRT : register(t0);
Texture2D DepthRT : register(t1);
Texture2D DiffuseRT : register(t2);

SamplerState NormalRTSampler : register(s0);
SamplerState DepthRTSampler : register(s1);
SamplerState DiffuseRTSampler : register(s2);

cbuffer PS_SSAOBuffer : register(b2)
{
    uint _ssaoSample;
    float _ssaoIntensity;
    float _ssaoScale;
    float _ssaoBias;

    float _ssaoRadius;
    float _ssaoMaxDistance;
    float2 _ssaoPadding1;

    float3 _ssaoMoo3;
    float _ssaoPadding2;
}

//#define SAMPLES 16
//#define INTENSITY 1.0f
//#define SCALE 2.5f
//#define BIAS 0.05f
//#define SAMPLE_RAD 0.02f
//#define MAX_DISTANCE 0.07f
//
//#define MOO3 float3(0.1031f, 0.11369f, 0.13787f)

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

PixelInput VS(uint vertexID : SV_VERTEXID)
{
    PixelInput output;
    
    output.position = float4(arrBasePos[vertexID].xy, 0.0f, 1.0f);
    output.uv = saturate(output.position.xy);
    output.uv.y = 1.0f - output.uv.y;
    
    return output;
}

float hash12(float2 p)
{
    float3 p3 = float3(p.xyx) * _ssaoMoo3;
    p3 += dot(p3, p3.yzx + 19.19f);
    return float(p3.x + p3.y) * p3.z;
}

float2 hash22(float2 p)
{
    float3 p3 = float3(p.xyx) * _ssaoMoo3;
    p3 += dot(p3, p3.yzx + 19.19f);
    return float2((p3.x + p3.y) * p3.z, (p3.x + p3.z) * p3.y);
}

float3 GetPosition(float2 uv)
{
    return DepthRT.Sample(DepthRTSampler, uv).gba;
}

float DoAmbientOcclusion(in float2 uv, in float2 uv2, in float3 pos, in float3 normal)
{
    float3 diff = GetPosition(uv + uv2) - pos;
    diff *= _valueFar;
    float l = length(diff);
    float3 v = diff / l;
    float d = l * _ssaoScale;
    float ao = max(0.0f, dot(normal, v) - _ssaoBias) * (1.0f / (1.0f + d));
    ao *= smoothstep(_ssaoMaxDistance, _ssaoMaxDistance * 0.5f, l);
    return ao;
}

float SpiralAO(float2 uv, float3 pos, float3 normal, float rad)
{
    float goldenAngle = 2.4f;
    float ao = 0.0f;
    float inv = 1.0f / float(_ssaoSample);
    float radius = 0.0f;

    float rotatePhase = hash12(uv * 100.0f) * 6.28f;
    float rStep = inv * rad;
    float2 spiralUV;

    for (int i = 0; i < _ssaoSample; i++)
    {
        spiralUV.x = sin(rotatePhase);
        spiralUV.y = cos(rotatePhase);
        radius += rStep;
        ao += DoAmbientOcclusion(uv, spiralUV * radius, pos, normal);
        rotatePhase += goldenAngle;
    }

    //ao *= inv;

    return ao;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 depth = DepthRT.Sample(DepthRTSampler, input.uv);
    float3 oPos = depth.gba;
    float3 normal = NormalRT.Sample(NormalRTSampler, input.uv);
    normal = NormalDecode3to3(normal);

    float ao = 0.0f;
    float rad = _ssaoRadius / (depth.r * _valueFar);

    ao = SpiralAO(input.uv, oPos, normal, rad);
    
    ao = 1.0f - ao * _ssaoIntensity;

    return float4(ao, ao, ao, 1.0f);
}