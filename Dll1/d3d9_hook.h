#pragma once
#include <d3d9.h>
#pragma comment(lib,"d3d9.lib")

typedef HRESULT(__stdcall* f_EndScene)(IDirect3DDevice9* pDevice);

class D3D9 {
public:
	f_EndScene originalEndScene;
	bool hook(f_EndScene);
private:
	bool getD3D9Device(void** pTable, size_t Size);
};