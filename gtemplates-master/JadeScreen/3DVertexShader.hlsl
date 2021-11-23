#pragma pack_matrix(row_major)

cbuffer ConstantBuffer : register(b0)
{
    matrix w, v, p;
}

struct VS_INPUT
{
    float3 pos : POSITION;
    float3 uvw : TEXCOORD;
    float3 norm : NORMAL;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float3 uvw : TEXCOORD;
};

PS_INPUT main(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    output.pos = mul(float4(input.pos, 1), w);
    output.pos = mul(output.pos, v);
    output.pos = mul(output.pos, p);
    output.norm = mul(input.norm, (float3x3) w);
    output.uvw = normalize(input.pos.xyz);
    return output;

}