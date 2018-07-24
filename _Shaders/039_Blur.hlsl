#include "000_Header.hlsl"

cbuffer PS_Value : register(b2)
{
    uint _valueCount;
    float _valueWidth;
    float _valueHeight;
    float _valuePadding;
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

static const int BlurSize = 20;
float4 PS(PixelInput input) : SV_TARGET
{
    float2 uv = input.uv;
    float4 color = RenderTarget.Sample(RenderTargetSampler, uv);
   
    float2 arr[BlurSize];

    int i = 0;
    for (i = 0; i < _valueCount; i++)
    {
        arr[i] = float2(0, 0);
        arr[i].x = (float) (i + 1) / _valueWidth;
        arr[i].y = (float) (i + 1) / _valueHeight;
    }

    float2 temp = float2(0, 0);
    for (i = 0; i < _valueCount; i++)
    {
        temp = float2(uv.x - arr[i].x, uv.y);
        color += RenderTarget.Sample(RenderTargetSampler, temp);

        temp = float2(uv.x + arr[i].x, uv.y);
        color += RenderTarget.Sample(RenderTargetSampler, temp);
        
        temp = float2(uv.x , uv.y - arr[i].y);
        color += RenderTarget.Sample(RenderTargetSampler, temp);
        
        temp = float2(uv.x , uv.y + arr[i].y);
        color += RenderTarget.Sample(RenderTargetSampler, temp);
    }

    color /= (float)_valueCount * 4.0f + 1.0f;

    return color;
}

float4 PS2(PixelInput input) : SV_TARGET
{
    float2 arr[9] =
    {
        float2(-1, -1), float2( 0, -1), float2( 1, -1),
        float2(-1,  0), float2( 0,  0), float2( 1,  0),
        float2(-1,  1), float2( 0,  1), float2( 1,  1)
    };
    
    float3 color = float3(0, 0, 0);
    int i = 0;
    for (i = 0; i < 9; i++)
    {
        float x = arr[i].x / _valueWidth;
        float y = arr[i].y / _valueHeight;
        //float2 uv = input.uv + float2(arr[i].x / _valueWidth, arr[i].y / _valueHeight);

        float2 uv = input.uv + float2(x, y);

        color += RenderTarget.Sample(RenderTargetSampler, uv);
    }

    color /= 9.0f;
    
    return float4(color, 1);
}