/* date = July 23rd 2022 6:11 pm */

#ifndef LIGHT_H
#define LIGHT_H

struct light_material
{
    v4 ambient;
    v4 diffuse;
    v4 specular;
    v4 reflect;
};

enum light_type
{
    LIGHT_DIRECTIONAL,
    LIGHT_POINT,
    LIGHT_SPOT,
};

struct light
{
    v4            ambient;
    v4            diffuse;
    v4            specular;
    
    v3            position;
    real32        range;
    
    v3            direction;
    real32        spot; // NOTE: cone angle
    
    v3            att;
    light_type    type;
    
    bool32        enabled;
    bool32        pad[3];
};

inline light
Light(light_type Type)
{
    light Light = {};
    
    Light.type = Type;
    
    return Light;
}

inline char*
GetLightName(light_type Type)
{
    switch(Type)
    {
        case LIGHT_SPOT: 
        {
            return "SPOT";
        } break;
        
        case LIGHT_DIRECTIONAL:
        {
            return "DIRECTIONAL";
        } break;
        
        case LIGHT_POINT:
        {
            return "POINT";
        } break;
        
        default:
        {
            return "Not label light type";
        } break;
    }
}

#endif //LIGHT_H
