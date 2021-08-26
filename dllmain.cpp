// dllmain.cpp : Defines the entry point for the DLL application
#include "includes.h"
configuration cfg;
void* d3d9Device[119];
BYTE EndSceneBytes[7]{ 0 };
tEndScene oEndScene = nullptr;
LPDIRECT3DDEVICE9 pDevice = nullptr;
WNDPROC oWndProc;

int windowX = GetSystemMetrics(SM_CXSCREEN);
int windowY = GetSystemMetrics(SM_CYSCREEN);;   // X-Y

bool WorldTooScreen(Vec3 pos, Vec2& screen, float matrix[16], int width, int height) {
    Vec4 clipCoords;
    clipCoords.x = pos.x * matrix[0] + pos.y * matrix[1] + pos.z * matrix[2] + matrix[3];
    clipCoords.y = pos.x * matrix[4] + pos.y * matrix[5] + pos.z * matrix[6] + matrix[7];
    clipCoords.z = pos.x * matrix[8] + pos.y * matrix[9] + pos.z * matrix[10] + matrix[11];
    clipCoords.w = pos.x * matrix[12] + pos.y * matrix[13] + pos.z * matrix[14] + matrix[15];

    if (clipCoords.w < 0.1f) {
        return false;
    }

    Vec3 NDC;
    NDC.x = clipCoords.x / clipCoords.w;
    NDC.y = clipCoords.y / clipCoords.w;
    NDC.z = clipCoords.z / clipCoords.w;

    screen.x = (width / 2 * NDC.x) + (NDC.x + width / 2);
    screen.y = -(height / 2 * NDC.y) + (NDC.y + height / 2);

    return true;
}

void InitImGui(LPDIRECT3DDEVICE9 pDevice)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
    ImGui::StyleColorsDark();
    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(pDevice);
}

bool init = false;
bool ShowMenu = true;
int tabs;
auto flags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize;

void APIENTRY hkEndScene(LPDIRECT3DDEVICE9 o_pDevice)
{
    if (GetAsyncKeyState(VK_INSERT) & 1)
    {
        ShowMenu = !ShowMenu;
    }
    if (!pDevice)
    {
        pDevice = o_pDevice;
    }
    if (!init)
    {
        InitImGui(pDevice);
        init = true;
    }

    if (cfg.betterCrosshair)
    {
        DrawFilledRect(windowX / 2 - 2, windowY / 2 - 2, 4, 4,FLOAT4TOD3DCOLOR(sniperColor));
    }
    if (cfg.boxesp) {

        uintptr_t localPlayer = *(uintptr_t*)(gameModule + dwLocalPlayer);
        if (localPlayer != NULL) {

            int localTeam = *(int*)(localPlayer + m_iTeamNum);

            Vec2 ScreenPosition;
            Vec2 HeadPosition;

            for (int x = 1; x < 32; x++) {

                uintptr_t Entity = *(uintptr_t*)(gameModule + dwEntityList + x * 0x10);
                if (Entity != NULL) {

                    int entityTeam = *(int*)(Entity + m_iTeamNum);
                    int entityHealth = *(int*)(Entity + m_iHealth);

                    if (entityTeam != localTeam && entityHealth > 0 && entityHealth <= 100) {

                        Vec3 EntityLocation = *(Vec3*)(Entity + m_vecOrigin);
                        float vMatrix[16];
                        memcpy(&vMatrix, (PBYTE*)(gameModule + dwViewMatrix), sizeof(vMatrix));
                        if (WorldTooScreen(EntityLocation, ScreenPosition, vMatrix, windowX, windowY)) {

                            uintptr_t BoneMatrix_Base = *(uintptr_t*)(Entity + m_dwBoneMatrix);
                            BoneMatrix_t Bone = *(BoneMatrix_t*)(BoneMatrix_Base + sizeof(Bone) * 9);
                            Vec3 Location = { Bone.x, Bone.y, Bone.z + 10 };
                            Vec2 ScreenCoords;
                            float vMatrix[16];
                            memcpy(&vMatrix, (PBYTE*)(gameModule + dwViewMatrix), sizeof(vMatrix));
                            if (WorldTooScreen(Location, ScreenCoords, vMatrix, windowX, windowY))
                            {
                                HeadPosition = ScreenCoords;
                                DrawBox(
                                    ScreenPosition.x - (((ScreenPosition.y - HeadPosition.y) * 1) / 2),
                                    HeadPosition.y,
                                    (ScreenPosition.y - HeadPosition.y) * 1,
                                    ScreenPosition.y - HeadPosition.y,
                                   0.1, 0, FLOAT4TOD3DCOLOR(boxColor)
                                    );
                            }
                        }
                    }
                }
            }
        }
    }
    // some features with ColorEdit3 (TODO fix with hack.cpp) *fixed*

    if (ShowMenu) {
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();


        ImGui::Begin("posejdon.pw", &ShowMenu, flags);
        if (ImGui::Button("AIM", ImVec2(100, 0))) {
            tabs = 1;
        }
        ImGui::SameLine(0.f, 2.f);
        if (ImGui::Button("ESP", ImVec2(100, 0))) {
            tabs = 2;
        }
        ImGui::SameLine(0.f, 2.f);
        if (ImGui::Button("MISC", ImVec2(100, 0))) {
            tabs = 3;
        }
        ImGui::SameLine(0.f, 2.f);
        if (ImGui::Button("COLOR", ImVec2(100, 0))) {
            tabs = 4;
        }

        if (tabs == 1) {
            ImGui::Checkbox("Trigger Bot", &cfg.triggerbotcfg);
            if (cfg.triggerbotcfg)
            {
                ImGui::Checkbox("Scope Check", &cfg.triggerbotScopedCheck);
                ImGui::Checkbox("Flash Check", &cfg.triggerbotFlashCheck);
                if (cfg.triggerbotFlashCheck)
                {
                    ImGui::SliderFloat("Custom Flash Check Time", &customTimeFlash, 1, 4);
                }
            }
        }
        if (tabs == 2) {
            ImGui::Checkbox("Glow", &cfg.glowcfg);
            ImGui::Checkbox("Box Enemy", &cfg.boxesp);
            ImGui::Checkbox("Legit Chams", &cfg.legitChams);

        }
        if (tabs == 3) {
            ImGui::Checkbox("Auto Bunny Hop", &cfg.bunnyhopcfg);
            ImGui::Checkbox("Radar Hack", &cfg.radarHack);
            ImGui::Checkbox("No Flash", &cfg.flashHack);
           // ImGui::Checkbox("No Hands", &cfg.noArms);
            ImGui::Checkbox("Third Person", &cfg.thirdperson);
            ImGui::Checkbox("Sniper Crosshair", &cfg.betterCrosshair);
        }
        if (tabs == 4)
        {
                ImGui::TextColored(ImVec4(255, 0, 0, 255), "Glow Color");
                ImGui::ColorEdit3("Glow", glowChams);
                ImGui::Separator();
                ImGui::TextColored(ImVec4(255, 0, 0, 255), "Legit Chams Color");
                ImGui::ColorEdit3("Color Chams", chams);
                ImGui::Separator();
                ImGui::TextColored(ImVec4(255, 0, 0, 255), "Sniper Crosshair Color");
                ImGui::ColorEdit3("Crosshair", sniperColor);
                ImGui::Separator();
                ImGui::TextColored(ImVec4(255, 0, 0, 255), "Enemy Box Color");
                ImGui::ColorEdit3("Box Color", boxColor);
                ImGui::Separator();
        }

        ImGui::End();

        ImGui::EndFrame();
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
    }
    oEndScene(pDevice);
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;
    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}
BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
    DWORD wndProcId;
    GetWindowThreadProcessId(handle, &wndProcId);

    if (GetCurrentProcessId() != wndProcId)
        return TRUE;
    window = handle;
    return FALSE;
}
HWND GetProcess()
{
    window = NULL;
    EnumWindows(EnumWindowsCallback, NULL);
    return window;
}

DWORD WINAPI mainThread(HMODULE hMod) {
    
    // Hook
    if (GetD3D9Device(d3d9Device, sizeof(d3d9Device)))
    {
        memcpy(EndSceneBytes, (char*)d3d9Device[42], 7);
        oEndScene = (tEndScene)TrampHook((char*)d3d9Device[42], (char*)hkEndScene, 7);
    }
    window = GetProcess();
    oWndProc = (WNDPROC)SetWindowLongPtr(window, -4, (LONG_PTR)WndProc);

    // Loop
    while (!GetAsyncKeyState(VK_DELETE))
    {
        hack.InitHack();
        Sleep(1);
    }

    // Unhook
    Patch((BYTE*)d3d9Device[42], EndSceneBytes, 7);
    FreeLibraryAndExitThread(hMod, 0);
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        MessageBox(NULL, __DATE__, "build from", MB_OK);
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)mainThread, 0, 0, 0);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

