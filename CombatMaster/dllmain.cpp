#include <Windows.h>

#include <iostream>

#include <string>

#include <vector>

#include "sdk.h"

#include "renderer.h"

#include <cmath>
#include <Xinput.h>

#include <cstdlib> // For rand() and srand()
#include <ctime> 
#pragma warning(disable : 4995)
#pragma comment(lib, "XInput.lib")
int STEPS = 100;
constexpr float PI = 3.14159265358979323846f;
float FOVRadius = 200.0f;
// Declare a global variable to store the FOV radius
void updateFOV(float* pFOV) {
    FOVRadius = *pFOV;
}

bool aimPosEnabled = true;
bool espEnabled = true;
bool noRecoil = true;
bool noSway = false;
bool controller = false;

bool isInCircle(int x_center, int y_center, int x_player, int y_player, int radius) {
    double distance = sqrt(pow((x_player - x_center), 2) + pow((y_player - y_center), 2));
    if (distance <= radius) {
        return true;
    } else {
        return false;
    }
}


typedef HRESULT(__stdcall * Present)(IDXGISwapChain * pSwapChain, UINT SyncInterval, UINT Flags);

Present oPresent;
HWND window = NULL;
ID3D11Device * pDevice = nullptr;
ID3D11DeviceContext * pContext = nullptr;
ID3D11RenderTargetView * mainRenderTargetView = nullptr;
std::vector < UnityVector3 > roots;

void AimAtPos(float x, float y) {
    if (aimPosEnabled) {
        
        ImVec2 screenSize = ImGui::GetIO().DisplaySize;
        float ScreenCenterX = screenSize.x / 2.f;
        float ScreenCenterY = screenSize.y / 2.f;
        float AimSpeed = 5;
        float TargetX = 0;
        float TargetY = 0;
      
        
        //X Axis
        if (x != 0) {
            if (x > ScreenCenterX) {
                TargetX = -(ScreenCenterX - x);
                TargetX /= AimSpeed;
                if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
            }

            if (x < ScreenCenterX) {
                TargetX = x - ScreenCenterX;
                TargetX /= AimSpeed;
                if (TargetX + ScreenCenterX < 0) TargetX = 0;
            }
        }

        //Y Axis

        if (y != 0) {
            if (y > ScreenCenterY) {
                TargetY = -(ScreenCenterY - y);
                TargetY /= AimSpeed;
                if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
            }

            if (y < ScreenCenterY) {
                TargetY = y - ScreenCenterY;
                TargetY /= AimSpeed;
                if (TargetY + ScreenCenterY < 0) TargetY = 0;
            }
        }

        if (isInCircle(ScreenCenterX, ScreenCenterY, x,y , FOVRadius)) {
            
            
            
            
            // Calculate step size for X and Y
            int stepX = (TargetX) / STEPS;
            int stepY = (TargetY) / STEPS;

            // Loop to move the mouse gradually
            for (int i = 0; i < STEPS; ++i) {
               
                   mouse_event(MOUSEEVENTF_MOVE, (DWORD)(stepX), (DWORD)(stepY), NULL, NULL);
                   Sleep(0.01);
            }

                // Final adjustment to ensure the mouse reaches the exact target position
                mouse_event(MOUSEEVENTF_MOVE, (DWORD)(TargetX), (DWORD)(TargetY), NULL, NULL);
            
        }
        else {

        }
    }
}

bool ispremium_hook(void * thisptr) {
    *(bool * )((uintptr_t) thisptr + 0x42) = false;
    return false;
}

HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags) {
    static bool isInitialized = false;

    if (!isInitialized) {
       
        pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice);
        pDevice->GetImmediateContext(&pContext);
        DXGI_SWAP_CHAIN_DESC sd;
        pSwapChain->GetDesc(&sd);
        window = sd.OutputWindow;
        ID3D11Texture2D* pBackBuffer;
        pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
        pDevice->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
        pBackBuffer->Release();

        ImGui::CreateContext();
        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(pDevice, pContext);

        isInitialized = true;
    }
    else if (CombatMaster::GameAssembly) {
        const auto myPlayerRoot = CombatMaster::PlayerRoot::GetMyPlayer();
        const auto myMainCamera = CombatMaster::PlayerRoot::GetMainCamera(myPlayerRoot);
        const auto myPlayerTeamId = CombatMaster::PlayerRoot::GetTeamId(myPlayerRoot);
        const auto myActiveMobView = CombatMaster::PlayerRoot::GetActiveMobView(myPlayerRoot);
        const auto myPlayerControllerTransform = CombatMaster::PlayerMobView::GetHeadTransform(myActiveMobView);
        const auto myPlayerControllerPosition = CombatMaster::Transform::GetPosition(myPlayerControllerTransform);
        const auto allPlayers = CombatMaster::PlayerRoot::GetAllPlayers();
        
        
        
        roots.clear();

        if (myPlayerRoot && myMainCamera && myActiveMobView && myPlayerControllerTransform && allPlayers) {
            ImGui_ImplDX11_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
            
            
            // Create a slider to adjust STEPS
            
            
                for (int i = 0; i < allPlayers->size; ++i) {
                    uintptr_t playerRoot = allPlayers->items->m_Items[i];
                    if (!playerRoot)
                        continue;

                    if (playerRoot == myPlayerRoot)
                        continue;


                    uintptr_t playerHealth = CombatMaster::PlayerRoot::GetHealth(playerRoot);
                    if (!playerHealth)
                        continue;

                    uintptr_t activeMobView = CombatMaster::PlayerRoot::GetActiveMobView(playerRoot);
                    if (!activeMobView)
                        continue;

                    float healthPercent = CombatMaster::PlayerHealth::GetPercent(playerHealth);
                    if (healthPercent <= 0.f || healthPercent > 1.f)
                        continue;
                    bool is_ai = CombatMaster::PlayerRoot::IsRealPlayer(playerRoot);
                    
                    int teamId = CombatMaster::PlayerRoot::GetTeamId(playerRoot);
                    
                    ImU32 color = IM_COL32_RED;
                    if (is_ai && teamId != myPlayerTeamId) {
                        color = IM_COL32_BLACK;
                    }
                    else if (is_ai && teamId == myPlayerTeamId) {
                        color = IM_COL32_WHITE;


                    }
                    
                    else if (teamId == myPlayerTeamId) {
                        color = IM_COL32_GREEN;
                    }
                    
                    

                    
                   
                    uintptr_t headTransform = CombatMaster::PlayerMobView::GetHeadTransform(activeMobView);
                    
                    
                    if (!headTransform)
                        continue;

                    UnityVector3 headScreenPosition = {};
                    UnityVector3 footScreenPosition = {};

                    UnityVector3 headPosition = CombatMaster::Transform::GetPosition(headTransform);
                    
                    headPosition.y += 0.2f;
                    UnityVector3 footPosition = headPosition;
                    footPosition.y -= 1.6f;

                    if (!CombatMaster::Camera::WorldToScreenPoint(myMainCamera, headPosition, headScreenPosition))
                        continue;
                    if (!CombatMaster::Camera::WorldToScreenPoint(myMainCamera, footPosition, footScreenPosition))
                        continue;


                    float distance = CombatMaster::Vector3::GetDistance(myPlayerControllerPosition, footPosition);

                    if (distance > 150.f)
                        continue;

                    roots.push_back(headScreenPosition);

                    float h = footScreenPosition.y - headScreenPosition.y;
                    float w = fabsf(h / 3.8f);

                    // 2D BOX
                   
                    if (espEnabled) {
                        ImVec2 wtf = ImGui::GetIO().DisplaySize;
                        renderer::drawCircle(wtf.x, wtf.y, FOVRadius);
                        renderer::drawLine(ImVec2(footScreenPosition.x - w, footScreenPosition.y), ImVec2(footScreenPosition.x + w, footScreenPosition.y), color);
                        renderer::drawLine(ImVec2(footScreenPosition.x - w, footScreenPosition.y - h), ImVec2(footScreenPosition.x + w, footScreenPosition.y - h), color);
                        renderer::drawLine(ImVec2(footScreenPosition.x - w, footScreenPosition.y), ImVec2(footScreenPosition.x - w, footScreenPosition.y - h), color);
                        renderer::drawLine(ImVec2(footScreenPosition.x + w, footScreenPosition.y), ImVec2(footScreenPosition.x + w, footScreenPosition.y - h), color);

                    }
                    
                    char distanceString[32] = {};
                    sprintf_s(distanceString, "%.1fm", distance);
                    const auto distanceStringLength = ImGui::CalcTextSize(distanceString);
                    footScreenPosition.x -= (distanceStringLength.x / 2);
                    footScreenPosition.y += (distanceStringLength.y / 2);
                    renderer::drawText(ImVec2(footScreenPosition.x, footScreenPosition.y), color, distanceString);
                    


                    // Draw circle using lines
                    
                    // fuck 68h b0h 28h 78h 40h 28h 20h
                    // uintptr_t unkTran = *(uintptr_t*)(activeMobView + 0x80); // left wrist
                    // uintptr_t unkTran = *(uintptr_t*)(activeMobView + 0x48); // mid chest
                    // 
                    // uintptr_t unkTran = *(uintptr_t*)(activeMobView + 0x58); // left knee
                    // uintptr_t unkTran = *(uintptr_t*)(activeMobView + 0x60); // right knee

                    //uintptr_t unkTran = *(uintptr_t*)(activeMobView + 0x30); // head
                    // uintptr_t unkTran = *(uintptr_t*)(activeMobView + 0x38); // neck
                    // uintptr_t unkTran = *(uintptr_t*)(activeMobView + 0x50); // pelvis
                    //missing ankles, shoulders, elbows
                    /*if (!unkTran)
                        continue                                                                                                                                        ;
                    Vector3 test = CombatMaster::Transform::GetPosition(unkTran);*/
                }
                XInputEnable(true);
                XINPUT_STATE state;
                ZeroMemory(&state, sizeof(XINPUT_STATE));
                DWORD result = XInputGetState(0, &state);
                
                if (GetAsyncKeyState(VK_XBUTTON2) or GetAsyncKeyState(VK_XBUTTON1) or controller && state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) {
                    float MaxDist = FLT_MAX;
                    ImVec2 wtf = ImGui::GetIO().DisplaySize;
                    
                    UnityVector3 screenCenter = {};
                    screenCenter.x = wtf.x / 2.f;
                    screenCenter.y = wtf.y / 2.f;

                    UnityVector3 closestVec = {};
                    
                    for (const auto& occ : roots) {
                        
                        float num = screenCenter.x - occ.x;
                        float num2 = screenCenter.y - occ.y;
                        float dist = (sqrtf(num * num + num2 * num2));

                        if (dist < MaxDist) {
                            MaxDist = dist;
                            closestVec = occ;
                            
                        }
                    }

                    roots.clear();

                    AimAtPos(closestVec.x, closestVec.y);
                }

                ImGui::Render();

                pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
                ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
            }
        }

        return oPresent(pSwapChain, SyncInterval, Flags);
    }
 


    
        uintptr_t original_set_target_recoil{
            NULL
        };
        void __cdecl hk_set_target_recoil(void* thisptr, float recoil) {
            return reinterpret_cast <decltype(&hk_set_target_recoil)> (original_set_target_recoil)(thisptr, 0.f);
        }
    
    
        uintptr_t original_test{
        NULL
        };

        void __cdecl hk_weapon_sway_update(void* thisptr) {
            return;
        }


DWORD WINAPI ThreadStartRoutine(LPVOID lpThreadParameter) {
    FILE * console = nullptr;
    if (AllocConsole())
        freopen_s( & console, "CONOUT$", "w", stdout);

    CombatMaster::GameAssembly = (uintptr_t) GetModuleHandle(L"project.dll");

    MH_Initialize();
    if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success) {
       
        kiero::bind(8, (void ** ) & oPresent, hkPresent);
        
        LPVOID lpTarget = (LPVOID)(CombatMaster::GameAssembly + 0x38C19E0);
        MH_CreateHook(lpTarget, hk_weapon_sway_update, (LPVOID * ) & original_set_target_recoil);
        if (noSway) {
            MH_EnableHook(lpTarget);
        }

        lpTarget = (LPVOID)(CombatMaster::GameAssembly + 0x38318B0);
        MH_CreateHook(lpTarget, hk_set_target_recoil, (LPVOID * ) & original_set_target_recoil);
        if (noRecoil) {
            MH_EnableHook(lpTarget);
        }

        lpTarget = (LPVOID)(CombatMaster::GameAssembly + 0x1BE7CE0);
        MH_CreateHook(lpTarget, ispremium_hook, (LPVOID * ) & original_test);
        //MH_EnableHook(lpTarget)                                                                                                                                   ;
    }

    while (!(GetAsyncKeyState(VK_END) & 0x8000))
        Sleep(50);

    kiero::shutdown();
 

  
    if (console) {
        fclose(console);
        FreeConsole();
    }


    FreeLibraryAndExitThread((HMODULE) lpThreadParameter, 0);
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        const auto hThread = CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE) & ThreadStartRoutine, (LPVOID) hModule, 0, nullptr);
        FILE* fDummy;
        int i;
        
        AllocConsole();
        freopen_s(&fDummy, "CONIN$", "r", stdin);
        freopen_s(&fDummy, "CONOUT$", "w", stderr);
        freopen_s(&fDummy, "CONOUT$", "w", stdout);


        std::cout << "Welcome to drexware";
        std::cout << "Press f1 in game (only works when tabbed into terminal and injected while in game)\n";
        std::cout << "Press end after game (u will have to inject again)\n";
        if (hThread) {
            
            while (true) {
            
                if (GetKeyState(VK_F1)) {
                    break;
                }
            }
            /*
            while (true) {
               

                printf("Aimbot is currently "+ aimPosEnabled);
                printf("Settings\n");
                printf("Aimbot is currently "+ aimPosEnabled);
                printf( "Esp is currently "+ espEnabled);
                printf("AntiRecoil is currently "+ noRecoil);
                printf("No sway is currently ");
                int option;
                std::cout << "What would you like to change (1-4 in order or 5 for continue to cheat) "; // no flush needed
                std::cin >> option;
                if (option == 1) {
                    aimPosEnabled = !aimPosEnabled;

                }
                if (option == 2) {
                    espEnabled = !espEnabled;


                }
                if (option == 3) {
                    noRecoil = !noRecoil;

                }

                if (option == 4) {
                    noSway = !noSway;

                }
                if (option == 5) {
                    break;

                }
            } */
           

            DisableThreadLibraryCalls(hModule);
            CloseHandle(hThread);
        }
    }

    return TRUE;
}