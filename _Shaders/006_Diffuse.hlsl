cbuffer VS_ViewProjection : register(b0)
{
    matrix _view;
    matrix _projection;
}

cbuffer VS_World : register(b1)
{
    matrix _world;
}

cbuffer PS_Light : register(b0)
{
    float3 _direction;
    float LightPadding;
}

struct VertexInput
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
};


SamplerState _sampler : register(s0);
Texture2D _map : register(t0);

PixelInput VS(VertexInput input)
{
    PixelInput output;
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.normal = mul(input.normal, (float3x3) _world);
    output.uv = input.uv;

    float4 color = float4(1, 1, 1, 1);
    
    if (input.position.y > 5)
        color = float4(0, 1, 1, 1);

    if (input.position.y > 10)
        color = float4(0, 0, 1, 1);

    if (input.position.y > 15)
        color = float4(0, 1, 0, 1);

    if (input.position.y > 20)
        color = float4(1, 0, 0, 1);

    output.color = color;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float3 light = _direction * -1.0f;
    float intensity = saturate(dot(input.normal, light));

    return input.color * intensity;
}