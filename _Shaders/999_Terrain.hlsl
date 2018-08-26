#include "000_Header.hlsl"

Texture2D _texture1 : register(t5);
Texture2D _texture2 : register(t6);
Texture2D _texture3 : register(t7);
Texture2D _texture4 : register(t8);

SamplerState _samplerState : register(s5);

cbuffer TerrainBuffer : register(b2)
{
    uint _on;
    uint _type;
    float _distance;
    float _padding1;
    float3 _point;
    float _padding2;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 oPosition : OPOSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
    float alpha : ALPHA0;
    float3 vPosition : POSITION0;
};

PixelInput VS(VertexColorTextureNormal input)
{
    PixelInput output;

    float4 world = mul(input.position, _world);
    output.position = mul(world, _view);
    output.vPosition = output.position;
    output.position = mul(output.position, _projection);

    output.oPosition = input.position;

    output.normal = GetWorldNormal(input.normal, _world);
    output.normal = normalize(output.normal);
    
    output.color = input.color;
    output.uv = input.uv;
    
    float temp, temp1;
    temp = world.y;
    temp1 = _waterHeight;

    output.alpha = saturate((temp / temp1) * 0.1f + 0.3f);
    
    return output;
}

PS_GBUFFEROUTPUT PS(PixelInput input)
{
    float4 penCol = float4(0, 0, 0, 0);

    if (_on == 1)
    {
        if (_type == 0)
        {
            float x = input.oPosition.x - _point.x;
            float z = input.oPosition.z - _point.z;

            float dis = sqrt(x * x + z * z);

            if (dis < _distance)
                penCol.r = 1.0f;
        }
        else if (_type == 1)
        {
            if(input.oPosition.x > _point.x - _distance &&
                input.oPosition.x < _point.x + _distance)
            {
                if (input.oPosition.z > _point.z - _distance &&
                    input.oPosition.z < _point.z + _distance)
                    penCol.g = 1.0f;
            }
        }

    }

    float4 color1 = _diffuseMap.Sample(_diffuseSampler, input.uv);
    float4 color2 = _texture1.Sample(_samplerState, input.uv);
    float4 diffuse = float4(0, 0, 0, 0);
    diffuse = lerp(color1, color2, input.color.a);

    color2 = _texture2.Sample(_samplerState, input.uv);
    diffuse = lerp(diffuse, color2, input.color.b);
    
    color2 = _texture3.Sample(_samplerState, input.uv);
    diffuse = lerp(diffuse, color2, input.color.g);
    
    color2 = _texture4.Sample(_samplerState, input.uv);
    diffuse = lerp(diffuse, color2, input.color.r);
    
    float3 color = diffuse.rgb + penCol.rgb;

    PS_GBUFFEROUTPUT output = (PS_GBUFFEROUTPUT) 0;
    output.color = float4(color, input.alpha);
    output.depth = float4(input.vPosition.z / _valueFar, input.oPosition.xyz);
    //output.normal.xy = NormalEncode(input.normal.xyz);
    output.normal.xyz = NormalEncode3to3(input.normal);

    return output;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Shadow Map
///////////////////////////////////////////////////////////////////////////////////////////////////

struct ShadowPixel
{
    float4 position : SV_POSITION;
    float3 vPosition : POSITION0;
};

ShadowPixel VS_Shadow(VertexColorTextureNormal input)
{
    ShadowPixel output;
    
    float4 world = mul(input.position, _world);
    output.position = mul(world, _lightView);
    output.vPosition = output.position;
    output.position = mul(output.position, _lightProjection);

    return output;
}

float PS_Shadow(ShadowPixel input) : SV_TARGET
{
    float depth = input.vPosition.z / _valueFar;

    return depth.rrrr;
}