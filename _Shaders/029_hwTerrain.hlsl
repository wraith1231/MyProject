#include "000_Header.hlsl"

cbuffer PS_Grid : register(b2)
{
    float4 _gridColor;

    uint _gridSpacing;
    float _gridThickness;

    uint _gridView;

    float _gridPadding;
}

cbuffer PS_Ver1 : register(b3)
{
    float3 _position1;
    float _ver1Padding1;
    float3 _position2;
    float _ver1Padding2;
}
cbuffer PS_Ver2 : register(b4)
{
    float4 _brushColor;

    float3 _oPosition;
    float _ver2Padding1;
    
    float _range;
    uint _type;
    float _opercity;

    float _ver2Padding2;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 oPosition : POSITION0;
    float4 color : COLOR0;
};

SamplerState _sampler : register(s0);

PixelInput VS(VertexColorTextureNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.normal = GetWorldNormal(input.normal, _world);
    
    output.oPosition = input.position.xyz;
    output.color = input.color;
    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    if (_type == 0)
    {
        if (_position1.x >= 0 && _position1.z >= 0)
        {
            float2 min = float2(0, 0);
            min.x = _position1.x - _range;
            min.y = _position1.z - _range;
            float2 max = float2(0, 0);
            max.x = _position2.x + _range;
            max.y = _position2.z + _range;
    
            if (input.oPosition.x > min.x && input.oPosition.x < max.x + _gridThickness &&
            input.oPosition.z > min.y && input.oPosition.z < max.y + _gridThickness)
                return _brushColor;
        }
    }
    if (_type == 1)
    {
        if (_position1.x >= 0 && _position1.z >= 0)
        {
            float xdis = abs(input.oPosition.x - _position1.x);
            float zdis = abs(input.oPosition.z - _position1.z);
        
            float dis = sqrt(xdis * xdis + zdis * zdis);
    
            if (dis < _range + _gridThickness)
                return _brushColor;
        }
    }
    if (_type == 2)
    {
        float2 min = float2(0, 0);
        float2 max = float2(0, 0);
        
        min.x = _oPosition.x;
        max.x = _position2.x;
        if(_oPosition.x >= _position2.x)
        {
            max.x = _oPosition.x;
            min.x = _position2.x;
        }
        min.y = _oPosition.z;
        max.y = _position2.z;
        if (_oPosition.z >= _position2.z)
        {
            max.y = _oPosition.z;
            min.y = _position2.z;
        }

        if (input.oPosition.x >= min.x && input.oPosition.x <= max.x + _gridThickness &&
            input.oPosition.z >= min.y && input.oPosition.z <= max.y + _gridThickness)
        {
            float2 grid = float2(0, 0);
            grid.x = frac(input.oPosition.x / (float) _gridSpacing);
            grid.y = frac(input.oPosition.z / (float) _gridSpacing);
            
            if (grid.x < _gridThickness || grid.y < _gridThickness)
                return _gridColor;

        }

    }
    
    float4 color1 = _diffuseMap.Sample(_diffuseSampler, input.uv);
    float4 color2 = _specularMap.Sample(_specularSampler, input.uv);

    float3 light = _direction * -1;
    float intensity = dot(input.normal, light);

    //intensity = difCal(intensity);
    
    float4 diffuse = (1 - input.color.a) * color1 + input.color.a * color2;
    
    color2 = _emissiveMap.Sample(_emissiveSampler, input.uv);
    diffuse = (1 - input.color.b) * diffuse + input.color.b * color2;
    
    color2 = _normalMap.Sample(_normalSampler, input.uv);
    diffuse = (1 - input.color.g) * diffuse + input.color.g * color2;
    
    color2 = _detailMap.Sample(_detailSampler, input.uv);
    diffuse = (1 - input.color.r) * diffuse + input.color.r * color2;
    

    if (_gridView == 0)
    {
        //float2 grid = float2(0, 0);
        //grid.x = frac(input.oPosition.x / (float) _gridSpacing);
        //grid.y = frac(input.oPosition.z / (float) _gridSpacing);
        //
        //if (grid.x < _gridThickness || grid.y < _gridThickness)
        //    return _gridColor;
        float2 grid = input.oPosition.xz / _gridSpacing;

        float2 range = abs(frac(grid - 0.5f) - 0.5f);
        float2 speed = fwidth(grid); //ddx, ddy 는 주변 픽셀에 대한 차이

        float2 pixelRange = range / speed;
        float lineWeight = saturate(min(pixelRange.x, pixelRange.y) - _gridThickness);
    
        return lerp(_gridColor, diffuse * intensity, lineWeight);
    }
    
    //float2 grid = input.oPosition.xz / 2;
    //
    //float2 range = abs(frac(grid - 0.5f) - 0.5f);
    //float2 speed = fwidth(grid);
    //
    //float2 pixelRange = range / speed;
    //float lineWeight = saturate(min(pixelRange.x, pixelRange.y) - 0.5f);
    //
    //return lerp(float4(0, 0, 0, 1), diffuse * intensity, lineWeight);


    return diffuse * intensity;
}