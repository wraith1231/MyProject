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

///////////////////////////////////////////////////////////////////////////////////////////////////

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 oPosition : OPOSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 color : COLOR0;
    float alpha : ALPHA0;
    float3 wPosition : POSITION0;
};

PixelInput VS(VertexColorTextureNormal input)
{
    PixelInput output;

    float4 world = mul(input.position, _world);
    output.position = mul(world, _view);
    output.position = mul(output.position, _projection);

    output.oPosition = input.position;

    output.normal = GetWorldNormal(input.normal, _world);
    output.normal = normalize(output.normal);
    
    output.color = input.color;
    output.uv = input.uv;

    output.alpha = 0.5f * (1.0f - saturate(1.0f / length(GetViewPosition() - world.xyz)));
    float4 temp = world;
    
    [branch]
    if (_waterHeight == 0)
    {
        temp.y++;
        float4 tempw = _waterHeight;
        tempw.y += 1;
        output.alpha = 1.0f - saturate((tempw - temp.y) / tempw - 0.4f);
    }
    else
        output.alpha = 1.0f - saturate((_waterHeight - world.y) / _waterHeight - 0.4f);

    output.wPosition = world;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
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

    //diffuse = GetDiffuseColor(diffuse, _direction, input.normal);
    
    float3 color = diffuse.rgb + penCol.rgb;
    //PointLightFunc(color.rgb, input.wPosition, input.normal);
    //SpotLightFunc(color.rgb, input.wPosition, input.normal);


    return float4(color, input.alpha);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

struct PixelNormalInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL0;
};

PixelNormalInput VS_Normal(VertexColorTextureNormal input)
{
    PixelNormalInput output;
    
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.normal = GetWorldNormal(input.normal, _world);
    output.normal = normalize(output.normal);
    
    return output;
}

half4 PS_Normal(PixelNormalInput input) : SV_TARGET
{
    half p = sqrt(input.normal.z * 8 + 8);
    return half4(input.normal.xy / p + 0.5, 0, 0);
    //float4 normal = float4(input.normal, 1);
    //normal = (normal + 1.0f) * 0.5f;
    //
    //return normal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////

struct PixelDepthInput
{
    float4 position : SV_POSITION;
    float2 depth : DEPTH0;
};

PixelDepthInput VS_Depth(VertexColorTextureNormal input)
{
    PixelDepthInput output;
    
    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.depth = output.position.zw;
    output.position = mul(output.position, _projection);
    
    return output;
}

float PS_Depth(PixelDepthInput input) : SV_TARGET
{
    return input.depth.x / _valueFar;
    return input.position.z / input.position.w;
    //return float4(input.position.zw, 1, 1);
    //return float4(input.zw.x, input.zw.y, 0, 1);
}
