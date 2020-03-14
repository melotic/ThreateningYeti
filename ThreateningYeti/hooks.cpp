#include "pch.h"
#include "hooks.h"
#include "globals.h"
#include <cstdio>
#include <winternl.h>

ty::hooks::cldb_do_some_stuff_t ty::hooks::og_do_some_stuff = nullptr;
ty::hooks::cldb_do_some_other_stuff_t ty::hooks::og_do_some_other_stuff = nullptr;
ty::hooks::cldb_do_yet_more_stuff_t ty::hooks::og_do_yet_more_stuff = nullptr;
ty::hooks::disable_task_manager_t ty::hooks::og_disable_task_manager = nullptr;
ty::hooks::empty_clipboard_t ty::hooks::og_empty_clipboard = nullptr;
ty::hooks::lockdown_log_t ty::hooks::og_lockdown_log = nullptr;
ty::hooks::create_file_t ty::hooks::og_create_file = nullptr;
ty::hooks::check_foreground_window_t ty::hooks::og_check_foreground_window = nullptr;
ty::hooks::nt_query_system_information_t ty::hooks::og_nt_query_system_information = nullptr;
WNDPROC ty::hooks::og_wnd_proc = nullptr;
ty::hooks::get_monitor_info_t ty::hooks::og_get_monitor_info = nullptr;

// Hooked implementations
int _cdecl ty::hooks::do_some_stuff(int* a1)
{
	LOG_F(WARNING, "blocked CLDBDoSomeStuff(int)");

	if (*a1 >> 6 & 1)
	{
		// todo check if this actually matters
		auto v1 = 1024;
		v1 += 2048;
		auto v6 = v1 + 2048;
		*a1 += 2 * v6;

		return v6;
	}

	return 0;
}

bool _cdecl ty::hooks::do_some_other_stuff(int* a1)
{
	LOG_F(WARNING, "blocked CLDBDoSomeOtherStuff(int)");
	// todo check if this actually matters
	*a1 += 2048;

	return TRUE;
}

bool _cdecl ty::hooks::do_yet_more_stuff()
{
	LOG_F(WARNING, "blocked CLDBDoYetMoreStuff()");
	return TRUE;
}

// xref string "DisableTaskMgr"
LSTATUS ty::hooks::disable_task_manager(void* a1)
{
	LOG_F(WARNING, "blocked call to disable taskmanager");
	return ERROR_SUCCESS;
}

BOOL ty::hooks::empty_clipboard()
{
	LOG_F(WARNING, "blocked call to empty clipboard");
	return TRUE;
}

// xref string "%02d:%02d:%02d.%03d - " --> function call with same parameter as strlen
void ty::hooks::lockdown_log(char* a1, ...)
{
	// seriously respoundus? lmfao

	struct _SYSTEMTIME SystemTime{};
	char msg[16368];
	va_list va;

	va_start(va, a1);
	GetLocalTime(&SystemTime);
	sprintf_s(static_cast<char*>(msg), 256, "%02d:%02d:%02d.%03d - ", SystemTime.wHour, SystemTime.wMinute,
	          SystemTime.wSecond, SystemTime.wMilliseconds);
	const auto v1 = strlen(msg);
	_vsprintf_s_l(&msg[v1], 16368 - v1, a1, nullptr, va);

	printf("[LOCKDOWN] %s\n", msg);
	va_end(va);
}

HANDLE WINAPI ty::hooks::create_file(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
                                     LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition,
                                     DWORD dwFlagsAndAttributes,
                                     HANDLE hTemplateFile)
{
	if (dwShareMode == 0)
	{
		dwShareMode = FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE;
		LOG_F(WARNING, "Intercepted Anti-Debug CreateFileA => dwShareMode = 7");
	}

	return og_create_file(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition,
	                      dwFlagsAndAttributes, hTemplateFile);
}

// xref string "ASC - "
BOOL ty::hooks::check_foreground_window()
{
	return TRUE;
}

NTSTATUS WINAPI ty::hooks::nt_query_system_information(SYSTEM_INFORMATION_CLASS SystemInformationClass,
                                                       PVOID SystemInformation, ULONG SystemInformationLength,
                                                       PULONG ReturnLength)
{
	const auto status = og_nt_query_system_information(SystemInformationClass, SystemInformation,
	                                                   SystemInformationLength,
	                                                   ReturnLength);

	if (SystemProcessInformation == SystemInformationClass && status == ERROR_SUCCESS)
	{
		auto previous = P_SYSTEM_PROCESS_INFORMATION(SystemInformation);
		auto current = P_SYSTEM_PROCESS_INFORMATION((PUCHAR)previous + previous->NextEntryOffset);
		auto count = 0u;

		while (previous->NextEntryOffset != NULL)
		{
			for (auto& exe : globals::blacklisted_exes)
			{
				if (_wcsicmp(exe, current->ImageName.Buffer) == 0)
				{
					// bad boi detected
					if (current->NextEntryOffset == 0)
					{
						previous->NextEntryOffset = 0;
					}
					else
					{
						previous->NextEntryOffset += current->NextEntryOffset;
					}

					count++;
					current = previous;

					break;
				}
			}

			previous = current;
			current = P_SYSTEM_PROCESS_INFORMATION((PUCHAR)current + current->NextEntryOffset);
		}

		//LOG_F(WARNING, "Hid %u processes", count);
	}

	return status;
}

LRESULT CALLBACK ty::hooks::wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// WM_SETFOCUS
	// WM_ACTIVATEAPP
	// 0x2B1
	if (message == WM_SETFOCUS || message == WM_ACTIVATEAPP || message == 0x2B1)
	{
		LOG_F(WARNING, "Blocked lose focus message");
		return 0;
	}

	return og_wnd_proc(hWnd, message, wParam, lParam);
}

BOOL WINAPI ty::hooks::get_monitor_info(HMONITOR hMonitor, LPMONITORINFO lpmi)
{
	auto result = og_get_monitor_info(hMonitor, lpmi);
	lpmi->dwFlags = 8;
	
	return result;
}

