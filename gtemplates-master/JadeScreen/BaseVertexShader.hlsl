#pragma pack_matrix(row_major)

cbuffer ConstantBuffer : register(b0)
{
    matrix w, v, p;
    float4 lightPos[2];
    float4 lightColor[2];
}

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 uv : TEXCOORD;
    float3 norm : NORMAL;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : WORLD;
    float3 norm : NORMAL;
    float2 uv : TEXCOORD;
    float3 lightDist[2] : DISTANCE;
};

PS_INPUT main(VS_INPUT input, uint index : SV_InstanceID)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.pos = mul(float4(input.pos, 1), w);
    output.pos.x += float(index * 5);
    output.worldPos = output.pos;
    
    output.lightDist[0] = (float3) lightPos[0] - (float3) output.pos;
    output.lightDist[1] = (float3) lightPos[1] - (float3) output.pos;
    
    output.pos = mul(output.pos, v);
    output.pos = mul(output.pos, p);
    output.norm = mul(input.norm, (float3x3) w);
    output.uv = input.uv.xy;
    
    return output;
}
