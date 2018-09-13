//AS - All Shader
//VS - Vertex Shader
//PS - Pixel Shader
//HS - Hull Shader
//DS - Domain Shader
//GS - Geometry Shader

cbuffer AS_ViewProjection : register(b10)
{
    matrix _view;
    matrix _projection;
    matrix _viewInverse;
}

cbuffer VS_World : register(b1)
{
    matrix _world;
}

cbuffer VPS_Light : register(b0)
{
    float3 _direction;
    float LightPadding;

    float4 _sunColor;

    float _overcast;
    float _sunIntensity;

    float2 LightPadding2; // 마지막 패딩은 없어도 됨

    float3 _lightPosition;
    float _lightPadding3;

    matrix _lightView;
    matrix _lightProjection;
}

cbuffer PS_Material : register(b1)
{
    float4 _ambient;
    float4 _diffuse;
    float4 _specular;
    float4 _emissive;
	
    float _shininess;
    float _detailFactor;

    float MaterialPadding[2];
}

cbuffer VPS_Value : register(b11)
{
    float _valueWidth;
    float _valueHeight;
    float _valueNear;
    float _valueFar;
}

cbuffer VS_WaterBuffer : register(b12)
{
    float2 _waterTextureScale;
    float _waterWaveFrequancy;
    float _waterWaveAmplitude;

    float _waterTime;
    float _waterBumpScale;
    float2 _waterBumpSpeed;

    float _waterHeight;
    float3 _waterVSPadding;
}

cbuffer PS_WaterBuffer : register(b12)
{
    float4 _waterDeepColor;
    float4 _waterShallowColor;
    
    float4 _waterReflectionColor;

    float _waterReflectionAmount;
    float _waterReflectionBlur;
    float _waterFresnelPower;
    float _waterFresnelBias;

    float _waterHDRMultiplier;
    float _waterAmount;
    float2 _waterPSPadding;
}

cbuffer PS_Fog : register(b13)
{
    float4 _fogColor;

    float _fogStart;
    float _fogEnd;
    uint _fogUse;
    float _fogPadding;
}

struct PS_GBUFFEROUTPUT
{
    float4 normal : SV_TARGET0;
    float4 depth : SV_TARGET1;
    float4 color : SV_TARGET2;
};

float3 NormalEncode3to3(float3 normal)
{
    float3 output = (normal + 1.0f) * 0.5f;

    return output;
}

float3 NormalDecode3to3(float3 normal)
{
    float3 output = (normal * 2.0f) - 1.0f;

    return output;
}

float2 NormalEncode3to2(float3 normal)
{
    float2 output = (float2) 0;
    
    output.x = normal.x / (1 + normal.z);
    output.y = normal.y / (1 + normal.z);
    return output;
    //float p = sqrt(input.normal.z * 8 + 8);
    //output.normal = float4(input.normal.xy / p + 0.5f, 0, 0);
}   

/*
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
*/
float3 NormalDecode2to3(float2 encnor)
{
    float3 output = (float3) 0;

    float denom = 2 / (1 + encnor.x * encnor + encnor.y * encnor.y);
    output.x = encnor.x * denom;
    output.y = encnor.y * denom;
    output.z = denom - 1;

    return output;
}

Texture2D _diffuseMap : register(t0);
Texture2D _specularMap : register(t1);
Texture2D _emissiveMap : register(t2);
Texture2D _normalMap : register(t3);
Texture2D _detailMap : register(t4);

SamplerState _diffuseSampler : register(s0);
SamplerState _specularSampler : register(s1);
SamplerState _emissiveSampler : register(s2);
SamplerState _normalSampler : register(s3);
SamplerState _detailSampler : register(s4);


struct Vertex
{
    float4 position : POSITION0;
};

struct VertexColor
{
    float4 position : POSITION0;
    float4 color : COLOR0;
};

struct VertexColorNormal
{
    float4 position : POSITION0;
    float4 color : COLOR0;
    float3 normal : NORMAL0;
};

struct VertexTexture
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
};

struct VertexTextureNormal
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VertexColorTextureNormal
{
    float4 position : POSITION0;
    float4 color : COLOR0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct VertexTextureNormalBlend
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float4 blendIndices : BLENDINDICES0;
    float4 blendWeights : BLENDWEIGHTS0;
};

struct VertexTextureNormalTangentBlend
{
    float4 position : POSITION0;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 tangent : TANGENT0;
    float4 blendIndices : BLENDINDICES0;
    float4 blendWeights : BLENDWEIGHTS0;
};

float3 GetViewPosition()
{
    return _viewInverse._41_42_43;
}

void IntensityCut(inout float intensity)
{
    [branch]
    if (intensity < 0.01f)
        intensity = 0.0f;
    else if (intensity < 0.2f)
        intensity = 0.2f;
    else if (intensity < 0.4f)
        intensity = 0.4f;
    else if (intensity < 0.6f)
        intensity = 0.6f;
    else if (intensity < 0.8f)
        intensity = 0.8f;
    else if (intensity < 1.0f)
        intensity = 1.0f;
    else if (intensity < 1.2f)
        intensity = 1.2f;
    else if (intensity < 1.4f)
        intensity = 1.4f;
    else
        intensity = 1.6f;
}

void Diffuse(inout float3 color, in float3 normal)
{
    float3 light = _direction * -1.0f;
    float intensity = saturate(dot(normal, light));

    //IntensityCut(intensity);
    
    color = _diffuse.rgb * intensity;
    //color = color + _diffuse.rgb * intensity;
}

void Diffuse(inout float3 color, in float3 diffuse, in float3 normal)
{
    float3 light = _direction * -1.0f;
    float intensity = saturate(dot(normal, light));

    //IntensityCut(intensity);
 
    color = diffuse.rgb * intensity;
    //color = color + diffuse.rgb * intensity;
}

void Specular(inout float3 color, in float3 direction, in float3 normal, in float3 eye)
{
    float3 reflection = reflect(_direction, normal);
    float intensity = saturate(dot(reflection, eye));

    //IntensityCut(intensity);

    float specular = pow(intensity, _shininess);

    color = color + _specular.rgb * specular;
}

void Specular(inout float3 color, in float3 mapIntensity, in float3 direction, in float3 normal, in float3 eye)
{
    float3 reflection = reflect(_direction, normal);
    float intensity = saturate(dot(reflection, eye));

    //IntensityCut(intensity);

    float specular = pow(intensity, _shininess);

    color = color + _specular.rgb * mapIntensity * specular;
}

//디퓨즈 음영강도 계산
float GetDiffuseIntensity(float3 direction, float3 normal)
{
    float3 light = direction * -1;
    
    float intensity = saturate(dot(normal, light));

    //IntensityCut(intensity);

    return intensity;
}

//디퓨즈 라이팅 계산
float4 GetDiffuseColor(float4 color, float3 direction, float3 normal)
{
    float3 light = direction * -1;
    float intensity = saturate(dot(normal, light));

    //IntensityCut(intensity);

    return float4(color.rgb * intensity, 0);
}

float3 GetWorldNormal(float3 normal, float4x4 world)
{
    return mul(normal, (float3x3) world);
}