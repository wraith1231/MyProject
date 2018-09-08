#include "000_Header.hlsl"

cbuffer DS_SpotLight : register(b2)
{
    float _sinAngle;
    float _cosAngle;
    float2 _spotDSPadding;
}

cbuffer PS_SpotLight : register(b2)
{
    float3 _spotLightPos;
    float _spotLightRangeRcp;

    float3 _spotDirToLight;
    float _spotCosOuter;

    float3 _spotLightColor;
    float _spotCosConeAttnRange;
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

HS_CONSTANT_DATA_OUTPUT ConstantHS()
{
    HS_CONSTANT_DATA_OUTPUT output;

    float tessfactor = 12.0f;
    output.Edges[0] = output.Edges[1] = output.Edges[2] = output.Edges[3] = tessfactor;
    output.Inside[0] = output.Inside[1] = tessfactor;

    return output;
}

struct HS_OUTPUT
{
    float3 HemiDir : POSITION;
};

static const float3 HemiDir[2] =
{
    float3(1.0f, 1.0f, 1.0f),
    float3(-1.0f, 1.0f, -1.0f),
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]
HS_OUTPUT HS(uint PatchID : SV_PRIMITIVEID)
{
    HS_OUTPUT output;

    output.HemiDir = HemiDir[PatchID];

    return output;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Domain Shader
///////////////////////////////////////////////////////////////////////////////////////////////////

#define CylinderPortion 0.2
#define ExpendAmount (1.0f + CylinderPortion)

struct DS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

[domain("quad")]
DS_OUTPUT DS(HS_CONSTANT_DATA_OUTPUT input, float2 UV : SV_DOMAINLOCATION, const OutputPatch<HS_OUTPUT, 4> quad)
{
    float2 posClipSpace = UV.xy * float2(2.0f, -2.0f) + float2(-1.0f, 1.0f);

    float2 clipAbs = abs(posClipSpace);
    float maxLen = max(clipAbs.x, clipAbs.y);

    float2 clipSpaceNoCylAbs = saturate(clipAbs * ExpendAmount);
    float maxLenNoCap = max(clipSpaceNoCylAbs.x, clipSpaceNoCylAbs.y);
    float2 clipSpaceNoCyl = sign(posClipSpace) * clipSpaceNoCylAbs;

    float3 halfSpacePos = normalize(float3(clipSpaceNoCyl, 1.0f - maxLenNoCap));
    halfSpacePos = normalize(float3(halfSpacePos.xy * _sinAngle, _cosAngle));

    float cylinderOffsetZ = saturate((maxLen * ExpendAmount - 1.0f) / CylinderPortion);

    float4 posLS = float4(halfSpacePos.xy * (1.0f - cylinderOffsetZ), halfSpacePos.z - cylinderOffsetZ * _cosAngle, 1.0f);

    DS_OUTPUT output;
    output.position = mul(posLS, _projection);
    //output.uv = output.position.xy / output.position.w;
    output.uv = (output.position.xy / output.position.w + 1.0f) * 0.5f;
    output.uv.y = 1.0f - output.uv.y;
    
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
    
    float3 toLight = _spotLightPos - oPos;
    float3 toEye = GetViewPosition() - oPos;
    float distToLight = length(toLight);

    toLight /= distToLight;
    float NDotL = saturate(dot(toLight, normal));
    float3 fc = diffuse * NDotL;

    toEye = normalize(toEye);
    float3 halfWay = normalize(toEye + toLight);
    float NDotH = saturate(dot(halfWay, normal));
    fc += NDotH;

    float cosAng = dot(_spotDirToLight, toLight);
    float conAtt = saturate((cosAng - _spotCosOuter) / _spotCosConeAttnRange);
    conAtt *= conAtt;

    float distToLightNorm = 1.0f - saturate(distToLight * _spotLightRangeRcp);
    float attn = distToLightNorm * distToLightNorm;
    fc *= _spotLightColor * attn * conAtt;

    return float4(fc, 1.0f);

}