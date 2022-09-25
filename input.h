#ifndef INPUT_H
#define INPUT_H

/*
INPUT concept:
Event based input process
- by default at each start of loop set every key to its last state (wasDown) and modify the current state to false (isUp).
- for the isDown key, set them to isDown
*/
#define MaxVerticalCameraAngle Pi/2.5f
#define PlayerMovementSpeed 0.1f

struct key_state
{
    // NOTE: bitwise operation to set values
    uint16 key;
    uint8 isDown;
    uint8 wasDown;
};

struct input_registry
{
    key_state up;
    key_state left;
    key_state right;
    key_state down;
    key_state switchMouseMode;
};

struct input_mouse
{
    real32 x;
    real32 y;
    
    real32 dx;
    real32 dy;
};

global input_registry KeyMap;
global input_mouse    MouseMap;
global bool32 ClipMouseMode = FALSE;
global bool32 OldClipMouseMode = FALSE;
internal void
ProcessPlayerInput()
{
    if(KeyMap.up.isDown)
    {
        Camera.ForwardBackTransition += PlayerMovementSpeed * System.Timer.elapsed/1000.0f; 
    }
    if(KeyMap.down.isDown)
    {
        Camera.ForwardBackTransition -= PlayerMovementSpeed * System.Timer.elapsed/1000.0f;
    }
    
    if(KeyMap.left.isDown)
    {
        Camera.LeftRightTransition += PlayerMovementSpeed * System.Timer.elapsed/1000.0f;
    }
    if(KeyMap.right.isDown)
    {
        Camera.LeftRightTransition -= PlayerMovementSpeed * System.Timer.elapsed/1000.0f;
    }
}

#endif //INPUT_H
