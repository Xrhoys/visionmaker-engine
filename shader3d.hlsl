struct light_material
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 reflect;
};

struct light_directional
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float3 direction;
};

struct light_point
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    
    float3 position;
    float  range;
    
    float3 att;
};

struct light_Spot
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	
	float3 position;
	float  range;

	float3 direction;
	float  spot;

	float3 att;
};

cbuffer g_cameraWVP
{
	float4x4       gWorld;
	float4x4       gWorldInvTranspose;
	float4x4       gWorldViewProj;
	light_material gMaterial;
};

cbuffer g_perFrame
{
	light_directional gLightDir;
	light_point       gLightPoint;
	light_Spot        gLightSpot;
	float3            gEyePosW;
};

struct VS_INPUT
{
	float3 pos    : POSITION;
	float4 color  : COLOR;
	float3 normal : NORMAL;
	float2 tex    : TEXCOORD;
};

struct VS_OUTPUT
{
	float4 posH    : SV_POSITION;
    float4 color   : COLOR;
    float3 posW    : POSITION;
	float3 normalW : NORMAL;
	float2 tex     : TEXCOORD;
};

void ComputeDirectionalLight(light_material Mat, light_directional Light, 
	float3 Normal, float3 ToEye,
	out float4 Ambient,
	out float4 Diffuse,
	out float4 Spec)
{
	Ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	Diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	Spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float3 LightVec = -Light.direction;

	Ambient = Mat.ambient * Light.ambient;

	float DiffuseFactor = dot(LightVec, Normal);

	[flatten]
	if(DiffuseFactor > 0.0f)
	{
		float3 V = reflect(-LightVec, Normal);
		float SpecFactor = pow(max(dot(V, ToEye), 0.0f), Mat.specular.w);

		Diffuse = DiffuseFactor * Mat.diffuse * Light.diffuse;
		Spec = SpecFactor * Mat.specular * Light.specular;
	}
}

VS_OUTPUT VS(VS_INPUT Input)
{
    VS_OUTPUT Output;

	Output.posH    = mul(float4(Input.pos, 1.0f),  gWorldViewProj);
	Output.posW    = mul(float4(Input.pos, 1.0f), gWorld).xyz;
	Output.normalW = mul(Input.normal, (float3x3)gWorldInvTranspose);
	Output.color   = Input.color;
	Output.tex     = Input.tex;
    
	return Output;
}

sampler Sampler0           : register(s0);
Texture2D<float4> Texture0 : register(t0);

float4 PS(VS_OUTPUT Input) : SV_TARGET
{
	Input.normalW = normalize(Input.normalW);

	float3 ToEyeW = normalize(gEyePosW - Input.posW);
	
	float4 Ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 Diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 Spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 A, D, S;

	ComputeDirectionalLight(gMaterial, gLightDir, Input.normalW, ToEyeW, A, D, S);
	Ambient += A;
	Diffuse += D;
	Spec    += S;

    float4 LitColor = Ambient + Diffuse + Spec;
	LitColor.a == gMaterial.diffuse.a;
	
    return Input.color * LitColor;
}