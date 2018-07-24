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
    
    output.uv = float2(1 - input.uv.x, input.uv.y);
    //output.uv.x = 1 - input.uv.x;
    
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    //float2 uv = float2((1 - input.uv.x), input.uv.y);

   return 1 - RenderTarget.Sample(RenderTargetSampler, input.uv);
}