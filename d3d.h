#ifndef D3D_H
#define D3D_H

struct d3d_texture
{
    // ID3D11Texture2D          *texture;
    ID3D11ShaderResourceView *view;
    ID3D11SamplerState       *sampler;
};

global ID3D11Device*               g_pd3dDevice = NULL;
global ID3D11DeviceContext*        g_pd3dDeviceContext = NULL;
global IDXGISwapChain*             g_pSwapChain = NULL;
global ID3D11RenderTargetView*     g_mainRenderTargetView = NULL;
global DXGI_SAMPLE_DESC            Sampling = { 1, 0 };

global ID3D11DepthStencilView*     g_depthStencilView = NULL;
global ID3D11DepthStencilState*    g_depthStencilState = NULL;
global ID3D11DepthStencilState*    g_depthStencilStateDisabled = NULL;

global ID3D11RasterizerState*      g_CCWcullMode;
global ID3D11RasterizerState*      g_CWcullMode;

global ID3D11BlendState*           g_transparency;
global ID3D11BlendState*           g_blendingEnable;
global ID3D11BlendState*           g_blendingDisable;

global d3d_shader                  g_shader2d;
global d3d_shader                  g_shader3d;
global d3d_shader                  g_shader3d_geomesh;
global d3d_shader                  g_shaderfont;

global ID3D11Texture2D*            g_renderTargetTextureMap = NULL;
global ID3D11RenderTargetView*     g_renderTargetViewMap = NULL;
global ID3D11ShaderResourceView*   g_shaderResourceViewMap = NULL;

internal d3d_shader
LoadCompiledShader(LPCWSTR VSPath,
                   LPCWSTR PSPath,
                   D3D11_INPUT_ELEMENT_DESC *Layout,
                   uint32 LayoutSize)
{
    d3d_shader Shader;
    
    ID3DBlob *VS_ShaderBlob;
    if(D3DReadFileToBlob(VSPath, &VS_ShaderBlob) != S_OK)
    {
        // TODO: Logging
    }
    void *VS_Data = (void *)VS_ShaderBlob->GetBufferPointer();
    uint32 VS_DataSize = VS_ShaderBlob->GetBufferSize();
    
    if(g_pd3dDevice->CreateVertexShader(VS_Data, VS_DataSize, 0, &Shader.vs) != S_OK)
    {
        // TODO: Logging
    }
    
    if(g_pd3dDevice->CreateInputLayout(Layout, LayoutSize, 
                                       VS_Data, VS_DataSize, 
                                       &Shader.layout) != S_OK)
    {
        // TODO: Logging
    }
    
    ID3DBlob *PS_ShaderBlob;
    if(D3DReadFileToBlob(PSPath, &PS_ShaderBlob) != S_OK)
    {
        // TODO: Logging
    }
    void *PS_Data = (void *)PS_ShaderBlob->GetBufferPointer();
    uint32 PS_DataSize = PS_ShaderBlob->GetBufferSize();
    
    if(g_pd3dDevice->CreatePixelShader(PS_Data, PS_DataSize, 0, &Shader.ps) != S_OK)
    {
        // TODO: Logging
    }
    
    Shader.layoutSize = LayoutSize;
    
    VS_ShaderBlob->Release();
    PS_ShaderBlob->Release();
    
    return Shader;
}

internal bool32 
CreateDeviceD3D(HWND hWnd)
{
    {
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 2;
        sd.BufferDesc.Width = 0;
        sd.BufferDesc.Height = 0;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60; // TODO: query monitor refresh rate
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hWnd;
        sd.SampleDesc = Sampling;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        
        UINT createDeviceFlags = 0;
        
#if DEBUG
        createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
        
        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevelArray[2] = 
        { 
            D3D_FEATURE_LEVEL_11_0, 
            D3D_FEATURE_LEVEL_10_0, 
        };
        
        if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 
                                          createDeviceFlags, featureLevelArray, 2,
                                          D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice,
                                          &featureLevel, &g_pd3dDeviceContext) != S_OK)
        {
            return FALSE;
        }
    }
    
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    
    {
        uint32 Width, Height;
        
        D3D11_TEXTURE2D_DESC Desc;
        pBackBuffer->GetDesc(&Desc);
        
        D3D11_TEXTURE2D_DESC dd;
        dd.Width = Desc.Width;
        dd.Height = Desc.Height;
        dd.MipLevels = 1;
        dd.ArraySize = 1;
        dd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dd.SampleDesc = Sampling;
        dd.Usage = D3D11_USAGE_DEFAULT;
        dd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        dd.CPUAccessFlags = 0;
        dd.MiscFlags = 0;
        
        ID3D11Texture2D* g_depthStencilBuffer = NULL;
        g_pd3dDevice->CreateTexture2D(&dd, NULL, &g_depthStencilBuffer);
        
        D3D11_DEPTH_STENCIL_VIEW_DESC dvd = {};
        dvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        dvd.Texture2D.MipSlice = 0;
        
        g_pd3dDevice->CreateDepthStencilView(g_depthStencilBuffer, &dvd, &g_depthStencilView);
        
        D3D11_DEPTH_STENCIL_DESC dsd = {};
        dsd.DepthEnable = TRUE;
        dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsd.DepthFunc = D3D11_COMPARISON_LESS;
        
        dsd.StencilEnable = TRUE;
        dsd.StencilReadMask = 0xFF;
        dsd.StencilWriteMask = 0xFF;
        
        // NOTE: Stencil operations if pixel is front-facing.
        dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        
        // NOTE: Stencil operations if pixel is back-facing.
        dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        
        g_pd3dDevice->CreateDepthStencilState(&dsd, &g_depthStencilState);
        // deviceContext->OMSetDepthStencilState(g_depthStencilState, 1);
        
        // NOTE: Second depth stencil disabling Z depth
        dsd.DepthEnable = FALSE;
        dsd.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dsd.DepthFunc = D3D11_COMPARISON_LESS;
        dsd.StencilEnable = TRUE;
        dsd.StencilReadMask = 0xFF;
        dsd.StencilWriteMask = 0xFF;
        dsd.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsd.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
        dsd.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsd.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        dsd.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        dsd.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
        dsd.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        dsd.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        
        g_pd3dDevice->CreateDepthStencilState(&dsd, &g_depthStencilStateDisabled);
        g_depthStencilBuffer->Release();
    }
    pBackBuffer->Release();
    // NOTE: render to back buffer
    // g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, g_depthStencilView);
    
    {
        D3D11_VIEWPORT viewport = {};
        viewport.TopLeftX = 0;
        viewport.TopLeftY = 0;
        viewport.Width = SCREEN_WIDTH;
        viewport.Height = SCREEN_HEIGHT;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        
        g_pd3dDeviceContext->RSSetViewports(1, &viewport);
    }
    
    {
        D3D11_INPUT_ELEMENT_DESC Layout_2d[] = 
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(vertex_2d, pos), D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(vertex_2d, color), D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        
        g_shader2d = LoadCompiledShader(L"fxc/shader2d_vs.fxc", L"fxc/shader2d_ps.fxc", 
                                        (D3D11_INPUT_ELEMENT_DESC *)Layout_2d, 2);
        
        D3D11_INPUT_ELEMENT_DESC Layout_3d[] = 
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(vertex_mesh, pos), D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(vertex_mesh, color), D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(vertex_mesh, normal), D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(vertex_mesh, tex), D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        
        g_shader3d = LoadCompiledShader(L"fxc/shader3d_vs.fxc", L"fxc/shader3d_ps.fxc", 
                                        (D3D11_INPUT_ELEMENT_DESC *)Layout_3d, 4);
        
        D3D11_INPUT_ELEMENT_DESC Layout_3dGeoMesh[] = 
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(vertex_geomesh, pos), D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(vertex_geomesh, normal), D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        
        g_shader3d_geomesh = LoadCompiledShader(L"fxc/shader3dgeo_vs.fxc", L"fxc/shader3dgeo_ps.fxc", 
                                                (D3D11_INPUT_ELEMENT_DESC *)Layout_3dGeoMesh, 2);
        
        D3D11_INPUT_ELEMENT_DESC Layout_font[] = 
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(vertex_font, pos), D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(vertex_font, color), D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(vertex_font, tex), D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        
        g_shaderfont  = LoadCompiledShader(L"fxc/shaderfont_vs.fxc", L"fxc/shaderfont_ps.fxc", 
                                           (D3D11_INPUT_ELEMENT_DESC *)Layout_font, 3);
    }
    
    {
        // NOTE: Constant buffer per object
        D3D11_BUFFER_DESC cbbd = {};
        cbbd.Usage = D3D11_USAGE_DEFAULT;
        cbbd.ByteWidth = sizeof(d3d_c_buffer_object);
        cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbbd.MiscFlags = 0;
        cbbd.CPUAccessFlags = 0;
        if(g_pd3dDevice->CreateBuffer(&cbbd, NULL, &g_objectCBuffer) != S_OK)
        {
            // TODO: logging
        }
        
        // NOTE: Constant buffer per object geomesh
        cbbd.Usage = D3D11_USAGE_DEFAULT;
        cbbd.ByteWidth = sizeof(d3d_c_buffer_object_geomesh);
        cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbbd.MiscFlags = 0;
        cbbd.CPUAccessFlags = 0;
        if(g_pd3dDevice->CreateBuffer(&cbbd, NULL, &g_objectGeoCBuffer) != S_OK)
        {
            // TODO: logging
        }
        
        // NOTE: Constant buffer per frame
        cbbd.Usage = D3D11_USAGE_DEFAULT;
        cbbd.ByteWidth = sizeof(d3d_c_buffer_frame);
        cbbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        cbbd.MiscFlags = 0;
        cbbd.CPUAccessFlags = 0;
        if(g_pd3dDevice->CreateBuffer(&cbbd, NULL, &g_frameCBuffer) != S_OK)
        {
            // TODO: logging
        }
    }
    
    {
        // NOTE: culling profiles
        D3D11_RASTERIZER_DESC cmdesc = {};
        cmdesc.FillMode = D3D11_FILL_SOLID;
        cmdesc.CullMode = D3D11_CULL_BACK;
        
        cmdesc.FrontCounterClockwise = TRUE;
        g_pd3dDevice->CreateRasterizerState(&cmdesc, &g_CCWcullMode);
        
        cmdesc.FrontCounterClockwise = FALSE;
        g_pd3dDevice->CreateRasterizerState(&cmdesc, &g_CWcullMode);
    }
    
    {
        // NOTE: Set blending states
        D3D11_BLEND_DESC blendDesc = {};
        D3D11_RENDER_TARGET_BLEND_DESC rtbd = {};
        
        rtbd.BlendEnable           = TRUE;
        rtbd.SrcBlend              = D3D11_BLEND_SRC_COLOR;
        rtbd.DestBlend             = D3D11_BLEND_BLEND_FACTOR;
        rtbd.BlendOp               = D3D11_BLEND_OP_ADD;
        rtbd.SrcBlendAlpha         = D3D11_BLEND_ONE;
        rtbd.DestBlendAlpha        = D3D11_BLEND_ZERO;
        rtbd.BlendOpAlpha          = D3D11_BLEND_OP_ADD;
        rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;
        
        blendDesc.AlphaToCoverageEnable = false;
        blendDesc.RenderTarget[0] = rtbd;
        
        g_pd3dDevice->CreateBlendState(&blendDesc, &g_transparency);
        
        // NOTE: Alpha enabled blend state
        blendDesc = {};
        rtbd = {};
        
        rtbd.BlendEnable           = TRUE;
        rtbd.SrcBlend              = D3D11_BLEND_ONE;
        rtbd.DestBlend             = D3D11_BLEND_INV_SRC_ALPHA;
        rtbd.BlendOp               = D3D11_BLEND_OP_ADD;
        rtbd.SrcBlendAlpha         = D3D11_BLEND_ONE;
        rtbd.DestBlendAlpha        = D3D11_BLEND_ZERO;
        rtbd.BlendOpAlpha          = D3D11_BLEND_OP_ADD;
        rtbd.RenderTargetWriteMask = 0x0f;
        
        blendDesc.RenderTarget[0] = rtbd;
        
        g_pd3dDevice->CreateBlendState(&blendDesc, &g_blendingEnable);
        
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
        
        g_pd3dDevice->CreateBlendState(&blendDesc, &g_blendingDisable);
    }
    
    {
        // NOTE: Render to texture
        D3D11_TEXTURE2D_DESC textureDesc = {};
        textureDesc.Width = SCREEN_WIDTH;
        textureDesc.Height = SCREEN_HEIGHT;
        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        textureDesc.SampleDesc = Sampling;
        textureDesc.Usage = D3D11_USAGE_DEFAULT;
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        textureDesc.CPUAccessFlags = 0;
        textureDesc.MiscFlags = 0;
        
        g_pd3dDevice->CreateTexture2D(&textureDesc, NULL, &g_renderTargetTextureMap);
        
        D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc = {};
        renderTargetViewDesc.Format = textureDesc.Format;
        renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        renderTargetViewDesc.Texture2D.MipSlice = 0;
        
        g_pd3dDevice->CreateRenderTargetView(g_renderTargetTextureMap, &renderTargetViewDesc, &g_renderTargetViewMap);
        
        D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc = {};
        shaderResourceViewDesc.Format = textureDesc.Format;
        shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
        shaderResourceViewDesc.Texture2D.MipLevels = 1;
        
        g_pd3dDevice->CreateShaderResourceView(g_renderTargetTextureMap, &shaderResourceViewDesc, &g_shaderResourceViewMap);
    }
    
    return TRUE;
}

internal void 
CleanupDeviceD3D()
{
    if (g_mainRenderTargetView) 
    { 
        g_mainRenderTargetView->Release(); 
        g_mainRenderTargetView = NULL; 
    }
    
    if (g_pSwapChain) 
    { 
        g_pSwapChain->Release(); 
        g_pSwapChain = NULL; 
    }
    if (g_pd3dDeviceContext) 
    { 
        g_pd3dDeviceContext->Release(); 
        g_pd3dDeviceContext = NULL; 
    }
    if (g_pd3dDevice) 
    { 
        g_pd3dDevice->Release(); 
        g_pd3dDevice = NULL; 
    }
    
    g_depthStencilView->Release();
    g_depthStencilView = NULL;
    
    g_depthStencilState->Release();
    g_depthStencilState = NULL;
    
    g_depthStencilStateDisabled->Release();
    g_depthStencilStateDisabled = NULL;
    
    g_CCWcullMode->Release();
    g_CCWcullMode;
    
    g_CWcullMode->Release();
    g_CWcullMode;
    
    g_transparency->Release();
    g_transparency;
    
    g_blendingEnable->Release();
    g_blendingEnable;
    
    g_blendingDisable->Release();
    g_blendingDisable;
    
    g_renderTargetTextureMap->Release();
    g_renderTargetTextureMap = NULL;
    
    g_renderTargetViewMap->Release();
    g_renderTargetViewMap = NULL;
    
    g_shaderResourceViewMap->Release();
    g_shaderResourceViewMap = NULL;
}

internal void 
ClearBackground(color *color)
{
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (real32 *)color);
    
    g_pd3dDeviceContext->ClearDepthStencilView(g_depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
}

internal bool32 
BeginDrawing(void)
{
    bool32 Result = FALSE;
    if (System.Timer.elapsed > System.Timer.maxFrameTime)
    {
        // NOTE: Initiate 3d mode with all its transforming matrices
        // ClearBackground(BLACK);
        
#if DEBUG
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, g_depthStencilView);
        
        real32 *FormattedColor = (real32 *)&GetFormattedColor(BLACK);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, FormattedColor);
        
        g_pd3dDeviceContext->ClearDepthStencilView(g_depthStencilView, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
#endif 
        
        // NOTE: reset rendering states
        real32 blendFactor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        g_pd3dDeviceContext->OMSetBlendState(g_blendingDisable, blendFactor, 0xFFFFFFFF);
        
        Result = TRUE;
    }
    
    LARGE_INTEGER EndCounter;
    QueryPerformanceCounter(&EndCounter);
    int64 CounterElapsed = EndCounter.QuadPart - System.Timer.lastCounter.QuadPart;
    
    LARGE_INTEGER Frequency;
    QueryPerformanceFrequency(&Frequency);
    real32 Elapsed = 1000.0f*(real32)CounterElapsed/(real32)Frequency.QuadPart;
    System.Timer.elapsed += Elapsed;
    System.Timer.time += Elapsed;
    
    System.Timer.lastCounter = EndCounter;
    
    return Result;
}

internal void
EndDrawing(void)
{
    g_pSwapChain->Present(0, 0);
    
    System.Timer.elapsed = 0;
}

internal void 
BeginMode2D()
{
    g_pd3dDeviceContext->VSSetShader(g_shader2d.vs, 0, 0);
    g_pd3dDeviceContext->PSSetShader(g_shader2d.ps, 0, 0);
    g_pd3dDeviceContext->IASetInputLayout(g_shader2d.layout);
    
    // NOTE: Set constant buffer for orthogonal projection
    Matrix Identity = MatrixIdentity();
    g_cbPerObject.g_world = Identity;
    g_cbPerObject.g_wvp = Camera.invProjection2d;
    
    g_pd3dDeviceContext->UpdateSubresource(g_objectCBuffer, 0, NULL, &g_cbPerObject, 0, 0);
    g_pd3dDeviceContext->VSSetConstantBuffers(0, 1, &g_objectCBuffer);
    
    g_pd3dDeviceContext->RSSetState(g_CWcullMode);
}

internal void 
EndMode2D(void)
{
    
}


void BeginMode3D()
{
    // NOTE: probably not appropriate
    g_pd3dDeviceContext->VSSetShader(g_shader3d_geomesh.vs, 0, 0);
    g_pd3dDeviceContext->PSSetShader(g_shader3d_geomesh.ps, 0, 0);
    g_pd3dDeviceContext->IASetInputLayout(g_shader3d_geomesh.layout);
    
    Matrix Identity = MatrixIdentity();
    Camera.wvp = Identity * Camera.view * Camera.projection3d;
    // g_cbPerObject.g_wvp = Transpose(&Camera.wvp);
    g_cbPerObjectGeo.g_wvp = Transpose(&Camera.wvp);
    g_cbPerFrame.g_eyePosW  = Camera.position;
    
    // g_pd3dDeviceContext->PSSetConstantBuffers(0, 1, &g_objectGeoCBuffer);
    // g_pd3dDeviceContext->UpdateSubresource(g_frameCBuffer, 0, NULL, &g_cbPerFrame, 0, 0);
    
    g_pd3dDeviceContext->PSSetConstantBuffers(1, 1, &g_frameCBuffer);
}

void EndMode3D(void)
{
    
}

/*
void BeginTextureMode(RenderTexture2D target);
void EndTextureMode(void);
void BeginShaderMode(Shader shader);
void EndShaderMode(void);
void BeginBlendMode(int mode);
void EndBlendMode(void);
*/

internal void
D3DLoadMesh(mesh *mesh,
            vertex_mesh *Vertices, uint32 VerticesCount, 
            uint32 *Indices,       uint32 IndicesCount,
            bool32 IsDynamic = FALSE)
{
    D3D11_BUFFER_DESC VertexBufferDesc = {};
    if (IsDynamic)
    {
        VertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        VertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    } 
    else
    {
        VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        VertexBufferDesc.CPUAccessFlags = 0;
    }
    VertexBufferDesc.ByteWidth = sizeof(vertex_mesh) * VerticesCount;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.MiscFlags = 0;
    
    D3D11_SUBRESOURCE_DATA VertexBufferData = {};
    VertexBufferData.pSysMem = Vertices;
    g_pd3dDevice->CreateBuffer(&VertexBufferDesc, &VertexBufferData, &mesh->vertexBuffer);
    
    D3D11_BUFFER_DESC IndexBufferDesc = {};
    if (IsDynamic)
    {
        IndexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        IndexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    } 
    else
    {
        IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        IndexBufferDesc.CPUAccessFlags = 0;
    }
    IndexBufferDesc.ByteWidth = sizeof(uint32) * IndicesCount;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.MiscFlags = 0;
    
    D3D11_SUBRESOURCE_DATA IndexBufferData = {};
    IndexBufferData.pSysMem = Indices;
    g_pd3dDevice->CreateBuffer(&IndexBufferDesc, &IndexBufferData, &mesh->indexBuffer);
    
    mesh->verticesSize = VerticesCount;
    mesh->indicesSize = IndicesCount;
}


internal void
D3DLoadMesh(mesh *mesh,
            vertex_geomesh *Vertices, uint32 VerticesCount, 
            uint32 *Indices,          uint32 IndicesCount,
            bool32 IsDynamic = FALSE)
{
    D3D11_BUFFER_DESC VertexBufferDesc = {};
    if (IsDynamic)
    {
        VertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        VertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    } 
    else
    {
        VertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        VertexBufferDesc.CPUAccessFlags = 0;
    }
    VertexBufferDesc.ByteWidth = sizeof(vertex_geomesh) * VerticesCount;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.MiscFlags = 0;
    
    D3D11_SUBRESOURCE_DATA VertexBufferData = {};
    VertexBufferData.pSysMem = Vertices;
    g_pd3dDevice->CreateBuffer(&VertexBufferDesc, &VertexBufferData, &mesh->vertexBuffer);
    
    D3D11_BUFFER_DESC IndexBufferDesc = {};
    if (IsDynamic)
    {
        IndexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        IndexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    } 
    else
    {
        IndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        IndexBufferDesc.CPUAccessFlags = 0;
    }
    IndexBufferDesc.ByteWidth = sizeof(uint32) * IndicesCount;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.MiscFlags = 0;
    
    D3D11_SUBRESOURCE_DATA IndexBufferData = {};
    IndexBufferData.pSysMem = Indices;
    g_pd3dDevice->CreateBuffer(&IndexBufferDesc, &IndexBufferData, &mesh->indexBuffer);
    
    mesh->verticesSize = VerticesCount;
    mesh->indicesSize = IndicesCount;
}

internal void
D3DLoadMesh(mesh *mesh,
            vertex_2d *Vertices, uint32 VerticesCount, 
            uint32 *Indices,     uint32 IndicesCount,
            bool32 IsDynamic = FALSE)
{
    D3D11_BUFFER_DESC VertexBufferDesc = {};
    if (IsDynamic)
    {
        VertexBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;
        VertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    } 
    else
    {
        VertexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
        VertexBufferDesc.CPUAccessFlags = 0;
    }
    VertexBufferDesc.ByteWidth = sizeof(vertex_2d) * VerticesCount;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.MiscFlags = 0;
    
    D3D11_SUBRESOURCE_DATA VertexBufferData = {};
    VertexBufferData.pSysMem = Vertices;
    
    if (g_pd3dDevice->CreateBuffer(&VertexBufferDesc, 
                                   &VertexBufferData, 
                                   &mesh->vertexBuffer) != S_OK)
    {
        // TODO: logging
    }
    
    D3D11_BUFFER_DESC IndexBufferDesc = {};
    if (IsDynamic)
    {
        IndexBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;
        IndexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    } 
    else
    {
        IndexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
        IndexBufferDesc.CPUAccessFlags = 0;
    }
    IndexBufferDesc.ByteWidth = sizeof(uint32) * IndicesCount;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.MiscFlags = 0;
    
    D3D11_SUBRESOURCE_DATA IndexBufferData = {};
    IndexBufferData.pSysMem = Indices;
    
    if (g_pd3dDevice->CreateBuffer(&IndexBufferDesc,
                                   &IndexBufferData, 
                                   &mesh->indexBuffer) == S_OK)
    {
        // TODO: logging
    }
    
    mesh->verticesSize = VerticesCount;
    mesh->indicesSize  = IndicesCount;
}

internal void
D3DLoadMesh(mesh *mesh,
            vertex_font *Vertices, uint32 VerticesCount, 
            uint32 *Indices,       uint32 IndicesCount,
            bool32 IsDynamic = FALSE)
{
    D3D11_BUFFER_DESC VertexBufferDesc = {};
    if (IsDynamic)
    {
        VertexBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;
        VertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    } 
    else
    {
        VertexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
        VertexBufferDesc.CPUAccessFlags = 0;
    }
    VertexBufferDesc.ByteWidth = sizeof(vertex_font) * VerticesCount;
    VertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VertexBufferDesc.MiscFlags = 0;
    
    D3D11_SUBRESOURCE_DATA VertexBufferData = {};
    VertexBufferData.pSysMem = Vertices;
    
    if (g_pd3dDevice->CreateBuffer(&VertexBufferDesc, 
                                   &VertexBufferData, 
                                   &mesh->vertexBuffer) != S_OK)
    {
        // TODO: logging
    }
    
    D3D11_BUFFER_DESC IndexBufferDesc = {};
    if (IsDynamic)
    {
        IndexBufferDesc.Usage          = D3D11_USAGE_DYNAMIC;
        IndexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    } 
    else
    {
        IndexBufferDesc.Usage          = D3D11_USAGE_DEFAULT;
        IndexBufferDesc.CPUAccessFlags = 0;
    }
    IndexBufferDesc.ByteWidth = sizeof(uint32) * IndicesCount;
    IndexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    IndexBufferDesc.MiscFlags = 0;
    
    D3D11_SUBRESOURCE_DATA IndexBufferData = {};
    IndexBufferData.pSysMem = Indices;
    
    if (g_pd3dDevice->CreateBuffer(&IndexBufferDesc,
                                   &IndexBufferData, 
                                   &mesh->indexBuffer) == S_OK)
    {
        // TODO: logging
    }
    
    mesh->verticesSize = VerticesCount;
    mesh->indicesSize  = IndicesCount;
}

internal void
TestLight()
{
    light Ld;
    {
        Ld.ambient     = V4(0.2f, 0.2f, 0.2f, 0.2f);
        Ld.diffuse     = V4(0.1f, 0.1f, 0.1, 1.0f);
        Ld.specular    = V4(0.1f, 0.1f, 0.1f, 1.0f);
        Ld.direction   = V3(0.0f, -20.0f, 0.0f);
    }
    
    light Ls;
    {
        Ls.ambient     = V4(1.0f, 1.0f, 1.0f, 1.0f);
        Ls.diffuse     = V4(1.0f, 1.0f, 1.0f, 1.0f);
        Ls.specular    = V4(1.0f, 1.0f, 1.0f, 1.0f);
        Ls.att         = V3(1.0f, 1.0f, 1.0f);
        Ls.spot        = 96.0f;
        Ls.range       = 100.0f;
    }
    
    light Lp;
    {
        Lp.ambient     = V4(0.3f, 0.3f, 0.3f, 1.0f);
        Lp.diffuse     = V4(0.7f, 0.7f, 0.7f, 1.0f);
        Lp.specular    = V4(0.7f, 0.7f, 0.7f, 1.0f);
        Lp.att         = V3(0.0f, 0.1f, 0.0f);
        Lp.position    = V3(0.0f, 0.0f, 0.0f);
        Lp.range       = 1000.0f;
    }
    
    g_cbPerFrame.g_Lights[0]   = Ld;
    g_cbPerFrame.g_Lights[1]   = Lp;
    g_cbPerFrame.g_Lights[2]   = Ls;
}

inline void
DrawGeomeshT(mesh *Mesh)
{
    uint32 stride = sizeof(vertex_geomesh);
    uint32 offset = 0;
    
    g_pd3dDeviceContext->IASetIndexBuffer(Mesh->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    g_pd3dDeviceContext->IASetVertexBuffers(0, 1, &Mesh->vertexBuffer, &stride, &offset);
    g_pd3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    g_pd3dDeviceContext->OMSetDepthStencilState(g_depthStencilState, 1);
    
    g_pd3dDeviceContext->RSSetState(g_CCWcullMode);
    g_pd3dDeviceContext->DrawIndexed(Mesh->indicesSize, 0, 0);
    g_pd3dDeviceContext->RSSetState(g_CWcullMode);
    g_pd3dDeviceContext->DrawIndexed(Mesh->indicesSize, 0, 0);
}

inline void
DrawGeomeshL(mesh *Mesh)
{
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

inline void
WindowResize(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
    {
        if (g_mainRenderTargetView) 
        {
            g_mainRenderTargetView->Release(); 
            g_mainRenderTargetView = NULL; 
            
            g_depthStencilView->Release();
            g_depthStencilView = NULL;
        }
        
        UINT Width = (UINT)LOWORD(lParam);
        UINT Height = (UINT)HIWORD(lParam);
        g_pSwapChain->ResizeBuffers(0, Width, Height, DXGI_FORMAT_UNKNOWN, 0);
        
        ID3D11Texture2D* pBackBuffer;
        g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
        
        D3D11_TEXTURE2D_DESC dd;
        dd.Width = Width;
        dd.Height = Height;
        dd.MipLevels = 1;
        dd.ArraySize = 1;
        dd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dd.SampleDesc = Sampling;
        dd.Usage = D3D11_USAGE_DEFAULT;
        dd.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        dd.CPUAccessFlags = 0;
        dd.MiscFlags = 0;
        
        ID3D11Texture2D* g_depthStencilBuffer = NULL;
        g_pd3dDevice->CreateTexture2D(&dd, NULL, &g_depthStencilBuffer);
        
        D3D11_DEPTH_STENCIL_VIEW_DESC dvd = {};
        dvd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        dvd.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        dvd.Texture2D.MipSlice = 0;
        
        g_pd3dDevice->CreateDepthStencilView(g_depthStencilBuffer, &dvd, &g_depthStencilView);
        
        pBackBuffer->Release();
    }
}

#endif //D3D_H