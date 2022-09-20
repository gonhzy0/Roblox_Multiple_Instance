
#include "defs.h"

#define NT_SUCCESS(x) ((x) >= 0)

HANDLE FindHandle(DWORD pid)
{
 HANDLE Process = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

 SYSTEM_HANDLE_INFORMATION handleInfo;

 PSYSTEM_HANDLE_INFORMATION pSysHandleInfo = new SYSTEM_HANDLE_INFORMATION;
 ULONG retlen;
 DWORD size = sizeof(SYSTEM_HANDLE_INFORMATION);
 NtQuerySystemInformation(16,
 pSysHandleInfo, size, &retlen);

 delete pSysHandleInfo;
 size = retlen + 1024;

 PSYSTEM_HANDLE_INFORMATION HandleInfo = (PSYSTEM_HANDLE_INFORMATION)new BYTE[size];

 NtQuerySystemInformation(16,
 HandleInfo, size, &retlen);

 for (ULONG i = 0; i < HandleInfo->NumberOfHandles; i++)
 {

 SYSTEM_HANDLE pHandle = (HandleInfo->Handles[i]);

 if (pHandle.ProcessId == pid)
 {
 HANDLE DuplicatedHandle;

 BOOL b = DuplicateHandle(Process, (HANDLE)pHandle.Handle,
 GetCurrentProcess(), &DuplicatedHandle, 0, FALSE, DUPLICATE_SAME_ACCESS);
 if (!b) continue;

 PVOID nameBuffer = malloc(0x1000);

 if (!NT_SUCCESS(NtQueryObject((HANDLE)DuplicatedHandle, ObjectNameInformation,
 nameBuffer, 0x1000, nullptr))) continue;

 auto name = reinterpret_cast<UNICODE_STRING*>(nameBuffer);


 if (name->Length > 29 && wcscmp(name->Buffer + 29, L"ROBLOX_singletonEvent") == 0)
 {
 printf("Handle Found and Closing: %S \n You can now open another roblox instance! ", name->Buffer);
 DuplicateHandle(Process, (HANDLE)pHandle.Handle, ::GetCurrentProcess(), &DuplicatedHandle,
 0, FALSE, DUPLICATE_CLOSE_SOURCE);

 free(nameBuffer);

 CloseHandle(Process);

 return DuplicatedHandle;

 }



 }

 }

 printf("Handle Already Closed.");
 CloseHandle(Process);
 return 0;
}

void CloseHandle() {
 HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
 if (hSnapshot == INVALID_HANDLE_VALUE)
 return;

 PROCESSENTRY32 pe;
 pe.dwSize = sizeof(pe);


 Process32First(hSnapshot, &pe);

 DWORD pid = 0;
 while (Process32Next(hSnapshot, &pe)) {
 if (_wcsicmp(pe.szExeFile, L"RobloxPlayerBeta.exe") == 0) {
 CloseHandle(hSnapshot);

 pid = pe.th32ProcessID;

 HANDLE handle = FindHandle(pid);

 CloseHandle(handle);

 return;
 }

 }
 std::cout << "Make Sure Roblox is running!" << std::endl;

}

int main()
{

 NtQuerySystemInformation = (fnNtQuerySystemInformation)GetProcAddress(GetModuleHandleA("ntdll"), "NtQuerySystemInformation");
 NtQueryObject = (fnNtQueryObj)GetProcAddress(GetModuleHandleA("ntdll"), "NtQueryObject");

 CloseHandle();

 std::cin.get();
}

