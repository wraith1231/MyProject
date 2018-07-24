#include "000_Header.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};


SamplerState _sampler : register(s0);

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
    float4 diffuseMap = _diffuseMap.Sample(_sampler, input.uv);
    float4 diffuse = GetDiffuseColor(_diffuse, _direction, input.normal);

    if (diffuse.r > 0.8f)
        diffuse = float4(1.0f, 1.0f, 1.0f, 1.0f);
    else if (diffuse.r > 0.6f)
        diffuse = float4(0.6f, 0.6f, 0.6f, 0.6f);
    else if (diffuse.r > 0.4f)
        diffuse = float4(0.4f, 0.4f, 0.4f, 0.4f);
    else
        diffuse = float4(0.2f, 0.2f, 0.2f, 0.2f);
    
    return diffuseMap * diffuse;
}