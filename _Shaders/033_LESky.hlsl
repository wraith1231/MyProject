#include "000_Header.hlsl"

cbuffer PS_Sky : register(b2)
{
    //시간 순서순?

    //수평선
    float4 _horizonColorTwilight;
    float4 _horizonColorDay;
    float4 _horizonColorNight;

    //최대 높이
    float4 _ceilingColorTwilight;
    float4 _ceilingColorDay;
    float4 _ceilingColorNight;

    //시간
    float _height;  //수평선 높이
    float _time;    //시간

    float2 _skyPadding;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 oPosition : POSITION0;
};

PixelInput VS(VertexColorTextureNormal input)
{
    PixelInput output;

    output.position = mul(input.position, _world);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.oPosition = input.position;
    output.uv = input.uv;
    
    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    //쉐이더 전용 문법, 이러면 전부 0으로 초기화
    float4 horizonColor = 0;
    float4 ceilingColor = 0;
    
    if (_direction.y > 0)
    {
        float amount = min(_direction.y * 1.5, 1.0f);
        
        horizonColor = lerp(_horizonColorTwilight, _horizonColorNight, amount);
        ceilingColor = lerp(_ceilingColorTwilight, _ceilingColorNight, amount);
    }
    else
    {
        float amount = min(-_direction.y * 1.5, 1.0f);
        
        horizonColor = lerp(_horizonColorTwilight, _horizonColorDay, amount);
        ceilingColor = lerp(_ceilingColorTwilight, _ceilingColorDay, amount);
    }

    float3 color = lerp(horizonColor, ceilingColor, saturate(input.oPosition.y / _height));
    color /= _sunOvercast;
    
    float3 temp = float3(1.2f, 0.8f, 0.0f);
    float dist = saturate(1 - distance(input.oPosition.xyz, _direction)) / 2;
    float intensity = (1 - _sunIntencity) / _sunOvercast;
    
    color += temp * dist * intensity;
    
    
    float horizonLerp = saturate(lerp(0, 1, input.oPosition.y * 1.5f));
    float directionLerp = lerp(0, 1, max(_direction.y, 0));
    float starAlpha = saturate(horizonLerp * directionLerp);
    
    float2 starUv = input.uv * 16;
    starUv.xy -= _time / 40; //앞은 시간, 뒤는 별의 속도
    
    color += _diffuseMap.Sample(_diffuseSampler, starUv) * starAlpha * 0.6f / _sunOvercast;    //0.6은 임의로
    
    return float4(color, 1);

    //float3 temp = float3(1.2f, 0.8f, 0.0f);
    //float dist = saturate(1 - distance(input.oPosition.xyz, _direction)) / 2; //잠시접어둬
    //float intensity = (1 - _sunIntencity) / _sunOvercast; // " 
    //
    //color += temp * dist * intensity;
    //
    //float horizonLerp = saturate(lerp(0, 1, input.oPosition.y * 1.5f));
    //float directionLerp = lerp(0, 1, max(_direction.y, 0));
    //float starAlpha = saturate(horizonLerp * directionLerp);
    //
    //float2 starUv = input.uv * 16;
    //starUv.xy -= _time / 40;
    //
    //color += _diffuseMap.Sample(_diffuseSampler, starUv) * starAlpha * 0.6f / _sunOvercast;
    //
    //return float4(color, 1);
}