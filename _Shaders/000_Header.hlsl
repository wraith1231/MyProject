cbuffer VS_ViewProjection : register(b0)
{
    matrix _view;
    matrix _projection;
    matrix _viewInverse;
}

cbuffer VS_World : register(b1)
{
    matrix _world;
}

cbuffer PS_Light : register(b0)
{
    float3 _direction;
    float LightPadding;
    
    float4 _sunColor;
    
    float _overcast;
    float _sunIntensity;
    
    float2 LightPadding2;
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


//디퓨즈 음영강도 계산
float GetDiffuseIntensity(float3 direction, float3 normal)
{
    float3 light = direction * -1;
    
    return saturate(dot(normal, light));
}

float ToonShading(float intensity)
{
    float inten = intensity;
    
    if (inten < 0.2f)
        inten = 0.2f;
    else if (inten > 0.2f && inten < 0.4f)
        inten = 0.4f;
    else if (inten > 0.4f && inten < 0.6f)
        inten = 0.6f;
    else if (inten > 0.6f && inten < 0.8f)
        inten = 0.8f;
    else if (inten > 0.8f)
        inten = 1.0f;

    return inten;
}

//디퓨즈 라이팅 계산
float4 GetDiffuseColor(float4 color, float3 direction, float3 normal)
{
    float3 light = direction * -1;
    float intensity = saturate(dot(normal, light));

    intensity = ToonShading(intensity);

    return float4(color.rgb * intensity, 0);
}

float3 GetWorldNormal(float3 normal, float4x4 world)
{
    return mul(normal, (float3x3) world);
}