#include "000_Header.hlsl"

cbuffer VS_Value : register(b2)
{
    float _vsWidth;
    float _vsHeight;
    float _vsNear;
    float _vsFar;
}
cbuffer PS_Value : register(b2)
{
    float _valueWidth;
    float _valueHeight;
    float _valueNear;
    float _valueFar;
}
#define FXAA_REDUCE_MIN (1.0f / 128.0f)
Texture2D RenderTarget : register(t5);
SamplerState RenderTargetSampler : register(s5);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float2 zw : TEXCOORD1;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);
    
    float2 rcpFrame = float2(1.0f / _vsWidth, 1.0f / _vsHeight);
    output.uv = input.uv;
    output.zw = input.uv - (rcpFrame * (0.5 + 1.0 / 4.0));
    
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 c = float4(0, 0, 0, 1);
    float2 rcpFrame = float2(1.0f / _valueWidth, 1.0f / _valueHeight);
    //float4 color = RenderTarget.Sample(RenderTargetSampler, input.uv);



    return color;

}