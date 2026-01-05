float4x4 gWorldViewProj : WorldViewProjection;
float4x4 gWorldMatrix : WORLD;

Texture2D gDiffuseMap : DiffuseMap;
Texture2D gNormalMap : NormalMap;
Texture2D gGlossinessMap : GlossinessMap;
Texture2D gSpecularMap : SpecularMap;

float3 gLightDir : LightDirection;
float3 gCameraPosition : CAMERA;

float gLightIntensity = 7.f;
float3 gAmbient = (0.025f, 0.025f, 0.025f);
float gPI = 3.14159265358979323846f;
float gShininess = 25.f;
bool gUseNormalMap;

SamplerState samPoint
{
    Filter = MIN_MAG_MIP_POINT;
    AddressU = Wrap;
    AddressV = Wrap;
};
SamplerState samLinear
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};
SamplerState samAnisotropic
{
    Filter = ANISOTROPIC;
    AddressU = Wrap;
    AddressV = Wrap;
};
//------------------------------------------
// Input/Output Structs
//------------------------------------------
struct VS_INPUT
{
    float3 Position : POSITION;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};
struct VS_OUTPUT
{
    float4 Position : SV_POSITION;
    float4 WorldPosition : TEXCOORD1;
    float2 Uv : TEXCOORD0;
    float3 Normal : NORMAL;
    float3 Tangent : TANGENT;
};
//---------------------------------------------
//RGB Functions
//---------------------------------------------
float3 CalculateRadiance(float intensity, float3 ambient)
{
    return intensity * ambient;
}
float4 CalculateLambert(float kd, float4 cd)
{
    return cd * kd / gPI;
}
float4 CalculatePhong(float4 ks, float exp, float3 l, float3 v, float3 n)
{
    const float3 reflect = normalize( l - 2 * dot(n, l) * n);
    const float cosAlfa = saturate(dot(reflect, v));
    float phong = 0;
    if (cosAlfa > 0)
    {
        phong = ks * pow(cosAlfa, exp);
    }

    return phong;
}
float4 RGB(VS_OUTPUT input, SamplerState sampleState)
{
    float cosArea = 0;
    if (gUseNormalMap)
	{
        const float3 biNormal = cross(input.Normal, input.Tangent);
        const float4x4 tangentSpaceAxis = float4x4(float4(input.Tangent, 0.f), float4(biNormal, 0.f), float4(input.Normal, 0.f), float4(0.f, 0.f, 0.f, 1.f));        
        const float3 normalVector = 2.f * gNormalMap.Sample(sampleState, input.Uv).rgb - float3(1.f, 1.f, 1.f);
        const float4 normalValue = mul(float4(normalVector, 0.f), tangentSpaceAxis);
        cosArea = saturate(dot(float4(-gLightDir, 0.f), normalValue));
    }
	else
	{
        float3 normalValue = input.Normal;
		cosArea = dot(-gLightDir, input.Normal);
	}

    float3 radiance = CalculateRadiance(gLightIntensity, gAmbient);
    float3 inViewDirection = normalize(gCameraPosition - input.WorldPosition.xyz);
    float4 lambert = CalculateLambert(gLightIntensity, float4(gDiffuseMap.Sample(sampleState, input.Uv).rgb, 0.f));
    float specularExp = gShininess * gGlossinessMap.Sample(sampleState, input.Uv).r;
    float4 specular = CalculatePhong(float4(gSpecularMap.Sample(sampleState, input.Uv).rgb, 0.f), specularExp, gLightDir, inViewDirection, input.Normal);
    
    return saturate(saturate(lambert + specular + float4(gAmbient, 0.f)) * cosArea);

}
//----------------------------------------------
// Vertex Shader
//----------------------------------------------
VS_OUTPUT VS(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.WorldPosition = mul(float4(input.Position, 1.f), (float4x4) gWorldMatrix);
    output.Position = mul(float4(input.Position, 1.f), gWorldViewProj);
    output.Uv = input.Uv;
    output.Tangent = mul(normalize(input.Tangent), (float3x3) gWorldMatrix);
    output.Normal = mul(normalize(input.Normal), (float3x3) gWorldMatrix);
    return output;
}
//----------------------------------------------
// Pixel Shader
//----------------------------------------------
float4 PS(VS_OUTPUT input) : SV_TARGET
{ 
    return RGB(input, samPoint);
}
float4 PS_Linear(VS_OUTPUT input) : SV_TARGET
{   
    return RGB(input, samLinear);
}
float4 PS_Anisotropic(VS_OUTPUT input) : SV_TARGET
{   
    return RGB(input, samAnisotropic);
}
BlendState gBlendState
{
    BlendEnable[0] = false;
    SrcBlend = src_alpha;
    DestBlend = inv_src_alpha;
    BlendOp = add;
    SrcBlendAlpha = zero;
    DestBlendAlpha = zero;
    BlendOpAlpha = add;
    RenderTargetWriteMask[0] = 0x0F;
};
DepthStencilState gDepthstencilState
{
    DepthEnable = true;
    DepthWriteMask = 1;
    DepthFunc = less;
    StencilEnable = false;
};
//----------------------------------------------
// Technique
//----------------------------------------------
//Skip set rasterizerstate to ensure culling gets done correctly
technique11 DefaultPoint
{
    pass P0
    {
        SetDepthStencilState(gDepthstencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
}
technique11 DefaultLinear
{
    pass P0
    {
        SetDepthStencilState(gDepthstencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Linear()));
    }
}
technique11 DefaultAnisotropic
{
    pass P0
    {
        SetDepthStencilState(gDepthstencilState, 0);
        SetBlendState(gBlendState, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xFFFFFFFF);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, PS_Anisotropic()));
    }
}

