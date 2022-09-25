cbuffer g_cameraWVP
{
	float4x4 gWorld;
	float4x4 gWVP;
};

struct VS_INPUT
{
	float3 pos   : POSITION;
	float4 color : COLOR;
	float2 tex   : TEXCOORD;
};

struct VS_OUTPUT
{
    float4 pos   : SV_POSITION;
    float4 color : COLOR;
	float2 tex   : TEXCOORD;
};

VS_OUTPUT VS(VS_INPUT Input)
{
    VS_OUTPUT Output;

    Output.pos   = mul(float4(Input.pos, 1.0f), gWVP);
    Output.color = Input.color;
	Output.tex   = Input.tex;

    return Output;
}

sampler Sampler0           : register(s0);
Texture2D<float4> Texture0 : register(t0);

float4 PS(VS_OUTPUT Input) : SV_TARGET
{
	float4 Texel = Texture0.Sample(Sampler0, Input.tex);
    return Texel * Input.color;
}