#include "000_Header.hlsl"

cbuffer PS_Value : register(b2)
{
    float _valueWidth;
    float _valueHeight;
    float _valueNear;
    float _valueFar;
}

Texture2D NormalRT : register(t5);
Texture2D DepthRT : register(t6);
Texture2D RealRT : register(t7);
SamplerState NormalRTSampler : register(s5);
SamplerState DepthRTSampler : register(s6);
SamplerState RealRTSampler : register(s7);

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
    float4 normalColor = NormalRT.Sample(NormalRTSampler, input.uv);
    float4 depthColor = DepthRT.Sample(DepthRTSampler, input.uv);
    float2 uv = input.uv;
    uv.y -= 1.0f / _valueHeight;
    float4 normalColor1 = NormalRT.Sample(NormalRTSampler, uv);
    float4 depthColor1 = DepthRT.Sample(DepthRTSampler, uv);

    uv.y += 1.0f / _valueHeight;
    uv.x -= 1.0f / _valueWidth;
    float4 normalColor2 = NormalRT.Sample(NormalRTSampler, uv);
    float4 depthColor2 = DepthRT.Sample(DepthRTSampler, uv);

    uv.x += 1.0f / _valueWidth;
    uv.x += 1.0f / _valueWidth;
    float4 normalColor3 = NormalRT.Sample(NormalRTSampler, uv);
    float4 depthColor3 = DepthRT.Sample(DepthRTSampler, uv);

    uv.x -= 1.0f / _valueWidth;
    uv.y += 1.0f / _valueHeight;
    float4 normalColor4 = NormalRT.Sample(NormalRTSampler, uv);
    float4 depthColor4 = DepthRT.Sample(DepthRTSampler, uv);
    
    float len, len1;

    //Normal Silhouette
    float poi = 0.1f;

    len = dot(normalColor, normalColor);
    len1 = dot(normalColor, normalColor1);
    if (abs(len - len1) > poi)
        return float4(0, 0, 0, 1);
    len1 = dot(normalColor, normalColor2);
    if (abs(len - len1) > poi)
        return float4(0, 0, 0, 1);
    len1 = dot(normalColor, normalColor3);
    if (abs(len - len1) > poi)
        return float4(0, 0, 0, 1);
    len1 = dot(normalColor, normalColor4);
    if (abs(len - len1) > poi)
        return float4(0, 0, 0, 1);
    
    //return normalColor;
   
    //Depth Silhouette
    poi = 0.00000006f;
    float pa = (_valueFar) / (_valueFar - _valueNear);
    float pb = (-_valueNear) / (_valueFar - _valueNear);

    float depth = pb / (depthColor.r - pa);
    float depth1 = pb / (depthColor1.r - pa);
    if (abs(depth - depth1) > poi)
        return float4(0, 0, 0, 1);
    depth1 = pb / (depthColor2.r - pa);
    if (abs(depth - depth1) > poi)
        return float4(0, 0, 0, 1);
    depth1 = pb / (depthColor3.r - pa);
    if (abs(depth - depth1) > poi)
        return float4(0, 0, 0, 1);
    depth1 = pb / (depthColor4.r - pa);
    if (abs(depth - depth1) > poi)
        return float4(0, 0, 0, 1);
    
    //return float4(depth, depth, depth, 1);
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