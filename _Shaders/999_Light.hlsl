#include "000_Header.hlsl"

//cbuffer PS_Value : register(b2)
//{
//    float _valueWidth;
//    float _valueHeight;
//    float _valueNear;
//    float _valueFar;
//}

cbuffer PS_LightValue : register(b3)
{
    float _lightAttenuation;
    float _specularPower;
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
    float3 view : VIEW0;
    float2 uv : TEXCOORD0;
};

PixelInput VS(VertexTextureNormal input)
{
    PixelInput output;

    float4 world = mul(input.position, _world);
    output.position = mul(world, _view);
    output.position = mul(output.position, _projection);
    
    output.view = GetViewPosition() - world.xyz;
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

half4 PS(PixelInput input) : SV_TARGET
{
    float depth = DepthRT.Sample(DepthRTSampler, input.uv).r;
    half4 normal = NormalRT.Sample(NormalRTSampler, input.uv);
    normal.rgb = decodeNormal(normal.rg);
    float4 diffuse = RealRT.Sample(RealRTSampler, input.uv);
    float3 pixelPos = input.view * depth;
    
    float lightDir = _direction;
    half attenuation = saturate(dot(lightDir / _lightAttenuation, lightDir / _lightAttenuation));
    lightDir = normalize(lightDir);

    float3 reflection = reflect(_direction, normal.rgb);
    float intensity = saturate(dot(reflection, input.view));

    //IntensityCut(intensity);

    float specular = pow(intensity, _specularPower);

    //float specular = pow(saturate(dot(reflect(normalize(-float3(0, 1, 0)), normal.rgb), lightDir)), _specularPower);

    float NL = dot(lightDir, normal.rgb) * attenuation;

    return float4(diffuse.rgb * NL, specular * NL);

    //return float4(0, 0, 0, 1);

}