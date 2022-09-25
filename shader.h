/* date = July 23rd 2022 6:31 pm */

#ifndef SHADER_H
#define SHADER_H

struct d3d_shader
{
    ID3D11VertexShader* vs;
    ID3D11PixelShader*  ps;
    ID3D11InputLayout*  layout;
    uint32              layoutSize;
};

struct d3d_c_buffer_object
{
    Matrix         g_world;
    Matrix         g_wvp;
    light_material g_material;
};

struct d3d_c_buffer_object_geomesh
{
    Matrix         g_world;
    Matrix         g_wvp;
    light_material g_material;
};

struct d3d_c_buffer_frame
{
    light             g_Lights[8];
	v3                g_eyePosW;
    uint32            pad;
};

global d3d_c_buffer_object         g_cbPerObject;
global ID3D11Buffer*               g_objectCBuffer;
global d3d_c_buffer_frame          g_cbPerFrame;
global ID3D11Buffer*               g_frameCBuffer;
global d3d_c_buffer_object_geomesh g_cbPerObjectGeo;
global ID3D11Buffer*               g_objectGeoCBuffer;

#endif //SHADER_H
