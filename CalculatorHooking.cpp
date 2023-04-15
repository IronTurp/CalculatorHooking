#include <Windows.h>
#include <iostream>
#include <detours.h>

// The original function prototype
typedef double(__stdcall* CalculatorAddFunction)(double, double);

// Function pointer to the original function
CalculatorAddFunction OriginalCalculatorAdd;

// Hooked function
double __stdcall HookedCalculatorAdd(double a, double b)
{
    // Add 7 to the result before returning it
    double modifiedResult = OriginalCalculatorAdd(a, b) + 7;
    std::cout << "Modified result: " << modifiedResult << std::endl;
    return modifiedResult;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        // Replace the Calculator "+" function with our hooked version
        OriginalCalculatorAdd = (CalculatorAddFunction)GetProcAddress(GetModuleHandle(L"CalcModule.dll"), "AddFunctionSymbol");
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)OriginalCalculatorAdd, HookedCalculatorAdd);
        DetourTransactionCommit();
        break;
    case DLL_PROCESS_DETACH:
        // Restore the original function
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)OriginalCalculatorAdd, HookedCalculatorAdd);
        DetourTransactionCommit();
        break;
    }
    return TRUE;
}
