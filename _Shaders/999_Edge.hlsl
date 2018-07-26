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
    //normal 비교를 위한 값
    //너무 크면 어지간히 normal값이 확 바뀌지 않는 이상 외곽선 안그려짐
    //너무 작으면 원같은 경우는 사실상 까맣게만 그려짐
    float nor = (3.141592f / 180.0f) * 5.0f;
    //depth 비교를 위한 값
    //너무 크면 깊이가 거리에 따라서 어느정도는 차이가 없다고 보고 외곽선 안그리는 경우 발생
    //너무 작으면 그냥 다 깊이 차이난다고 보고 외곽선을 덕지덕지 그림
    float dep = 1.0f;

    float nordot, depth, ndot, de;  //nordot-현재 픽셀 dot값, depth-현재 픽셀 depth값, ndot-비교 픽셀 dot값, de-비교 픽셀 depth값 
    bool bd = true;
    //uv값을 pixel에 맞게 조정하기 위함
    float hei = 1.0f / _valueHeight;
    float wid = 1.0f / _valueWidth;
    //depth용 좀 더 수월하게 계산하기 위함
    float fn = _valueFar / _valueNear;
    //현재 픽셀의 normal, depth, real pixel 값을 가져온다
    float4 normalColor = NormalRT.Sample(NormalRTSampler, input.uv);
    float4 depthColor = DepthRT.Sample(DepthRTSampler, input.uv);
    float4 realColor = RealRT.Sample(RealRTSampler, input.uv);
    
    //depth 값을 near-far 값이랑 비례해서 좀 더 잘보이게 분화
    //normal에서 자기 자신에 대한 내적값을 구한다
    nordot = dot(normalColor, normalColor);
    //depth는 원래 z / w로 구해야함, depth 맵의 값은 r, g에다 해놓은 관계로 이렇게 연산
    depth = depthColor.r / depthColor.g;
    depth *= fn;

    //윗픽셀
    float2 uv = input.uv;
    uv.y -= hei;
    float4 normalColor1 = NormalRT.Sample(NormalRTSampler, uv);
    float4 depthColor1 = DepthRT.Sample(DepthRTSampler, uv);
    
    //해당 픽셀과 현재 픽셀의 내적값 
    ndot = dot(normalColor, normalColor1);
    //현재 픽셀의 depth값
    de = depthColor1.r / depthColor1.g;
    //depth값 보정
    de *= fn;
    //normal 비교
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    //depth 비교
    if (abs(depth - de) > dep)
        return float4(0, 0, 0, 1);
    
    //왼쪽 픽셀
    uv.y += hei;
    uv.x -= wid;
    normalColor1 = NormalRT.Sample(NormalRTSampler, uv);
    depthColor1 = DepthRT.Sample(DepthRTSampler, uv);
    
    ndot = dot(normalColor, normalColor1);
    de = depthColor1.r / depthColor1.g;
    de *= fn;
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    if (abs(depth - de) > dep)
        return float4(0, 0, 0, 1);
    
    //오른쪽 픽셀
    uv.x += wid;
    uv.x += wid;
    normalColor1 = NormalRT.Sample(NormalRTSampler, uv);
    depthColor1 = DepthRT.Sample(DepthRTSampler, uv);
    
    ndot = dot(normalColor, normalColor1);
    de = depthColor1.r / depthColor1.g;
    de *= fn;
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    if (abs(depth - de) > dep)
        return float4(0, 0, 0, 1);
    
    //아래쪽 픽셀
    uv.x -= wid;
    uv.y += hei;
    normalColor1 = NormalRT.Sample(NormalRTSampler, uv);
    depthColor1 = DepthRT.Sample(DepthRTSampler, uv);
    
    ndot = dot(normalColor, normalColor1);
    de = depthColor1.r / depthColor1.g;
    de *= fn;
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    if (abs(depth - de) > dep)
        return float4(0, 0, 0, 1);

    //검사결과 이상없으면 원래 그려질 색을 그린다
    return realColor;
}