#include "000_Header.hlsl"

cbuffer PS_Sky : register(b2)
{
    float4 Center;
    float4 Apex;
    float Height;
    float3 Padding;
}

struct VertexTextureNormalColor
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 wPosition : OUTPUT0;
    //float2 uv : TEXCOORD0;
    //float3 normal : NORMAL0;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.wPosition = input.position;
    //output.normal = GetWorldNormal(input.normal, _world);
    
    //output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    //float4 diffuseMap = _diffuseMap.Sample(_diffuseSampler, input.uv);
    //float4 diffuse = GetDiffuseColor(_diffuse, _direction, input.normal);

    float height = saturate(input.wPosition.y);

    return lerp(Center, Apex, height * Height);

    //return diffuse;
}