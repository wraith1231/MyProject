#include "000_Header.hlsl"

cbuffer PS_Color : register(b2)
{
    float4 color;
    uint wire;
    float3 paddding;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};

//SamplerState _sampler : register(s0);
Texture2D _texture : register(t5);
SamplerState _sampler : register(s0);

PixelInput VS(VertexColorTextureNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.normal = GetWorldNormal(input.normal, _world);
    output.normal = normalize(output.normal);
    
    output.uv = input.uv;
    //output.color = input.color;

    return output;
}

float difCal(float diffuse)
{
    float result = diffuse;
    if (diffuse < 0.30f)
        result = 0.15f;
    if (diffuse > 0.30f && diffuse < 0.60f)
        result = 0.45f;
    if (diffuse > 0.60f && diffuse < 0.80f)
        result = 0.70f;
    if (diffuse > 0.80f && diffuse < 1.00f)
        result = 1.00f;

    return result;
}

float4 PS(PixelInput input) : SV_TARGET
{
    if (wire == 0)
    {
        float4 textureMap = _diffuseMap.Sample(_sampler, input.uv);
        //float4 textureMap = _diffuseMap.Sample(_diffuseSampler, input.uv);
        //float4 textureMap = _texture.Sample(_sampler, input.uv);

        //float3 light = 0;
        float3 light = _direction;// * -1;
        //if (_direction.y > 0)
        //{
        //    light = _direction * -1;
        //    light.y = saturate(light.y * 2);
        //}
        //else
        //{
        //    light = _direction * 1;
        //    light.y = saturate(light.y * 2);
        //
        //}

        float intensity = dot(input.normal, light);

        //intensity = intensity * 0.5f + 0.5f;
        intensity = difCal(intensity);
        //return textureMap;

        //return color * intensity;
        return (color + textureMap) * intensity;
    }
    
    return color;

}