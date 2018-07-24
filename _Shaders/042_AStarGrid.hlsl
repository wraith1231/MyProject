#include "000_Header.hlsl"

cbuffer AStarBuffer : register(b2)
{
    float3 _astarPos;
    float _astarPadding1;
    float3 _astaroPic;
    float _astarPadding2;
    float3 _astarPic;
    float _astarPadding3;
    uint _astarCheck;
    float3 _astarPadding4;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float3 oPosition : POSITION0;
    float3 color : COLOR0;
};

PixelInput VS(VertexColor input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.oPosition = input.position;
    output.color = input.color;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    //return float4(0, 0, 0, 1);

    float2 grid = input.oPosition.xz / 1.0f;

    float2 range = abs(frac(grid - 0.5f) - 0.5f);
    float2 speed = fwidth(grid);

    float2 pixelRange = range / speed;
    float lineWeight = saturate(min(pixelRange.x, pixelRange.y) - 0.2f);

    float4 color = float4(input.color, 1.0f);

    if (abs(input.oPosition.x - _astarPos.x) < 0.5f && abs(input.oPosition.z - _astarPos.z) < 0.5f)
        color = float4(1.0f, 0.0f, 0.0f, 1.0f);

    //if(input.oPosition.z > _astarPic.x)
    //    return float4(0.0f, 1.0f, 1.0f, 1.0f);

    if (_astarCheck > 0)
    {
        float3 max = float3(0.0f, 0.0f, 0.0f);
        float3 min = float3(0.0f, 0.0f, 0.0f);
        
        max.x = _astarPic.x;
        min.x = _astaroPic.x;
        if (_astaroPic.x > _astarPic.x)
        {
            max.x = _astaroPic.x;
            min.x = _astarPic.x;
        }
        
        max.z = _astarPic.z;
        min.z = _astaroPic.z;
        if (_astaroPic.z > _astarPic.z)
        {
            max.z = _astaroPic.z;
            min.z = _astarPic.z;
        }

        if(input.oPosition.x > min.x && input.oPosition.x < max.x- 1.0f &&
            input.oPosition.z > min.z && input.oPosition.z < max.z - 1.0f)
            return float4(0.0f, 1.0f, 1.0f, 1.0f);
            //color = float4(0.0f, 0.0f, 1.0f, 1.0f);
    }

    return lerp(color, float4(1, 1, 1, 1), lineWeight);
}