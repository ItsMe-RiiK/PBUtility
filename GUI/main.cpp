#include <string>
#include <cstdlib>
#include "core.h" 

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            if (wmId == 1)
            {
                macroEnabled = (SendMessage(hBtnMacro, BM_GETCHECK, 0, 0) == BST_CHECKED);
            }
            else if (wmId == 5)
            {
                showCrosshair = (SendMessage(hBtnCrosshairToggle, BM_GETCHECK, 0, 0) == BST_CHECKED);
            }
            else if (wmId == 2)
            {
                crosshairMode = (crosshairMode == 1) ? 2 : 1;
                std::string txt = "Crosshair: " + std::string(crosshairMode == 1 ? "DOT" : "PLUS");
                SetWindowTextA(hTextCrosshairMode, txt.c_str());
            }
            else if (wmId == 3)
            {
                macroMode = (macroMode == 1) ? 2 : 1;
                std::string modeText = "Macro Mode: " + std::string(macroMode == 1 ? "Scope 3Q" : "3Q");
                SetWindowTextA(hTextMode, modeText.c_str());
            }
            else if (wmId == 6)
            {
                crosshairColorIndex++;
                if (crosshairColorIndex > 5) crosshairColorIndex = 0;

                std::string colorName = "";
                switch (crosshairColorIndex)
                {
                    case 0: colorName = "Green"; break;
                    case 1: colorName = "Red"; break;
                    case 2: colorName = "Cyan"; break;
                    case 3: colorName = "Yellow"; break;
                    case 4: colorName = "Magenta"; break;
                    case 5: colorName = "White"; break;
                }
                std::string txt = "Color: " + colorName;
                SetWindowTextA(hTextCrosshairColor, txt.c_str());
            }
            else if (wmId == 4)
            {
                char buffer[10];
                GetWindowTextA(hInputDelay, buffer, 10);
                int newDelay = atoi(buffer);
                if (newDelay >= 5)
                {
                    delayMs = newDelay;
                    MessageBoxA(hwnd, ("Delay set to " + std::to_string(delayMs) + "ms").c_str(), "Success", MB_OK);
                }
                else
                {
                    MessageBoxA(hwnd, "Minimal delay is 5ms!", "Error", MB_OK | MB_ICONERROR);
                    SetWindowTextA(hInputDelay, std::to_string(delayMs).c_str());
                }
            }
            break;
        }
        case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    StartMacroEngine();

    WNDCLASSA wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "PBUtility_GUI";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(
        0, wc.lpszClassName, "Point Blank Utility",
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 420, 350,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    hBtnMacro = CreateWindowA("BUTTON", "Enable Macro (F1)",
        WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
        20, 20, 180, 30, hwnd, (HMENU)1, hInstance, NULL);

    hBtnMode = CreateWindowA("BUTTON", "Change Macro Mode (F3)",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        20, 60, 180, 30, hwnd, (HMENU)3, hInstance, NULL);

    hTextMode = CreateWindowA("STATIC", "Macro Mode: Scope 3Q",
        WS_VISIBLE | WS_CHILD,
        210, 65, 180, 20, hwnd, NULL, hInstance, NULL);

    hBtnCrosshairToggle = CreateWindowA("BUTTON", "Enable Crosshair (F2)",
        WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
        20, 100, 180, 30, hwnd, (HMENU)5, hInstance, NULL);

    hBtnCrosshairMode = CreateWindowA("BUTTON", "Change Crosshair (F4)",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        20, 140, 180, 30, hwnd, (HMENU)2, hInstance, NULL);

    hTextCrosshairMode = CreateWindowA("STATIC", "Crosshair: DOT",
        WS_VISIBLE | WS_CHILD,
        210, 145, 180, 20, hwnd, NULL, hInstance, NULL);

    CreateWindowA("BUTTON", "Change Color (F5)",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        20, 180, 180, 30, hwnd, (HMENU)6, hInstance, NULL);

    hTextCrosshairColor = CreateWindowA("STATIC", "Color: Green",
        WS_VISIBLE | WS_CHILD,
        210, 185, 180, 20, hwnd, NULL, hInstance, NULL);

    CreateWindowA("STATIC", "Macro Delay (-/+):",
        WS_VISIBLE | WS_CHILD,
        20, 235, 130, 20, hwnd, NULL, hInstance, NULL);

    hInputDelay = CreateWindowA("EDIT", "35",
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
        160, 233, 40, 20, hwnd, NULL, hInstance, NULL);

    hBtnSetDelay = CreateWindowA("BUTTON", "Set",
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        210, 232, 50, 22, hwnd, (HMENU)4, hInstance, NULL);


    hUIDelay = hInputDelay;
    hUIMacroMode = hTextMode;
    hUICrosshairMode = hTextCrosshairMode;

    ShowWindow(hwnd, nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}