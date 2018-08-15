#include "000_Header.hlsl"

cbuffer PS_LightNumber : register(b2)
{
    uint _lightNumber;
    float3 _lightPadding;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 oPosition : POSITION0;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;
    
    output.position = mul(input.position, _world);
    output.oPosition = output.position;
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);
    
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    //return float4(1, 1, 1, 1);
    //float4 color = (float4) 1;
    //
    //float dist = length(_pointLight[_lightNumber].Position - input.oPosition);
    //float intensity = saturate((_pointLight[_lightNumber].Range - dist) / _pointLight[_lightNumber].Range);
    //intensity = pow(intensity, _pointLight[_lightNumber].Intensity);
    //
    ////IntensityCut(intensity);
    //
    //color = float4(_pointLight[_lightNumber].Color, intensity);
    
    return float4(0, 0, 0, 1);
}