#include "000_Header.hlsl"

Texture2D _texture1 : register(t5);
Texture2D _texture2 : register(t6);
Texture2D _texture3 : register(t7);
Texture2D _texture4 : register(t8);

///////////////////////////////////////////////////////////////////////////////////////////////////

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

PixelInput VS(VertexColorTextureNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.normal = GetWorldNormal(input.normal, _world);
    output.normal = normalize(output.normal);
    
    output.color = input.color;
    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{

    float4 color1 = _diffuseMap.Sample(_diffuseSampler, input.uv);
    float4 color2 = _texture1.Sample(_diffuseSampler, input.uv);
    
    float4 diffuse = lerp(color1, color2, input.color.a);
    color2 = _texture2.Sample(_diffuseSampler, input.uv);
    diffuse = lerp(color1, color2, input.color.b);
    color2 = _texture3.Sample(_diffuseSampler, input.uv);
    diffuse = lerp(color1, color2, input.color.g);
    color2 = _texture4.Sample(_diffuseSampler, input.uv);
    diffuse = lerp(color1, color2, input.color.r);

    return GetDiffuseColor(diffuse, _direction, input.normal);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

struct PixelNormalInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL0;
};

PixelNormalInput VS_Normal(VertexColorTextureNormal input)
{
    PixelNormalInput output;
    
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.normal = GetWorldNormal(input.normal, _world);
    output.normal = normalize(output.normal);
    
    return output;
}

float4 PS_Normal(PixelNormalInput input) : SV_TARGET
{
    float4 normal = float4(input.normal, 1);
    normal = abs(normal);

    return normal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

struct PixelDepthInput
{
    float4 position : SV_POSITION;
    float2 zw : DEPTH0;
};

PixelDepthInput VS_Depth(VertexColorTextureNormal input)
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
    return float4(input.zw.x, input.zw.y, 0, 1);
}