#include "000_Header.hlsl"

cbuffer PS_Value : register(b2)
{
    uint _valueSelect;
    float3 _valuePadding;
}

Texture2D RenderTarget : register(t5);
SamplerState RenderTargetSampler : register(s5);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

PixelInput VS(VertexColorTextureNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);
    
    output.uv = float2(1 - input.uv.x, input.uv.y);
    
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float3 color = RenderTarget.Sample(RenderTargetSampler, input.uv);

    if (_valueSelect == 0)
    {
        float avg = (color.r + color.g + color.b) / 3;

        return float4(avg, avg, avg, 1);
    }

    float3 factor = float3(0.298f, 0.587f, 0.114f);
    if (_valueSelect == 1)
    {
        //float c = color.r * factor.r + color.g * factor.g + color.b * factor.b;
        float c = dot(color, factor);
        return float4(c, c, c, 1);
    }

    return float4(color, 1);
}