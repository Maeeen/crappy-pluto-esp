// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "maincheat.h"

MainCheat cheat;

void launch() {
    cheat.main();
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)launch, NULL, NULL, NULL);
    }
    return TRUE;
}

