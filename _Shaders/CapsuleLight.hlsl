#include "000_Header.hlsl"

cbuffer DS_CapsuleLight : register(b2)
{
    float _capsuleHalfSeg;
    float _capsuleRange;
    float2 _capsuleDSPadding;
}

cbuffer PS_CapsuleLight : register(b2)
{
    float3 _capsulePos;
    float _capsuleRangeRcp;

    float3 _capsuleDir;
    float _capsuleLen;

    float3 _capsuleColor;
    float _capsulePSPadding;
}

Texture2D NormalRT : register(t0);
Texture2D DepthRT : register(t1);
Texture2D DiffuseRT : register(t2);

SamplerState NormalRTSampler : register(s0);
SamplerState DepthRTSampler : register(s1);
SamplerState DiffuseRTSampler : register(s2);

///////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex Shader
///////////////////////////////////////////////////////////////////////////////////////////////////

float4 VS() : SV_Position
{
    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Hull Shader
///////////////////////////////////////////////////////////////////////////////////////////////////

struct HS_CONSTANT_DATA_OUTPUT
{
    float Edges[4] : SV_TessFactor;
    float Inside[2] : SV_InsideTessFactor;
};

HS_CONSTANT_DATA_OUTPUT CapsuleConstantHS()
{
    HS_CONSTANT_DATA_OUTPUT output;

    float tessfactor = 12.0f;
    output.Edges[0] = output.Edges[1] = output.Edges[2] = output.Edges[3] = tessfactor;
    output.Inside[0] = output.Inside[1] = tessfactor;

    return output;
}

struct HS_OUTPUT
{
    float4 CapsuleDir : POSITION;
};

static const float4 CapsuleDir[2] =
{
    float4(1.0f, 1.0f, 1.0f, 1.0f),
    float4(-1.0f, 1.0f, -1.0f, 1.0f),
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CapsuleConstantHS")]
HS_OUTPUT HS(uint PatchID : SV_PRIMITIVEID)
{
    HS_OUTPUT output;

    output.CapsuleDir = CapsuleDir[PatchID];

    return output;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Domain Shader
///////////////////////////////////////////////////////////////////////////////////////////////////

#define CylinderPortion 0.3
#define SpherePortion   (1.0 - CylinderPortion)
#define ExpendAmount    (1.0 + CylinderPortion)

struct DS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

[domain("quad")]
DS_OUTPUT DS(HS_CONSTANT_DATA_OUTPUT input, float2 UV : SV_DOMAINLOCATION, const OutputPatch<HS_OUTPUT, 4> quad)
{
    float2 posClipSpace = UV.xy * 2.0f - 1.0f;

    float2 clipAbs = abs(posClipSpace);
    float maxLen = max(clipAbs.x, clipAbs.y);

    float2 posClipSpaceNoCylAbs = saturate(clipAbs * ExpendAmount);
    float2 posClipSpaceNoCyl = sign(posClipSpace) * posClipSpaceNoCylAbs;
    float maxLenNoCapsule = max(posClipSpaceNoCylAbs.x, posClipSpaceNoCylAbs.y);

    float3 normDir = normalize(float3(posClipSpaceNoCyl.xy, (maxLenNoCapsule - 1.0f)) * _capsuleRange);
    float cylinderOffSetZ = saturate(maxLen - min(maxLenNoCapsule, SpherePortion)) / CylinderPortion;

    normDir *= _capsuleRange;
    float4 posLS = float4(normDir.xy, normDir.z + _capsuleHalfSeg - cylinderOffSetZ * _capsuleHalfSeg, 1.0f);

    posLS *= quad[0].CapsuleDir;

    DS_OUTPUT output;
    output.position = mul(posLS, _projection);
    output.uv = (output.position.xy / output.position.w + 1.0f) * 0.5f;
    output.uv.y = 1 - output.uv.y;

    return output;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Pixel Shader
///////////////////////////////////////////////////////////////////////////////////////////////////

float4 PS(DS_OUTPUT input) : SV_TARGET
{
    float4 depth = DepthRT.Sample(DepthRTSampler, input.uv);
    float3 oPos = depth.gba;
    float3 normal = NormalRT.Sample(NormalRTSampler, input.uv);
    normal = NormalDecode3to3(normal);
    float4 diffuse = DiffuseRT.Sample(DiffuseRTSampler, input.uv);
    
    float3 viewPos = _viewInverse._41_42_43;
    
    float3 toEye = viewPos - oPos;
    float3 toCapsuleStart = oPos - _capsulePos;
    float distOnLine = dot(toCapsuleStart, _capsuleDir) / _capsuleLen;
    distOnLine = saturate(distOnLine) * _capsuleLen;
    float3 pointOnLine = _capsulePos + _capsuleDir * distOnLine;
    float3 toLight = pointOnLine - oPos;
    float distToLight = length(toLight);

    toLight /= distToLight;
    float NDotL = saturate(dot(toLight, normal));
    float3 fc = diffuse * NDotL;

    toEye = normalize(toEye);
    float3 HalfWay = normalize(toEye + toLight);
    float NDotH = saturate(dot(HalfWay, normal));
    fc += NDotH;

    float distToLightNorm = 1.0f - saturate(distToLight * _capsuleRangeRcp);
    float attn = distToLightNorm * distToLightNorm;
    fc *= _capsuleColor * attn;

    return float4(fc, 1.0f);

}