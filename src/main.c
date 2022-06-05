#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <process.h>

#include "include/MinHook.h"

typedef void(__stdcall *LoginCallBackHandler)(char *, int);
typedef int(__stdcall *SDKShowLoginPanel)(const char *, const bool, LoginCallBackHandler);
typedef int(__stdcall *SDKInit)(const char *, HWND);

#define SIZE 1024

void OnLogin(void *pCallBack)
{
    LoginCallBackHandler CallBack = pCallBack;

    // 读取 login.json, 格式参考 SDK 文档
    FILE *DataFile = fopen("login.json", "rb");
    char data[SIZE];
    fgets(data, SIZE, DataFile);
    // 烦死个人的转码
    char *pszMultiByte = data;
    int iSize;
    wchar_t *pwszUnicode;
    iSize = MultiByteToWideChar(CP_UTF8, 0, pszMultiByte, -1, NULL, 0);
    pwszUnicode = (wchar_t *)malloc(iSize * sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, pszMultiByte, -1, pwszUnicode, iSize);
    // 显示数据用以确认
    MessageBoxW(NULL, pwszUnicode, L"登录数据", MB_OK | MB_SYSTEMMODAL);
    switch (MessageBoxW(NULL, L"是否登录?\n请务必确保数据正确!", L"登录确认", MB_YESNO | MB_SYSTEMMODAL))
    {
    case 6:
        // 确认登录, 调用回调
        puts("yes");
        CallBack(data, strlen(data));
        break;
    default:
        // 其它操作, 一律取消
        puts("no");
        char *data2 = "{\"code\":-2,\"data\":{\"message\":\"cancel\"}}";
        CallBack(data2, strlen(data2));
    }
    // 执行完毕, 停止线程
    _endthread();
}

int __stdcall HookLogin(const char *szAppKey, const bool bBackToLogin, LoginCallBackHandler CallBack)
{
    puts("Hooked: SDKShowLoginPanel");
    // 开新线程, 防止阻塞
    _beginthread(OnLogin, 0, CallBack);
    return 0;
}

typedef HMODULE(WINAPI *OldLoadLibraryW)(LPCWSTR lpLibFileName);

OldLoadLibraryW OrgLoadLibraryW = NULL;

HMODULE WINAPI NewLoadLibraryW(LPCWSTR lpLibFileName)
{
    // 烦死个人的转码
    wchar_t *pwszUnicode = lpLibFileName;
    int iSize;
    char *pszMultiByte;
    iSize = WideCharToMultiByte(CP_ACP, 0, pwszUnicode, -1, NULL, 0, NULL, NULL);
    pszMultiByte = (char *)malloc(iSize * sizeof(char));
    WideCharToMultiByte(CP_ACP, 0, pwszUnicode, -1, pszMultiByte, iSize, NULL, NULL);
    // 找 PCGameSDK.dll
    if (strstr(pszMultiByte, "PCGameSDK.dll"))
    {
        puts("Hooking: PCGameSDK.dll");
        HMODULE module = OrgLoadLibraryW(lpLibFileName);
        // 找 SDKShowLoginPanel
        SDKShowLoginPanel SDKLoginFunc = (SDKShowLoginPanel)GetProcAddress(module, "SDKShowLoginPanel");
        SDKShowLoginPanel OrgLogin = NULL;
        // 找到了, Hook 掉
        if (MH_CreateHook(SDKLoginFunc, &HookLogin, (LPVOID *)(&OrgLogin)) != MH_OK)
            puts("Hook Create Error: PCGameSDK.dll");
        else if (MH_EnableHook(SDKLoginFunc) != MH_OK)
            puts("Hook Enable Error: PCGameSDK.dll");
        // Hook 成没成都得返回 HMODULE
        return module;
    }
    // 找没找到都得返回 HMODULE
    return OrgLoadLibraryW(lpLibFileName);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        // 来个控制台, 但好像不咋好用, 总是没有输出, 不知道为啥, 蹲个能给修一下的 dalao
        AllocConsole();
        freopen("CONIN$", "rb", stdin);
        freopen("CONOUT$", "wb", stdout);
        freopen("CONOUT$", "wb", stderr);
        // 初始化 MinHook
        puts("Init MinHook");
        if (MH_Initialize() != MH_OK)
        {
            puts("Hook Init Error!");
            return FALSE;
        }
        // PCGameSDK.dll 是动态加载的, 所以 Hook 掉 LoadLibraryW
        puts("Hooking: LoadLibraryW");
        if (MH_CreateHook(&LoadLibraryW, &NewLoadLibraryW, (LPVOID *)(&OrgLoadLibraryW)) != MH_OK)
        {
            puts("Hook Create Error: LoadLibraryW");
            return FALSE;
        }
        else if (MH_EnableHook(&LoadLibraryW) != MH_OK)
        {
            puts("Hook Enable Error: LoadLibraryW");
            return FALSE;
        }
    }
    return TRUE;
}
