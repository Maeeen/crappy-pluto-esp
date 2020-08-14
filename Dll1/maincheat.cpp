#include "d3d9_hook.h"
#include "maincheat.h"
#include <string>
#include <cmath>
#include <iostream>
#include "structs.h"
bool on = false;

D3D9 hooker;
void print(std::string mdr) {
	std::cout << "[main] " << mdr << std::endl;
}

bool worldToScreen(Vector3D world, float* screenPos) {
	GetScreenMatrix_t GetScreenMatrix_ = (GetScreenMatrix_t)0x4B7610;
	World2Screen_t WorldToScreen_ = (World2Screen_t)0x44ADC0;
	
	return WorldToScreen_(0, GetScreenMatrix_(), &world, screenPos);
}

HRESULT __stdcall d3d9hook(IDirect3DDevice9* pDevice) {
	if (GetAsyncKeyState(VK_F4))
		on = !on;

	if (!on)
		return hooker.originalEndScene(pDevice);

	if (*(int*)0x1065C90 == 0) {
		D3DRECT rect = { 20, 20, 30, 30 };
		pDevice->Clear(1, &rect, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 255, 255, 125), 1.0f, NULL);
		return hooker.originalEndScene(pDevice);
	}

	CG_T* cg = (CG_T*)0x900FA0;

	int localClientNbr = cg->ClientNumber;
	Vector3D localPos = { 0,0,0 };
	int localTeam = -1;

	for (int i = 0; i < 18; i++) {
		ClientInfo_T* infoPtr = (ClientInfo_T*)(0x9FE678 + i * 0x564);
		Entity_T* entityPtr = (Entity_T*)(10528192 + i * 0x1F8);

		if (entityPtr->ClientNumber == localClientNbr && (entityPtr->Valid & 1) == 1) {
			localTeam = infoPtr->Team;
			localPos = entityPtr->Origin;
		}
	}

	for (int i = 0; i < 18; i++) {
		ClientInfo_T* infoPtr = (ClientInfo_T*)(0x9FE678 + i * 0x564);
		Entity_T* entityPtr = (Entity_T*)(10528192 + i * 0x1F8);

		if (entityPtr->ClientNumber != localClientNbr && entityPtr->Valid && infoPtr->Valid) {
			if (entityPtr->Type == Entity_Type::Player) {
				D3DCOLOR color;

				if ((entityPtr->IsAlive & 1) == 1) {
					if (localTeam > 0 && infoPtr->Team == localTeam)
						color = D3DCOLOR_ARGB(150, 0, 250, 0);
					else
						color = D3DCOLOR_ARGB(150, 250, 0, 0);
				}
				else
					color = D3DCOLOR_ARGB(150, 0, 0, 50);

				float pos[2];

				Vector3D vec = entityPtr->Origin;
				vec.y += 10;

				worldToScreen(vec, pos);
				
				int width = min(1/sqrt(pow(vec.x - localPos.x, 2) + pow(vec.y - localPos.y, 2) + pow(vec.y - localPos.y, 2)) * 5000, 10);
				D3DRECT rect = { pos[0] - width, pos[1] - width, pos[0] + width, pos[1] + width };
				pDevice->Clear(1, &rect, D3DCLEAR_TARGET, color, 1.0f, NULL);
			}
		}
	}
	return hooker.originalEndScene(pDevice);
}

void MainCheat::main() {
	print(hooker.hook(d3d9hook) ? "Hook successful" : "Hook unsuccessful");
}