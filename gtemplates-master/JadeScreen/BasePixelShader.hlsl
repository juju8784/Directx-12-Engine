SamplerState sample : register(s0);
Texture2D tex2D : register(t0);

cbuffer ConstantBuffer : register(b0)
{
    matrix w, v, p;
    float4 lightPos[2];
    float4 lightColor[2];
    float4 lightDirection[1];
    float4 worldCamera;
}

struct PS_INPUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : WORLD;
    float3 norm : NORMAL;
    float2 uv : TEXCOORD;
    float3 lightDist[2] : DISTANCE;
};

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 surfaceNormal = normalize(input.norm);
    float4 tex = tex2D.Sample(sample, input.uv);
    if (tex.a < 0.5)
    {
        discard;
    }
    //return tex;
    float4 finalColor = 0;
    float4 lightDirW1 = normalize(float4(0.0f, -1.0f, 1.0f, 0));
    float4 lightDirW2 = normalize(float4(-2.0f, 1.0f, 1.0f, 0));
    float4 lightColor1 = float4(1.0f, 0, 0, 0);
    float4 lightColor2 = float4(0, 1.0f, 0, 0);
    float3 viewDir = normalize(input.lightDist[1]);
    float3 viewDir2 = normalize(worldCamera - input.worldPos);
    float specularPower = 128;
    float specularIntensity = 0.8f;
    //finalColor += saturate(dot(input.norm, -(float3) lightDirW1)) * lightColor1;
    //finalColor += saturate(dot(input.norm, -(float3) lightDirW2)) * lightColor2;
    //Point Light
    if (lightPos[0].w != -1)
    {
        float attenuation = 1.0f - saturate(length(lightPos[0] - input.worldPos) / 10.0f);
        float3 lightDir = normalize(lightPos[0] - input.worldPos);
        finalColor += saturate(dot(input.norm, (float3) lightDir)) * lightColor[0] * attenuation;
        
        float3 reflection = normalize(reflect((float3) -lightDir, input.norm));
        float intensity = pow(saturate(dot(viewDir2, reflection)), specularPower);
        float lightRatio = saturate(dot(input.norm, (float3) lightDir));
        finalColor += (lightColor[0] * specularIntensity * intensity * lightRatio * attenuation);
    }
    //Spot light
    if (lightDirection[0].w != -1)
    {
        float3 spotPos = float3(5, 10, 0);
        float3 spotDir = float3(0, -1, 1);
        float innerRatio = 0.95f;
        float outerRatio = 0.8f;
        float radiusAttenuation = 1.0f - saturate(length(lightPos[1] - input.worldPos) / 30.0f);
        float3 lightDir = normalize(lightPos[1] - input.worldPos);
        float surfaceRatio = saturate(dot(-lightDir, normalize((float3) lightDirection[0])));
        //float radiusAttenuation = 1.0f - saturate(length(spotPos - input.worldPos.xyz) / 30.0f);
        //float3 lightDir = normalize(spotPos - input.worldPos.xyz);
        //float surfaceRatio = saturate(dot(-lightDir, normalize(spotDir)));
        float coneAttenuation = 1.0f - saturate((innerRatio - surfaceRatio) / (innerRatio - outerRatio));
        float spotfactor = (surfaceRatio > outerRatio) ? 1 : 0;
        finalColor += saturate(dot((float3) lightDir, surfaceNormal)) * spotfactor * float4(1, 1, 1, 1) * radiusAttenuation * coneAttenuation;
        
        float3 reflection = normalize(reflect((float3) -lightDir, surfaceNormal));
        float intensity = pow(saturate(dot(viewDir2, reflection)), specularPower);
        float lightRatio = saturate(dot(surfaceNormal, (float3) lightDir));
        finalColor += (float4(1, 0, 0, 1) * specularIntensity * intensity * lightRatio * spotfactor * radiusAttenuation * coneAttenuation);
    }
    
    if (lightDirection[0].w == -1 && lightPos[0].w == -1)
        return tex;
    finalColor *= tex;
    finalColor.a = tex.a;
    return finalColor;
}