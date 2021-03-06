#include "000_Header.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 wPosition : POSITION0;
    float3 vPosition : POSITION1;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;
    
    output.position = mul(input.position, _world);
    output.wPosition = output.position;
    output.position = mul(output.position, _view);
    output.vPosition = output.position;
    output.position = mul(output.position, _projection);

    output.normal = mul(input.normal, (float3x3) _world);
    output.normal = normalize(output.normal);
    output.uv = input.uv;

    return output;
}

PS_GBUFFEROUTPUT PS(PixelInput input)
{
    float4 color = _diffuseMap.Sample(_diffuseSampler, input.uv);
    
    PS_GBUFFEROUTPUT output = (PS_GBUFFEROUTPUT) 0;
    output.color = color;
    output.depth = float4(input.vPosition.z / _valueFar, input.wPosition.xyz);
    //output.normal.xy = NormalEncode(input.normal.xyz);
    output.normal.xyz = NormalEncode3to3(input.normal);

    return output;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Shadow Map
///////////////////////////////////////////////////////////////////////////////////////////////////

struct ShadowPixel
{
    float4 position : SV_POSITION;
    float3 vPosition : POSITION0;
};

ShadowPixel VS_Shadow(VertexColorTextureNormal input)
{
    ShadowPixel output;
    
    float4 world = mul(input.position, _world);
    output.position = mul(world, _lightView);
    output.vPosition = output.position;
    output.position = mul(output.position, _lightProjection);

    return output;
}

float PS_Shadow(ShadowPixel input) : SV_TARGET
{
    float depth = input.vPosition.z / _valueFar;

    return depth;
}