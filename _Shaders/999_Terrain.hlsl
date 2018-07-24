#include "000_Header.hlsl"

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

Texture2D _texture1 : register(t5);
Texture2D _texture2 : register(t6);
Texture2D _texture3 : register(t7);
Texture2D _texture4 : register(t8);

PixelInput VS(VertexColorTextureNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.normal = GetWorldNormal(input.normal, _world);
    //output.normal = input.normal;
    
    output.color = input.color;
    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{

    float4 color1 = _diffuseMap.Sample(_diffuseSampler, input.uv);
    float4 color2 = _texture1.Sample(_diffuseSampler, input.uv);

    //float3 light = _direction * -1;
    //float intensity = dot(input.normal, light);
    
    float4 diffuse = (1 - input.color.a) * color1 + input.color.a * color2;
    
    color2 = _texture2.Sample(_diffuseSampler, input.uv);
    diffuse = (1 - input.color.b) * diffuse + input.color.b * color2;
    
    color2 = _texture3.Sample(_diffuseSampler, input.uv);
    diffuse = (1 - input.color.g) * diffuse + input.color.g * color2;
    
    color2 = _texture4.Sample(_diffuseSampler, input.uv);
    diffuse = (1 - input.color.r) * diffuse + input.color.r * color2;
    
    //float3 dir = float3(_direction.x, -_direction.y, _direction.z);
    return GetDiffuseColor(diffuse, -_direction, input.normal);
    //return diffuse;
    //return diffuse * intensity;
}