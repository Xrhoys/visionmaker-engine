/* date = July 28th 2022 1:42 pm */

#ifndef DEBUG_H
#define DEBUG_H

#define MAX_LIGHTS 8

global entity          Registry[100] = {};
global entity*         SelectedEntity;
global bool32          ReachedMaxEntity = FALSE;

global light           Lights[MAX_LIGHTS] = {};
global light*          SelectedLight = {};
global bool32          ReachedMaxLight = FALSE;

global mesh            Cube;
global mesh            Sphere;

global ImVec4           SelectedColor = ImVec4(*BLUE.X, *BLUE.Y, *BLUE.Z, *BLUE.W);
internal void
AddEntityToRegistry(mesh *Mesh, entity_type Type, entity_draw_mode Mode = DRAW_MODE_TRIANGLE)
{
    for(uint32 Index = 0;
        Index < 100;
        ++Index)
    {
        if(!Registry[Index].isSet)
        {
            light_material Material =
            {
                V4(0.48f, 0.77f, 0.66f,  1.0f),
                V4(0.48f, 0.77f, 0.46f,  1.0f),
                V4( 0.2f,  0.2f,  0.2f, 16.0f),
                V4( 0.2f,  0.2f,  0.2f, 16.0f),
            };
            
            Registry[Index] = Entities(Mesh, &Material, Type, Mode);
            return;
        }
    }
}

internal void
AddLight(light_type Type)
{
    light Light;
    
    Light.type = Type;
    
    for(uint32 Index = 0;
        Index < MAX_LIGHTS;
        ++Index)
    {
        if(!Lights[Index].enabled)
        {
            light Light = {};
            
            Light.type = Type;
            Light.enabled = TRUE;
            
            Lights[Index] = Light;
            return;
        }
    }
}

internal void
InitImGui(HWND hwnd)
{
    // NOTE: Imgui setup
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& Io = ImGui::GetIO(); (void)Io;
        
        ImGui::StyleColorsDark();
        
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    }
    
    
    // NOTE: 3D Entities, fixed amount of entities that can be added at runtime
    Cube   = InitCube();
    Sphere = InitSphere(100, 100, 5.0f);
    
    {
        light_material CubeMaterial =
        {
            V4(1.0f, 1.0f, 1.0f, 1.0f),
            V4(1.0f, 1.0f, 1.0f, 1.0f),
            V4(1.0f, 1.0f, 1.0f, 1.0f),
            V4(1.0f, 1.0f, 1.0f, 1.0f),
        };
        
        light_material SphereMaterial =
        {
            V4(0.48f, 0.77f, 0.66f,  1.0f),
            V4(0.48f, 0.77f, 0.46f,  1.0f),
            V4( 0.2f,  0.2f,  0.2f,  1.0f),
            V4( 0.2f,  0.2f,  0.2f,  1.0f),
        };
        
        Registry[0] = Entities(&Cube, &CubeMaterial, ENTITY_CUBE);
        Registry[1] = Entities(&Sphere, &SphereMaterial, ENTITY_SPHERE);
        
        // NOTE: Initial scene setup for demo purpose
        entity *CubeEntity = &Registry[0];
        CubeEntity->position = V3(0.0f, 0.0f, 0.0f);
        CubeEntity->scale    = V3(200.0f, 1.0f, 200.0f); 
        
        entity *SphereEntity = &Registry[1];
        SphereEntity->position = V3(0.0f, 20.0f, 0.0f);
        SphereEntity->scale    = V3(0.5f, 0.5f, 0.5f);
    }
};

inline void
RenderImGui()
{
    // NOTE: Update lighting for demo !! DEBUG DEMO ONLY
    {
        for(uint32 Index = 0;
            Index < MAX_LIGHTS;
            ++Index)
        {
            g_cbPerFrame.g_Lights[Index] = Lights[Index];
        }
        
        g_pd3dDeviceContext->UpdateSubresource(g_frameCBuffer, 0, NULL, &g_cbPerFrame, 0, 0);
    }
    
    ImGui::SetNextFrameWantCaptureMouse(!ClipMouseMode);
    ImGui::SetNextFrameWantCaptureKeyboard(!ClipMouseMode);
    
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
    
    //ImGui::ShowStackToolWindow();
    //ImGui::ShowDebugLogWindow();
    
    if(ImGui::Begin("Controls"))
    {
        ImGui::Text("FPS: %.2f", 1000.0f/System.Timer.elapsed);
        ImGui::Text("Frame time: %.2f", System.Timer.elapsed);
        ImGui::Text("Internal time: %.2f", System.Timer.time);
        
        ImGui::Separator();
        
        ImGui::Text("Mouse position: X:%d ; Y:%d", System.cursorPosition.x, System.cursorPosition.y);
        
        ImGui::Separator();
        
        ImGui::DragFloat3("Camera Position", (real32 *)&Camera.position);
        ImGui::DragFloat("Camera Pitch", &Camera.yaw, 0.01f, -Pi, Pi);
        ImGui::DragFloat("Camera Yaw", &Camera.pitch, 0.01f, -Pi, Pi);
        
        ImGui::Separator();
        
        if(ImGui::BeginListBox("Entity list (max. 100)"))
        {
            for(uint32 Index = 0;
                Index < 100;
                ++Index)
            {
                if(Registry[Index].isSet)
                {
                    char Buffer[20];
                    _snprintf_s(Buffer, sizeof(Buffer), "%s-%d", GetEntityName(Registry[Index].type), Index);
                    
                    bool32 TempSet = FALSE;
                    if(SelectedEntity == &Registry[Index])
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, SelectedColor);
                        TempSet = TRUE;
                    }
                    
                    if(ImGui::Selectable(Buffer))
                    {
                        SelectedEntity = &Registry[Index];
                    }
                    
                    if(TempSet)
                    {
                        ImGui::PopStyleColor();
                    }
                }
            }
			ImGui::EndListBox();
        }
        
        
        ImGui::Separator();
        
        if(ImGui::Button("Add Cube"))
        {
            AddEntityToRegistry(&Cube, ENTITY_CUBE);
        }
        
        if(ImGui::Button("Add Sphere"))
        {
            AddEntityToRegistry(&Sphere, ENTITY_SPHERE);
        }
        
        if(ReachedMaxEntity)
        {
            ImGui::Text("Reached max entity size.");
        }
        
        ImGui::Separator();
        
        if(ImGui::BeginListBox("Light list (max. 8)"))
        {
            for(uint32 Index = 0;
                Index < MAX_LIGHTS;
                ++Index)
            {
                if(Lights[Index].enabled)
                {
                    char Buffer[20];
                    _snprintf_s(Buffer, sizeof(Buffer), "%s-%d", GetLightName(Lights[Index].type), Index);
                    
                    bool32 TempSet = FALSE;
                    if(SelectedLight == &Lights[Index])
                    {
                        ImGui::PushStyleColor(ImGuiCol_Text, SelectedColor);
                        TempSet = TRUE;
                    }
                    
                    if(ImGui::Selectable(Buffer))
                    {
                        SelectedLight = &Lights[Index];
                    }
                    
                    if(TempSet)
                    {
                        ImGui::PopStyleColor();
                    }
                }
            }
			ImGui::EndListBox();
        }
        
        if(ImGui::Button("Add directional light"))
        {
            AddLight(LIGHT_DIRECTIONAL);
        }
        
        if(ImGui::Button("Add point light"))
        {
            AddLight(LIGHT_POINT);
        }
        
        if(ImGui::Button("Add spot light"))
        {
            AddLight(LIGHT_SPOT);
        }
        
        if(ReachedMaxLight)
        {
            ImGui::Text("Reached max light count.");
        }
        
    }
    ImGui::End();
    
    if(ImGui::Begin("Entity Properties"))
    {
        if(SelectedEntity)
        {
            ImGui::Text("Type: %s", GetEntityName(SelectedEntity->type));
            ImGui::DragFloat3("Position", (real32 *)&SelectedEntity->position);
            ImGui::DragFloat3("Rotation", (real32 *)&SelectedEntity->rotation);
            ImGui::DragFloat3("Size", (real32 *)&SelectedEntity->scale);
            ImGui::Separator();
            
            ImGui::Text("Material");
            
            ImGui::ColorEdit4("Ambient", (real32 *)&SelectedEntity->material.ambient);
            ImGui::ColorEdit4("Diffuse", (real32 *)&SelectedEntity->material.diffuse);
            ImGui::ColorEdit4("Specular", (real32 *)&SelectedEntity->material.specular);
            ImGui::ColorEdit4("Reflect", (real32 *)&SelectedEntity->material.reflect);
            
            ImGui::Separator();
            
            if(ImGui::Button("Remove"))
            {
                FreeEntity(SelectedEntity);
                SelectedEntity = NULL;
            }
        }
        else
        {
            ImGui::Text("Nothing selected");
        }
    }
    ImGui::End();
    
    if(ImGui::Begin("Light Properties"))
    {
        if(SelectedLight)
        {
            ImGui::Text("Type: %s", GetLightName(SelectedLight->type));
            if(SelectedLight->type == LIGHT_SPOT || SelectedLight->type == LIGHT_POINT)
            {
                ImGui::DragFloat3("Position", (real32 *)&SelectedLight->position);
                ImGui::DragFloat("Range", (real32 *)&SelectedLight->range);
            }
            if(SelectedLight->type == LIGHT_DIRECTIONAL)
            {
                ImGui::DragFloat3("Direction", (real32 *)&SelectedLight->direction);
            }
            if(SelectedLight->type == LIGHT_SPOT)
            {
                ImGui::DragFloat("Cone angle", (real32 *)&SelectedLight->spot);
            }
            
            ImGui::Separator();
            
            ImGui::Text("Material");
            ImGui::ColorEdit4("Ambient", (real32 *)&SelectedLight->ambient);
            ImGui::ColorEdit4("Diffuse", (real32 *)&SelectedLight->diffuse);
            ImGui::ColorEdit4("Specular", (real32 *)&SelectedLight->specular);
            if(SelectedLight->type == LIGHT_SPOT || SelectedLight->type == LIGHT_POINT)
            {
                ImGui::ColorEdit3("Attenuation", (real32 *)&SelectedLight->att);
            }
            
            ImGui::Separator();
            
            if(ImGui::Button("Remove"))
            {
                SelectedLight->enabled = FALSE;
                SelectedLight = NULL;
            }
        }
        else
        {
            ImGui::Text("Nothing selected");
        }
    }
    ImGui::End();
    
    if(ImGui::Begin("HELP"))
    {
        ImGui::Text("[Space] to switch control mode");
        ImGui::Text("[W], [A], [S], [D] to move around");
        
        ImGui::Text("[Esc] to quit");
        ImGui::Separator();
        
        ImGui::Text("Current Camera Mode:");
        if(ClipMouseMode)
        {
            ImGui::Text("Freecam");
        }
        else
        {
            ImGui::Text("Editing");
        }
    }
    ImGui::End();
    
#if 0
    // NOTE: maybe useful later, renders to texture 
    if(ImGui::Begin("Scene", NULL, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Image((void *)g_shaderResourceViewMap, ImVec2(1280, 720));
    }
    ImGui::End();
#endif
    
    ImGui::Render();
    
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    const real32 clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
    // g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
};

#endif //DEBUG_H
