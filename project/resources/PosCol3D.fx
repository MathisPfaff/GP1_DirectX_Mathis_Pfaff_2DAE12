//---------------------------------------------------------------
// Constants
//---------------------------------------------------------------
#define PI 3.14159265359f
#define LIGHT_INTENSITY 7.0f
#define SHININESS 25.0f
static const float3 gLightDirection = { -0.577f, 0.577f, -0.577f };

//---------------------------------------------------------------
// Rasterizer States
//---------------------------------------------------------------
RasterizerState gRasterizerStateNoCull
{
    CullMode = none;
    FrontCounterClockwise = false;
};

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

float3 LambertDiffuse(float kd, float3 cd, float nDotL)
{
    return (cd * kd / PI) * nDotL;
}

float3 PhongSpecular(float ks, float exp, float3 l, float3 v, float3 n)
{
    // Calculate reflection vector using the same formula as BRDFs.h
    // r = -l - 2 * dot(-l, n) * n
    float3 r = -l - 2.0f * dot(-l, n) * n;
    float cosAngle = max(dot(r, v), 0.0f);
    return ks * pow(cosAngle, exp) * float3(1.0f, 1.0f, 1.0f);
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
    // Sample diffuse map (required)
    float4 diffuseColor = gDiffuseMap.Sample(samplerState, input.TexCoord);
    
    // Sample normal map if available, otherwise use vertex normal
    float3 sampledNormal = float3(0.0f, 0.0f, 1.0f);
    float3 normal = normalize(input.Normal);
    float3 tangent = normalize(input.Tangent);
    
    // Check if we have a normal map by sampling it
    float3 normalMapSample = gNormalMap.Sample(samplerState, input.TexCoord).rgb;
    if (any(normalMapSample))
    {
        sampledNormal = SampleNormalMap(gNormalMap, samplerState, input.TexCoord);
        normal = TransformNormal(sampledNormal, normal, tangent);
    }
    
    // Sample specular map if available, otherwise use default
    float specularStrength = gSpecularMap.Sample(samplerState, input.TexCoord).r;
    
    // Sample glossiness map if available, otherwise use default
    float glossiness = gGlossinessMap.Sample(samplerState, input.TexCoord).r;
    if (glossiness == 0.0f)
    {
        glossiness = 1.0f;
    }
    
    // Light direction (pointing FROM surface TO light) - this is in world space
    float3 l = normalize(gLightDirection);
    
    // View direction (FROM surface TO camera) - in world space
    float3 v = normalize(gCameraPosition - input.WorldPosition.xyz);
    
    // Calculate cosine of angle between normal and light direction
    float nDotL = max(dot(normal, l), 0.0f);
    
    // Calculate diffuse
    float3 diffuse = LambertDiffuse(LIGHT_INTENSITY, diffuseColor.rgb, nDotL);
    
    // Calculate specular
    float3 specular = PhongSpecular(specularStrength, glossiness * SHININESS, l, v, normal);
    
    // Combine diffuse and specular
    float3 finalColor = diffuse + specular;
    
    // Clamp color to valid range [0, 1]
    finalColor = saturate(finalColor);
    
    // Return with alpha from diffuse map for transparency support
    return float4(finalColor, diffuseColor.a);
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
// Fire FX Pixel Shaders (No Lighting - Just UV Sampling)
//---------------------------------------------------------------
float4 PSFirePoint(VS_OUTPUT input) : SV_TARGET
{
    // Sample diffuse map based on UV coordinates only (no lighting calculations)
    return gDiffuseMap.Sample(samplerPoint, input.TexCoord);
}

float4 PSFireLinear(VS_OUTPUT input) : SV_TARGET
{
    // Sample diffuse map based on UV coordinates only (no lighting calculations)
    return gDiffuseMap.Sample(samplerLinear, input.TexCoord);
}

float4 PSFireAnisotropic(VS_OUTPUT input) : SV_TARGET
{
    // Sample diffuse map based on UV coordinates only (no lighting calculations)
    return gDiffuseMap.Sample(samplerAnisotropic, input.TexCoord);
}

//---------------------------------------------------------------
// Techniques
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

technique11 FirePointTechnique
{
    pass P0
    {
        SetRasterizerState(gRasterizerStateNoCull);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSFirePoint()));
    }
}

technique11 FireLinearTechnique
{
    pass P0
    {
        SetRasterizerState(gRasterizerStateNoCull);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSFireLinear()));
    }
}

technique11 FireAnisotropicTechnique
{
    pass P0
    {
        SetRasterizerState(gRasterizerStateNoCull);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PSFireAnisotropic()));
    }
}