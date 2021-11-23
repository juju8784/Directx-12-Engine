#pragma pack_matrix(row_major)

cbuffer ConstantBuffer : register(b0)
{
    matrix w, v, p;
}

struct VS_INPUT
{
    float3 pos : POSITION;
};

struct PS_INPUT
{
    float4 pos : SV_POSITION;
};

PS_INPUT main( VS_INPUT input ) : SV_POSITION
{
    PS_INPUT output = (PS_INPUT) 0;
    output.pos = mul(float4(input.pos, 1), w);
    output.pos = mul(output.pos, v);
    output.pos = mul(output.pos, p);
	return output;
}