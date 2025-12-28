//---------------------------------------------------------------
// Constant Buffer
//---------------------------------------------------------------
cbuffer MatrixBuffer : register(b0)
{
    float4x4 gWorldViewProj : WorldViewProjection;
};

//---------------------------------------------------------------
// Shader Resources
//---------------------------------------------------------------
Texture2D gDiffuseMap : DiffuseMap;

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
};

struct VS_OUTPUT
{
	float4 Position : SV_POSITION;
    float3 Color : COLOR;
    float2 TexCoord : TEXCOORD0;
};

//---------------------------------------------------------------
// Vertex Shader
//---------------------------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    output.Position = mul(float4(input.Position, 1.0f), gWorldViewProj);
    output.Color = input.Color;
    output.TexCoord = input.TexCoord;
	return output;
}

//---------------------------------------------------------------
// Pixel Shader
//---------------------------------------------------------------
float4 PSPoint(VS_OUTPUT input) : SV_TARGET
{
    float4 texColor = gDiffuseMap.Sample(samplerPoint, input.TexCoord);
    return texColor;
}

float4 PSLinear(VS_OUTPUT input) : SV_TARGET
{
    float4 texColor = gDiffuseMap.Sample(samplerLinear, input.TexCoord);
    return texColor;
}

float4 PSAnisotropic(VS_OUTPUT input) : SV_TARGET
{
    float4 texColor = gDiffuseMap.Sample(samplerAnisotropic, input.TexCoord);
    return texColor;
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