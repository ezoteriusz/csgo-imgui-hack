#pragma once


class Cheat {
public:
	void InitHack(); // client.dll
	void bhop();
	void trigger();
	void LegitChams();
	void RadarHack();
	void FlashHack();
	void ThirdPerson();
	void glowplayers();
	//void NoArms();
};

extern DWORD gameModule;
extern DWORD engineModule;
extern DWORD LocalPlayer;

extern Cheat hack;
