#include "000_Header.hlsl"

cbuffer VS_Bone : register(b2)
{
    matrix _bones[128];
}

cbuffer VS_Render : register(b3)
{
    int _boneNumber;
    float3 _bonePadding;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct PixelNormalInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL0;
};

struct PixelDepthInput
{
    float4 position : SV_POSITION;
    float depth : DEPTH0;
};

struct PixelNDInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL0;
    float depth : DEPTH0;
};

PixelInput VS_Bone(VertexTextureNormal input)
{
    PixelInput output;
    
    output.position = mul(input.position, _bones[_boneNumber]);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.normal = mul(input.normal, (float3x3) _bones[_boneNumber]);
    output.normal = normalize(output.normal);
    output.uv = input.uv;

    return output;
}

PixelNormalInput VS_Normal(VertexTextureNormal input)
{
    PixelNormalInput output;
    
    output.position = mul(input.position, _bones[_boneNumber]);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    //output.normal = input.normal;
    output.normal = mul(input.normal, (float3x3) _bones[_boneNumber]);
    output.normal = normalize(output.normal);

    return output;
}

PixelDepthInput VS_Depth(VertexTextureNormal input)
{
    PixelDepthInput output;
    
    output.position = mul(input.position, _bones[_boneNumber]);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.depth = 1.0f - output.position.z / output.position.w;
    
    return output;
}

PixelNDInput VS_ND(VertexTextureNormal input)
{
    PixelNDInput output;
    
    output.position = mul(input.position, _bones[_boneNumber]);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.depth = 1.0f - output.position.z / output.position.w;

    output.normal = mul(input.normal, (float3x3) _bones[_boneNumber]);
    output.normal = normalize(output.normal);

    return output;

}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = _diffuseMap.Sample(_diffuseSampler, input.uv);
    
    color = GetDiffuseColor(color, _direction, input.normal);

    return color;
}

float4 PS_Normal(PixelNormalInput input) : SV_TARGET
{
    return float4(input.normal, 1);
}

float4 PS_Depth(PixelDepthInput input) : SV_TARGET
{
    return float4(input.depth, input.depth, input.depth, 1);
}

float4 PS_ND(PixelNDInput input) : SV_TARGET
{
    return float4(input.normal, input.depth);

}