//---------------------------------------------------------------
// Constants
//---------------------------------------------------------------
#define PI 3.14159265359f
#define LIGHT_INTENSITY 7.0f
#define SHININESS 25.0f
static const float3 gLightDirection = { 0.577f, -0.577f, 0.577f };

//---------------------------------------------------------------
// Constant Buffer
//---------------------------------------------------------------
cbuffer MatrixBuffer : register(b0)
{
    float4x4 gWorldViewProj : WorldViewProjection;
    float4x4 gWorld : WORLD;
    float3 gCameraPosition : CAMERA;
    float gPadding;
};

//---------------------------------------------------------------
// Shader Resources
//---------------------------------------------------------------
Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gSpecularMap : SpecularMap;
Texture2D gGlossinessMap : GlossinessMap;

SamplerState samplerPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = WRAP;
    AddressV = WRAP;
};

SamplerState samplerLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = WRAP;
    AddressV = WRAP;
};

SamplerState samplerAnisotropic
{
    Filter = ANISOTROPIC;
    AddressU = WRAP;
    AddressV = WRAP;
    MaxAnisotropy = 16;
};

//---------------------------------------------------------------
// Input/Output structs
//---------------------------------------------------------------
struct VS_INPUT
{
    float3 Position : POSITION;
    float3 Color : COLOR;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};

struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : TEXCOORD4;
    float3 Color : COLOR;
    float2 TexCoord : TEXCOORD0;
    float3 Normal : TEXCOORD1;
    float3 Tangent : TEXCOORD2;
};

//---------------------------------------------------------------
// Helper Functions
//---------------------------------------------------------------
float3 SampleNormalMap(Texture2D normalMap, SamplerState samplerState, float2 uv)
{
    float3 sampledNormal = normalMap.Sample(samplerState, uv).rgb;
    // Convert from [0, 1] to [-1, 1]
    sampledNormal = (2.0f * sampledNormal) - 1.0f;
    return sampledNormal;
}

float3 TransformNormal(float3 sampledNormal, float3 normal, float3 tangent)
{
    // Calculate biNormal
    float3 biNormal = cross(normal, tangent);
    
    // Create TBN matrix and transform normal
    float3 transformedNormal = (sampledNormal.x * tangent) +
                               (sampledNormal.y * biNormal) +
                               (sampledNormal.z * normal);
    
    return normalize(transformedNormal);
}

float3 LambertDiffuse(float kd, float3 cd, float cosAngle)
{
    return (cd * kd / PI) * cosAngle;
}

float3 PhongSpecular(float ks, float shininess, float3 lightDir, float3 viewDir, float3 normal)
{
    float3 reflectedLight = reflect(lightDir, normal);
    float cosAngle = max(dot(reflectedLight, viewDir), 0.0f);
    return ks * pow(cosAngle, shininess) * float3(1.0f, 1.0f, 1.0f);
}

//---------------------------------------------------------------
// Vertex Shader
//---------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Position = mul(float4(input.Position, 1.0f), gWorldViewProj);
    output.WorldPosition = mul(float4(input.Position, 1.0f), gWorld);
    output.Color = input.Color;
    output.TexCoord = input.TexCoord;
    output.Normal = mul(input.Normal, (float3x3) gWorld);
    output.Tangent = mul(input.Tangent, (float3x3) gWorld);
    return output;
}

//---------------------------------------------------------------
// Pixel Shader Helper
//---------------------------------------------------------------
float4 PSShading(VS_OUTPUT input, SamplerState samplerState) : SV_TARGET
{
    // DEBUG: Visualize UV coordinates
    // This will show a rainbow gradient based on UV values
    return float4(input.TexCoord.x, input.TexCoord.y, 0.5f, 1.0f);
}

//---------------------------------------------------------------
// Pixel Shaders for different samplers
//---------------------------------------------------------------
float4 PSPoint(VS_OUTPUT input) : SV_TARGET
{
    return PSShading(input, samplerPoint);
}

float4 PSLinear(VS_OUTPUT input) : SV_TARGET
{
    return PSShading(input, samplerLinear);
}

float4 PSAnisotropic(VS_OUTPUT input) : SV_TARGET
{
    return PSShading(input, samplerAnisotropic);
}

//---------------------------------------------------------------
// Technique
//---------------------------------------------------------------
technique11 PointTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSPoint()));
    }
}

technique11 LinearTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSLinear()));
    }
}

technique11 AnisotropicTechnique
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSAnisotropic()));
    }
}