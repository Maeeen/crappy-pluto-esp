
#include <Windows.h>
#include "add-libs/detours.h"
#include "d3d9_hook.h"
#include <iostream>
#include <string>
#include <sstream>

#include <d3d9.h> //Comes with VS
#include <d3dx9.h> //SDK only  //Error here: "Cannot open include file 'd3dx9.h': No such file or directory"
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib") 

void print_d3d9(std::string mdr) {
	std::cout << "[d3dx9h][*] " << mdr << std::endl;
}

f_EndScene orig;

int GetWindowString(HWND hwnd, std::string& s) {
	char buffer[65536];

	int txtlen = GetWindowTextLength(hwnd) + 1;
	GetWindowTextA(hwnd, buffer, txtlen);

	s = buffer;
	return txtlen;
}

bool D3D9::getD3D9Device(void** pTable, size_t Size) {
	if (!pTable) {
		print_d3d9("No ptable.");
		return false;
	}

	IDirect3D9* pD3d = Direct3DCreate9(D3D_SDK_VERSION);
	if (!pD3d) {
		print_d3d9("Error at init D3D9.");
		return false;
	}

	print_d3d9("D3D9 created.");

	D3DPRESENT_PARAMETERS d3dpp = { 0 };
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = GetForegroundWindow();
	d3dpp.Windowed = ((GetWindowLong(d3dpp.hDeviceWindow, GWL_STYLE) & WS_POPUP) != 0) ? FALSE : TRUE;

	std::string windowName;
	GetWindowString(d3dpp.hDeviceWindow, windowName);
	print_d3d9(windowName);


	IDirect3DDevice9* pDummyDevice = nullptr;
	HRESULT res = pD3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, d3dpp.hDeviceWindow, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &pDummyDevice);
	if (FAILED(res)) {
		pD3d->Release();
		print_d3d9("Error at creating device : ");
		std::cout << std::hex << res << std::endl;
		return false;
	}

	print_d3d9("Device created!");

	memcpy(pTable, *reinterpret_cast<void***>(pDummyDevice), Size);
	pDummyDevice->Release();
	pD3d->Release();
	print_d3d9("Devices releases & pTable exported.");

	return true;
}

bool D3D9::hook(f_EndScene func) {
	void* pTable[119];
	if (!this->getD3D9Device(pTable, sizeof(pTable))) {
		Beep(650, 1000);
		return false;
	}

	std::stringstream ss;
	ss << pTable[42];

	print_d3d9("Original D3DEndScene 0x" + ss.str());

	originalEndScene = orig = (f_EndScene)DetourFunction((PBYTE)pTable[42], (PBYTE)func);

	return true;
}