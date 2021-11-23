SamplerState sample : register(s0);
TextureCube tex3D : register(t0);


struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float3 norm : NORMAL;
    float3 uvw : TEXCOORD;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    return tex3D.Sample(sample, input.uvw);
}