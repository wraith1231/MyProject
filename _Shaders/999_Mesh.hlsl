#include "000_Header.hlsl"

///////////////////////////////////////////////////////////////////////////////////////////////////

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

    output.normal = mul(input.normal, (float3x3) _world);
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

PixelNormalInput VS_Normal(VertexTextureNormal input)
{
    PixelNormalInput output;
    
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.normal = mul(input.normal, (float3x3) _world);
    output.normal = normalize(output.normal);
    
    return output;
}

float4 PS_Normal(PixelNormalInput input) : SV_TARGET
{
    float4 normal = float4(input.normal, 1);

    return abs(normal);

}

///////////////////////////////////////////////////////////////////////////////////////////////////

struct PixelDepthInput
{
    float4 position : SV_POSITION;
    float2 zw : Depth0;
};

PixelDepthInput VS_Depth(VertexTextureNormal input)
{
    PixelDepthInput output;
    
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.zw = float2(output.position.z, output.position.w);

    return output;
}

float4 PS_Depth(PixelDepthInput input) : SV_TARGET
{
    return float4(input.position.zw, 1, 1);
    return float4(input.zw.x, input.zw.y, 0, 1);

}