#include "000_Header.hlsl"

cbuffer PS_DIRLIGHT : register(b2)
{
    float3 _dirAmbientDown;
    float _dirPadding1;
    float3 _dirAmbientRange;
    float _dirPadding2;
    float3 _dirToLight;
    float _dirPadding3;
    float3 _dirLightColor;
    float _dirPadding4;
}

Texture2D NormalRT : register(t5);
Texture2D DepthRT : register(t6);
Texture2D RealRT : register(t7);

SamplerState NormalRTSampler : register(s5);
SamplerState DepthRTSampler : register(s6);
SamplerState RealRTSampler : register(s7);

static const float2 arrBasePos[4] =
{
    float2(-1.0f, 1.0f),
    float2(1.0f, 1.0f),
    float2(-1.0f, -1.0f),
    float2(1.0f, -1.0f),
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 view : VIEW0;
};

VS_OUTPUT VS(uint VertexID : SV_VERTEXID)
{
    VS_OUTPUT output;

    output.position = float4(arrBasePos[VertexID].xy, 0.0f, 1.0f);
    output.uv = saturate(output.position.xy);
    output.uv.y = 1.0f - output.uv.y;
    output.view = GetViewPosition().xyz;
    
    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 depth = DepthRT.Sample(DepthRTSampler, input.uv);
    float3 oPos = depth.gba;
    float3 normal = NormalRT.Sample(NormalRTSampler, input.uv);
    float4 diffuse = RealRT.Sample(RealRTSampler, input.uv);

    float up = normal.y * 0.5f + 0.5f;
    float3 ambient = _dirAmbientDown + up * _dirAmbientRange;
    float3 finalColor = ambient * diffuse.rgb;

    float NDotL = dot(_dirToLight, normal);
    float3 fc = _dirLightColor.rgb * saturate(NDotL);

    float3 toe = input.view - oPos;
    toe = normalize(toe);
    float3 halfWay = normalize(toe + _dirToLight);
    float ndoth = saturate(dot(halfWay, normal));
    
    fc += _dirLightColor.rgb * ndoth;
    fc *= diffuse.rgb;

    finalColor += fc;

    return float4(finalColor, 1.0f);
}