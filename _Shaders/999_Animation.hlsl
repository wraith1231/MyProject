#include "000_Header.hlsl"

cbuffer VS_Bones : register(b2)
{
    matrix Bones[128];
}

///////////////////////////////////////////////////////////////////////////////////////////////////

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
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

    //output.position = mul(output.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);
    
    //output.normal = GetWorldNormal(output.normal, _world);
    output.normal = normalize(output.normal);

    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = _diffuseMap.Sample(_diffuseSampler, input.uv);
    return GetDiffuseColor(color, _direction, input.normal);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

struct PixelNormalInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL0;
};

PixelNormalInput VS_Normal(VertexTextureNormalBlend input)
{
    PixelNormalInput output;

    float4x4 transform = 0;
    transform += mul(input.blendWeights.x, Bones[(uint) input.blendIndices.x]);
    transform += mul(input.blendWeights.y, Bones[(uint) input.blendIndices.y]);
    transform += mul(input.blendWeights.z, Bones[(uint) input.blendIndices.z]);
    transform += mul(input.blendWeights.w, Bones[(uint) input.blendIndices.w]);
    
    output.position = mul(input.position, transform);
    output.normal = mul(input.normal, (float3x3) transform);

    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);
    
    output.normal = normalize(output.normal);

    return output;
}

float4 PS_Normal(PixelNormalInput input) : SV_TARGET
{
    float3 normal = input.normal;

    normal = abs(normal);

    return float4(normal, 1);

    //float3 normal = input.normal;
    //normal = abs(normal);
    //
    //return float4(normal, 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

struct PixelDepthInput
{
    float4 position : SV_POSITION;
    float2 depth : DEPTH0;
};

PixelDepthInput VS_Depth(VertexTextureNormalBlend input)
{
    PixelDepthInput output;

    float4x4 transform = 0;
    transform += mul(input.blendWeights.x, Bones[(uint) input.blendIndices.x]);
    transform += mul(input.blendWeights.y, Bones[(uint) input.blendIndices.y]);
    transform += mul(input.blendWeights.z, Bones[(uint) input.blendIndices.z]);
    transform += mul(input.blendWeights.w, Bones[(uint) input.blendIndices.w]);
    
    output.position = mul(input.position, transform);

    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);
    
    output.depth = output.position.zw;

    return output;
}

float4 PS_Depth(PixelDepthInput input) : SV_TARGET
{
    return float4(input.depth, 1, 1);
}