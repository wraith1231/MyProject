#include "000_Header.hlsl"
//�̷��� ���ָ� có�� ��� ����

SamplerState _diffuseSampler : register(s0);
//SamplerState _detailSampler : register(s1);

cbuffer PS_Select : register(b2)
{
    float4 _color;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

//�� �ʿ��� vertex ������ �Ʒ�ó�� ������ָ� ��
//struct VertexTextureColor2
//{
//    float4 position : POSITION0;    //shader���� ��ġ�� �����ϱ� 4��
//    float2 uv : TEXCOORD0;
//    float4 color    : COLOR0;
//    float4 color2   : COLOR1;
//};

PixelInput VS(VertexColorTextureNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.normal = GetWorldNormal(input.normal, _world);
    
    output.uv = input.uv;

    output.color = _color;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    //float4 diffuseMap = _diffuseMap.Sample(_diffuseSampler, input.uv);
    //float4 diffuse = GetDiffuseColor(_diffuse, _direction, input.normal);
    //float2 detailUv = input.uv * _detailFactor * 2.0f;
    //float4 detail = _detailMap.Sample(_detailSampler, detailUv);
    
    return _color; //diffuseMap * diffuse; //* detail;
    //return diffuseMap * diffuse;
}