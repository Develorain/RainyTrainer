#include <Windows.h>
#include <stdbool.h>
#include <stdio.h>

char* base;
char* healthUpdateInstruction;
char* ammoUpdateInstruction;
bool healthHackActivated = false;
bool ammoHackActivated = false;

void writeMemory(char* destination, char* source, size_t numberOfBytes) {
	DWORD origProject;
	VirtualProtect(destination, numberOfBytes, PAGE_EXECUTE_READWRITE, &origProject);
	memcpy(destination, source, numberOfBytes);
	VirtualProtect(destination, numberOfBytes, origProject, NULL);
}

void start() {
	base = (char*) GetModuleHandle(NULL);
	healthUpdateInstruction = base + 0x2CA5E;
	ammoUpdateInstruction = base + 0x637E9;

	if (!RegisterHotKey(NULL, 1, MOD_NOREPEAT, VK_F1)) {
		OutputDebugString("Failed to register F1 hotkey");
	}

	if (!RegisterHotKey(NULL, 2, MOD_NOREPEAT, VK_F2)) {
		OutputDebugString("Failed to register F2 hotkey");
	}

	MSG msg = { 0 };

	while (GetMessage(&msg, NULL, 0, 0)) {
		if (msg.message == WM_HOTKEY) {
			if (msg.wParam == 1) {
				healthHackActivated = !healthHackActivated;

				if (healthHackActivated) {
					char newHealthUpdateInstruction[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };
					writeMemory(healthUpdateInstruction, &newHealthUpdateInstruction[0], 6);
				} else {
					char newHealthUpdateInstruction[] = { 0x89, 0x82, 0xF8, 0x00, 0x00, 0x00 };
					writeMemory(healthUpdateInstruction, &newHealthUpdateInstruction[0], 6);
				}
			}

			if (msg.wParam == 2) {
				ammoHackActivated = !ammoHackActivated;

				if (ammoHackActivated) {
					char newAmmoUpdateInstruction[] = { 0x90, 0x90 };
					writeMemory(ammoUpdateInstruction, &newAmmoUpdateInstruction[0], 2);
				} else {
					char newAmmoUpdateInstruction[] = { 0xFF, 0x0E };
					writeMemory(ammoUpdateInstruction, &newAmmoUpdateInstruction[0], 2);
				}
			}
		}
	}
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH: {
			DisableThreadLibraryCalls(hModule);
			CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)&start, NULL, 0, NULL);
			break;
		}
		case DLL_THREAD_ATTACH: {
			break;
		}
		case DLL_THREAD_DETACH: {
			break;
		}
		case DLL_PROCESS_DETACH: {
			break;
		}
	}
	return TRUE;
}