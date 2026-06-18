#include <string>
#include <cstdlib>
#include "core.hpp"
#include "XOR.hpp"

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

                std::string txt = XOR("Crosshair: ");
                if (crosshairMode == 1) txt += XOR("DOT");
                else txt += XOR("PLUS");

                SetWindowTextA(hTextCrosshairMode, txt.c_str());
            }
            else if (wmId == 3)
            {
                macroMode = (macroMode == 1) ? 2 : 1;

                // FIX: Hindari ternary operator untuk XOR
                std::string modeText = XOR("Macro Mode: ");
                if (macroMode == 1) modeText += XOR("Scope 3Q");
                else modeText += XOR("3Q");

                SetWindowTextA(hTextMode, modeText.c_str());
            }
            else if (wmId == 6)
            {
                crosshairColorIndex++;
                if (crosshairColorIndex > 5) crosshairColorIndex = 0;

                std::string colorName = "";
                switch (crosshairColorIndex)
                {
                    case 0: colorName = XOR("Green"); break;
                    case 1: colorName = XOR("Red"); break;
                    case 2: colorName = XOR("Cyan"); break;
                    case 3: colorName = XOR("Yellow"); break;
                    case 4: colorName = XOR("Magenta"); break;
                    case 5: colorName = XOR("White"); break;
                }
                std::string txt = std::string(XOR("Color: ")) + colorName;
                SetWindowTextA(hTextCrosshairColor, txt.c_str());
            }
            else if (wmId == 7)
            {
                espNameEnabled = (SendMessage(hBtnEspName, BM_GETCHECK, 0, 0) == BST_CHECKED);
                ToggleNameTagESP(espNameEnabled);
            }
            else if (wmId == 4)
            {
                char buffer[10];
                GetWindowTextA(hInputDelay, buffer, 10);
                int newDelay = atoi(buffer);
                if (newDelay >= 5)
                {
                    delayMs = newDelay;
                    std::string msg = std::string(XOR("Delay set to ")) + std::to_string(delayMs) + std::string(XOR("ms"));
                    MessageBoxA(hwnd, msg.c_str(), XOR("Success"), MB_OK);
                }
                else
                {
                    MessageBoxA(hwnd, XOR("Minimal delay is 5ms!"), XOR("Error"), MB_OK | MB_ICONERROR);
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

    std::string className = XOR("PBUtility_GUI");
    std::string windowTitle = XOR("Point Blank Utility");

    WNDCLASSA wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = className.c_str();
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(
        0, className.c_str(), windowTitle.c_str(),
        WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX,
        CW_USEDEFAULT, CW_USEDEFAULT, 420, 375,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    hBtnMacro = CreateWindowA(XOR("BUTTON"), XOR("Enable Macro (F1)"),
        WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
        20, 20, 180, 30, hwnd, (HMENU)1, hInstance, NULL);

    hBtnMode = CreateWindowA(XOR("BUTTON"), XOR("Change Macro Mode (F3)"),
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        20, 60, 180, 30, hwnd, (HMENU)3, hInstance, NULL);

    hTextMode = CreateWindowA(XOR("STATIC"), XOR("Macro Mode: Scope 3Q"),
        WS_VISIBLE | WS_CHILD,
        210, 65, 180, 20, hwnd, NULL, hInstance, NULL);

    hBtnCrosshairToggle = CreateWindowA(XOR("BUTTON"), XOR("Enable Crosshair (F2)"),
        WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
        20, 100, 180, 30, hwnd, (HMENU)5, hInstance, NULL);

    hBtnCrosshairMode = CreateWindowA(XOR("BUTTON"), XOR("Change Crosshair (F4)"),
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        20, 140, 180, 30, hwnd, (HMENU)2, hInstance, NULL);

    hTextCrosshairMode = CreateWindowA(XOR("STATIC"), XOR("Crosshair: DOT"),
        WS_VISIBLE | WS_CHILD,
        210, 145, 180, 20, hwnd, NULL, hInstance, NULL);

    CreateWindowA(XOR("BUTTON"), XOR("Change Color (F5)"),
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        20, 180, 180, 30, hwnd, (HMENU)6, hInstance, NULL);

    hTextCrosshairColor = CreateWindowA(XOR("STATIC"), XOR("Color: Green"),
        WS_VISIBLE | WS_CHILD,
        210, 185, 180, 20, hwnd, NULL, hInstance, NULL);

    hBtnEspName = CreateWindowA(XOR("BUTTON"), XOR("Enable NameTag (`)"),
        WS_VISIBLE | WS_CHILD | BS_AUTOCHECKBOX,
        20, 220, 180, 30, hwnd, (HMENU)7, hInstance, NULL);

    CreateWindowA(XOR("STATIC"), XOR("Macro Delay (-/+):"),
        WS_VISIBLE | WS_CHILD,
        20, 275, 130, 20, hwnd, NULL, hInstance, NULL);

    hInputDelay = CreateWindowA(XOR("EDIT"), XOR("35"),
        WS_VISIBLE | WS_CHILD | WS_BORDER | ES_NUMBER,
        160, 273, 40, 20, hwnd, NULL, hInstance, NULL);

    hBtnSetDelay = CreateWindowA(XOR("BUTTON"), XOR("Set"),
        WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
        210, 272, 50, 22, hwnd, (HMENU)4, hInstance, NULL);

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