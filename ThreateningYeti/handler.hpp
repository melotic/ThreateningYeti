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
			std::cout << "\n\nVersion 1.3" << std::endl;

			loguru::g_colorlogtostderr = true;

			LOG_F(INFO, "Threatening Yeti Loading");
			LOG_F(INFO, "Base Address: 0x%p", hModule);
			CHECK_F(!MH_Initialize(), "Error initializing MinHook");

			// init cldb hooks
			LOG_F(INFO, "Initializing CLDB hooks");
			CHECK_F(
				!MH_CreateHookApi(L"LockDownBrowser.dll", "?CLDBDoSomeStuff@@YAHPAH@Z", &hooks::do_some_stuff, (void**)
					&
					hooks::og_do_some_stuff), "Error hooking CLDBDOSomeStuff");
			CHECK_F(
				!MH_CreateHookApi(L"LockDownBrowser.dll", "?CLDBDoSomeOtherStuff@@YAHPAH@Z", &hooks::do_some_other_stuff,
					(void**)&hooks::og_do_some_other_stuff), "Error hooking CLDSoSomeOtherStuff");
			CHECK_F(
				!MH_CreateHookApi(L"LockDownBrowser.dll", "?CLDBDoYetMoreStuff@@YAHPAH@Z", &hooks::do_yet_more_stuff, (
					void**)&hooks::og_do_yet_more_stuff), "Error hooking CLDBDoYetMoreStuff");

			// hook internal functions
			LOG_F(INFO, "Initializing internal function hooks");
			CHECK_F(
				!MH_CreateHook(LD_OFFSET(0x057710), &hooks::
					disable_task_manager, (void**)&hooks::og_disable_task_manager),
				"Error hooking disable task manager");
#ifdef _DEBUG
			CHECK_F(!MH_CreateHook(LD_OFFSET(0x065810), &hooks::lockdown_log, (void**)&hooks::og_lockdown_log),
			        "Error hooking internal logging function");
#endif
			CHECK_F(
				!MH_CreateHook(LD_OFFSET(0x05E0A0), &hooks::check_foreground_window, (void**)&hooks::
					og_check_foreground_window), "Error hooking check foreground window function");
			CHECK_F(!MH_CreateHook(LD_OFFSET(0x025D70), &hooks::wnd_proc, (void**)&hooks::og_wnd_proc),
			        "Error hooking WndProc");
			CHECK_F(!MH_CreateHook(LD_OFFSET(0x0557A0), &hooks::check_vm, (void**) &hooks::og_check_vm), "Error hooking check vm");

			// hook winapi functions
			LOG_F(INFO, "Hooking WinAPI functions");
			CHECK_F(!MH_CreateHook(&EmptyClipboard, &hooks::empty_clipboard, (void**)hooks::og_empty_clipboard),
			        "Error hooking EmptyClipboard");
			CHECK_F(!MH_CreateHook(&CreateFileA, &hooks::create_file, (void**)&hooks::og_create_file),
			        "Error hooking CreateFileA");
			CHECK_F(!MH_CreateHook(&GetMonitorInfoW, &hooks::get_monitor_info, (void**)&hooks::og_get_monitor_info),
			        "Error hooking GetMonitorInfoW");
			CHECK_F(
				!MH_CreateHookApi(L"ntdll.dll", "NtQuerySystemInformation", &hooks::nt_query_system_information, (void**)
					&hooks::og_nt_query_system_information), "Error hooking NtQuerySystemInformation");


			LOG_F(INFO, "Enabling Hooks");
			MH_EnableHook(nullptr);
		}
	};
}
