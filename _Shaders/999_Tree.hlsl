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
    float3 wPosition : POSITION0;
    float3 normal : NORMAL0;
    float3 vPosition : POSITION1;
};

PixelInput VS(VertexTextureNormal input)
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
    
    if (input.position.y > 0)
        output.position += float4(_windDirection * _windPower, 0);

    output.wPosition = output.position;
    output.position = mul(output.position, _view);
    output.vPosition = output.position;
    output.position = mul(output.position, _projection);

    output.normal = normalize(mul(float3(0, 1, 0), (float3x3) mat));
    output.uv = input.uv;

    return output;
}

PS_GBUFFEROUTPUT PS(PixelInput input)
{
    float4 diffuseMap = _diffuseMap.Sample(_diffuseSampler, input.uv);

    clip(diffuseMap.a - 0.9f);

    PS_GBUFFEROUTPUT output = (PS_GBUFFEROUTPUT) 0;
    output.color = diffuseMap;
    output.depth = float4(input.vPosition.z / _valueFar, input.wPosition.xyz);
    //output.normal.xy = NormalEncode(input.normal.xyz);
    output.normal.xyz = input.normal;

    return output;
}