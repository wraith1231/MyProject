#include "000_Header.hlsl"

cbuffer PS_DIRLIGHT : register(b2)
{
    float3 _dirAmbientDown;
    float _dirPadding1;
    float3 _dirAmbientRange;
    float _dirPadding2;
    float3 _dirToLight;
    float _dirPadding3;
    float3 _dirLightColor;
    float _dirPadding4;
}

Texture2D NormalRT : register(t0);
Texture2D DepthRT : register(t1);
Texture2D DiffuseRT : register(t2);
Texture2D ShadowMap : register(t3);

SamplerState NormalRTSampler : register(s0);
SamplerState DepthRTSampler : register(s1);
SamplerState DiffuseRTSampler : register(s2);
SamplerState ShadowMapSampler : register(s3);

static const float2 arrBasePos[4] =
{
    float2(-1.0f, 1.0f),
    float2(1.0f, 1.0f),
    float2(-1.0f, -1.0f),
    float2(1.0f, -1.0f),
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 view : VIEW0;
};

VS_OUTPUT VS(uint VertexID : SV_VERTEXID)
{
    VS_OUTPUT output;

    output.position = float4(arrBasePos[VertexID].xy, 0.0f, 1.0f);
    output.uv = saturate(output.position.xy);
    output.uv.y = 1.0f - output.uv.y;
    output.view = GetViewPosition().xyz;
    
    return output;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
    float4 depth = DepthRT.Sample(DepthRTSampler, input.uv);
    float3 oPos = depth.gba;
    float3 normal = NormalRT.Sample(NormalRTSampler, input.uv);
    normal = NormalDecode3to3(normal);
    float4 diffuse = DiffuseRT.Sample(DiffuseRTSampler, input.uv);
    
    float3 finalColor = (float4) 0;
    
    float3 fc = (float3) 1;
    
    float NDotL = dot(_dirToLight, normal);
    fc = _dirLightColor.rgb * saturate(NDotL);

    float3 lightDir = normalize(_dirToLight);

    float4 oTemp = float4(oPos, 1);
    float4 lDep = mul(oTemp, _lightView);
    float lDepth = lDep.z / _valueFar;
    float4 lTex = mul(lDep, _lightProjection);
    float2 tex = float2(lTex.xy / lTex.w);
    tex.x = tex.x * 0.5f + 0.5f;
    tex.y = -tex.y * 0.5f + 0.5f;
    
    if ((saturate(tex.x) == tex.x) && (saturate(tex.y) == tex.y))
    {
        float lDepth1 = ShadowMap.Sample(ShadowMapSampler, tex).r;
        lDepth1 += 0.00101f;
        
        if (lDepth1 < lDepth)
        {
            float lightInten = saturate(dot(lightDir, normal));
        
            if (lightInten > 0.0f)
            {
                fc *= lightInten;
            }
        
        }
    }
    
    fc *= diffuse.rgb;

    finalColor += fc;
    
    return float4(finalColor, 1.0f);
}