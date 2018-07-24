cbuffer VS_ViewProjection : register(b0)
{
    matrix _view;
    matrix _projection;
}

cbuffer VS_World : register(b1)
{
    matrix _world;
}

cbuffer VS_Bursh : register(b2)
{
    uint _brushType;
    float3 _brushLocation;
    int _brushRange;
    float3 _brushColor;
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
    float4 color : COLOR0;
    float3 normal : OUTPUT0;
    float3 brushColor : COLOR1;
};

SamplerState _sampler : register(s0);
Texture2D _map : register(t0);

float3 GetBrushColor(float3 location)
{
    if(_brushType == 0)
        return float3(0, 0, 0);

    if(_brushType == 1) //사각형
    {
        if((location.x >= (_brushLocation.x - _brushRange)) &&
            (location.x <= (_brushLocation.x + _brushRange)) &&
            (location.z >= (_brushLocation.z - _brushRange)) &&
            (location.z <= (_brushLocation.z + _brushRange)))
        {
            return _brushColor;
        }
    }

    if(_brushType == 2) //원
    {
        float dx = location.x - _brushLocation.x;
        float dy = location.z - _brushLocation.z;

        float dist = sqrt(dx * dx + dy * dy);
        
        if(dist <= _brushRange)
            return _brushColor;

        //float dist = distance(location, _brushLocation);
    }

    return float3(0, 0, 0);
}

PixelInput VS(VertexInput input)
{
    PixelInput output;

    output.brushColor = GetBrushColor(input.position.xyz);
    
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.normal = mul(input.normal, (float3x3) _world);
    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float3 light = _direction * -1;
    float intensity = saturate(dot(input.normal, light));

    float4 brushColor = float4(input.brushColor, 0);
    float4 diffuseMap = _map.Sample(_sampler, input.uv);

    return diffuseMap * intensity + brushColor;
}