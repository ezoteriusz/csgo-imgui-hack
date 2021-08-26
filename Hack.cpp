#include "includes.h"

configuration cfg;

Cheat hack;
DWORD gameModule; // client.dll
DWORD engineModule; // engine.dll
DWORD LocalPlayer;

DWORD checkLocal;

void Cheat::InitHack()
{
	gameModule = (DWORD)GetModuleHandleA("client.dll");
	engineModule = (DWORD)GetModuleHandleA("engine.dll");
    checkLocal = *(DWORD*)(gameModule + dwLocalPlayer);
    if (checkLocal != NULL) {
        // init cheats
        hack.bhop();
        hack.trigger();
        hack.LegitChams();
        hack.FlashHack();
        hack.RadarHack();
        hack.ThirdPerson();
        hack.glowplayers();
    }
  // 
   // hack.NoArms();
}


bool flashCheck()
{
    LocalPlayer = *(DWORD*)(gameModule + dwLocalPlayer);
    if (cfg.triggerbotFlashCheck)
    {
        float flashAmmount = *(float*)(LocalPlayer + m_flFlashDuration);
        if (flashAmmount > customTimeFlash) {
            return false;
        }
        else 
            return true;
    }
}

int getCurrentWeapon()
{
    DWORD LocalPlayer = *(DWORD*)(gameModule + dwLocalPlayer);
    int currentWeapon = *(int*)(LocalPlayer + m_hActiveWeapon);
    DWORD weaponEnt = *(DWORD*)(gameModule + dwEntityList + ((currentWeapon & 0xFFF) - 1) * 0x10);
    int weaponID = *(int*)(weaponEnt + m_iItemDefinitionIndex);

    return *(int*)(weaponEnt + m_iItemDefinitionIndex);
}

bool isScoped()
{
    DWORD LocalPlayer = *(DWORD*)(gameModule + dwLocalPlayer);
    if (cfg.triggerbotScopedCheck)
    {
        if (LocalPlayer != NULL) {
            bool scoped = *(bool*)(LocalPlayer + m_bIsScoped);

            if (scoped && getCurrentWeapon() == weapon_awp || getCurrentWeapon() == weapon_ssg08)
            {
                return true;
            }
            else
            {
                return false;
            }
        }
    }
}


void Cheat::bhop()
{
		LocalPlayer = *(DWORD*)(gameModule + dwLocalPlayer);
        if(cfg.bunnyhopcfg)
		if (LocalPlayer != NULL) {
			int flag = *(BYTE*)(LocalPlayer + m_fFlags);
			if (GetAsyncKeyState(VK_SPACE) && flag & (1 << 0)) {
				*(DWORD*)(gameModule + dwForceJump) = 6;
			}
		}
}

void Cheat::trigger()
{

    DWORD LocalPlayer = *(DWORD*)(gameModule + dwLocalPlayer);
    if (cfg.triggerbotcfg )
        if (LocalPlayer != NULL) {
            int crossId = *(int*)(LocalPlayer + m_iCrosshairId);
            uintptr_t entity = *(uintptr_t*)(gameModule + dwEntityList + (crossId - 1) * 0x10);
            if (entity != NULL) {
                int localTeam = *(int*)(LocalPlayer + m_iTeamNum);
                int entityTeam = *(int*)(entity + m_iTeamNum);
                int entityHealth = *(int*)(entity + m_iHealth);
                if (flashCheck())
                if (isScoped())
                if (localTeam != entityTeam && entityHealth > 0 && entityHealth < 101) {
                    *(int*)(gameModule + dwForceAttack) = 5;
                    Sleep(20);
                    *(int*)(gameModule + dwForceAttack) = 4;
                }
            }
        }
}

void Cheat::LegitChams()
{
    DWORD Localplayer = *(uintptr_t*)(gameModule + dwLocalPlayer);
    if (cfg.legitChams) {
        if(LocalPlayer != NULL)
        for (int i = 1; i < 32; i++)
        {
            int localteam = *(int*)(Localplayer + m_iTeamNum);
            uintptr_t entity = *(uintptr_t*)((gameModule + dwEntityList) + i * 0x10);

            if (entity != NULL)
            {
                int entityteam = *(int*)(entity + m_iTeamNum);

                if (localteam != entityteam)
                {
                    *(int*)(entity + m_clrRender) = FLOAT4TOD3DCOLOR(chams);
                }
            }
        }
    }
}

void Cheat::RadarHack()
{
    DWORD Localplayer = *(uintptr_t*)(gameModule + dwLocalPlayer);
    if (cfg.radarHack)
    if (LocalPlayer != NULL) {
        
        for (int i = 0; i < 64; i++)
        {
            DWORD entity = *(DWORD*)(gameModule + dwEntityList + (i * 0x10));
            if (entity)
            {
                *(bool*)(entity + m_bSpotted) = true;
            }
        }
    }
}

/*void Cheat::NoArms()
{
    DWORD Localplayer = *(uintptr_t*)(gameModule + dwLocalPlayer);
    if (cfg.noArms) {
    if (Localplayer != NULL)
        {
           *(int*)(LocalPlayer + m_szArmsModel) = 0;
       }
    }
}
*/
void Cheat::FlashHack()
{
    DWORD Localplayer = *(uintptr_t*)(gameModule + dwLocalPlayer);
    if (cfg.flashHack) 
    {
        if (LocalPlayer != NULL)
        {
            *(float*)(LocalPlayer + m_flFlashMaxAlpha) = 0.f;
        }
    }
    else 
    {
        *(float*)(LocalPlayer + m_flFlashMaxAlpha) = 255.0f;
    }
}

void Cheat::ThirdPerson()
{
    DWORD Localplayer = *(uintptr_t*)(gameModule + dwLocalPlayer);
    if (cfg.thirdperson)
    {
        if (LocalPlayer != NULL)
        {
            *(int*)(LocalPlayer + m_iObserverMode) = 1;
        }
    }
    else
    {
        *(int*)(LocalPlayer + m_iObserverMode) = 0;
    }
}


void Cheat::glowplayers()
{
    if (cfg.glowcfg) {
        for (int i = 1; i < 32; i++)
        {
            uintptr_t glowobj = *(uintptr_t*)(gameModule + dwGlowObjectManager);
            DWORD Localplayer = *(uintptr_t*)(gameModule + dwLocalPlayer);
            int localteam = *(int*)(Localplayer + m_iTeamNum);
            uintptr_t entity = *(uintptr_t*)((gameModule + dwEntityList) + i * 0x10);

            if (entity != NULL)
            {
                int entityteam = *(int*)(entity + m_iTeamNum);
                int glowindex = *(int*)(entity + m_iGlowIndex);

                if (localteam != entityteam)
                {
                    *(float*)(glowobj + ((glowindex * 0x38) + 0x8)) = glowChams[0];//R
                    *(float*)(glowobj + ((glowindex * 0x38) + 0xC)) = glowChams[1];//G
                    *(float*)(glowobj + ((glowindex * 0x38) + 0x10)) = glowChams[2];//B
                    *(float*)(glowobj + ((glowindex * 0x38) + 0x14)) = 1.f;//A
                }
                *(bool*)(glowobj + ((glowindex * 0x38) + 0x28)) = true;
                *(bool*)(glowobj + ((glowindex * 0x38) + 0x29)) = false;
            }
        }
    }
}