cbuffer VS_ViewProjection : register(b0)
{
    matrix _view;
    matrix _projection;
}

cbuffer VS_World : register(b1)
{
    matrix _world;
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
    float4 color : OUPUT0;
};

SamplerState _sampler : register(s0);
Texture2D _map : register(t0);

PixelInput VS(VertexInput input)
{
    PixelInput output;
    
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.uv = input.uv;

    float height = input.position.y;
    float4 color = float4(1, 1, 1, 1);

    if (height > 10)
        color = float4(1, 0, 1, 1);

    if (height > 30)
        color = float4(0, 0, 1, 1);

    if (height > 50)
        color = float4(0, 1, 0, 1);

    if (height > 100)
        color = float4(1, 0, 0, 1);

    output.color = color;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    return input.color;
    
    //return _map.Sample(_sampler, input.uv);   
}