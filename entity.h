/* date = July 26th 2022 8:48 pm */

#ifndef ENTITY_H
#define ENTITY_H

global uint32 IDRegistry = 0;

enum entity_type
{
    ENTITY_SPHERE,
    ENTITY_GRID,
    ENTITY_CUBE,
    ENTITY_MODEL,
};

enum entity_draw_mode
{
    DRAW_MODE_TRIANGLE,
    DRAW_MODE_LINES,
};

struct entity
{
    light_material material;
    
    uint32 id;
    entity_type type;
    bool32 isSet;
    
    mesh mesh;
    
    v3 position;
    v3 rotation;
    v3 scale;
    
    entity_draw_mode mode;
};

inline entity
Entities(mesh *Mesh, light_material *Material, entity_type Type, entity_draw_mode Mode = DRAW_MODE_TRIANGLE)
{
    entity Entity;
    
    Entity.id = ++IDRegistry;
    Entity.isSet = TRUE;
    Entity.position = V3(0.0f, 0.0f, 0.0f);
    Entity.rotation = V3(0.0f, 0.0f, 0.0f);
    Entity.scale    = V3(1.0f, 1.0f, 1.0f);
    Entity.type     = Type;
    
    Entity.mesh = *Mesh;
    Entity.material = *Material;
    Entity.mode = Mode;
    
    return Entity;
}

inline void
FreeEntity(entity *Entity)
{
    Entity->mesh.vertexBuffer->Release();
    Entity->mesh.indexBuffer->Release();
    
    Entity->isSet = FALSE;
};

inline void
DrawEntity(entity *Entity)
{
    g_pd3dDeviceContext->IASetInputLayout(g_shader3d_geomesh.layout);
    g_pd3dDeviceContext->VSSetShader(g_shader3d_geomesh.vs, 0, 0);
    g_pd3dDeviceContext->PSSetShader(g_shader3d_geomesh.ps, 0, 0);
    g_pd3dDeviceContext->PSSetConstantBuffers(0, 1, &g_objectGeoCBuffer);
    g_pd3dDeviceContext->VSSetConstantBuffers(0, 1, &g_objectGeoCBuffer);
    
    Matrix WorldMatrix = MatrixIdentity();
    Scaling(&WorldMatrix, Entity->scale.X, Entity->scale.Y, Entity->scale.Z);
    Translation(&WorldMatrix, Entity->position.X, Entity->position.Y, Entity->position.Z);
    // NOTE: rotation
    
    g_cbPerObjectGeo.g_world    = WorldMatrix;
    g_cbPerObjectGeo.g_material = Entity->material;
    
    g_pd3dDeviceContext->UpdateSubresource(g_objectGeoCBuffer, 0, NULL, &g_cbPerObjectGeo, 0, 0);
    
    if(Entity->mode == DRAW_MODE_LINES)
    {
        DrawGeomeshL(&Entity->mesh);
    }
    else
    {
        DrawGeomeshT(&Entity->mesh);
    }
}

inline char*
GetEntityName(entity_type Type)
{
    switch(Type)
    {
        case ENTITY_CUBE: 
        {
            return "Cube";
        } break;
        
        case ENTITY_SPHERE:
        {
            return "Sphere";
        } break;
        
        case ENTITY_MODEL:
        {
            return "Model";
        } break;
        
        case ENTITY_GRID:
        {
            return "Grid";
        } break;
        
        default:
        {
            return "Not label entity type";
        } break;
    }
}

#endif //ENTITY_H
