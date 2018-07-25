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
    //연산 순서는 노멀~뎁스순으로 갑시다
    //노멀은 dot 값 비교 - 1도단위(pi / 180) 비교로
    //깊이는 적당한 값으로...

    //혼란을 막기 위해 미리 정의
    float nor = (3.141592 / 180) * 1;
    float dep = 0.0003f;

    float nordot, depth, ndot, de;  //nordot-현재 픽셀 dot값, depth-현재 픽셀 depth값, ndot-비교 픽셀 dot값, de-비교 픽셀 depth값 
    bool bd = true;
    float hei = 1.0f / _valueHeight;
    float wid = 1.0f / _valueWidth;
    float fn = _valueFar / _valueNear;

    float4 normalColor = NormalRT.Sample(NormalRTSampler, input.uv);
    float4 depthColor = DepthRT.Sample(DepthRTSampler, input.uv);
    float4 realColor = RealRT.Sample(RealRTSampler, input.uv);
    
    depthColor.rg *= fn;
    if(depthColor.r >= 1.00f  || depthColor.r <= 0.00f)
        bd = false;
    
    nordot = dot(normalColor, normalColor);
    depth = depthColor.r / depthColor.g;

    //윗픽셀
    float2 uv = input.uv;
    uv.y -= hei;
    float4 normalColor1 = NormalRT.Sample(NormalRTSampler, uv);
    float4 depthColor1 = DepthRT.Sample(DepthRTSampler, uv);
    depthColor1.rg *= fn;
    
    ndot = dot(normalColor, normalColor1);
    de = depthColor1.r / depthColor1.g;
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    if (abs(depth - de) > dep)//  && bd == true)
        return float4(0, 0, 0, 1);
    
    //왼쪽 픽셀
    uv.y += hei;
    uv.x -= wid;
    normalColor1 = NormalRT.Sample(NormalRTSampler, uv);
    depthColor1 = DepthRT.Sample(DepthRTSampler, uv);
    depthColor1.rg *= fn;
    
    ndot = dot(normalColor, normalColor1);
    de = depthColor1.r / depthColor1.g;
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    if (abs(depth - de) > dep)//  && bd == true)
        return float4(0, 0, 0, 1);
    
    //오른쪽 픽셀
    uv.x += wid;
    uv.x += wid;
    normalColor1 = NormalRT.Sample(NormalRTSampler, uv);
    depthColor1 = DepthRT.Sample(DepthRTSampler, uv);
    depthColor1.rg *= fn;
    
    ndot = dot(normalColor, normalColor1);
    de = depthColor1.r / depthColor1.g;
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    if (abs(depth - de) > dep)//  && bd == true)
        return float4(0, 0, 0, 1);
    
    //아래쪽 픽셀
    uv.x -= wid;
    uv.y += hei;
    normalColor1 = NormalRT.Sample(NormalRTSampler, uv);
    depthColor1 = DepthRT.Sample(DepthRTSampler, uv);
    depthColor1.rg *= fn;
    
    ndot = dot(normalColor, normalColor1);
    de = depthColor1.r / depthColor1.g;
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    if (abs(depth - de) > dep)//  && bd == true)
        return float4(0, 0, 0, 1);
    //return float4(1, 1, 1, 1);

    return realColor;
}