/* date = July 19th 2022 0:09 pm */

#ifndef TEXTURE_H
#define TEXTURE_H

#pragma pack(push, 1)
// NOTE: There's not really a use for the header right now, maybe later
struct DDS_PIXELFORMAT
{
    DWORD dwSize;
    DWORD dwFlags;
    DWORD dwFourCC;
    DWORD dwRGBBitCount;
    DWORD dwRBitMask;
    DWORD dwGBitMask;
    DWORD dwBBitMask;
    DWORD dwABitMask;
};

struct DDS_HEADER
{
    DWORD           dwSize;
    DWORD           dwFlags;
    DWORD           dwHeight;
    DWORD           dwWidth;
    DWORD           dwPitchOrLinearSize;
    DWORD           dwDepth;
    DWORD           dwMipMapCount;
    DWORD           dwReserved1[11];
    DDS_PIXELFORMAT ddspf;
    DWORD           dwCaps;
    DWORD           dwCaps2;
    DWORD           dwCaps3;
    DWORD           dwCaps4;
    DWORD           dwReserved2;
};

struct DDS_HEADER_DXT10
{
    DXGI_FORMAT              dxgiFormat;
    D3D10_RESOURCE_DIMENSION resourceDimension;
    UINT                     miscFlag;
    UINT                     arraySize;
    UINT                     miscFlags2;
};
#pragma pack(pop)

struct dds_image
{
    DDS_HEADER header;
    uint32     size;
    uint32     pitch;
    void*      data;
};

internal d3d_texture
LoadTexture(char *FileName)
{
    file File = Win32_ReadEntireFile(FileName);
    Assert(*(DWORD *)File.memory == 0x20534444); // NOTE: dds magic word
    
    void* Data = File.memory;
    
    dds_image Image;
    d3d_texture TextureHandler = {};
    
    DDS_HEADER *Header = (DDS_HEADER *)((uint8 *)Data + sizeof(DWORD));
    
    if(Header->ddspf.dwFlags == 0x4 && Header->ddspf.dwFourCC == 'DX10') // DDPF_FOURCC
    {
        // TODO: not surpported
        return TextureHandler;
    }
    
    uint32 Pitch;
    //if(Header->ddspf.dwFourCC == '1TXD')
    {
        // Pitch = max(1, ((Header->dwWidth + 3)/4))*8;
        Pitch = (Header->dwWidth * 32 + 7)/8;
    }
    
    Image.header = *Header;
    Image.pitch = Pitch;
    Image.size = Header->dwHeight*Image.pitch;
    Image.data = (uint8 *)Data + sizeof(DDS_HEADER);
    
    ID3D11Texture2D *Texture;
    D3D11_TEXTURE2D_DESC desc; 
    desc.Width = Image.header.dwWidth;
    desc.Height = Image.header.dwHeight;
    desc.MipLevels = 1; 
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    
    D3D11_SUBRESOURCE_DATA initData;
    initData.pSysMem = Image.data;
    initData.SysMemPitch = Image.pitch;
    initData.SysMemSlicePitch = 0;
    
    g_pd3dDevice->CreateTexture2D(&desc, &initData, &Texture);
    
    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
    SRVDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MipLevels = 1;
    
    g_pd3dDevice->CreateShaderResourceView(Texture, &SRVDesc, &TextureHandler.view);
    
    D3D11_SAMPLER_DESC sampDesc = {};
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    sampDesc.MinLOD = 0;
    sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
    
    g_pd3dDevice->CreateSamplerState(&sampDesc, &TextureHandler.sampler);
    
    Win32_FreeFile(&File);
    Texture->Release();
    
    return TextureHandler;
}

#endif //TEXTURE_H
