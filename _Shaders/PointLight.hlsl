#include "000_Header.hlsl"

cbuffer PS_PointLight : register(b2)
{
    float3 _pointLightPos;
    float _pointLightRangeRcp;

    float3 _pointLightColor;
    float _pointLightPadding1;
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

    float tessfactor = 18.0f;
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

    float3 normDir = normalize(float3(posClipSpace.xy, (maxLen - 1.0f)) * quad[0].HemiDir);
    float4 posLS = float4(normDir.xyz, 1.0f);

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
    float3 oPos = DepthRT.Sample(DepthRTSampler, input.uv).gba;
    //ȭ����� depth���� �ʿ��� �Ȱ�����
    float3 normal = NormalRT.Sample(NormalRTSampler, input.uv);
    normal = NormalDecode3to3(normal);
    float4 diffuse = DiffuseRT.Sample(DiffuseRTSampler, input.uv);
    
    float3 tolight = _pointLightPos - oPos;
    float3 viewPos = _viewInverse._41_42_43;
    
    float3 toEye = viewPos - oPos;
    float dist = length(tolight);

    tolight = normalize(tolight);
    float ndotl = saturate(dot(tolight, normal));
    float3 fc = diffuse.rgb * ndotl;
    
    toEye = normalize(toEye);
    float3 halfway = normalize(toEye + tolight);
    float ndoth = saturate(dot(halfway, normal));
    fc += ndoth;

    float distToLightNorm = 1.0f - saturate(dist * _pointLightRangeRcp);
    float attn = distToLightNorm * distToLightNorm;
    fc *= _pointLightColor * attn;
    
    return float4(fc, 1.0f);

}