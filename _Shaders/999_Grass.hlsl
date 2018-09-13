#include "000_Header.hlsl"

cbuffer VS_WIND : register(b2)
{
    matrix _rotate;

    float3 _windDirection;
    float _windPaddig1;

    float _windPower;
    float3 _windPadding2;
}

struct VertexGrass
{
    float3 position : POSITION0;
    float2 size : SIZE0;
};

struct VSOutput
{
    float4 position : POSITION0;
    float2 size : SIZE0;
};

VSOutput VS(VertexGrass input)
{
    VSOutput output;
    
    output.position = float4(input.position, 1.0f);
    output.size = input.size;

    return output;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//Geometry Shader
///////////////////////////////////////////////////////////////////////////////////////////////////

cbuffer GS_WindPower : register(b2)
{
    float _grassWind;
    float3 _grassPadding;
}

struct GSOutput
{
    float4 posH : SV_POSITION;
    float3 posW : POSITION0;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

[maxvertexcount(4)]
void GS(point VSOutput gin[1], uint primID : SV_PRIMITIVEID, inout TriangleStream<GSOutput> triStream)
{
    float2 tex[4] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f)
    };

    float3 up = float3(0.0f, 1.0f, 0.0f);
    
    float3 look = GetViewPosition() - gin[0].position.xyz;
    
    look.y = 0.0f;
    look = normalize(look);
    float3 right = cross(up, look);
    
    float halfWidth = 0.5f * gin[0].size.x;
    float halfHeight = 0.5f * gin[0].size.y;
    
    float3 pos = gin[0].position;
    pos += halfHeight * up;

    float4 v[4];
    v[0] = float4(pos + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(pos + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(pos - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(pos - halfWidth * right + halfHeight * up, 1.0f);


    GSOutput output = (GSOutput) 0;
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        output.posH = mul(v[i], _view);
        output.posH = mul(output.posH, _projection);
        output.posW = v[i].xyz;
        output.normal = look;
        output.uv = tex[i];

        triStream.Append(output);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//Pixel Shader
///////////////////////////////////////////////////////////////////////////////////////////////////

Texture2D GrassMap : register(t0);
SamplerState GrassSampler : register(s0);

PS_GBUFFEROUTPUT PS(GSOutput input)
{
    float4 diffuseMap = GrassMap.Sample(GrassSampler, input.uv);

    clip(diffuseMap.a - 0.8f);

    PS_GBUFFEROUTPUT output = (PS_GBUFFEROUTPUT) 0;
    output.color = diffuseMap;
    output.depth = float4(input.posH.z / _valueFar, input.posW.xyz);
    //output.normal.xy = NormalEncode(input.normal.xyz);
    output.normal.xyz = NormalEncode3to3(input.normal);

    return output;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Shadow Map
///////////////////////////////////////////////////////////////////////////////////////////////////

struct ShadowPixel
{
    float4 pos : SV_POSITION;
    float3 position : POSITION0;
    float2 size : SIZE0;
};

ShadowPixel VS_Shadow(VertexGrass input)
{
    ShadowPixel output;
    
    output.pos = float4(0, 0, 0, 1);
    output.position = input.position;
    output.size = input.size;

    return output;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//Geometry Shader
///////////////////////////////////////////////////////////////////////////////////////////////////

struct GSShadowOutput
{
    float4 posH : SV_POSITION;
    float4 vPosition : POSITION0;
    float2 uv : TEXCOORD0;
};

[maxvertexcount(4)]
void GS_Shadow(point ShadowPixel gin[1], uint primID : SV_PRIMITIVEID, inout TriangleStream<GSShadowOutput> triStream)
{
    float3 up = float3(0.0f, 1.0f, 0.0f);
    float3 look = GetViewPosition() - gin[0].position;
    look.y = 0.0f;
    look = normalize(look);
    float3 right = cross(up, look);
    
    float halfWidth = 0.5f * gin[0].size.x;
    float halfHeight = 0.5f * gin[0].size.y;

    float4 v[4];
    v[0] = float4(gin[0].position + halfWidth * right - halfHeight * up, 1.0f);
    v[1] = float4(gin[0].position + halfWidth * right + halfHeight * up, 1.0f);
    v[2] = float4(gin[0].position - halfWidth * right - halfHeight * up, 1.0f);
    v[3] = float4(gin[0].position - halfWidth * right + halfHeight * up, 1.0f);

    float2 tex[4] =
    {
        float2(0.0f, 1.0f),
        float2(0.0f, 0.0f),
        float2(1.0f, 1.0f),
        float2(1.0f, 0.0f)
    };

    GSShadowOutput output;
    [unroll]
    for (int i = 0; i < 4; i++)
    {
        output.posH = mul(v[i], _lightView);
        output.vPosition = output.posH;
        output.posH = mul(v[i], _lightProjection);
        output.uv = tex[i];

        triStream.Append(output);
    }
}


float PS_Shadow(GSShadowOutput input) : SV_TARGET
{
    float4 diffuseMap = GrassMap.Sample(GrassSampler, input.uv);

    clip(diffuseMap.a - 0.8f);

    float depth = input.vPosition.z / _valueFar;

    return depth;
}