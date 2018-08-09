#include "000_Header.hlsl"

cbuffer VS_Bone : register(b2)
{
    matrix _bones[256];
}

cbuffer VS_Render : register(b3)
{
    int _boneNumber;
    float3 _bonePadding;
}

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL0;
    float3 wPosition : POSITION0;
};

struct PixelNormalInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL0;
};

struct PixelDepthInput
{
    float4 position : SV_POSITION;
    float2 depth : DEPTH0;
};

struct PixelNDInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL0;
    float depth : DEPTH0;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
//기본 VS, PS
PixelInput VS_Bone(VertexTextureNormal input)
{
    PixelInput output;
    
    float4 world = mul(input.position, _bones[_boneNumber]);
    output.position = mul(world, _view);
    output.position = mul(output.position, _projection);

    output.wPosition = world;
    output.normal = mul(input.normal, (float3x3) _bones[_boneNumber]);
    output.normal = normalize(output.normal);
    output.uv = input.uv;

    return output;
}

float4 PS(PixelInput input) : SV_TARGET
{
    float4 color = _diffuseMap.Sample(_diffuseSampler, input.uv);
    //toon shading에 맞게 diffuse 컬러를 끊어놓았다
    //이 함수에서 그림자 구하고 바로 끊어버림
    color = GetDiffuseColor(color, _direction, input.normal);

    PointLightFunc(color.rgb, input.wPosition, input.normal);
    SpotLightFunc(color.rgb, input.wPosition, input.normal);


    return color;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//Normal Map VS, PS
PixelNormalInput VS_Normal(VertexTextureNormal input)
{
    PixelNormalInput output;
    
    output.position = mul(input.position, _bones[_boneNumber]);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);
    
    output.normal = mul(input.normal, (float3x3) _bones[_boneNumber]);
    output.normal = normalize(output.normal);

    return output;
}


float4 PS_Normal(PixelNormalInput input) : SV_TARGET
{
    float3 normal = input.normal;

    normal = (normal + 1.0f) * 0.5f;

    return float4(normal, 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//Depth VS, PS
PixelDepthInput VS_Depth(VertexTextureNormal input)
{
    PixelDepthInput output;
    
    output.position = mul(input.position, _bones[_boneNumber]);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);
    
    output.depth = output.position.zw;

    return output;
}


float4 PS_Depth(PixelDepthInput input) : SV_TARGET
{
    return float4(input.position.z, input.position.w, 1, 1);
    return float4(input.depth.x, input.depth.y, 1, 1);
    float depth = input.depth.x / input.depth.y;

    return float4(depth,depth, depth, 1);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//두개 혼용인데 이거 안쓰는게 좋다
PixelNDInput VS_ND(VertexTextureNormal input)
{
    PixelNDInput output;
    
    output.position = mul(input.position, _bones[_boneNumber]);
    output.position = mul(output.position, _view);
    output.position = mul(output.position, _projection);

    output.depth = 1.0f - output.position.z / output.position.w;

    output.normal = mul(input.normal, (float3x3) _bones[_boneNumber]);
    output.normal = normalize(output.normal);

    return output;

}

float4 PS_ND(PixelNDInput input) : SV_TARGET
{
    float3 normal = input.normal;

    normal = abs(normal);

    return float4(normal, input.depth);

}