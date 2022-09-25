#ifndef MAIN_H
#define MAIN_H

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float real32;
typedef double real64;

typedef int32_t bool32;

#define internal static
#define global static

#if DEBUG
#define Assert(Expression) if(!(Expression)) {*(int *)0 = 0;}
#else
#define Assert(Expression)
#endif

#define MEM_TOTAL_SIZE 256*(1024 * 1024)
#define MEM_BLOCK_SIZE 1024*1024
#define MEM_BLOCK_NUM  256

#define SCREEN_WIDTH  1600
#define SCREEN_HEIGHT 900
#define RENDER_WIDTH  1280
#define RENDER_HEIGHT 720

struct segment
{
    void *Data;
    bool32 Used;
};

struct memory_block
{
    void *Cursor;
    uint32 SegmentSize;
    segment Segments[MEM_BLOCK_NUM]; // Fixed size memory pool
};

struct file
{
    void *memory;
    uint32 size;
};

struct timer
{
    LARGE_INTEGER lastCounter;
    real32 elapsed; // NOTE: in ms
    real32 maxFrameTime;
    
    real32 frametime; // NOTE: in ms
    real32 fps;
    real32 time;
};

struct win32_system
{
    HWND hwnd;
    memory_block Block;
    bool32 Running = TRUE;
    timer Timer;
    
    POINT cursorPosition;
    
    RAWINPUTDEVICE Win32_rawInputDevice[2];
    ImVec2 LastPos;
    RECT RcOldClip;
    RECT RcClip;
};

global win32_system System;

file Win32_ReadEntireFile(char *FileName);
void Win32_FreeFile(file *File);
void Win32_ProcessMouseInput(RAWMOUSE *Data);

inline void
MemInit()
{
    void *DataPtr = VirtualAlloc(0, MEM_TOTAL_SIZE, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    if (DataPtr != nullptr)
    {
        System.Block.Cursor = DataPtr;
        System.Block.SegmentSize = MEM_BLOCK_SIZE;
        
        for (uint32 Index = 0;
             Index < MEM_BLOCK_NUM;
             ++Index)
        {
            System.Block.Segments[Index].Data = (uint8 *)DataPtr + Index * MEM_BLOCK_SIZE;
            System.Block.Segments[Index].Used = FALSE;
        }
    }
    else
    {
        // TODO: log errors 
    }
}

// NOTE: very basic implementation of fixed size bump allocator
inline segment*
malloc()
{
    Assert(System.Block.Cursor);
    
    for(int Index = 0;
        Index < MEM_BLOCK_SIZE;
        ++Index)
    {
        if (System.Block.Segments[Index].Used == FALSE)
        {
            System.Block.Segments[Index].Used = TRUE;
            return &System.Block.Segments[Index];
        }
    }
    
    return nullptr;
}

inline void
free(segment *seg, bool32 ZeroMemory = FALSE)
{
    Assert(System.Block.Cursor);
    
    seg->Used = FALSE;
    // NOTE: Set segment to zero (memset)
    if (ZeroMemory)
    {
        char *Addr = (char *)System.Block.Cursor + MEM_BLOCK_SIZE;
        for(int MemoryIndex = 0;
            MemoryIndex < MEM_BLOCK_SIZE;
            ++MemoryIndex)
        {
            Addr[MemoryIndex] = 0;
        }
    }
}

inline void
InitTimer()
{
    System.Timer = {};
    QueryPerformanceCounter(&System.Timer.lastCounter);
    
    System.Timer.maxFrameTime = 1000.0f / 144.0f; // NOTE: temporary placeholder
}

inline uint32
GetTextLength(char *Text)
{
    uint32 Result = 0;
    while(*(Text++) != '\0')
    {
        Result++;
    }
    
    return Result;
}

inline void
ConcatChars(char *Dest, char *A, char* B)
{
    uint32 Index = 0;
    
    while(*A != '\0')
    {
        Dest[Index++] = *A++;
    }
    
    while(*B != '\0')
    {
        Dest[Index++] = *B++;
    }
}


#endif //MAIN_H
