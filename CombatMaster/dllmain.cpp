#include <Windows.h>
#include <Xinput.h>
#include <chrono>
#include <cmath>
#include <cstdlib>  // For rand() and srand()
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include "renderer.h"
#include "sdk.h"
#pragma warning(disable : 4995)
#pragma comment(lib, "XInput.lib")

int STEPS = 100;
constexpr float PI = 3.14159265358979323846f;
float FOVRadius = 200.0f;
bool aimPosEnabled = true;
bool espEnabled = true;
bool noRecoil = true;
bool noSway = false;
bool controller = true;
bool legit = false;
float healthLine_Spacing = 10;
float healthLine_Thickness = 10;
void UpdateAim(float x, float y, float stepX, float stepY, bool legit) {
    if (!legit) {
        mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(x), static_cast<DWORD>(y), NULL, NULL);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    else {
        for (int i = 0; i < STEPS; ++i) {
            mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(stepX), static_cast<DWORD>(stepY), NULL, NULL);
        }
    }
}

bool isInCircle(int x_center, int y_center, int x_player, int y_player, int radius) {
    double distance = sqrt(pow((x_player - x_center), 2) + pow((y_player - y_center), 2));
    if (distance <= radius) {
        return true;
    }
    return false;
}

using Present = HRESULT(__stdcall*)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

Present oPresent;
HWND window = nullptr;
ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pContext = nullptr;
ID3D11RenderTargetView* mainRenderTargetView = nullptr;
std::vector<UnityVector3> roots;

void draw_gui() {}
void AimAtPos(float x, float y) {
    if (aimPosEnabled) {
        ImVec2 screenSize = ImGui::GetIO().DisplaySize;
        float ScreenCenterX = screenSize.x / 2.f;
        float ScreenCenterY = screenSize.y / 2.f;
        float AimSpeed = 3;
        float TargetX = 0;
        float TargetY = 0;

        // X Axis
        if (x != 0) {
            if (x > ScreenCenterX) {
                TargetX = -(ScreenCenterX - x);
                TargetX /= AimSpeed;
                if (TargetX + ScreenCenterX > ScreenCenterX * 2)
                    TargetX = 0;
            }

            if (x < ScreenCenterX) {
                TargetX = x - ScreenCenterX;
                TargetX /= AimSpeed;
                if (TargetX + ScreenCenterX < 0)
                    TargetX = 0;
            }
        }

        // Y Axis

        if (y != 0) {
            if (y > ScreenCenterY) {
                TargetY = -(ScreenCenterY - y);
                TargetY /= AimSpeed;
                if (TargetY + ScreenCenterY > ScreenCenterY * 2)
                    TargetY = 0;
            }

            if (y < ScreenCenterY) {
                TargetY = y - ScreenCenterY;
                TargetY /= AimSpeed;
                if (TargetY + ScreenCenterY < 0)
                    TargetY = 0;
            }
        }

        if (isInCircle(ScreenCenterX, ScreenCenterY, x, y, FOVRadius)) {
            // Calculate step size for X and Y
            int stepX = (TargetX) / STEPS;
            int stepY = (TargetY) / STEPS;

            // Loop to move the mouse gradually
            if (legit) {
                UpdateAim(TargetX, TargetX, stepX, stepY, legit);
            }

            // Final adjustment to ensure the mouse reaches the exact
            // target position
            mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(TargetX), static_cast<DWORD>(TargetY), NULL, NULL);
        }
        else {
        }
    }
}

bool ispremium_hook(void* thisptr) {
    *(bool*)((uintptr_t)thisptr + 0x42) = false;
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
        pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
        pBackBuffer->Release();

        ImGui::End();
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(window);
        ImGui_ImplDX11_Init(pDevice, pContext);

        // ImGui rendering loop
        // End ImGui frame

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

            // Iterate through all players
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

                // Health calculation
                float healthPercent = CombatMaster::PlayerHealth::GetPercent(playerHealth);
                if (healthPercent <= 0.f || healthPercent > 1.f)
                    continue;

                bool is_ai = CombatMaster::PlayerRoot::IsRealPlayer(playerRoot);
                int teamId = CombatMaster::PlayerRoot::GetTeamId(playerRoot);

                // Color determination
                ImU32 color = IM_COL32_RED;
                if (is_ai && teamId != myPlayerTeamId)
                    color = IM_COL32_BLACK;
                else if (is_ai && teamId == myPlayerTeamId)
                    color = IM_COL32_WHITE;
                else if (teamId == myPlayerTeamId)
                    color = IM_COL32_GREEN;

                // Head and foot screen positions
                uintptr_t headTransform = CombatMaster::PlayerMobView::GetHeadTransform(activeMobView);
                if (!headTransform)
                    continue;

                UnityVector3 headScreenPosition = {};
                UnityVector3 footScreenPosition = {};

                UnityVector3 headPosition = CombatMaster::Transform::GetPosition(headTransform);
                headPosition.y += 0.2f;
                UnityVector3 footPosition = headPosition;
                footPosition.y -= 1.6f;

                if (!CombatMaster::Camera::WorldToScreenPoint(myMainCamera, headPosition, headScreenPosition) || !CombatMaster::Camera::WorldToScreenPoint(myMainCamera, footPosition, footScreenPosition))
                    continue;

                // Distance calculation
                float distance = CombatMaster::Vector3::GetDistance(myPlayerControllerPosition, footPosition);
                if (distance > 150.f)
                    continue;

                roots.push_back(headScreenPosition);

                // Drawing ESP box and text
                float h = footScreenPosition.y - headScreenPosition.y;
                float w = fabsf(h / 3.8f);
                uintptr_t bones[] = {
                    activeMobView + 0x80,  // left wrist
                    activeMobView + 0x48,  // mid chest
                    activeMobView + 0x58,  // left knee
                    activeMobView + 0x60,  // right knee
                    activeMobView + 0x30,  // head
                    activeMobView + 0x38,  // neck
                    activeMobView + 0x50   // pelvis
                };
                if (espEnabled) {
                    for (int i = 0; i < sizeof(bones) / sizeof(bones[0]); i++) {
                        for (int j = i + 1; j < sizeof(bones) / sizeof(bones[0]); j++) {
                            uintptr_t bone1 = *(uintptr_t*)(bones[i]);
                            uintptr_t bone2 = *(uintptr_t*)(bones[j]);

                            if (!bone1 || !bone2)
                                continue;

                            UnityVector3 pos1 = CombatMaster::Transform::GetPosition(bone1);
                            UnityVector3 pos2 = CombatMaster::Transform::GetPosition(bone2);

                            renderer::drawLine(ImVec2(pos1.x, pos1.y), ImVec2(pos2.x, pos2.y), ImColor(255, 255, 255, 255));
                        }
                    }
                }
                renderer::drawLine(ImVec2(footScreenPosition.x - w, footScreenPosition.y), ImVec2(footScreenPosition.x + w, footScreenPosition.y), color);
                renderer::drawLine(ImVec2(footScreenPosition.x - w, footScreenPosition.y - h), ImVec2(footScreenPosition.x + w, footScreenPosition.y - h), color);
                renderer::drawLine(ImVec2(footScreenPosition.x - w, footScreenPosition.y), ImVec2(footScreenPosition.x - w, footScreenPosition.y - h), color);
                renderer::drawLine(ImVec2(footScreenPosition.x + w, footScreenPosition.y), ImVec2(footScreenPosition.x + w, footScreenPosition.y - h), color);

                // Health bar
                int health = healthPercent * 100;
                float maxHealth = 100.0f;
                float lineHeight = footScreenPosition.y - headScreenPosition.y;
                float healthRatio = health / maxHealth;
                float healthY = footScreenPosition.y - healthRatio * lineHeight;
                for (int i = 0; i < healthLine_Thickness; ++i) {
                    renderer::drawLine(ImVec2(footScreenPosition.x + healthLine_Spacing + i + w, footScreenPosition.y), ImVec2(footScreenPosition.x + healthLine_Spacing + i + w, healthY), IM_COL32_WHITE);
                }

                char healthString[32] = {};
                sprintf_s(healthString, "Health: %d%%", health);  // Assuming healthPercent is a value between 0 and 1
                const auto healthStringLength = ImGui::CalcTextSize(healthString);
                float healthX = footScreenPosition.x - (healthStringLength.x / 2);
                healthY = footScreenPosition.y - lineHeight + 5;  // Adjust as needed
                renderer::drawText(ImVec2(healthX, healthY), color, healthString);

                char distanceString[32] = {};
                sprintf_s(distanceString, "%.1fm", distance);
                const auto distanceStringLength = ImGui::CalcTextSize(distanceString);
                footScreenPosition.x -= (distanceStringLength.x / 2);
                footScreenPosition.y += (distanceStringLength.y / 2);
                renderer::drawText(ImVec2(footScreenPosition.x, footScreenPosition.y), color, distanceString);

                char isAiString[32] = {};
                sprintf_s(isAiString, "Is ai?: %s", is_ai ? "true" : "false");
                const auto isAiStringLength = ImGui::CalcTextSize(isAiString);
                footScreenPosition.x -= (isAiStringLength.x / 2);
                footScreenPosition.y += (distanceStringLength.y / 2) + 5;
                renderer::drawText(ImVec2(footScreenPosition.x, footScreenPosition.y), color, isAiString);
            }

            // Drawing bones

            // Controller input handling
            XInputEnable(true);
            XINPUT_STATE state;
            ZeroMemory(&state, sizeof(XINPUT_STATE));
            DWORD result = XInputGetState(0, &state);

            if (GetAsyncKeyState(VK_XBUTTON2) || GetAsyncKeyState(VK_XBUTTON1) || (controller && state.Gamepad.bRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)) {
                float MaxDist = FLT_MAX;
                ImVec2 wtf = ImGui::GetIO().DisplaySize;
                UnityVector3 screenCenter = {};
                screenCenter.x = wtf.x / 2.f;
                screenCenter.y = wtf.y / 2.f;
                UnityVector3 closestVec = {};

                // Find closest vector for aiming
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
        }
        ImGui::Render();

        pContext->OMSetRenderTargets(1, &mainRenderTargetView, nullptr);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    }

    return oPresent(pSwapChain, SyncInterval, Flags);
}

uintptr_t original_set_target_recoil{ NULL };

void __cdecl hk_set_target_recoil(void* thisptr, float recoil) {
    return reinterpret_cast<decltype(&hk_set_target_recoil)>(original_set_target_recoil)(thisptr, 0.f);
}

uintptr_t original_test{ NULL };

void __cdecl hk_weapon_sway_update(void* thisptr) {}

DWORD WINAPI ThreadStartRoutine(LPVOID lpThreadParameter) {
    FILE* console = nullptr;
    if (AllocConsole())
        freopen_s(&console, "CONOUT$", "w", stdout);

    CombatMaster::GameAssembly = (uintptr_t)GetModuleHandle(L"project.dll");

    MH_Initialize();
    if (init(kiero::RenderType::D3D11) == kiero::Status::Success) {
        kiero::bind(8, (void**)&oPresent, hkPresent);

        auto lpTarget = (LPVOID)(CombatMaster::GameAssembly + 0x38C19E0);
        MH_CreateHook(lpTarget, hk_weapon_sway_update, (LPVOID*)&original_set_target_recoil);
        if (noSway) {
            MH_EnableHook(lpTarget);
        }

        lpTarget = (LPVOID)(CombatMaster::GameAssembly + 0x38318B0);
        MH_CreateHook(lpTarget, hk_set_target_recoil, (LPVOID*)&original_set_target_recoil);
        if (noRecoil) {
            MH_EnableHook(lpTarget);
        }

        lpTarget = (LPVOID)(CombatMaster::GameAssembly + 0x1BE7CE0);
        MH_CreateHook(lpTarget, ispremium_hook, (LPVOID*)&original_test);
        MH_EnableHook(lpTarget);
    }

    while (!(GetAsyncKeyState(VK_END) & 0x8000))
        Sleep(50);

    kiero::shutdown();

    if (console) {
        fclose(console);
        FreeConsole();
    }

    FreeLibraryAndExitThread(static_cast<HMODULE>(lpThreadParameter), 0);
    return 0;
}
DWORD WINAPI GUIStartRoutine(LPVOID lpParam) {
    HINSTANCE hInstance = reinterpret_cast<HINSTANCE>(lpParam);

    // Your GUI initialization code here
    // For example:
    MessageBox(nullptr, L"GUI Thread Started!", L"Thread Info", MB_OK | MB_ICONINFORMATION);

    while (true) {
        ImGui::Begin("Cheat GUI");

        // Checkbox for aimPosEnabled
        ImGui::Checkbox("Aim Position", &aimPosEnabled);
        if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            aimPosEnabled = !aimPosEnabled;  // Toggle the value
        }

        // Checkbox for espEnabled
        ImGui::Checkbox("ESP", &espEnabled);
        if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            espEnabled = !espEnabled;  // Toggle the value
        }

        // Checkbox for noRecoil
        ImGui::Checkbox("No Recoil", &noRecoil);
        if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            noRecoil = !noRecoil;  // Toggle the value
        }

        // Checkbox for noSway
        ImGui::Checkbox("No Sway", &noSway);
        if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            noSway = !noSway;  // Toggle the value
        }

        // Checkbox for controller
        ImGui::Checkbox("Controller", &controller);
        if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            controller = !controller;  // Toggle the value
        }

        // Checkbox for legit
        ImGui::Checkbox("Legit", &legit);
        if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
            legit = !legit;  // Toggle the value
        }

        ImGui::End();  // Sleep for 5 seconds

        // Your GUI cleanup code here if needed
    }
    return 0;
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        HINSTANCE hInstance = GetModuleHandle(nullptr);

        // Creating the thread
        /*const auto gThread = CreateThread(
                nullptr, 0, &GUIStartRoutine,
                hInstance, 0, nullptr);*/ // Gui code doesnt work... too lazy to fix
        const auto hThread = CreateThread(nullptr, 0, &ThreadStartRoutine, hModule, 0, nullptr);
        FILE* fDummy;
        int i;

        AllocConsole();
        freopen_s(&fDummy, "CONIN$", "r", stdin);
        freopen_s(&fDummy, "CONOUT$", "w", stderr);
        freopen_s(&fDummy, "CONOUT$", "w", stdout);

        system("cls");
        system("title DrexWareX");
        
       
   
         
std::cout << R"( 
					 ____                         __      __                             __   __     
					/\  _`\                      /\ \  __/\ \                           /\ \ /\ \    
					\ \ \/\ \  _ __    __   __  _\ \ \/\ \ \ \     __     _ __    __    \ `\`\/'/'   
					 \ \ \ \ \/\`'__\/'__`\/\ \/'\\ \ \ \ \ \ \  /'__`\  /\`'__\/'__`\   `\/ > <     
					  \ \ \_\ \ \ \//\  __/\/>  </ \ \ \_/ \_\ \/\ \L\.\_\ \ \//\  __/      \/'/\`\  
					   \ \____/\ \_\\ \____\/\_/\_\ \ `\___x___/\ \__/.\_\\ \_\\ \____\     /\_\\ \_\
						\/___/  \/_/ \/____/\//\/_/  '\/__//__/  \/__/\/_/ \/_/ \/____/     \/_/ \/_/
                              Made by ドレクシー published on github and uc                                                   
                                                                                 
			)";
                                                                                


        std::cout << "ONLY INJECT IN GAME IS DETECTED IN LOBBY\n";
        std::cout << "Press end after game (u will have to inject again)\n";
        if (hThread) {
            int x;
            std::cin >> x;

            while (true) {
                printf("Settings\n");
                printf("Aimbot is currently %s\n", aimPosEnabled ? "enabled" : "disabled");
                printf("Esp is currently %s\n", espEnabled ? "enabled" : "disabled");
                printf("AntiRecoil is currently %s\n", noRecoil ? "enabled" : "disabled");
                printf("No sway is currently %s\n", noSway ? "enabled" : "disabled");
                printf("Legit mode is currently %s\n", legit ? "enabled" : "disabled");
                int option;
                std::cout << "\nWhat would you like to change (1-5 "
                    "in order or 6 for continue to "
                    "cheat) ";  // no flush needed
                std::cin >> option;
                if (option == 1) {
                    aimPosEnabled = !aimPosEnabled;
                }
                if (option == 2) {
                    int esp;
                    printf("Enter health line thickness: ");
                    std::cin >> esp;
                    healthLine_Thickness = esp;
                    int c;
                    printf("disable esp (1 for yes 0 for no)");
                    std::cin >> c;
                    if (c == 1) {
                        espEnabled = false;
                    }
                    else if (c == 2)
                        ;
                    { espEnabled = true; }
                }
                if (option == 3) {
                    noRecoil = !noRecoil;
                }

                if (option == 4) {
                    noSway = !noSway;
                }
                if (option == 5) {
                    legit = !legit;
                }
                if (option == 6) {
                    break;
                }

                DisableThreadLibraryCalls(hModule);
                CloseHandle(hThread);
            }
        }

        return TRUE;
    }
}
