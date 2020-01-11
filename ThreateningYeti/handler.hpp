#pragma once

#include "pch.h"
#include <iostream>
#include "hooks.h"
#include <shellapi.h>

namespace ty
{
	class handler
	{
	public:

		handler() = default;

		void init(HMODULE hModule)
		{
			// important ascii art
			AllocConsole();
			FILE* f;
			freopen_s(&f, "CONOUT$", "w", stdout);
			f = nullptr;
			freopen_s(&f, "CONOUT$", "w", stderr);
			f = nullptr;
			freopen_s(&f, "CONIN$", "r", stdin);
			SetConsoleTitleW(L"Threatening Yeti 😁");

			std::cout << std::endl;
			std::cout << "  ________                    __             _                __  __     __  _ \n";
			std::cout << " /_  __/ /_  ________  ____ _/ /____  ____  (_)___  ____ _    \\ \\/ /__  / /_(_)\n";
			std::cout << "  / / / __ \\/ ___/ _ \\/ __ `/ __/ _ \\/ __ \\/ / __ \\/ __ `/     \\  / _ \\/ __/ / \n";
			std::cout << " / / / / / / /  /  __/ /_/ / /_/  __/ / / / / / / / /_/ /      / /  __/ /_/ /  \n";
			std::cout << "/_/ /_/ /_/_/   \\___/\\__,_/\\__/\\___/_/ /_/_/_/ /_/\\__, /      /_/\\___/\\__/_/   \n";
			std::cout << "                                                 /____/                        \n";
			std::cout << "\n\nVersion 1.0" << std::endl;

			loguru::g_colorlogtostderr = true;

			LOG_F(INFO, "Threatening Yeti Loading");
			LOG_F(INFO, "Base Address: 0x%p", hModule);
			CHECK_F(MH_Initialize() == MH_OK, "Error initializing MinHook");

			// init cldb hooks
			LOG_F(INFO, "Initializing CLDB hooks");
			CHECK_F(
				MH_CreateHookApi(L"LockDownBrowser.dll", "?CLDBDoSomeStuff@@YAHPAH@Z", &hooks::do_some_stuff, (void**) &
					hooks::og_do_some_stuff) == MH_OK, "Error hooking CLDBDOSomeStuff");
			CHECK_F(
				MH_CreateHookApi(L"LockDownBrowser.dll", "?CLDBDoSomeOtherStuff@@YAHPAH@Z", &hooks::do_some_other_stuff,
					(void**)&hooks::og_do_some_other_stuff) == MH_OK, "Error hooking CLDSoSomeOtherStuff");
			CHECK_F(
				MH_CreateHookApi(L"LockDownBrowser.dll", "?CLDBDoYetMoreStuff@@YAHPAH@Z", &hooks::do_yet_more_stuff, (
					void**)&hooks::og_do_yet_more_stuff) == MH_OK, "Error hooking CLDBDoYetMoreStuff");

			// hook internal functions
			LOG_F(INFO, "Initializing internal function hooks");
			CHECK_F(
				MH_CreateHook(LD_OFFSET(0x0543C0), &hooks::
					disable_task_manager, (void**)&hooks::og_disable_task_manager) == MH_OK,
				"Error hooking disable task manager");
			CHECK_F(
				MH_CreateHook(LD_OFFSET(0x038150), &hooks::on_cover_windows, (void**)&hooks::og_on_cover_windows) ==
				MH_OK, "Error hooking OnCoverWindows");
#ifdef _DEBUG
			CHECK_F(MH_CreateHook(LD_OFFSET(0x061230), &hooks::lockdown_log, (void**)&hooks::og_lockdown_log) == MH_OK,
			        "Error hooking internal logging function");
#endif
			CHECK_F(
				MH_CreateHook(LD_OFFSET(0x05A120), &hooks::check_foreground_window, (void**)&hooks::
					og_check_foreground_window) == MH_OK, "Error hooking check foreground window function");

			// hook winapi functions
			LOG_F(INFO, "Hooking WinAPI functions");
			CHECK_F(MH_CreateHook(&EmptyClipboard, &hooks::empty_clipboard, (void**)hooks::og_empty_clipboard) == MH_OK,
			        "Error hooking EmptyClipboard");
			CHECK_F(MH_CreateHook(&CreateFileA, &hooks::create_file, (void**)&hooks::og_create_file) == MH_OK,
			        "Error hooking CreateFileA");
			CHECK_F(
				MH_CreateHookApi(L"ntdll.dll", "NtQuerySystemInformation", &hooks::nt_query_system_information, (void**)
					&hooks::og_nt_query_system_information) == MH_OK, "Error hooking NtQuerySystemInformation");


			LOG_F(INFO, "Enabling Hooks");
			MH_EnableHook(nullptr);
		}
	};
}
