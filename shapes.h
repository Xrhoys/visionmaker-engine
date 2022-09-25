#ifndef SHAPES_H
#define SHAPES_H

global mesh m_Rectangle;
global mesh m_Triangle;
global mesh m_Circle;

global mesh m_Cube;
global mesh m_CubeBounding;

global mesh m_Sphere;
global mesh m_SphereBounding;

global mesh m_Grid;

internal void
InitShapes()
{
    // NOTE: Rectangle
    {
        vertex_2d Vertices[] = 
        {
            Vertex2d(1.0f, 0.0f, 0.5f, RED),
            Vertex2d(1.0f, 1.0f, 0.5f, RED),
            Vertex2d(0.0f, 1.0f, 0.5f, RED),
            Vertex2d(0.0f, 0.0f, 0.5f, RED),
        };
        
        uint32 Indices[6] =
        {
            0, 1, 2,
            0, 2, 3,
        };
        
        D3DLoadMesh(&m_Rectangle, Vertices, 4, (uint32 *)Indices, 6, TRUE);
    }
    
    // NOTE: Point
    // NOTE: Triangle
    // NOTE: Line
    // NOTE: Elipses (Circle)
}

inline void
DrawRectangle(uint32 PosX, uint32 PosY, 
              uint32 Width, uint32 Height,
              color *Color)
{
    real32 DrawX = -SCREEN_WIDTH / 2.0f + PosX;
    real32 DrawY = SCREEN_HEIGHT / 2.0f - PosY;
    
    color FormattedColor = GetFormattedColor(Color);
    
    D3D11_MAPPED_SUBRESOURCE VerticesMappedResource;
    g_pd3dDeviceContext->Map(m_Rectangle.vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0,
                             &VerticesMappedResource);
    
    vertex_2d Vertices[] =
    {
        Vertex2d(        DrawX,  DrawY - Height, 0.5f, &FormattedColor),
        Vertex2d(        DrawX,           DrawY, 0.5f, &FormattedColor),
        Vertex2d(DrawX + Width,           DrawY, 0.5f, &FormattedColor),
        Vertex2d(DrawX + Width,  DrawY - Height, 0.5f, &FormattedColor),
    };
    
    vertex_2d *Ptr = (vertex_2d *)VerticesMappedResource.pData;
    for (uint32 Index = 0;
         Index < 4;
         ++Index)
    {
        Ptr[Index] = Vertices[Index];
    }
    
    g_pd3dDeviceContext->Unmap(m_Rectangle.vertexBuffer, 0);
    
    uint32 stride = sizeof(vertex_2d);
    uint32 offset = 0;
    g_pd3dDeviceContext->IASetIndexBuffer(m_Rectangle.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    g_pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_Rectangle.vertexBuffer, &stride, &offset);
    g_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    //g_pd3dDeviceContext->OMSetDepthStencilState(g_depthStencilStateDisabled, 1);
    
    // g_pd3dDeviceContext->RSSetState(g_CCWcullMode);
    g_pd3dDeviceContext->DrawIndexed(6, 0, 0);
    // g_pd3dDeviceContext->RSSetState(g_CCWcullMode);
    // g_pd3dDeviceContext->DrawIndexed(6, 0, 0);
}

// NOTE: temporary, should be model.h instead
internal void
InitModels()
{
    // NOTE: Cube
    {
        vertex_geomesh Vertices[] = 
        {
            // TOP
            VertexGeomesh(-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f),
            VertexGeomesh(0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f),
            VertexGeomesh(-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f),
            VertexGeomesh(0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f),
            // BOTTOM
            VertexGeomesh(-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f),
            VertexGeomesh(0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f),
            VertexGeomesh(-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f),
            VertexGeomesh(0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f),
            // LEFT
            VertexGeomesh(-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f),
            VertexGeomesh(-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f),
            VertexGeomesh(-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f),
            VertexGeomesh(-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f),
            // RIGHT
            VertexGeomesh(0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f),
            VertexGeomesh(0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f),
            VertexGeomesh(0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f),
            VertexGeomesh(0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f),
            // FRONT
            VertexGeomesh(-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f),
            VertexGeomesh(0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f),
            VertexGeomesh(-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f),
            VertexGeomesh(0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f),
            // BACK
            VertexGeomesh(-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f),
            VertexGeomesh(0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f),
            VertexGeomesh(-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f),
            VertexGeomesh(0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f),
        };
        
        uint32 Indices[] = 
        {
            //Top
            0, 1, 3,
            0, 3, 2,
            
            //Bottom
            4, 5, 7,
            4, 7, 6,
            
            //Left
            8, 9, 11,
            8, 11, 10,
            
            //Right
            12, 13, 15,
            12, 15, 14,
            
            //Front
            16, 17, 19,
            16, 19, 18,
            
            //Back
            20, 21, 23,
            20, 23, 22,
        };
        
        D3DLoadMesh(&m_Cube, (vertex_geomesh *)Vertices, 24, (uint32 *)Indices, 36, TRUE);
        
        uint32 BoundingIndices[] =
        {
            0, 1, 2, 3, 0 ,2, 1, 3,
            4, 5, 6, 7, 4, 6, 5, 7,
            0, 4, 1, 5, 2, 6, 3, 7,
        };
        
        D3DLoadMesh(&m_CubeBounding, (vertex_geomesh *)Vertices, 8, (uint32 *)BoundingIndices, 24, TRUE);
    }
}

// NOTE: Static grid generation
inline mesh
InitGrid(real32 Width, real32 Depth, uint32 Resolution)
{
    mesh Mesh;
    
    uint32 m = Width / Resolution;
    uint32 n = Depth / Resolution;
    
    uint32 verticesCount = m * n;
    uint32 faceCount = (m - 1)*(n - 1)*2;
    
    real32 halfWidth = 0.5f * Width;
    real32 halfDepth = 0.5f * Depth;
    
    real32 dx = Width / (n - 1);
    real32 dz = Depth / (m - 1);
    
    real32 du = 1.0f / (n - 1);
    real32 dv = 1.0f / (m - 1);
    
    // Compute vertices
    Mesh.verticesSize = verticesCount;
    segment *VerticesSeg = malloc();
    vertex_geomesh *Vertices = (vertex_geomesh *)VerticesSeg->Data;
    
    for (int X = 0; 
         X < n; 
         ++X)
    {
        real32 z = halfDepth - X*dz;
        for (int Y = 0; 
             Y < m; 
             ++Y)
        {
            real32 x = -halfWidth + Y*dx;
            // real32 y = 0.3f*(z*sin(0.1f * x) + x*cos(0.1f * z));
            Vertices[X*n + Y] = VertexGeomesh(x, 0.0f, z, 0.0f, 1.0f, 0.0f);
        }
    }
    
    // Compute indices
    Mesh.indicesSize = faceCount*3;
    segment *IndicesSeg = malloc();
    uint32 *Indices = (uint32 *)IndicesSeg->Data;
    for(int i = 0;
        i < n - 1;
        ++i)
    {
        for(int j = 0;
            j < m - 1;
            ++j)
        {
            *Indices++ = i*n + j;
            *Indices++ = i*n + j + 1;
            *Indices++ = (i + 1)*n + j;
            
            *Indices++ = (i + 1)*n + j;
            *Indices++ = i*n + j + 1;
            *Indices++ = (i + 1)*n + j + 1;
        }
    }
    
    D3DLoadMesh(&Mesh, (vertex_geomesh *)Vertices, Mesh.verticesSize, (uint32 *)IndicesSeg->Data, Mesh.indicesSize);
    
    free(VerticesSeg);
    free(IndicesSeg);
    
    return Mesh;
}

// NOTE: https://www.songho.ca/opengl/gl_sphere.html
internal mesh
InitSphere(uint32 StackCount, uint32 SectorCount, real32 Radius)
{
    mesh Mesh = {};
    
    segment *VerticesSeg = malloc();
    segment *IndicesSeg  = malloc();
    segment *LineSeg     = malloc();
    
    vertex_geomesh *Vertices = (vertex_geomesh *)VerticesSeg->Data;
    uint32 *Indices          = (uint32 *)IndicesSeg->Data;
    uint32 *LineIndices      = (uint32 *)LineSeg->Data;
    
    real32 LengthInv  = 1.0f/Radius;
    real32 SectorStep = 2*Pi / SectorCount;
    real32 StackStep  = 2*Pi / StackCount;
    
    for(uint32 StackIndex = 0;
        StackIndex < StackCount;
        ++StackIndex)
    {
        real32 StackAngle = Pi/2 - StackIndex*StackStep;
        real32 Xy = Radius * cos(StackAngle);
        real32 Z  = Radius * sin(StackAngle);
        
        for(uint32 SectorIndex = 0;
            SectorIndex < SectorCount;
            ++SectorIndex)
        {
            real32 SectorAngle = SectorIndex * SectorStep;
            real32 X = Xy * cos(SectorAngle);
            real32 Y = Xy * sin(SectorAngle);
            
            real32 Nx = X*LengthInv;
            real32 Ny = Y*LengthInv;
            real32 Nz = Z*LengthInv;
            
            *Vertices++ = VertexGeomesh(X, Y, Z, Nx, Ny, Nz);
        }
    }
    
    for(uint32 StackIndex = 0;
        StackIndex < StackCount;
        ++StackIndex)
    {
        uint32 K1 = StackIndex * (SectorCount);
        uint32 K2 = K1 + SectorCount;
        
        for(uint32 SectorIndex = 0;
            SectorIndex < SectorCount;
            ++SectorIndex, ++K1, ++K2)
        {
            if(StackIndex != 0)
            {
                *Indices++ = K1;
                *Indices++ = K2;
                *Indices++ = K1 + 1;
            }
            
            if(StackIndex != (StackCount - 1))
            {
                *Indices++ = K1 + 1;
                *Indices++ = K2;
                *Indices++ = K2 + 1;
            }
            
            *LineIndices++ = K1;
            *LineIndices++ = K2;
            if(StackIndex != 0)
            {
                *LineIndices++ = K1;
                *LineIndices++ = K1 + 1;
            }
        }
    }
    
    Mesh.verticesSize             = StackCount * SectorCount;
    Mesh.indicesSize              = StackCount * SectorCount * 3;
    m_SphereBounding.verticesSize = m_Sphere.verticesSize;
    m_SphereBounding.indicesSize  = StackCount * SectorCount * 2;
    
    D3DLoadMesh(&Mesh, (vertex_geomesh *)VerticesSeg->Data, Mesh.verticesSize, (uint32 *)IndicesSeg->Data, Mesh.indicesSize);
    
    // D3DLoadMesh(&m_SphereBounding, (vertex_geomesh *)VerticesSeg->Data, m_SphereBounding.verticesSize, (uint32 *)LineSeg->Data, m_SphereBounding.indicesSize);
    
    free(VerticesSeg); 
    free(IndicesSeg);
    free(LineSeg);
    
    return Mesh;
}

inline mesh
InitCube()
{
    mesh Mesh;
    vertex_geomesh Vertices[] = 
    {
        // TOP
        VertexGeomesh(-0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f),
        VertexGeomesh(0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f),
        VertexGeomesh(-0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f),
        VertexGeomesh(0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f),
        // BOTTOM
        VertexGeomesh(-0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f),
        VertexGeomesh(0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f),
        VertexGeomesh(-0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f),
        VertexGeomesh(0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f),
        // LEFT
        VertexGeomesh(-0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f),
        VertexGeomesh(-0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f),
        VertexGeomesh(-0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f),
        VertexGeomesh(-0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f),
        // RIGHT
        VertexGeomesh(0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f),
        VertexGeomesh(0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f),
        VertexGeomesh(0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f),
        VertexGeomesh(0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f),
        // FRONT
        VertexGeomesh(-0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f),
        VertexGeomesh(0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f),
        VertexGeomesh(-0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f),
        VertexGeomesh(0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f),
        // BACK
        VertexGeomesh(-0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f),
        VertexGeomesh(0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f),
        VertexGeomesh(-0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f),
        VertexGeomesh(0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f),
    };
    
    uint32 Indices[] = 
    {
        //Top
        0, 1, 3,
        0, 3, 2,
        
        //Bottom
        4, 5, 7,
        4, 7, 6,
        
        //Left
        8, 9, 11,
        8, 11, 10,
        
        //Right
        12, 13, 15,
        12, 15, 14,
        
        //Front
        16, 17, 19,
        16, 19, 18,
        
        //Back
        20, 21, 23,
        20, 23, 22,
    };
    
    D3DLoadMesh(&Mesh, (vertex_geomesh *)Vertices, 24, (uint32 *)Indices, 36);
    
    return Mesh;
}

inline void
DrawSphere(mesh *Mesh, v3 *Position, real32 Size, color *Face, color *Edge)
{
    g_pd3dDeviceContext->IASetInputLayout(g_shader3d_geomesh.layout);
    g_pd3dDeviceContext->VSSetShader(g_shader3d_geomesh.vs, 0, 0);
    g_pd3dDeviceContext->PSSetShader(g_shader3d_geomesh.ps, 0, 0);
    g_pd3dDeviceContext->PSSetConstantBuffers(0, 1, &g_objectGeoCBuffer);
    g_pd3dDeviceContext->VSSetConstantBuffers(0, 1, &g_objectGeoCBuffer);
    
    Matrix WorldMatrix = MatrixIdentity();
    Scaling(&WorldMatrix, Size, Size, Size);
    Translation(&WorldMatrix, Position->X, Position->Y, Position->Z);
    
    g_cbPerObjectGeo.g_world    = WorldMatrix;
    
    g_pd3dDeviceContext->UpdateSubresource(g_objectGeoCBuffer, 0, NULL, &g_cbPerObjectGeo, 0, 0);
    
    DrawGeomeshT(Mesh);
    
    // g_cbPerObjectGeo.g_color = *Edge;
    // g_pd3dDeviceContext->UpdateSubresource(g_objectGeoCBuffer, 0, NULL, &g_cbPerObjectGeo, 0, 0);
    // DrawGeomeshL(&m_SphereBounding);
}

inline void
DrawCube(real32 PosX, real32 PosY, real32 PosZ, real32 Size, color *Face, color *Edge)
{
    // color ColorCube = GetFormattedColor(Face);
    // color ColorSegment = GetFormattedColor(Segment);
    
    g_pd3dDeviceContext->IASetInputLayout(g_shader3d_geomesh.layout);
    g_pd3dDeviceContext->VSSetShader(g_shader3d_geomesh.vs, 0, 0);
    g_pd3dDeviceContext->PSSetShader(g_shader3d_geomesh.ps, 0, 0);
    g_pd3dDeviceContext->PSSetConstantBuffers(0, 1, &g_objectGeoCBuffer);
    g_pd3dDeviceContext->VSSetConstantBuffers(0, 1, &g_objectGeoCBuffer);
    
    // TODO: merge into one single matrix transform call
    Matrix WorldMatrix = MatrixIdentity();
    Scaling(&WorldMatrix, Size, Size, Size);
    Translation(&WorldMatrix, PosX, PosY, PosZ);
    
    g_cbPerObjectGeo.g_world    = WorldMatrix;
    g_cbPerObjectGeo.g_material =
    {
        V4(0.48f, 0.77f, 0.66f,  1.0f),
        V4(0.48f, 0.77f, 0.46f,  1.0f),
        V4( 0.2f,  0.2f,  0.2f, 16.0f),
        V4( 0.2f,  0.2f,  0.2f, 16.0f),
    };
    
    g_pd3dDeviceContext->UpdateSubresource(g_objectGeoCBuffer, 0, NULL, &g_cbPerObjectGeo, 0, 0);
    
    uint32 stride = sizeof(vertex_geomesh);
    uint32 offset = 0;
    
    g_pd3dDeviceContext->IASetIndexBuffer(m_Cube.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    g_pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_Cube.vertexBuffer, &stride, &offset);
    g_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_pd3dDeviceContext->OMSetDepthStencilState(g_depthStencilState, 1);
    
    g_pd3dDeviceContext->RSSetState(g_CCWcullMode);
    g_pd3dDeviceContext->DrawIndexed(36, 0, 0);
    g_pd3dDeviceContext->RSSetState(g_CWcullMode);
    g_pd3dDeviceContext->DrawIndexed(36, 0, 0);
    
    g_pd3dDeviceContext->UpdateSubresource(g_objectGeoCBuffer, 0, NULL, &g_cbPerObjectGeo, 0, 0);
    
    g_pd3dDeviceContext->IASetIndexBuffer(m_CubeBounding.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    g_pd3dDeviceContext->IASetVertexBuffers(0, 1, &m_CubeBounding.vertexBuffer, &stride, &offset);
    g_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    
    g_pd3dDeviceContext->DrawIndexed(24, 0, 0);
}

inline void
DrawGrid(mesh *Mesh, color *Color)
{
    g_pd3dDeviceContext->IASetInputLayout(g_shader3d_geomesh.layout);
    g_pd3dDeviceContext->VSSetShader(g_shader3d_geomesh.vs, 0, 0);
    g_pd3dDeviceContext->PSSetShader(g_shader3d_geomesh.ps, 0, 0);
    g_pd3dDeviceContext->PSSetConstantBuffers(0, 1, &g_objectGeoCBuffer);
    g_pd3dDeviceContext->VSSetConstantBuffers(0, 1, &g_objectGeoCBuffer);
    
    // TODO: merge into one single matrix transform call
    Matrix WorldMatrix = MatrixIdentity();
    
    g_cbPerObjectGeo.g_world    = WorldMatrix;
    
    g_pd3dDeviceContext->UpdateSubresource(g_objectGeoCBuffer, 0, NULL, &g_cbPerObjectGeo, 0, 0);
    
    uint32 stride = sizeof(vertex_geomesh);
    uint32 offset = 0;
    g_pd3dDeviceContext->IASetIndexBuffer(Mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    g_pd3dDeviceContext->IASetVertexBuffers(0, 1, &Mesh->vertexBuffer, &stride, &offset);
    g_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    g_pd3dDeviceContext->OMSetDepthStencilState(g_depthStencilState, 1);
    
    g_pd3dDeviceContext->RSSetState(g_CCWcullMode);
    g_pd3dDeviceContext->DrawIndexed(Mesh->indicesSize, 0, 0);
    g_pd3dDeviceContext->RSSetState(g_CWcullMode);
    g_pd3dDeviceContext->DrawIndexed(Mesh->indicesSize, 0, 0);
}

#endif //SHAPES_H
