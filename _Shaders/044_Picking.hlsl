#include "000_Header.hlsl"

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
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.8f);

    //if (frac(abs(input.uv.x - 1.0f)) > 0.01f && frac(abs(input.uv.y - 1.0f)) > 0.01f)
    //    clip(color.a - 0.9f);
    
    //float2 grid = float2(0, 0);
    //grid.x = frac(input.uv.x / 1.0f);
    //grid.y = frac(input.uv.y / 1.0f);
    //
    //if (grid.x > 0.05f && grid.y > 0.05f)
    //    clip(color.a - 0.9f);
    
    float2 grid = input.uv / 1.0f;

    float2 range = abs(frac(grid - 0.5f) - 0.5f);
    float2 speed = fwidth(grid); //ddx, ddy 는 주변 픽셀에 대한 차이

    float2 pixelRange = range / speed;
    float lineWeight = saturate(min(pixelRange.x, pixelRange.y) - 0.1f);
    
    if (lineWeight > 0.99f)
        clip(color.a - 0.9f);

    return color;
}   
