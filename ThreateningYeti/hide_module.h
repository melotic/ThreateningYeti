#pragma once
#include <WTypesbase.h>

namespace ty
{
	typedef struct _UNICODE_STRING
	{
		USHORT Length;
		USHORT MaximumLength;
		PWCH Buffer;
	} UNICODE_STRING;

	typedef UNICODE_STRING* PUNICODE_STRING;

	typedef struct _PEB_LDR_DATA
	{
		ULONG Length;
		BOOLEAN Initialized;
		PVOID SsHandle;
		LIST_ENTRY InLoadOrderModuleList;
		LIST_ENTRY InMemoryOrderModuleList;
		LIST_ENTRY InInitializationOrderModuleList;
	} PEB_LDR_DATA, *PPEB_LDR_DATA;


	typedef struct _PEB
	{
		UINT8 _PADDING_[12];
		PEB_LDR_DATA* Ldr;
	} PEB, *PPEB;

	typedef struct _LDR_MODULE
	{
		LIST_ENTRY InLoadOrderModuleList;
		LIST_ENTRY InMemoryOrderModuleList;
		LIST_ENTRY InInitializationOrderModuleList;
		PVOID BaseAddress;
		PVOID EntryPoint;
		ULONG SizeOfImage;
		UNICODE_STRING FullDllName;
		UNICODE_STRING BaseDllName;
		ULONG Flags;
		SHORT LoadCount;
		SHORT TlsIndex;
		LIST_ENTRY HashTableEntry;
		ULONG TimeDateStamp;
	} LDR_MODULE, *PLDR_MODULE;

#define UNLINK(x)					\
	(x).Flink->Blink = (x).Blink;	\
	(x).Blink->Flink = (x).Flink;

	class hide_module
	{
	public:
		static bool unlink_module_peb(HMODULE hModule)
		{
			// read peb
			auto peb = PPEB(__readfsdword(0x30));
			auto current_entry = peb->Ldr->InLoadOrderModuleList.Flink;

			// loop through module list
			while (current_entry != &peb->Ldr->InLoadOrderModuleList && current_entry != nullptr)
			{
				// get current module
				const auto current = CONTAINING_RECORD(current_entry, LDR_MODULE, InLoadOrderModuleList);
				
				if (current->BaseAddress == hModule)
				{
					// unlink module from peb
					UNLINK(current->InLoadOrderModuleList);
					UNLINK(current->InInitializationOrderModuleList);
					UNLINK(current->InMemoryOrderModuleList);

					return true;
				}

				current_entry = current_entry->Flink;
			}

			return false;
		}

		static bool remove_pe_header(HANDLE module_base)
		{
			const auto dos_header = PIMAGE_DOS_HEADER(module_base);
			const auto nt_header = PIMAGE_NT_HEADERS(PBYTE(dos_header) + DWORD(dos_header->e_lfanew));

			if (nt_header->Signature != IMAGE_NT_SIGNATURE)
			{
				return false;
			}

			if (nt_header->FileHeader.SizeOfOptionalHeader)
			{
				DWORD old_protect;
				const auto size = nt_header->FileHeader.SizeOfOptionalHeader;

				VirtualProtect(module_base, size, PAGE_EXECUTE_READWRITE, &old_protect);
				RtlSecureZeroMemory(module_base, size);
				VirtualProtect(module_base, size, old_protect, &old_protect);
				
				return true;
			}

			return false;
		}
	};
}
