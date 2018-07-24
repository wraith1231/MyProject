#include "000_Header.hlsl"

struct VertexInput
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

SamplerState _sampler : register(s0);

//Texture2D _map : register(t0);
Texture2D _map[2] : register(t0);

PixelInput VS(VertexInput input)
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
    //전역 라이팅 테스트
    //return float4(_direction * 0.5f + 0.5f, 1);

    //return _map.Sample(_sampler, input.uv);

    float4 color = _map[0].Sample(_sampler, input.uv);
    float4 color2 = _map[1].Sample(_sampler, input.uv);

    return color * color2 * 2;
}