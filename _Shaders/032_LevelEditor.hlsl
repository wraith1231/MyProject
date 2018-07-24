#include "000_Header.hlsl"

cbuffer PS_Grid : register(b2)
{
    float4 _gridColor;

    uint _gridSpacing;
    float _gridThickness;

    uint _gridView;

    float _gridPadding;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 oPosition : POSITION0;
};


SamplerState _sampler : register(s0);

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.normal = GetWorldNormal(input.normal, _world);
    
    output.oPosition = input.position;
    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 diffuseMap = _diffuseMap.Sample(_sampler, input.uv);
    float4 diffuse = GetDiffuseColor(_diffuse, _direction, input.normal);

    if (_gridView == 1)
    {
    
        float2 grid = float2(0, 0);
        grid.x = frac(input.oPosition.x / (float) _gridSpacing);
        grid.y = frac(input.oPosition.z / (float) _gridSpacing);

        if (grid.x < _gridThickness || grid.y < _gridThickness)
            return _gridColor;
    //if (frac(input.oPosition.x / (float) _gridSpacing) < _gridThickness)
    //    return _gridColor;
    }

    return diffuseMap * diffuse;
}