cbuffer g_cameraWVP
{
	float4x4 gWorld;
	float4x4 gWorldViewProj;
};

struct VS_INPUT
{
	float3 pos: POSITION;
	float4 color: COLOR;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

VS_OUTPUT VS(VS_INPUT Input)
{
    VS_OUTPUT Output;

    Output.pos   = mul(float4(Input.pos, 1.0f), gWorldViewProj);
    Output.color = Input.color;

    return Output;
}

float4 PS(VS_OUTPUT Input) : SV_TARGET
{
    return Input.color;
}