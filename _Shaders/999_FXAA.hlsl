#include "000_Header.hlsl"
#include "FXAA.hlsl"

Texture2D RenderTarget : register(t0);
SamplerState RenderTargetSampler : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float2 zw : TEXCOORD1;
};

static const float2 arrBasePos[4] =
{
    float2(-1.0f, 1.0f),
    float2(1.0f, 1.0f),
    float2(-1.0f, -1.0f),
    float2(1.0f, -1.0f),
};

PixelInput VS(uint vertexID : SV_VERTEXID)
{
    PixelInput output;
    
    output.position = float4(arrBasePos[vertexID].xy, 0.0f, 1.0f);
    output.uv = saturate(output.position.xy);
    output.uv.y = 1.0f - output.uv.y;
    
    float2 rcpFrame = float2(1.0f / _valueWidth, 1.0f / _valueHeight);
    output.zw = output.uv - (rcpFrame * (0.5 + 1.0 / 4.0));
    
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float2 rcpFrame = float2(1.0f / _valueWidth, 1.0f / _valueHeight);
    
    FxaaTex te;
    te.smpl = RenderTargetSampler;
    te.tex = RenderTarget;

    return FxaaPixelShader(input.zw, float4(0, 0, 0, 0),
    te, te, te, rcpFrame,
    float4(0, 0, 0, 0), float4(0, 0, 0, 0), float4(0, 0, 0, 0),
    0.75f, 0.166f, 0.0833f, 0.0f, 0.0f, 0.0f, float4(0, 0, 0, 0));
}