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
Texture2D LightRT : register(t8);

SamplerState NormalRTSampler : register(s5);
SamplerState DepthRTSampler : register(s6);
SamplerState RealRTSampler : register(s7);
SamplerState LightRTSampler : register(s8);

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

half3 decodeNormal(half2 enc)
{
    half2 fenc = enc * 4 - 2;
    half f = dot(fenc, fenc);
    half g = sqrt(1 - f / 4);
    half3 n;
    n.xy = fenc * g;
    n.z = 1 - f / 2;
    return n;
}


float4 PS(PixelInput input) : SV_TARGET
{
    float4 realColor = RealRT.Sample(RealRTSampler, input.uv);
    float4 light = LightRT.Sample(LightRTSampler, input.uv);
    float3 NLATTColor = light.xyz;// * realColor.rgb;
    float3 Lighting = NLATTColor * light.www;

    return float4(Lighting, 1.0f);

    float nor = (3.141592f / 180.0f) * 5.0f;
    float dep = 0.002f;

    float nordot, ndot;
    bool bd = true;
    float hei = 1.0f / _valueHeight;
    float wid = 1.0f / _valueWidth;

    float fn = 1;// / (_valueFar - _valueNear);

    half4 normalColor = NormalRT.Sample(NormalRTSampler, input.uv);
    normalColor.rgb = decodeNormal(normalColor.rg);
    float depthColor = DepthRT.Sample(DepthRTSampler, input.uv).r;

    float depthColor1;
    half4 normalColor1;
    float2 uv = input.uv;
   
    nordot = dot(normalColor, normalColor);

    float factor = 0;
    [branch]
    if (_fogUse == 1)
    {
        factor = saturate((_fogEnd - depthColor) / (_fogEnd - _fogStart));
    
        if (factor > 0.6f)
            return lerp(realColor, _fogColor, factor) * (_sunColor * _sunIntensity);
    }
    
    depthColor *= fn;

    uv.y -= hei;
    normalColor1 = NormalRT.Sample(NormalRTSampler, uv);
    normalColor1.rgb = decodeNormal(normalColor1.rg);
    depthColor1 = DepthRT.Sample(DepthRTSampler, uv).r;

    ndot = dot(normalColor, normalColor1);
    depthColor1 *= fn;
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    if (abs(depthColor - depthColor1) > dep)
        return float4(0, 0, 0, 1);
    
    uv.y += hei;
    uv.x -= wid;
    normalColor1 = NormalRT.Sample(NormalRTSampler, uv);
    normalColor1.rgb = decodeNormal(normalColor1.rg);
    depthColor1 = DepthRT.Sample(DepthRTSampler, uv).r;
    
    ndot = dot(normalColor, normalColor1);
    depthColor1 *= fn;
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    if (abs(depthColor - depthColor1) > dep)
        return float4(0, 0, 0, 1);
    
    uv.x += wid;
    uv.x += wid;
    normalColor1 = NormalRT.Sample(NormalRTSampler, uv);
    normalColor1.rgb = decodeNormal(normalColor1.rg);
    depthColor1 = DepthRT.Sample(DepthRTSampler, uv).r;
    
    ndot = dot(normalColor, normalColor1);
    depthColor1 *= fn;
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    if (abs(depthColor - depthColor1) > dep)
        return float4(0, 0, 0, 1);
    
    uv.x -= wid;
    uv.y += hei;
    normalColor1 = NormalRT.Sample(NormalRTSampler, uv);
    normalColor1.rgb = decodeNormal(normalColor1.rg);
    depthColor1 = DepthRT.Sample(DepthRTSampler, uv).r;
    
    ndot = dot(normalColor, normalColor1);
    depthColor1 *= fn;
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    if (abs(depthColor - depthColor1) > dep)
        return float4(0, 0, 0, 1);
    
    if(_fogUse == 1)
        return lerp(realColor, _fogColor, factor) * (_sunColor * _sunIntensity);
    else
        return realColor * (_sunColor * _sunIntensity);
}