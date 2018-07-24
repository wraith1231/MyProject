#include "000_Header.hlsl"

SamplerState _diffuseSampler : register(s0);
SamplerState _detailSampler : register(s1);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.normal = GetWorldNormal(input.normal, _world);
    
    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 diffuseMap = _diffuseMap.Sample(_diffuseSampler, input.uv);
    float4 diffuse = GetDiffuseColor(_diffuse, _direction, input.normal);

    float2 detailUv = input.uv * _detailFactor;
    float4 detail = _detailMap.Sample(_detailSampler, detailUv) * 2.0f;

    return diffuseMap * diffuse * detail;
}