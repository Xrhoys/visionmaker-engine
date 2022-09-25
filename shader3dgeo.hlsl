#define MAX_LIGHTS 8

#define LIGHT_DIRECTIONAL 0
#define LIGHT_POINT 1
#define LIGHT_SPOT 2

struct light_material
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 reflect;
};

struct light
{
	float4 ambient;
	float4 diffuse;
	float4 specular;
	
	float3 position;
	float  range;

	float3 direction;
	float  spot;

	float3 att;
	int    type;

    bool   enabled;
};

cbuffer g_cbPerObject
{
	float4x4       gWorld;
	float4x4       gWVP;
	light_material gMaterial;
};

cbuffer g_perFrame
{
	light             gLights[MAX_LIGHTS];
	float3            gEyePosW;
};

struct VS_INPUT
{
	float3 pos    : POSITION;
	float3 normal : NORMAL;
};

struct VS_OUTPUT
{
    float4 pos     : SV_POSITION;
    float3 posW    : POSITION;
    float3 normalW : NORMAL;
};

VS_OUTPUT VS(VS_INPUT Input)
{
    VS_OUTPUT Output;

	Output.posW    = mul(float4(Input.pos, 1.0f), gWorld).xyz;
	Output.normalW = mul(Input.normal, (float3x3)gWorld);
	Output.pos     = mul(float4(Output.posW, 1.0f), gWVP);
	
	return Output;
}

void ComputeDirectionalLight(light_material Mat, light Light, 
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

void ComputePointLight(light_material Mat, light Light,
	float3 Pos, float3 Normal, float3 ToEye,
	out float4 Ambient,
	out float4 Diffuse,
	out float4 Spec)
{
	Ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	Diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	Spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	float3 LightVec = Light.position - Pos;

	float D = length(LightVec);

	if(D > Light.range)
		return;

	LightVec /= D;

	Ambient = Mat.ambient * Light.ambient;

	float DiffuseFactor = dot(LightVec, Normal);

	[flatten]
	if(DiffuseFactor > 0.0f)
	{
		float3 V = reflect(-LightVec, Normal);
		float SpecFactor = pow(max(dot(ToEye, V), 0.0f), Mat.specular.w);
		
		Diffuse = DiffuseFactor * Mat.diffuse * Light.diffuse;
		Spec = SpecFactor * Mat.specular * Light.specular;
	}

	float Att = 1.0f / dot(Light.att, float3(1.0f, D, D*D));

	Diffuse *= Att;
	Spec *= Att;
}

void ComputeSpotLight(light_material Mat, light Light,
	float3 Pos, float3 Normal, float3 ToEye,
	out float4 Ambient,
	out float4 Diffuse,
	out float4 Spec)
{
	Ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	Diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	Spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	float3 LightVec = Light.position - Pos;

	float D = length(LightVec);

	if(D > Light.range)
		return;

	LightVec /= D;

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

	float3 Direction = normalize(Light.direction);
	float Spot = pow(max(dot(-LightVec, Direction), 0.0f), Light.spot);

	float Att = Spot / dot(Light.att, float3(1.0f, D, D*D));

	Ambient += Spot;
	Diffuse *= Att;
	Spec *= Att;
}

float4 PS(VS_OUTPUT Input) : SV_TARGET
{
	Input.normalW = normalize(Input.normalW);

	float3 ToEyeW = normalize(gEyePosW - Input.posW);
	
	float4 Ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 Diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 Spec    = float4(0.0f, 0.0f, 0.0f, 0.0f);
    
	float4 A, D, S;
	
	[unroll]
	for(int Index = 0; Index < MAX_LIGHTS; ++Index)
	{
		if(!gLights[Index].enabled) continue;

		switch(gLights[Index].type)
		{
			case LIGHT_DIRECTIONAL:
			{
				ComputeDirectionalLight(gMaterial, gLights[Index], 
					Input.normalW, ToEyeW, A, D, S);
				Ambient += A;
				Diffuse += D;
				Spec    += S;
			} break;

			case LIGHT_POINT:
			{
				ComputePointLight(gMaterial, gLights[Index], 
					Input.posW, Input.normalW, ToEyeW, A, D, S);
				Ambient += A;
				Diffuse += D;
				Spec    += S;	
			} break;

			case LIGHT_SPOT:
			{
				ComputeSpotLight(gMaterial, gLights[Index], 
					Input.posW, Input.normalW, ToEyeW, A, D, S);
				Ambient += A;
				Diffuse += D;
				Spec    += S;
			} break;
		}
	}
	
	float4 LitColor = Ambient + Diffuse + Spec;
	LitColor.a == gMaterial.diffuse.a;
	
    return LitColor;
}