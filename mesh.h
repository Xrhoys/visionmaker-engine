#ifndef MESH_H
#define MESH_H

#define color v4
#define Color V4

// NOTE: Raylib color palette
#define LIGHTGRAY  &Color(200, 200, 200, 255)        // Light Gray
#define GRAY       &Color(130, 130, 130, 255)        // Gray
#define DARKGRAY   &Color( 80,  80,  80, 255)        // Dark Gray
#define YELLOW     &Color(253, 249,   0, 255)        // Yellow
#define GOLD       &Color(255, 203,   0, 255)        // Gold
#define ORANGE     &Color(255, 161,   0, 255)        // Orange
#define PINK       &Color(255, 109, 194, 255)        // Pink
#define RED        &Color(230,  41,  55, 255)        // Red
#define MAROON     &Color(190,  33,  55, 255)        // Maroon
#define GREEN      &Color(  0, 228,  48, 255)        // Green
#define LIME       &Color(  0, 158,  47, 255)        // Lime
#define DARKGREEN  &Color(  0, 117,  44, 255)        // Dark Green
#define SKYBLUE    &Color(102, 191, 255, 255)        // Sky Blue
#define BLUE       &Color(  0, 121, 241, 255)        // Blue
#define DARKBLUE   &Color(  0,  82, 172, 255)        // Dark Blue
#define PURPLE     &Color(200, 122, 255, 255)        // Purple
#define VIOLET     &Color(135,  60, 190, 255)        // Violet
#define DARKPURPLE &Color(112,  31, 126, 255)        // Dark Purple
#define BEIGE      &Color(211, 176, 131, 255)        // Beige
#define BROWN      &Color(127, 106,  79, 255)        // Brown
#define DARKBROWN  &Color( 76,  63,  47, 255)        // Dark Brown
#define WHITE      &Color(255, 255, 255, 255)        // White
#define BLACK      &Color(  0,   0,   0, 255)        // Black
#define BLANK      &Color(  0,   0,   0,   0)        // Transparent
#define MAGENTA    &Color(255,   0, 255, 255)        // Magenta
#define RAYWHITE   &Color(245, 245, 245, 255)        // Ray White

struct vertex_mesh
{
    v3 pos;
    v4 color;
    v3 normal;
    v2 tex;
};

struct vertex_geomesh
{
    v3 pos;
    v3 normal;
};

struct vertex_2d
{
    v3 pos;
    v4 color;
};

struct vertex_font
{
    v3 pos;
    v4 color;
    v2 tex;
};

struct mesh
{
    uint32        verticesSize;
    uint32        indicesSize;
    
    ID3D11Buffer* vertexBuffer;
    ID3D11Buffer* indexBuffer;
};

inline vertex_mesh
VertexMesh(real32 px, real32 py, real32 pz,
           real32 tu, real32 tv,
           real32 nx, real32 ny, real32 nz,
           color Color)
{
    vertex_mesh Mesh;
    Mesh.pos    = V3(px, py, pz);
    Mesh.tex    = V2(tu, tv);
    Mesh.normal = V3(nx, ny, nz);
    Mesh.color  = Color;
    
    return Mesh;
}

inline vertex_mesh
VertexMesh(v3 Pos,
           real32 tu, real32 tv,
           color Color,
           real32 nx, real32 ny, real32 nz)
{
    vertex_mesh Mesh;
    Mesh.pos    = Pos;
    Mesh.tex    = V2(tu, tv);
    Mesh.normal = V3(nx, ny, nz);
    Mesh.color  = Color;
    
    return Mesh;
}

inline vertex_geomesh
VertexGeomesh(real32 px, real32 py, real32 pz,
              real32 nx, real32 ny, real32 nz)
{
    vertex_geomesh Mesh;
    Mesh.pos    = V3(px, py, pz);
    Mesh.normal = V3(nx, ny, nz);
    
    return Mesh;
}

inline vertex_geomesh
VertexGeomesh(v3 Pos,
              real32 nx, real32 ny, real32 nz)
{
    vertex_geomesh Mesh;
    Mesh.pos    = Pos;
    Mesh.normal = V3(nx, ny, nz);
    
    return Mesh;
}

inline vertex_2d
Vertex2d(real32 px, real32 py, real32 pz,
         color *Color)
{
    vertex_2d Vertex;
    
    Vertex.pos   = V3(px, py, pz);
    Vertex.color = *Color;
    
    return Vertex;
}

inline vertex_font
VertexFont(real32 px, real32 py, real32 pz,
           color Color,
           real32 u, real32 v)
{
    vertex_font Vertex;
    
    Vertex.pos = V3(px, py, pz);
    Vertex.color = Color;
    Vertex.tex = V2(u, v);
    
    return Vertex;
};

inline color
GetFormattedColor(color *Color)
{
    color FormattedColor;
    FormattedColor.X = Color->X * 0.0039215863f;
    FormattedColor.Y = Color->Y * 0.0039215863f;
    FormattedColor.Z = Color->Z * 0.0039215863f;
    FormattedColor.W = Color->W * 0.0039215863f;
    
    return FormattedColor;
}

#endif //MESH_H
