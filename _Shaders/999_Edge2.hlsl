#include "000_Header.hlsl"

cbuffer PS_Value : register(b2)
{
    uint _valueCount;
    float _valueWidth;
    float _valueHeight;
    float _valuePadding;
}

Texture2D NDRT : register(t5);
Texture2D RealRT : register(t6);
SamplerState NDRTSampler : register(s5);
SamplerState RealRTSampler : register(s6);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

PixelInput VS(VertexTextureNormal input)
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
    float4 NDColor = NDRT.Sample(NDRTSampler, input.uv);
    
    //return NDColor;
    //return float4(NDColor.a, NDColor.a, NDColor.a, 1);
    float2 uv = input.uv;
    uv.y -= 1.0f / _valueHeight;
    float4 NDColor1 = NDRT.Sample(NDRTSampler, uv);

    uv.y += 1.0f / _valueHeight;
    uv.x -= 1.0f / _valueWidth;
    float4 NDColor2 = NDRT.Sample(NDRTSampler, uv);

    uv.x += 1.0f / _valueWidth;
    uv.x += 1.0f / _valueWidth;
    float4 NDColor3 = NDRT.Sample(NDRTSampler, uv);

    uv.x -= 1.0f / _valueWidth;
    uv.y += 1.0f / _valueHeight;
    float4 NDColor4 = NDRT.Sample(NDRTSampler, uv);
    
    float len;

    //Normal Silhouette
    float poi = 0.3f; //(3.14 * 57.324847) / 180;
    //len = dot(NDColor.rgb, NDColor1.rgb);
    //if (len > poi)
    //    return float4(0, 0, 0, 1);
    //len = dot(NDColor.rgb, NDColor2.rgb);
    //if (len > poi)
    //    return float4(0, 0, 0, 1);
    //len = dot(NDColor.rgb, NDColor3.rgb);
    //if (len > poi)
    //    return float4(0, 0, 0, 1);
    //len = dot(NDColor.rgb, NDColor4.rgb);
    //if (len > poi)
    //    return float4(0, 0, 0, 1);
    len = length(NDColor.rgb - NDColor1.rgb);
    if (len >= poi)
        return float4(0, 0, 0, 1);
    len = length(NDColor.rgb - NDColor2.rgb);
    if (len >= poi)
        return float4(0, 0, 0, 1);
    len = length(NDColor.rgb - NDColor3.rgb);
    if (len >= poi)
        return float4(0, 0, 0, 1);
    len = length(NDColor.rgb - NDColor4.rgb);
    if (len >= poi)
        return float4(0, 0, 0, 1);

    //Depth Silhouette
    poi = 0.001f;
    if (NDColor.a - NDColor1.a >= poi)
        return float4(0, 0, 0, 1);
    if (NDColor.a - NDColor2.a >= poi)
        return float4(0, 0, 0, 1);
    if (NDColor.a - NDColor3.a >= poi)
        return float4(0, 0, 0, 1);
    if (NDColor.a - NDColor4.a >= poi)
        return float4(0, 0, 0, 1);
    
    //return float4(1, 1, 1, 1);

    float4 realColor = RealRT.Sample(RealRTSampler, input.uv);

    return realColor;
}


    //if (abs(color1.x - color11.x) > 0.1f)
    //    return float4(0, 0, 0, 1);
    //if (abs(color1.y - color11.y) > 0.1f)
    //    return float4(0, 0, 0, 1);
    //if (abs(color1.z - color11.z) > 0.1f)
    //    return float4(0, 0, 0, 1);
    //
    //if (abs(color1.x - color12.x) > 0.1f)
    //    return float4(0, 0, 0, 1);
    //if (abs(color1.y - color12.y) > 0.1f)
    //    return float4(0, 0, 0, 1);
    //if (abs(color1.z - color12.z) > 0.1f)
    //    return float4(0, 0, 0, 1);
    //
    //if (abs(color1.x - color13.x) > 0.1f)
    //    return float4(0, 0, 0, 1);
    //if (abs(color1.y - color13.y) > 0.1f)
    //    return float4(0, 0, 0, 1);
    //if (abs(color1.z - color13.z) > 0.1f)
    //    return float4(0, 0, 0, 1);
    //
    //if (abs(color1.x - color14.x) > 0.1f)
    //    return float4(0, 0, 0, 1);
    //if (abs(color1.y - color14.y) > 0.1f)
    //    return float4(0, 0, 0, 1);
    //if (abs(color1.z - color14.z) > 0.1f)
    //    return float4(0, 0, 0, 1);
    //float len1 = color1.x + color1.y + color1.z;
    //float len11 = color11.x + color11.y + color11.z;
    //float len12 = color12.x + color12.y + color12.z;
    //float len13 = color13.x + color13.y + color13.z;
    //float len14 = color14.x + color14.y + color14.z;
    //
    //if (abs(len1 - len11) > 0.1f)
    //    return float4(0, 0, 0, 1);
    //if (abs(len1 - len12) > 0.1f)
    //    return float4(0, 0, 0, 1);
    //if (abs(len1 - len13) > 0.1f)
    //    return float4(0, 0, 0, 1);
    //if (abs(len1 - len14) > 0.1f)
    //    return float4(0, 0, 0, 1);