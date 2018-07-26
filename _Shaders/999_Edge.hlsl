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
    //���� ������ ���~���������� ���ô�
    //����� dot �� �� - 1������(pi / 180) �񱳷�
    //���̴� ������ ������...

    //ȥ���� ���� ���� �̸� ����
    //normal �񱳸� ���� ��
    //�ʹ� ũ�� �������� normal���� Ȯ �ٲ��� �ʴ� �̻� �ܰ��� �ȱ׷���
    //�ʹ� ������ ������ ���� ��ǻ� ��İԸ� �׷���
    float nor = (3.141592f / 180.0f) * 5.0f;
    //depth �񱳸� ���� ��
    //�ʹ� ũ�� ���̰� �Ÿ��� ���� ��������� ���̰� ���ٰ� ���� �ܰ��� �ȱ׸��� ��� �߻�
    //�ʹ� ������ �׳� �� ���� ���̳��ٰ� ���� �ܰ����� �������� �׸�
    float dep = 1.0f;

    float nordot, depth, ndot, de;  //nordot-���� �ȼ� dot��, depth-���� �ȼ� depth��, ndot-�� �ȼ� dot��, de-�� �ȼ� depth�� 
    bool bd = true;
    //uv���� pixel�� �°� �����ϱ� ����
    float hei = 1.0f / _valueHeight;
    float wid = 1.0f / _valueWidth;
    //depth�� �� �� �����ϰ� ����ϱ� ����
    float fn = _valueFar / _valueNear;
    //���� �ȼ��� normal, depth, real pixel ���� �����´�
    float4 normalColor = NormalRT.Sample(NormalRTSampler, input.uv);
    float4 depthColor = DepthRT.Sample(DepthRTSampler, input.uv);
    float4 realColor = RealRT.Sample(RealRTSampler, input.uv);
    
    //depth ���� near-far ���̶� ����ؼ� �� �� �ߺ��̰� ��ȭ
    //normal���� �ڱ� �ڽſ� ���� �������� ���Ѵ�
    nordot = dot(normalColor, normalColor);
    //depth�� ���� z / w�� ���ؾ���, depth ���� ���� r, g���� �س��� ����� �̷��� ����
    depth = depthColor.r / depthColor.g;
    depth *= fn;

    //���ȼ�
    float2 uv = input.uv;
    uv.y -= hei;
    float4 normalColor1 = NormalRT.Sample(NormalRTSampler, uv);
    float4 depthColor1 = DepthRT.Sample(DepthRTSampler, uv);
    
    //�ش� �ȼ��� ���� �ȼ��� ������ 
    ndot = dot(normalColor, normalColor1);
    //���� �ȼ��� depth��
    de = depthColor1.r / depthColor1.g;
    //depth�� ����
    de *= fn;
    //normal ��
    if (abs(nordot - ndot) > nor)
        return float4(0, 0, 0, 1);
    //depth ��
    if (abs(depth - de) > dep)
        return float4(0, 0, 0, 1);
    
    //���� �ȼ�
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
    
    //������ �ȼ�
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
    
    //�Ʒ��� �ȼ�
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

    //�˻��� �̻������ ���� �׷��� ���� �׸���
    return realColor;
}