#include "000_Header.hlsl"

cbuffer VS_Bones : register(b2)
{
    matrix Bones[256];
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 wPosition : POSITION0;
    float3 vPosition : POSITION1;
};

PixelInput VS(VertexTextureNormalBlend input)
{
    PixelInput output;

    float4x4 transform = 0;
    transform += mul(input.blendWeights.x, Bones[(uint) input.blendIndices.x]);
    transform += mul(input.blendWeights.y, Bones[(uint) input.blendIndices.y]);
    transform += mul(input.blendWeights.z, Bones[(uint) input.blendIndices.z]);
    transform += mul(input.blendWeights.w, Bones[(uint) input.blendIndices.w]);
    
    output.position = mul(input.position, transform);
    output.normal = mul(input.normal, (float3x3) transform);

    output.wPosition = output.position;
    output.position = mul(output.position, _view);
    output.vPosition = output.position;
    output.position = mul(output.position, _projection);
    
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
    output.normal.xyz = input.normal;

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

ShadowPixel VS_Shadow(VertexTextureNormalBlend input)
{
    ShadowPixel output;

    float4x4 transform = 0;
    transform += mul(input.blendWeights.x, Bones[(uint) input.blendIndices.x]);
    transform += mul(input.blendWeights.y, Bones[(uint) input.blendIndices.y]);
    transform += mul(input.blendWeights.z, Bones[(uint) input.blendIndices.z]);
    transform += mul(input.blendWeights.w, Bones[(uint) input.blendIndices.w]);
    
    output.position = mul(input.position, transform);
    
    output.position = mul(output.position, _lightView);
    output.vPosition = output.position;
    output.position = mul(output.position, _projection);
    
    return output;
}

float PS_Shadow(ShadowPixel input) : SV_TARGET
{
    float depth = input.vPosition.z / _valueFar;
    
    return depth;
}