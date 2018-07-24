#include "000_Header.hlsl"

cbuffer VS_Matrix : register(b2)
{
    matrix _scale;
    matrix _rotate;
    matrix _trans;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};


//float difCal(float diffuse)
//{
//    float result = diffuse;
//    if (diffuse < 0.30f)
//        result = 0.15f;
//    if (diffuse > 0.30f && diffuse < 0.60f)
//        result = 0.45f;
//    if (diffuse > 0.60f && diffuse < 0.80f)
//        result = 0.70f;
//    if (diffuse > 0.80f && diffuse < 1.00f)
//        result = 1.00f;
//
//    return result;
//}

PixelInput VS(VertexColorTextureNormal input)
{
    PixelInput output;

    matrix invView = _viewInverse;
    invView._41 = invView._42 = invView._43 = 0.0f;
    invView._12 = invView._32 = 0.0f;
    invView._21 = invView._23 = 0.0f;

    matrix mat, matR;
    matR = mul(_rotate, invView);
    mat = mul(_scale, matR);
    mat = mul(mat, _trans);

    output.position = mul(input.position, _world);
    output.position = mul(output.position, mat);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);
    
    output.uv = input.uv;


    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{

    float4 diffuseMap = _diffuseMap.Sample(_diffuseSampler, input.uv);

    clip(diffuseMap.a - 0.8f);
    float3 light = _direction * -1;
    light = saturate(light + 0.5);
    float3 nor = float3(0.25f, 0.5f, 0.25f);
    float intensity = dot(nor, light);

    intensity = difCal(intensity);

    return diffuseMap * intensity;
}