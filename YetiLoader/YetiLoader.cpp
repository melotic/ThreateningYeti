// YetiLoader.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "loguru.hpp"
#include <Windows.h>
#include <cstring>
#include <shlwapi.h>

BOOL file_exists(LPCWSTR file)
{
	WIN32_FIND_DATA find_file_data;
	const auto file_handle = FindFirstFileW(file, &find_file_data);

	if (file_handle == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	FindClose(file_handle);
	return TRUE;
}

int main(int argc, char** argv)
{
	std::cout << "\n";
	std::cout <<
		"                                                                                                             \n";
	std::cout <<
		"8b        d8                   88  88                                            88                          \n";
	std::cout <<
		" Y8,    ,8P             ,d     \"\"  88                                            88                          \n";
	std::cout <<
		"  Y8,  ,8P              88         88                                            88                          \n";
	std::cout <<
		"   \"8aa8\"  ,adPPYba,  MM88MMM  88  88           ,adPPYba,   ,adPPYYba,   ,adPPYb,88   ,adPPYba,  8b,dPPYba,  \n";
	std::cout <<
		"    `88'  a8P_____88    88     88  88          a8\"     \"8a  \"\"     `Y8  a8\"    `Y88  a8P_____88  88P'   \"Y8  \n";
	std::cout <<
		"     88   8PP\"\"\"\"\"\"\"    88     88  88          8b       d8  ,adPPPPP88  8b       88  8PP\"\"\"\"\"\"\"  88          \n";
	std::cout <<
		"     88   \"8b,   ,aa    88,    88  88          \"8a,   ,a8\"  88,    ,88  \"8a,   ,d88  \"8b,   ,aa  88          \n";
	std::cout <<
		"     88    `\"Ybbd8\"'    \"Y888  88  88888888888  `\"YbbdP\"'   `\"8bbdP\"Y8   `\"8bbdP\"Y8   `\"Ybbd8\"'  88          \n";
	std::cout <<
		"                                                                                                             \n";
	std::cout <<
		"                                                                                                             \n";
	std::cout << "";
	
	loguru::init(argc, argv);
	LOG_F(INFO, "YetiLoader Version 1.0");

	// get full path to dll
	const auto ty_dll = L"ThreateningYeti.dll";
	wchar_t ty_dll_full[MAX_PATH];
	CHECK_F(_wfullpath(ty_dll_full, ty_dll, MAX_PATH) != nullptr);

	// check dll exists
	if (!file_exists(ty_dll_full))
	{
		MessageBoxW(
			nullptr,
			L"ThreateningYeti.dll could not be found. Please place it in the same directory as this executable.",
			L"Missing Dependency", MB_ICONERROR | MB_OK);
		ABORT_F("Missing ThreateningYeti.dll");
	}

	// find path to lockdown, and prompt user if we cant find it
	auto ld_exe = L"C:\\Program Files (x86)\\Respondus\\LockDown Browser\\LockDownBrowser.exe";

	if (!file_exists(ld_exe))
	{
		// prompt user to select where their lockdown browser installation is
		LOG_F(WARNING, "Could not find LockDownBrowser executable");
		OPENFILENAMEW ofn;
		WCHAR szFile[MAX_PATH] = { 0 };

		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = nullptr;
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = L"LockDownBrowserOEM\0LockDownBrowserOEM.exe;LockDownBrowser.exe\0\0";
		ofn.nFilterIndex = 1;
		// supports both LockDownBrowser and LockDownBrowserOEM
		ofn.lpstrTitle = L"Browse to LockDownBrowserOEM.exe or LockDownBrowser.exe";
		ofn.lpstrInitialDir = nullptr;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileNameW(&ofn) == TRUE)
		{
			ld_exe = ofn.lpstrFile;
		}
		else
		{
			ABORT_F("User failed to locate LockDownBrowser");
		}
	}

	LOG_F(INFO, "Found LockDownBrowserOEM -> %ls", ld_exe);

	// Extract directory from full path
	WCHAR path[MAX_PATH];
	_wsplitpath_s(ld_exe, nullptr, 0, path, _countof(path), nullptr, 0, nullptr, 0);

	// init process data structures
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	ZeroMemory(&pi, sizeof(pi));
	si.cb = sizeof(si);

	// add support for LDB URIs
	std::wstring ld_input_str = L"";

	char c;
	std::cout << "\nDo you normally open lockdown browser from a website? Type -> (Y/N):";

	std::cin >> c;
	if (toupper(c) == 'Y') {
		std::cout << "\nPlease enter the URL for the lockdown browser, this is usually found by copying the address of the button that would normally open lockdown browser. For example \"Start Test\" or whatever:";

		// get user input
		std::wcin >> ld_input_str;

		// convert wstring to LPWSTR
		LPWSTR ld_input = const_cast<LPWSTR>(ld_input_str.c_str());

		// Create the process suspended (with our URI as an argument)
		CHECK_F(CreateProcessW(ld_exe, ld_input, NULL, NULL, false, CREATE_SUSPENDED, NULL, path, &si, &pi) == TRUE,
			"Failed to create the LockDownBrowserOEM process. Try running as Admin and make sure you entered the correct URL.");
		LOG_F(INFO, "Created process");
	}
	else {
		// Create the process suspended (in normal mode)
		CHECK_F(CreateProcessW(ld_exe, NULL, NULL, NULL, false, CREATE_SUSPENDED, NULL, path, &si, &pi) == TRUE,
			"Failed to create the LockDownBrowserOEM process. Try running as Admin.");
		LOG_F(INFO, "Created process");
	}

	/*
	 * Inject the dll. We will create a remote thread that calls loadlibrary on our dll
	 * then our dll will unlink itself from the PEB to stay hidden-ish
	 */

	const auto ll_addr = static_cast<LPVOID>(GetProcAddress(GetModuleHandleW(L"kernel32.dll"), "LoadLibraryW"));
	CHECK_F(ll_addr != NULL, "Coult not get address of LoadLibraryW");
	LOG_F(INFO, "LoadLibraryW: 0x%p", ll_addr);

	// allocate memory for dll string
	const auto ll_arg_mem = VirtualAllocEx(pi.hProcess, nullptr, wcslen(ty_dll_full) * sizeof(wchar_t) + 1,
		MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	CHECK_F(ll_arg_mem != NULL, "Could not allocate memory for LoadLibrary argument");
	LOG_F(INFO, "LoadLibraryW Arg: 0x%p", ll_arg_mem);

	// write dll name into new buffer
	CHECK_F(WriteProcessMemory(pi.hProcess, ll_arg_mem, ty_dll_full, wcslen(ty_dll_full) * sizeof(wchar_t) + 1, NULL) == TRUE);

	const auto h_thread = CreateRemoteThread(pi.hProcess, nullptr, NULL, static_cast<LPTHREAD_START_ROUTINE>(ll_addr),
		ll_arg_mem, NULL, nullptr);
	CHECK_F(h_thread != nullptr, "Error creating remote thread");
	LOG_F(INFO, "Error Code: %lu", GetLastError());

	// wait for loadlibrary to finish
	WaitForSingleObject(h_thread, INFINITE);

	DWORD exit_code;
	GetExitCodeThread(h_thread, &exit_code);
	LOG_F(INFO, "Remote Thread returned: %lu", exit_code);

	// wait 100ms for dll to initialize
	Sleep(100);
	ResumeThread(pi.hThread);

	LOG_F(ERROR, "The Yeti has been unleashed");

	return 0;
}
