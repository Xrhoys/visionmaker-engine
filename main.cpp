/*
 Attempt to d3d + win32 backend powered game engine
 
DONE:
- Win32 init // 
- Timer //
- D3D init //
- Texture
- Shader init and 
- Pipeline build script //
- Basic shapes //
- Camera //
- Memory allocation/management //
- Input 
- Orthogonal projection 
- Font Engine
- Math
- DDS parser
- Ray and intersections
- Frustum
- Imgui integration
- Render to texture
- Imgui editor UI
- MSAA (disabled currently) // Legacy hardware mode, refere to mmozeiko/win32_d3d11.c gist
- Lighting (directional, point, spot);
- [Space] to switch between first person mode to editing mode
- Redimension
- DEMO!

TODO (ranked in priority):
- Shadow
- Animation (some light hearted animation :D)
- Additional math: transformation matrix + quaternions rotations
- Shapes / Models / Adding more common patterns
- Geomesh generation and drawing with instances / (for now, software instances)
- Sound
- Physics
- Bitmap Font
- Ray tracer
- FBX parser
- Collision 
- Fog
- Mirror
- Draw normals from vertex (POINTLIST), from geometry shader
- Sphere subdivision using geometry shader
- Borderless fullscreen

Minor:
- Ray picking 2D foreground elements (UI events) 
- Frustum culling clip entities out if not in fov
- RawInput confines the handling of mouse input (can't select window frame)
- buffered raw input on seperate thread
- Alt+Tab could sometimes leave the cursor being shown
*/

#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"

#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <stdint.h>
#include <hidusage.h>
#include <stdio.h> // NOTE: replace that later

#include "main.h"
#include "math.h"
#include "mesh.h"
#include "light.h"
#include "camera.h"
#include "shader.h"
#include "d3d.h"
#include "shapes.h"
#include "input.h"
#include "texture.h"
#include "font.h"
#include "collision.h"
#include "entity.h"
#include "debug.h"

internal void
Win32_InitInput()
{
    System.Win32_rawInputDevice[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    System.Win32_rawInputDevice[0].usUsage     = HID_USAGE_GENERIC_KEYBOARD;
    System.Win32_rawInputDevice[0].dwFlags     = RIDEV_NOLEGACY;
    System.Win32_rawInputDevice[0].hwndTarget  = 0;
    
    System.Win32_rawInputDevice[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
    System.Win32_rawInputDevice[1].usUsage     = HID_USAGE_GENERIC_MOUSE;
    System.Win32_rawInputDevice[1].dwFlags     = RIDEV_NOLEGACY;
    System.Win32_rawInputDevice[1].hwndTarget  = 0;
    
    if (RegisterRawInputDevices(System.Win32_rawInputDevice, 2, sizeof(System.Win32_rawInputDevice[0])) == FALSE)
    {
        // TODO: logging
    }
    
    // NOTE: Initialize mouse sensitity and keyboard default mapping 
    KeyMap   = {};
    MouseMap = {};
    MouseMap.dx = 0.001f;
    MouseMap.dy = 0.001f;
}

internal void
Win32_ProcessKeyboardInput(RAWKEYBOARD *Data)
{
#if DEBUG
    {
        ImGuiIO& io = ImGui::GetIO();
        if(!ClipMouseMode)
        {
            uint8 IsDown = !(Data->Flags && RI_KEY_BREAK);
            if(IsDown)
            {
                if(Data->VKey >= '0' && Data->VKey <= 'Z')
                {
                    io.AddInputCharacter(Data->VKey);
                }
                else if(Data->VKey == VK_OEM_PERIOD)
                {
                    io.AddInputCharacter('.');
                }
                else if(Data->VKey == VK_RETURN)
                {
                    io.AddKeyEvent(ImGuiKey_Enter, IsDown);
                }
                else if(Data->VKey == VK_BACK)
                {
                    io.AddKeyEvent(ImGuiKey_Backspace, IsDown);
                }
                
            }
        }
    }
#endif
    
    // NOTE: register previous key state for state sensitive keys
    KeyMap.switchMouseMode.isDown = FALSE;
    
    uint8 IsDown = !(Data->Flags && RI_KEY_BREAK);
    if (Data->VKey == 'W') 
    {
        KeyMap.up.wasDown = KeyMap.up.isDown;
        KeyMap.up.isDown = IsDown;
    }
    else if (Data->VKey == 'A')
    {
        KeyMap.left.wasDown = KeyMap.left.isDown;
        KeyMap.left.isDown = IsDown;
    }
    else if (Data->VKey == 'S')
    {
        KeyMap.down.wasDown = KeyMap.down.isDown;
        KeyMap.down.isDown = IsDown;
    }
    else if (Data->VKey == 'D')
    {
        KeyMap.right.wasDown = KeyMap.right.isDown;
        KeyMap.right.isDown = IsDown;
    }
    else if (Data->VKey == VK_ESCAPE)
    {
        System.Running = FALSE;
    }
    else if (Data->VKey == VK_SPACE)
    {
        KeyMap.switchMouseMode.wasDown = KeyMap.switchMouseMode.isDown;
        KeyMap.switchMouseMode.isDown = IsDown;
    }
    
    // NOTE: switch between freecam and editing mode
    if(KeyMap.switchMouseMode.isDown && !KeyMap.switchMouseMode.wasDown)
    {
        OldClipMouseMode = ClipMouseMode;
        ClipMouseMode = !ClipMouseMode;
        if(ClipMouseMode != OldClipMouseMode)
        {
            if(ClipMouseMode)
            {
                GetWindowRect(System.hwnd, &System.RcClip);
                ShowCursor(FALSE);
                ClipCursor(&System.RcClip);
            }
            else
            {
                ShowCursor(TRUE);
                ClipCursor(&System.RcOldClip);
            }
        }
    }
    
}

internal void
Win32_ProcessMouseInput(RAWMOUSE *Data)
{
#if DEBUG
    {
        ImGuiIO& io = ImGui::GetIO();
        if(!ClipMouseMode)
        {
            // NOTE: register mouse events to IMGUI
            uint16 Flags = Data->usButtonFlags;
            
            if(Flags == RI_MOUSE_LEFT_BUTTON_DOWN)
            {
                io.AddMouseButtonEvent(ImGuiMouseButton_Left, TRUE);
            }
            else if(Flags == RI_MOUSE_LEFT_BUTTON_UP)
            {
                io.AddMouseButtonEvent(ImGuiMouseButton_Left, FALSE);
            }
            
            return;
        }
    }
#endif
    
#if 0
    // NOTE: This section makes no sense, need a proper mapping to a linear scale
    real32 XOffset = Data->lLastX - LastMouseInput.lLastX;
    real32 YOffset = LastMouseInput.lLastY - Data->lLastY;
    LastMouseInput = *Data;
    
    XOffset *= CameraHorizontalSpeed;
    YOffset *= CameraVerticalSpeed;
#endif
    
    Camera.yaw   -= MouseMap.dx * Data->lLastX;
    Camera.pitch -= MouseMap.dy * Data->lLastY;
    
    if(Camera.pitch > MaxVerticalCameraAngle)
    {
        Camera.pitch = MaxVerticalCameraAngle;
    }
    
    if(Camera.pitch < -MaxVerticalCameraAngle)
    {
        Camera.pitch = -MaxVerticalCameraAngle;
    }
}

internal file
Win32_ReadEntireFile(char *FileName)
{
    file File = {};
    
    void *Result = 0;
    DWORD BytesRead;
    uint32 FileSize32;
    HANDLE FileHandle = CreateFileA(FileName,
                                    GENERIC_READ,
                                    FILE_SHARE_READ,
                                    0,
                                    OPEN_EXISTING,
                                    0,
                                    0);
    
    if(FileHandle != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER FileSize;
        if(GetFileSizeEx(FileHandle, &FileSize))
        {
            // NOTE: Will cause problem for 64bits
            FileSize32 = (uint32)FileSize.QuadPart;
            Result = VirtualAlloc(0, FileSize.QuadPart, 
                                  MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
            
            if(Result)
            {
                if(ReadFile(FileHandle, Result, FileSize32, &BytesRead, 0))
                {
                    File.memory = Result;
                    File.size = FileSize32;
                }
                else
                {
                    VirtualFree(Result, 0, MEM_RELEASE);
                }
            }
            else
            {
                // TODO: Logging
            }
        }
        else
        {
            // TODO: Logging
        }
    }
    else
    {
        // TODO: Logging
    }
    
    return File;
}

internal void
Win32_FreeFile(file *File)
{
    VirtualFree(File->memory, 0, MEM_RELEASE);
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_SIZE:
        {
            WindowResize(hWnd, msg, wParam, lParam);
            return 0;
        } break;
        
        case WM_MOVE:
        {
            
        } break;
        
        case WM_INPUT:
        {
            uint32 dwSize;
            
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));
            
            if(!dwSize == 0)
            {
                // TODO: Replace "new", however the callback frequency is big, so might run out of memory slot really fast in the current bump allocator. Needs to make difference resolution bump allocators and measure the polling rate ofWM_INPUT event. That said, moving input on a secondary thread might removed the need for this.
                LPBYTE lpb = new BYTE[dwSize];
                
                GetRawInputData((HRAWINPUT)lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER));
                
                RAWINPUT* Raw = (RAWINPUT *)lpb;
                
                if (Raw->header.dwType == RIM_TYPEKEYBOARD)
                {
                    Win32_ProcessKeyboardInput(&Raw->data.keyboard);
                }
                else if (Raw->header.dwType == RIM_TYPEMOUSE)
                {
                    Win32_ProcessMouseInput(&Raw->data.mouse);
                }
                
                // TODO: errk
                delete[] lpb;
            }
        } break;
        
        case WM_SYSCOMMAND:
        {
            if ((wParam & 0xfff0) == SC_KEYMENU) // NOTE: Disable ALT application menu
                return 0;
        } break;
        
        case WM_DESTROY:
        {
            PostQuitMessage(0);
            return 0;
        } break;
    }
    
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int CALLBACK
WinMain(HINSTANCE Instance,
        HINSTANCE PrevInstance,
        LPSTR CommandLine,
        int ShowCode)
{
    System = {};
    WNDCLASSEX wc = { 
        sizeof(WNDCLASSEX), 
        CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS, 
        WndProc, 
        0L, 0L, 
        GetModuleHandle(NULL), 
        NULL, NULL, NULL, NULL, 
        "Window Title", 
        NULL 
    };
    
    RegisterClassEx(&wc);
    HWND hwnd = CreateWindow(wc.lpszClassName, 
                             "Window Title", 
                             WS_OVERLAPPEDWINDOW, 
                             100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 
                             NULL, NULL, wc.hInstance, NULL);
    
    System.hwnd = hwnd;
    
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }
    
    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);
    GetClipCursor(&System.RcOldClip);
    
    MemInit();
    InitShapes();
    InitModels();
    InitCamera();
    InitTimer();
    Win32_InitInput();
    InitFont("assets/FontMapping.txt", "assets/font2.dds");
    
#if DEBUG
    InitImGui(hwnd);
#endif
    
    // NOTE: Position variables
    TestLight();
    
    while (System.Running)
    {
        MSG Message;
        
        while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
        {
            if(Message.message == WM_QUIT)
            {
                System.Running = FALSE;
            }
            
            TranslateMessage(&Message);
            DispatchMessageA(&Message);
        }
        
        ProcessPlayerInput();
        UpdateCamera();
        
        ray FromCamera = c_PickRayFromCenter();
        
        if(BeginDrawing())
        {
            GetCursorPos(&System.cursorPosition);
            
            BeginMode3D();
            {
                for(uint32 Index = 0;
                    Index < 100;
                    ++Index)
                {
                    if(Registry[Index].isSet)
                    {
                        DrawEntity(&Registry[Index]);
                    }
                }
            }
            EndMode3D();
            
            BeginMode2D();
            {
                // DrawRectangle(100, 100, 100, 100, GOLD);
                // DrawFont("testing font engine\n", 100.0f, 100.0f, GREEN);
            }
            EndMode2D();
            
#if DEBUG
            RenderImGui();
#endif
            
            EndDrawing();
        }
    }
    
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    CleanupDeviceD3D();
    DestroyWindow(hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);
    
    return 0;
}