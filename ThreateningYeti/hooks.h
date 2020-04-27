#pragma once

#include "pch.h"
#include <winternl.h>

namespace ty::hooks
{
	// TYPEDEFS ----------------------------------------------
	typedef int (*cldb_do_some_stuff_t)(int*);
	typedef BOOL (*cldb_do_some_other_stuff_t)(int*);
	typedef BOOL (*cldb_do_yet_more_stuff_t)();
	typedef int (*cldb_do_some_other_stuff_s_t)(int*);
	typedef LSTATUS (*disable_task_manager_t)(void*);
	typedef decltype(&EmptyClipboard) empty_clipboard_t;
	typedef void (*lockdown_log_t)(char*, ...);
	typedef decltype(&CreateFileA) create_file_t;
	typedef BOOL (*check_foreground_window_t)();
	typedef decltype(&NtQuerySystemInformation) nt_query_system_information_t;
	typedef decltype(&GetMonitorInfoW) get_monitor_info_t;
	typedef int (*lockdown_check_vm_t)();

	typedef struct __SYSTEM_PROCESS_INFORMATION
	{
		ULONG NextEntryOffset;
		ULONG NumberOfThreads;
		LARGE_INTEGER WorkingSetPrivateSize;
		ULONG HardFaultCount;
		ULONG NumberOfThreadsHighWatermark;
		ULONGLONG CycleTime;
		LARGE_INTEGER CreateTime;
		LARGE_INTEGER UserTime;
		LARGE_INTEGER KernelTime;
		UNICODE_STRING ImageName;
		LONG BasePriority;
		PVOID UniqueProcessId;
		PVOID InheritedFromUniqueProcessId;
		ULONG HandleCount;
		ULONG SessionId;
		ULONG_PTR UniqueProcessKey;
		ULONG_PTR PeakVirtualSize;
		ULONG_PTR VirtualSize;
		ULONG PageFaultCount;
		ULONG_PTR PeakWorkingSetSize;
		ULONG_PTR WorkingSetSize;
		ULONG_PTR QuotaPeakPagedPoolUsage;
		ULONG_PTR QuotaPagedPoolUsage;
		ULONG_PTR QuotaPeakNonPagedPoolUsage;
		ULONG_PTR QuotaNonPagedPoolUsage;
		ULONG_PTR PagefileUsage;
		ULONG_PTR PeakPagefileUsage;
		ULONG_PTR PrivatePageCount;
		LARGE_INTEGER ReadOperationCount;
		LARGE_INTEGER WriteOperationCount;
		LARGE_INTEGER OtherOperationCount;
		LARGE_INTEGER ReadTransferCount;
		LARGE_INTEGER WriteTransferCount;
		LARGE_INTEGER OtherTransferCount;
	} S_SYSTEM_PROCESS_INFORMATION, *P_SYSTEM_PROCESS_INFORMATION;

	// -------------------------------------------------------

	// POINTERS TO UNHOOKED FUNCTIONS ------------------------
	extern cldb_do_some_stuff_t og_do_some_stuff;
	extern cldb_do_some_other_stuff_t og_do_some_other_stuff;
	extern cldb_do_yet_more_stuff_t og_do_yet_more_stuff;
	extern cldb_do_some_other_stuff_s_t og_do_some_other_stuff_s;
	extern disable_task_manager_t og_disable_task_manager;
	extern empty_clipboard_t og_empty_clipboard;
	extern lockdown_log_t og_lockdown_log;
	extern create_file_t og_create_file;
	extern check_foreground_window_t og_check_foreground_window;
	extern nt_query_system_information_t og_nt_query_system_information;
	extern WNDPROC og_wnd_proc;
	extern get_monitor_info_t og_get_monitor_info;
	extern lockdown_check_vm_t og_check_vm;
	// -------------------------------------------------------

	// HOOKED FUNCTIONS --------------------------------------
	extern int do_some_stuff(int* a1);
	extern bool do_some_other_stuff(int* a1);
	extern bool do_yet_more_stuff();
	extern int do_some_other_stuff_s(int * a1);
	extern LSTATUS disable_task_manager(void* a1);
	extern BOOL empty_clipboard();
	extern void lockdown_log(char* a1, ...);
	extern HANDLE WINAPI create_file(LPCSTR lpFileName,
	                                 DWORD dwDesiredAccess,
	                                 DWORD dwShareMode,
	                                 LPSECURITY_ATTRIBUTES lpSecurityAttributes,
	                                 DWORD dwCreationDisposition,
	                                 DWORD dwFlagsAndAttributes,
	                                 HANDLE hTemplateFile);
	extern BOOL check_foreground_window();
	extern NTSTATUS WINAPI nt_query_system_information(SYSTEM_INFORMATION_CLASS SystemInformationClass,
	                                                   PVOID SystemInformation,
	                                                   ULONG SystemInformationLength,
	                                                   PULONG ReturnLength);

	extern LRESULT CALLBACK wnd_proc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	extern BOOL WINAPI get_monitor_info(HMONITOR      hMonitor,
		LPMONITORINFO lpmi);
	extern int check_vm();
	// -------------------------------------------------------
}
