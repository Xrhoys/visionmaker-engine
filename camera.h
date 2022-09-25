/* date = July 10th 2022 4:52 pm */

#ifndef CAMERA_H
#define CAMERA_H

struct camera
{
    Matrix wvp;
    Matrix world;
    Matrix view;
    Matrix projection2d;
    Matrix projection3d;
    Matrix invProjection2d;
    
    v3 position;
    v3 target;
    v3 up;
    
    real32 yaw   = 0.0f;
    real32 pitch = 0.0f;
    
    real32 ForwardBackTransition = 0.0f;
    real32 LeftRightTransition   = 0.0f;
};

global camera Camera;

inline void
CameraLookAtLH(Matrix *Output, v3 *Position, v3 *LookAt, v3 *ReferenceUp)
{
    v3 Pos = *Position;
    v3 Look = *LookAt;
    v3 Up = *ReferenceUp;
    
    // REFERENCE: 
    // https://docs.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
    v3 Result = Look - Pos;
    v3 ZAxis  = Normalize(&Result);
    
    Result = Up * ZAxis;
    v3 XAxis  = Normalize(&Result);
    
    Result = ZAxis * XAxis;
    v3 YAxis  = Normalize(&Result);
    
    real32 TX = -Dot(&XAxis, Position);
    real32 TY = -Dot(&YAxis, Position);
    real32 TZ = -Dot(&ZAxis, Position);
    
    Output->E[0] = V4( XAxis.X, YAxis.X, ZAxis.X, 0 );
    Output->E[1] = V4( XAxis.Y, YAxis.Y, ZAxis.Y, 0 );
    Output->E[2] = V4( XAxis.Z, YAxis.Z, ZAxis.Z, 0 );
    Output->E[3] = V4(      TX,      TY,      TZ, 1 );
}

inline void
PerspectiveFovLH(Matrix *Output, 
                 real32 FovAngleY, real32 AspectRatio, 
                 real32 NearZ, real32 FarZ)
{
    // REFERENCE:
    // MSDN: win32/dxtecharts/the-direct3d-transformation-pipeline
    real32 X22 = 1.0f/tan(FovAngleY/2);
    real32 X11 = X22*AspectRatio;
    real32 X33 = FarZ/(FarZ - NearZ);
    real32 X34 = 1.0f;
    real32 X43 = -NearZ*FarZ/(FarZ - NearZ);;
    
    Output->E[0] = V4( X11,   0,   0,   0 );
    Output->E[1] = V4(   0, X22,   0,   0 );
    Output->E[2] = V4(   0,   0, X33, X34 );
    Output->E[3] = V4(   0,   0, X43,   0 );
}

inline void
PerspectiveOrthLH(Matrix *Output, real32 Width, real32 Height, real32 NearZ, real32 FarZ)
{
    real32 X11 = 2.0f / Width;
    real32 X22 = 2.0f / Height;
    real32 X33 = 1.0f / (FarZ - NearZ);
    real32 X43 = -NearZ / (FarZ - NearZ);
    
    Output->E[0] = V4( X11,   0,   0, 0 );
    Output->E[1] = V4(   0, X22,   0, 0 );
    Output->E[2] = V4(   0,   0, X33, 0 );
    Output->E[3] = V4(   0,   0, X43, 1 );
}

internal void
InitCamera()
{
    Camera.position = V3(-50.0f, 10.0f, -3.0f);
    Camera.target   = V3(0.0f, 0.0f, -5.0f);
    Camera.up       = V3(0.0f, 1.0f,  0.0f);
    
    CameraLookAtLH(&Camera.view, &Camera.position, &Camera.target, &Camera.up);
    PerspectiveFovLH(&Camera.projection3d, 0.25f*3.14f, 
                     (real32)SCREEN_HEIGHT/SCREEN_WIDTH, 0.1f, 1000.0f);
    PerspectiveOrthLH(&Camera.projection2d, SCREEN_WIDTH, SCREEN_HEIGHT, 0.1f, 3.0f);
    
    Camera.invProjection2d = Transpose(&Camera.projection2d);
}

internal void
UpdateCamera()
{
    Camera.position += (Camera.target * Camera.ForwardBackTransition);
    v3 Result = Camera.target * Camera.up;
    Camera.position += Normalize(&Result) * Camera.LeftRightTransition;
    
    Camera.ForwardBackTransition = 0;
    Camera.LeftRightTransition = 0;
    
    // Update camera coords and angle
    Result = Camera.position + Camera.target;
    CameraLookAtLH(&Camera.view, &Camera.position, &Result, &Camera.up);
    
    v3 Direction;
    
    Direction.X = cos(Camera.yaw) * cos(Camera.pitch);
    Direction.Y = sin(Camera.pitch);
    Direction.Z = sin(Camera.yaw) * cos(Camera.pitch);
    
    Camera.target = Direction;
}

/*
Ray GetMouseRay(Vector2 mousePosition, Camera camera);
Matrix GetCameraMatrix(Camera camera);
Matrix GetCameraMatrix2D(Camera2D camera);
Vector2 GetWorldToScreen(Vector3 position, Camera camera);
Vector2 GetWorldToScreenEx(Vector3 position, Camera camera, int width, int height);
Vector2 GetWorldToScreen2D(Vector2 position, Camera2D camera);
Vector2 GetScreenToWorld2D(Vector2 position, Camera2D camera);
*/

#endif //CAMERA_H
