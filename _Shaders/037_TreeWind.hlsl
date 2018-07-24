#include "000_Header.hlsl"

cbuffer VS_WIND : register(b2)
{
    matrix _rotate;

    float3 _windDirection;
    float _windPaddig1;

    float _windPower;
    float3 _windPadding2;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

PixelInput VS(VertexColorTextureNormal input)
{
    PixelInput output;
    
    matrix S, R, T, mat;
    R = _rotate;
    matrix temp = _viewInverse;
    S = _world;
    T = _world;
    
    S._41 = S._42 = S._43 = 0;
    T._11 = T._22 = T._33 = 1;
    
    temp._41 = temp._42 = temp._43 = 0.0f;
    
    temp._12 = temp._32 = temp._21 = temp._23 = 0.0f;
    
    R = mul(R, temp);
    
    mat = mul(S, R);
    mat = mul(mat, T);
    
    output.position = mul(input.position, mat);

    //땅 위면 바람의 영향을 받는다는 가정하에 연산
    //_windPower는 offset의 sin값, 즉 1~-1을 왔다갔다 하게 됨
    //바람 방향대로 _windPower값만큼 이동하면 원래 위치에서 -1~1까지 왔다갔다
    if (input.position.y > 0)
        output.position += float4(_windDirection * _windPower, 0);

    //output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{

    float4 diffuseMap = _diffuseMap.Sample(_diffuseSampler, input.uv);

    clip(diffuseMap.a - 0.9f);
    float3 light = _direction * -1;
    //light = saturate(light + 0.5);
    float3 nor = float3(0.0f, 1.0f, 0.0f);
    float intensity = dot(nor, light);

    //intensity = difCal(intensity);

    return diffuseMap * intensity;
}