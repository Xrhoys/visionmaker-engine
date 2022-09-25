/* date = July 21st 2022 10:10 pm */

#ifndef COLLISION_H
#define COLLISION_H

struct boundingAABB_axis
{
    v3 center;
    v3 stride;
};

struct boundingAABB_oriented
{
    v3 center;
    v3 stride;
    v4 direction;
};

struct frustum
{
    v4 right;
    v4 left;
    v4 top;
    v4 bottom;
    v4 nearp; // NOTE: near/far plan equation, apparently it's reserved keyword ???
    v4 farp;
};

struct ray
{
    v3 origin;
    v3 direction;
};

internal void
c_GenerateFrustum(frustum *Frustum, Matrix *Proj)
{
    // TODO: vectorize this
    v4 M1 = V4(Proj->E[0][0], Proj->E[1][0], Proj->E[2][0], Proj->E[3][0]);
    v4 M2 = V4(Proj->E[0][1], Proj->E[1][1], Proj->E[2][1], Proj->E[3][1]);
    v4 M3 = V4(Proj->E[0][2], Proj->E[1][2], Proj->E[2][2], Proj->E[3][2]);
    v4 M4 = V4(Proj->E[0][3], Proj->E[1][3], Proj->E[2][3], Proj->E[3][3]);
    
    Frustum->right   = M1 + M4;
    Frustum->left    = M4 - M1;
    Frustum->top     = M4 - M2;
    Frustum->bottom  = M2 + M4;
    Frustum->nearp   = M3;
    Frustum->farp    = M4 - M3;
};

internal boundingAABB_axis
c_ComputeBoundingAABB(vertex_geomesh *Vertices, uint32 VerticesCount)
{
    v3 VMin = V3( INFINITY,  INFINITY,  INFINITY);
    v3 VMax = V3(-INFINITY, -INFINITY, -INFINITY);
    
    for(int Index = 1;
        Index < VerticesCount;
        ++Index)
    {
        v3 Pos = Vertices[Index].pos;
        
        // VMin
        if(Pos.X < VMin.X)
        {
            VMin.X = Pos.X;
        }
        
        if(Pos.Y < VMin.Y)
        {
            VMin.Y = Pos.Y;
        }
        
        if(Pos.Z < VMin.Z)
        {
            VMin.Z = Pos.Z;
        }
        
        // VMax
        if(Pos.X > VMax.X)
        {
            VMax.X = Pos.X;
        }
        
        if(Pos.Y > VMax.Y)
        {
            VMax.Y = Pos.Y;
        }
        
        if(Pos.Z > VMax.Z)
        {
            VMax.Z = Pos.Z;
        }
    }
    
    boundingAABB_axis Result = { (VMin + VMax) * 0.5f , (VMax - VMin) * 0.5 };
    return Result;
}

internal mesh
c_GenerateAxisBoundingMesh(boundingAABB_axis *Box, color Color)
{
    mesh Mesh = {};
    color FormattedColor = GetFormattedColor(&Color);
    
    real32 dX = Box->stride.X;
    real32 dY = Box->stride.Y;
    real32 dZ = Box->stride.Z;
    
    v3 Center = Box->center;
    
    vertex_geomesh Vertices[] =
    {
        VertexGeomesh(Center + V3(-dX, -dY, -dZ), 0.0f, 0.0f, 0.0f),
        VertexGeomesh(Center + V3(dX, -dY, -dZ), 0.0f, 0.0f, 0.0f),
        VertexGeomesh(Center + V3(dX, -dY, dZ), 0.0f, 0.0f, 0.0f),
        VertexGeomesh(Center + V3(-dX, -dY, dZ), 0.0f, 0.0f, 0.0f),
        VertexGeomesh(Center + V3(-dX, dY, -dZ), 0.0f, 0.0f, 0.0f),
        VertexGeomesh(Center + V3(dX, dY, -dZ), 0.0f, 0.0f, 0.0f),
        VertexGeomesh(Center + V3(dX, dY, dZ), 0.0f, 0.0f, 0.0f),
        VertexGeomesh(Center + V3(-dX, dY, dZ), 0.0f, 0.0f, 0.0f),
    };
    
    uint32 Indices[] = 
    {
        0, 1, 2, 3, 0 ,3, 1, 2,
        4, 5, 6, 7, 4, 7, 5, 6,
        0, 4, 1, 5, 2, 6, 3, 7
    };
    
    Mesh.verticesSize = 8;
    Mesh.indicesSize  = 24;
    
    D3DLoadMesh(&Mesh, (vertex_geomesh *)Vertices, 8, (uint32 *)Indices, 24);
    
    return Mesh;
}

internal void 
c_DrawBoundingAABB(mesh *Mesh)
{
    uint32 stride = sizeof(vertex_geomesh);
    uint32 offset = 0;
    g_pd3dDeviceContext->IASetIndexBuffer(Mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    g_pd3dDeviceContext->IASetVertexBuffers(0, 1, &Mesh->vertexBuffer, &stride, &offset);
    g_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    
    g_pd3dDeviceContext->DrawIndexed(Mesh->indicesSize, 0, 0);
};

inline int32
c_IntersectsPlane(v4 *Plane, boundingAABB_axis *Box)
{
    v3 VMin = Box->center - Box->stride;
    v3 VMax = Box->center + Box->stride;
    
    uint32 Result = 1;
    v3 P, Q;
    
    // NOTE: replace PQ according to orientation
    // Convention, normal inward (inside of frustum)
    if(Plane->X >= 0.0f)
    {
        P.X = VMin.X;
        Q.X = VMax.X;
    }
    else
    {
        P.X = VMax.X;
        Q.X = VMin.X;
    }
    
    if(Plane->Y >= 0.0f)
    {
        P.Y = VMin.Y;
        Q.Y = VMax.Y;
    }
    else
    {
        P.Y = VMax.Y;
        Q.Y = VMin.Y;
    }
    
    if(Plane->Z >= 0.0f)
    {
        P.Z = VMin.Z;
        Q.Z = VMax.Z;
    }
    else
    {
        P.Z = VMax.Z;
        Q.Z = VMin.Z;
    }
    
    // NOTE: Plane's equation ax + by + cz + d = 0
    v3 PlaneV3 = Normalize(Plane->X, Plane->Y, Plane->Z);
    if(Dot(&PlaneV3, &Q) + Plane->W < 0)
    {
        return 0;
    }
    
    
#if 0    
    // NOTE: entirely in volume, not sure if it will ever be needed
    if(DotProduct(&PlaneV3, &P) + plane->W >= 0)
    {
        Result = 2;
    }
#endif
    
    return Result;
}

inline bool
c_IntersectsAABBFrustum(boundingAABB_axis *Box, frustum *Fm)
{
    return c_IntersectsPlane(&Fm->right, Box) > 0 && 
        c_IntersectsPlane(&Fm->left, Box) > 0 && 
        c_IntersectsPlane(&Fm->top, Box) > 0 && 
        c_IntersectsPlane(&Fm->bottom, Box) > 0 && 
        c_IntersectsPlane(&Fm->nearp, Box) > 0 && 
        c_IntersectsPlane(&Fm->farp, Box) > 0;
}

// NOTE: Returns ray in World Space
inline ray
c_PickRay(real32 x, real32 y)
{
    // View space 
    real32 vx = (2.0f * x/SCREEN_WIDTH - 1.0f) / Camera.projection3d[0][0];
    real32 vy = (-2.0f * y/SCREEN_HEIGHT + 1.0f) / Camera.projection3d[1][1];
    
    // World space
    ray Ray;
    real32 Det;
    Matrix InvView = InvMatrix(&Det, &Camera.view); // NOTE: as costly as product :think:
    
    v4 Origin     = V4(0.0f, 0.0f, 0.0f, 1.0f);
    v4 Direction  = V4(vx, vy, 1.0f, 0.0f);
    Direction     = ProductMatrixByV4(&InvView, &Direction);
    Origin        = ProductMatrixByV4(&InvView, &Origin);
    
    Ray.origin    = V3(Origin.X, Origin.Y, Origin.Z);; 
    Ray.direction = V3(Direction.X, Direction.Y, Direction.Z);
    
    return Ray;
}

// NOTE: Returns ray shot from center of screen
inline ray
c_PickRayFromCenter()
{
    ray Ray;
    Ray.origin = Camera.position; 
    Ray.direction = Camera.target;
    
    return Ray;
}

// NOTE: returns the computed t value for p(t) = p0 + t*u, ray
inline real32
c_IntersectsRayPlane(v4 *Plane, ray *Ray)
{
    v3 Normal = V3(-Plane->X, -Plane->Y, -Plane->Z);
    real32 XD = Dot(&Normal, &Ray->origin);
    real32 XN = Dot(&Normal, &Ray->direction);
    return (XD - Plane->W) / XN;
}

// NOTE: returns t intersection parameter, -1 if there's no intersection
// NOTE: https://izzofinal.wordpress.com/2012/11/09/ray-vs-box-round-1/
inline real32
c_IntersectsRayAABB(boundingAABB_axis *Box, ray *Ray)
{
    v3 Vmin = Box->center - Box->stride;
    v3 Vmax = Box->center + Box->stride;
    
    real32 tMinX = (Vmin.X - Ray->origin.X) / Ray->direction.X;
    real32 tMaxX = (Vmax.X - Ray->origin.X) / Ray->direction.X;
    
    real32 tMinY = (Vmin.Y - Ray->origin.Y) / Ray->direction.Y;
    real32 tMaxY = (Vmax.Y - Ray->origin.Y) / Ray->direction.Y;
    
    real32 tMinZ = (Vmin.Z - Ray->origin.Z) / Ray->direction.Z;
    real32 tMaxZ = (Vmax.Z - Ray->origin.Z) / Ray->direction.Z;
    
    // Find the greater tMin and the lesser tMax
    real32 MinTX = Min(tMinX, tMaxX);
    real32 MinTY = Min(tMinY, tMaxY);
    real32 MinTZ = Min(tMinZ, tMaxZ);
    real32 tMin  = Max(Max(MinTX, MinTY), MinTZ);
    
    real32 MaxTX = Max(tMinX, tMaxX);
    real32 MaxTY = Max(tMinY, tMaxY);
    real32 MaxTZ = Max(tMinZ, tMaxZ);
    real32 tMax  = Min(Min(MaxTX, MaxTY), MaxTZ);
    
    if (tMax < 0)
    {
        return -1;
    }
    
    if (tMin > tMax) 
    {
        return -1;
    }
    
    if (tMin < 0.0f)
    {
        return tMax;
    }
    
    return tMin;
}

#endif //COLLISION_H
