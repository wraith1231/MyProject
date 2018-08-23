#include "000_Header.hlsl"


struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;

    float3 row[3] : ROW0;
    float2 bump[3] : BUMP0;

    float3 eye : VIEW0;
    
    float3 wPosition : POSITION0;
    float3 vPosition : POSITION1;
};

struct Wave
{
    float frequancy;
    float amplitude;
    float phase;
    float2 direction;
};

PixelInput VS(VertexTexture input)
{
    PixelInput output;
    Wave wave[3] =
    {
        { 1.0, 1.0, 0.5, float2(-1, 0) },
        { 2.0, 0.5, 1.7, float2(-0.7, -0.7) },
        { 0.6, 1.2, 1.3, float2(-0.3, -0.4) }
    };

    wave[0].frequancy = _waterWaveFrequancy;
    wave[0].amplitude = _waterWaveAmplitude;
    
    wave[1].frequancy = _waterWaveFrequancy * 3.0f;
    wave[1].amplitude = _waterWaveAmplitude * 0.33f;

    wave[2].frequancy = _waterWaveFrequancy * 1.5f;
    wave[2].amplitude = _waterWaveAmplitude * 1.2f;

    float4 pos = input.position;
    pos.y = _waterHeight;

    float4 world = mul(pos, _world);

    float ddx, ddy;
    float deriv;
    float angle;
    ddx = ddy = 0.0f;

    for (int i = 0; i < 3; i++)
    {
        angle = dot(wave[i].direction, world.xz) * wave[i].frequancy + _waterTime * wave[i].phase;
        world.y += wave[i].amplitude * sin(angle);
        deriv = wave[i].frequancy * wave[i].amplitude * cos(angle);
        ddx -= deriv * wave[i].direction.x;
        ddy -= deriv * wave[i].direction.y;
    }
    
    output.wPosition = world;
    output.position = mul(world, _view);
    output.vPosition = output.position;
    output.position = mul(output.position, _projection);

    output.eye = normalize(GetViewPosition() - world.xyz);
    output.uv = input.uv;

    output.row[0] = _waterBumpScale * normalize(float3(1, ddy, 0));
    output.row[1] = _waterBumpScale * normalize(float3(0, ddx, 1));
    output.row[2] = normalize(float3(ddx, 1, ddy));
    
    output.bump[0] = input.uv * _waterTextureScale + _waterTime * _waterBumpSpeed;
    output.bump[1] = input.uv * _waterTextureScale * 2.0f + _waterTime * _waterBumpSpeed * 4.0f;
    output.bump[2] = input.uv * _waterTextureScale * 4.0f + _waterTime * _waterBumpSpeed * 8.0f;
    

    return output;
}

PS_GBUFFEROUTPUT PS(PixelInput input) 
{
    float4 t0 = _normalMap.Sample(_normalSampler, input.bump[0]) * 2.0f - 1.0f;
    float4 t1 = _normalMap.Sample(_normalSampler, input.bump[1]) * 2.0f - 1.0f;
    float4 t2 = _normalMap.Sample(_normalSampler, input.bump[2]) * 2.0f - 1.0f;
    float3 n = t0.xyz + t1.xyz + t2.xyz;

    float3x3 mat;
    mat[0] = input.row[0];
    mat[1] = input.row[1];
    mat[2] = input.row[2];

    n = normalize(mul(n, mat));
    
    float4 r;
    r.xyz = reflect(input.eye, n);

    r.z = -r.z;
    r.w = _waterReflectionBlur;
    int2 offset = int2(0, 1);
    float4 reflection = _specularMap.SampleBias(_specularSampler, r.xy, r.w, offset);
    reflection.rgb *= (reflection.r + reflection.g + reflection.b) * _waterHDRMultiplier;

    float facing = saturate(1.0f - max(-dot(-input.eye, n), 0.0f));
    float fresnel = saturate(_waterFresnelBias + pow(facing, _waterFresnelPower));

    float4 waterColor = lerp(_waterShallowColor, _waterDeepColor, facing) * _waterAmount;

    reflection = lerp(waterColor, reflection * _waterReflectionColor, fresnel) * _waterReflectionAmount;

    float3 color = waterColor.rgb + reflection.rgb;
    
    PS_GBUFFEROUTPUT output = (PS_GBUFFEROUTPUT) 0;
    output.color = float4(color, 1);
    output.depth = float4(input.vPosition.z / _valueFar, input.wPosition);
    //output.normal.xy = NormalEncode(n.xyz);
    output.normal.xyz = n;

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

ShadowPixel VS_Shadow(VertexTexture input)
{
    ShadowPixel output;
    Wave wave[3] =
    {
        { 1.0, 1.0, 0.5, float2(-1, 0) },
        { 2.0, 0.5, 1.7, float2(-0.7, -0.7) },
        { 0.6, 1.2, 1.3, float2(-0.3, -0.4) }
    };

    wave[0].frequancy = _waterWaveFrequancy;
    wave[0].amplitude = _waterWaveAmplitude;
    
    wave[1].frequancy = _waterWaveFrequancy * 3.0f;
    wave[1].amplitude = _waterWaveAmplitude * 0.33f;

    wave[2].frequancy = _waterWaveFrequancy * 1.5f;
    wave[2].amplitude = _waterWaveAmplitude * 1.2f;

    float4 pos = input.position;
    pos.y = _waterHeight;

    float4 world = mul(pos, _world);

    float ddx, ddy;
    float deriv;
    float angle;
    ddx = ddy = 0.0f;

    for (int i = 0; i < 3; i++)
    {
        angle = dot(wave[i].direction, world.xz) * wave[i].frequancy + _waterTime * wave[i].phase;
        world.y += wave[i].amplitude * sin(angle);
        deriv = wave[i].frequancy * wave[i].amplitude * cos(angle);
        ddx -= deriv * wave[i].direction.x;
        ddy -= deriv * wave[i].direction.y;
    }
    
    output.position = mul(world, _lightView);
    output.vPosition = output.position;
    output.position = mul(output.position, _projection);

    return output;
}

float PS_Shadow(ShadowPixel input) : SV_TARGET
{
    float depth = input.vPosition.z / _valueFar;

    return depth;
}