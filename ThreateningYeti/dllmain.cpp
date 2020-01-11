// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "globals.h"
#include "hide_module.h"


BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD ul_reason_for_call,
                      LPVOID lpReserved
)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ty::globals::handler.init(hModule);
		CHECK_F(ty::hide_module::unlink_module_peb(hModule), "Failed to unlink from PEB");
		CHECK_F(ty::hide_module::remove_pe_header(hModule), "Failed to remove PE Header");
		break;
	case DLL_PROCESS_DETACH:
		break;
	default: ;
	}
	return TRUE;
}
