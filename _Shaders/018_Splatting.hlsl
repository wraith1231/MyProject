cbuffer VS_ViewProjection : register(b0)
{
    matrix _view;
    matrix _projection;
}

cbuffer VS_World : register(b1)
{
    matrix _world;
}

cbuffer PS_Select : register(b0)
{
    uint _selected;

    float SelectPadding[3];
}

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
Texture2D _map[3] : register(t0);

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
    float4 color1 = _map[0].Sample(_sampler, input.uv);
    float4 color2 = _map[1].Sample(_sampler, input.uv);
    float4 alphaMap = _map[2].Sample(_sampler, input.uv);
    
    //float4 result = color1 * color2 * 2;
    float4 result = (1 - alphaMap.a) * color1 + alphaMap.a * color2;

    return result;
}