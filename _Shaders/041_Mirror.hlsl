#include "000_Header.hlsl"

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
    
    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    return RenderTarget.Sample(RenderTargetSampler, input.uv);
}
float4 PS2(PixelInput input) : SV_TARGET
{
    float4 color = RenderTarget.Sample(RenderTargetSampler, input.uv);

    float2 grid = input.uv / 1.0f;

    float2 range = abs(frac(grid - 0.5f) - 0.5f);
    float2 speed = fwidth(grid); //ddx, ddy 는 주변 픽셀에 대한 차이

    float2 pixelRange = range / speed;
    float lineWeight = saturate(min(pixelRange.x, pixelRange.y) - 0.1f);
    
    return lerp(float4(0, 0, 0, 1), color, lineWeight);
}