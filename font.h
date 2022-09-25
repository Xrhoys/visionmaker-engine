/* date = July 19th 2022 9:39 pm */

#ifndef FONT_H
#define FONT_H
#define FontListSize 95
#define ASCII_MAPPING_DEBUT_CURSOR 32
#define FONT_TEXT_MAX_SIZE 256
#define EOF (-1)

// NOTE: Bitmap font generator
// http://www.angelcode.com/products/bmfont/doc/file_format.html#bin
#pragma pack(push, 1)
struct bmf_block_info
{
    uint16 fontSize;
    uint8  bitField;
    uint8  charset;
    uint16 stretchH;
    uint8  aa;
    uint8  paddingUp;
    uint8  paddingRight;
    uint8  paddingDown;
    uint8  paddingLeft;
    uint8  spacingHoriz;
    uint8  spacingVert;
    uint8  outline;
    char*  fontName; // NOTE: null terminated
};

struct bmf_block_common
{
    uint16 lineHeight;
    uint16 base;
    uint16 scaleW;
    uint16 scaleH;
    uint16 pages;
    uint8  bitField;
    uint8  alphaChannel;
    uint8  redChannel;
    uint8  greenChannel;
    uint8  blueChannel;
};

struct bmf_block_pages
{
    char* Data;
    uint32 Size;
};

struct bmf_block_chars
{
    uint32 id;
    uint16 x;
    uint16 y;
    uint16 width;
    uint16 height;
    uint16 xoffset;
    uint16 yoffset;
    uint16 xadvance;
    uint8  page;
    uint8  channel;
};
#pragma pack(pop)

struct font_type
{
    real32 left; 
    real32 right;
    uint32 size;
};

// global bmf_block_chars FontRegistry[200];
global font_type    Fonts[FontListSize];
global mesh         FontMesh;
global d3d_texture  FontTexture;

internal void
InitFontEngine(char* FileName)
{
    file File = Win32_ReadEntireFile(FileName);
    
    if(File.size)
    {
        uint8* Cursor = (uint8 *)File.memory;
        
        // Assert(*(DWORD *)Cursor == 0x424D4603); // NOTE: B M F + v3
        Cursor += 5;
        
        // NOTE: Size of block info
        uint32 InfoBlockSize = *(uint32 *)Cursor;
        Cursor += 4;
        
        bmf_block_info* Info = (bmf_block_info *)Cursor;
        Cursor += sizeof(bmf_block_info);
        
        char Buffer[256];
        char* ReadCharCursor = (char *)Cursor;
        uint32 Index = 0;
        while(*(ReadCharCursor++) != '\0')
        {
            Buffer[Index++] = *ReadCharCursor++;
        }
        
        Info->fontName = Buffer;
        
        Cursor = (uint8 *)File.memory + InfoBlockSize + 10;
        
        bmf_block_common* Common = (bmf_block_common *)Cursor;
        Cursor += sizeof(bmf_block_common) + 1;
        
        uint32 PagesBlockSize = *(uint32 *)Cursor;
        Cursor += PagesBlockSize + 1;
        
        uint32 CharsBlockSize = *(uint32 *)Cursor;
        uint32 CharArraySize = CharsBlockSize / sizeof(bmf_block_chars);
        Cursor += 4;
        
        bmf_block_chars *CharsCursor = (bmf_block_chars *)Cursor;
        for(uint32 Index = 0;
            Index < CharArraySize;
            ++Index)
        {
            // FontRegistry[Index] = *CharsCursor++;
        }
    }
    else
    {
        
    }
}

internal void
InitFont(char *FileName, char *TextureImage)
{
    file File = Win32_ReadEntireFile(FileName);
    // Load Font index
    char *Cursor = (char *)File.memory;
    
    // NOTE: ignore "space" 
    Fonts[0] = {0.0f, 0.0f, 0};
    Cursor += 15;
    for(int LineIndex = 1;
        LineIndex < FontListSize;
        ++LineIndex)
    {
        //Node CurrentLine[4];
        uint32 Index = 0;
        
        while(*Cursor++ != '\n')
        {
            char Buffer[20] = {0};
            uint32 NodeCharIndex = 0;
            while(*Cursor != ' '  && *Cursor != '\n' && *Cursor != EOF)
            {
                Buffer[NodeCharIndex++] = *Cursor++;
            }
            
            switch(Index++)
            {
                case 2:
                {
                    Fonts[LineIndex].left = atof(Buffer); 
                }
                break;
                
                case 3:
                {
                    Fonts[LineIndex].right = atof(Buffer); 
                }
                break;
                
                case 4:
                {
                    Fonts[LineIndex].size = atoi(Buffer);
                }
                break;
                
                default:
                break;
            }
            
            if(*Cursor == '\n')
                continue;
            
            // NOTE: jump space
            //cursor++;
        }
        
    }
    
    {
        // NOTE: init font mesh
        vertex_font vertices[256];
        uint32 indices[256];
        D3DLoadMesh(&FontMesh, vertices, 256, indices, 256, TRUE);
    }
    
    {
        // NOTE: load font texture
        FontTexture = LoadTexture(TextureImage);
    }
}

internal void
DrawFont(char* Text, real32 PosX, real32 PosY, color *Color)
{
    color FormattedColor = GetFormattedColor(Color);
    uint32 TextLength = GetTextLength(Text);
    Assert(TextLength > 0);
    
    {
        D3D11_MAPPED_SUBRESOURCE VerticesMappedResource;
        g_pd3dDeviceContext->Map(FontMesh.vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &VerticesMappedResource);
        vertex_font *Vertices = (vertex_font *)VerticesMappedResource.pData;
        
        D3D11_MAPPED_SUBRESOURCE IndicesMappedResource;
        g_pd3dDeviceContext->Map(FontMesh.indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &IndicesMappedResource);
        uint32 *Indices = (uint32 *)IndicesMappedResource.pData;
        
        FontMesh.verticesSize = TextLength * 4;
        FontMesh.indicesSize= TextLength * 6;
        
        real32 DrawX = -SCREEN_WIDTH / 2.0f + PosX;
        real32 DrawY = SCREEN_HEIGHT / 2.0f - PosY;
        
        for(int Index = 0;
            Index < TextLength;
            ++Index)
        {
            if(Text[Index] == '\n')
            {
                DrawX = -SCREEN_WIDTH / 2.0f + PosX;
                DrawY -= 20.0f;
                continue;
            }
            
            uint32 Character = (int)Text[Index] - ASCII_MAPPING_DEBUT_CURSOR;
            
            if(Character == 0)
            {
                DrawX += 3.0f;
                continue;
            }
            
            Vertices[Index*4] = VertexFont(DrawX, DrawY, 0.5f, FormattedColor, Fonts[Character].left, 0.0f);
            Vertices[Index*4 + 1] = VertexFont(DrawX + Fonts[Character].size, DrawY, 0.5f, FormattedColor, Fonts[Character].right, 0.0f);
            Vertices[Index*4 + 2] = VertexFont(DrawX + Fonts[Character].size, DrawY - 16, 0.5f, FormattedColor,  Fonts[Character].right, 1.0f);
            Vertices[Index*4 + 3] = VertexFont(DrawX, DrawY - 16, 0.5f, FormattedColor, Fonts[Character].left, 1.0f);
            
            uint32 VertexGroup = Index * 4;
            uint32 IndexGroup = Index * 6;
            Indices[IndexGroup]     = VertexGroup;
            Indices[IndexGroup + 1] = VertexGroup + 1;
            Indices[IndexGroup + 2] = VertexGroup + 2;
            Indices[IndexGroup + 3] = VertexGroup;
            Indices[IndexGroup + 4] = VertexGroup + 2;
            Indices[IndexGroup + 5] = VertexGroup + 3;
            
            DrawX += Fonts[Character].size + 1.0f;
        }
        
        g_pd3dDeviceContext->Unmap(FontMesh.vertexBuffer, 0);
        g_pd3dDeviceContext->Unmap(FontMesh.indexBuffer, 0);
    }
    
    {
        g_pd3dDeviceContext->VSSetShader(g_shaderfont.vs, 0, 0);
        g_pd3dDeviceContext->PSSetShader(g_shaderfont.ps, 0, 0);
        g_pd3dDeviceContext->IASetInputLayout(g_shaderfont.layout);
        
        uint32 stride = sizeof(vertex_font);
        uint32 offset = 0;
        g_pd3dDeviceContext->IASetIndexBuffer(FontMesh.indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        g_pd3dDeviceContext->IASetVertexBuffers(0, 1, &FontMesh.vertexBuffer, &stride, &offset);
        g_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        
        g_pd3dDeviceContext->PSSetShaderResources(0, 1, &FontTexture.view);
        g_pd3dDeviceContext->PSSetSamplers(0, 1, &FontTexture.sampler);
        
        //g_pd3dDeviceContext->OMSetDepthStencilState(g_depthStencilStateDisabled, 1);
        
        real32 blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        g_pd3dDeviceContext->OMSetBlendState(g_blendingEnable, blendFactor, 0xFFFFFFFF);
        
        g_pd3dDeviceContext->DrawIndexed(FontMesh.indicesSize, 0, 0);
    }
    
}

#endif //FONT_H
