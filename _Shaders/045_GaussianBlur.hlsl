#include "000_Header.hlsl"

cbuffer PS_Value : register(b2)
{
    float _blurWidth;
    float _blurHeight;
    int _blurCount;

    float _blurPadding;
}

Texture2D RenderTarget : register(t5);
SamplerState RenderTargetSampler : register(s5);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

static const float Total = 6.2108f; //수식에 의해 가우시안을 내놓으면 그 총합값이 이것

PixelInput VS(VertexColorTextureNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);
    
    output.uv = float2(1 - input.uv.x, input.uv.y);
    
    return output;
}

static const int BlurSize = 20;

static const float weights[13] =
{
    0.0561f, 0.1353f, 0.2780f, 0.4868f, 0.7261f, 0.9231f,
        1.0f,
        0.9231f, 0.7261f, 0.4868f, 0.2780f, 0.1353f, 0.0561f
};

float4 PS_X(PixelInput input) : SV_TARGET
{
    float2 uv = input.uv;
    float u = 1.0f / _blurWidth;

    float4 diffuse = float4(0, 0, 0, 0);
    float total = 0.0f;
    for (int i = -_blurCount; i <= _blurCount; i++)
    {
        float2 temp = uv + float2(u * (float) i, 0);
        diffuse += weights[6 + i] * RenderTarget.Sample(RenderTargetSampler, temp);
        total += weights[6 + i];
    }
    //diffuse /= Total;
    diffuse /= total;

    return float4(diffuse.rgb, 1);
}

float4 PS_Y(PixelInput input) : SV_TARGET
{
    float2 uv = input.uv;
    float v = 1.0f / _blurHeight;

    float4 diffuse = float4(0, 0, 0, 0);
    float total = 0.0f;
    for (int i = -_blurCount; i <= _blurCount; i++)
    {
        float2 temp = uv + float2(0, v * (float)i);
        diffuse += weights[6 + i] * RenderTarget.Sample(RenderTargetSampler, temp);
        total += weights[6 + i];
    }
    //diffuse /= Total;
    diffuse /= total;

    return float4(diffuse.rgb, 1);
}

float4 PS_XY(PixelInput input) : SV_TARGET
{
    return RenderTarget.Sample(RenderTargetSampler, input.uv);

}