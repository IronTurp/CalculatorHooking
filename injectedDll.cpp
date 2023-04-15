#include <Windows.h>
#include <iostream>

// Shared memory structure for communication
struct SharedData {
    bool buttonClicked;
    int buttonID;
};

// Hook procedure for button click messages
LRESULT CALLBACK ButtonClickHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && wParam == WM_LBUTTONDOWN)
    {
        // lParam points to a CWPSTRUCT structure
        CWPSTRUCT* cwp = (CWPSTRUCT*)lParam;
        if (cwp->message == WM_LBUTTONDOWN)
        {
            // Check if the window class is "Button"
            wchar_t className[256];
            GetClassName(cwp->hwnd, className, 256);
            if (wcscmp(className, L"Button") == 0)
            {
                // Button clicked, update shared memory
                SharedData* sharedData = (SharedData*)::MapViewOfFile(::OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, L"SharedData"), FILE_MAP_ALL_ACCESS, 0, 0, sizeof(SharedData));
                if (sharedData != nullptr)
                {
                    sharedData->buttonClicked = true;
                    sharedData->buttonID = GetDlgCtrlID(cwp->hwnd);
                    ::UnmapViewOfFile(sharedData);
                }
            }
        }
    }

    // Call the next hook in the chain
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    static HHOOK buttonClickHook = NULL;

    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            // Set up a hook for button click messages
            buttonClickHook = SetWindowsHookEx(WH_CALLWNDPROC, ButtonClickHookProc, hModule, GetCurrentThreadId());
            break;
        }
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
            break;
        case DLL_PROCESS_DETACH:
        {
            // Remove the hook
            if (buttonClickHook != NULL)
            {
                UnhookWindowsHookEx(buttonClickHook);
            }
            break;
        }
    }
    return TRUE;
}
