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
};

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
    //if (diffuse > 1.00f && diffuse < 1.20f)
    //    result = 1.20f;
    //if (diffuse > 1.20f && diffuse < 1.40f)
    //    result = 1.40f;
    //if (diffuse < 0.20f && diffuse > 0.00f)
    //    result = 0.20f;

    return result;
}

float4 PS(PixelInput input) : SV_TARGET
{
    if (wire == 0)
    {
        float4 diffuseMap = _diffuseMap.Sample(_diffuseSampler, input.uv);
        float4 diffuse = GetDiffuseColor(_diffuse, _direction, input.normal);

        //float tex = _diffuseMap.Sample(_diffuseSampler, input.uv);

        diffuse.r = difCal(diffuse.r);
        diffuse.g = difCal(diffuse.g);
        diffuse.b = difCal(diffuse.b);
        diffuse.a = difCal(diffuse.a);

        //if (diffuse.r > 1.50f)
        //    return diffuse;
    
        return color * diffuse;
    }
    
    return color;

}