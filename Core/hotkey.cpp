#include <string>
#include "core.h"

void HotkeyThread()
{
	bool isF1Held = false, isF2Held = false, isF3Held = false, isF4Held = false, isF5Held = false;
    bool isPlusHeld = false, isMinusHeld = false;

    while (true)
    {
        Sleep(1);

        if (GetAsyncKeyState(VK_F1) & 0x8000)
        {
            if (!isF1Held)
            {
                isF1Held = true;
                macroEnabled = !macroEnabled;
                if (hBtnMacro) SendMessageA(hBtnMacro, BM_SETCHECK, macroEnabled ? BST_CHECKED : BST_UNCHECKED, 0);
            }
        }
        else
        {
            isF1Held = false;
        }

        if (GetAsyncKeyState(VK_F2) & 0x8000)
        {
            if (!isF2Held)
            {
                isF2Held = true;
                showCrosshair = !showCrosshair;
                if (hBtnCrosshairToggle) SendMessageA(hBtnCrosshairToggle, BM_SETCHECK, showCrosshair ? BST_CHECKED : BST_UNCHECKED, 0);
            }
        }
        else
        {
            isF2Held = false;
        }

        if (GetAsyncKeyState(VK_F3) & 0x8000)
        {
            if (!isF3Held)
            {
                isF3Held = true;
                macroMode = (macroMode == 1) ? 2 : 1;
                if (hUIMacroMode)
                {
                    std::string txt = "Macro Mode: " + std::string(macroMode == 1 ? "Scope 3Q" : "3Q");
                    SetWindowTextA(hUIMacroMode, txt.c_str());
                }
            }
        }
        else
        {
            isF3Held = false;
        }

        if (GetAsyncKeyState(VK_F4) & 0x8000)
        {
            if (!isF4Held)
            {
                isF4Held = true;
                crosshairMode = (crosshairMode == 1) ? 2 : 1;
                if (hUICrosshairMode)
                {
                    std::string txt = "Crosshair: " + std::string(crosshairMode == 1 ? "DOT" : "PLUS");
                    SetWindowTextA(hUICrosshairMode, txt.c_str());
                }
            }
        }
        else
        {
            isF4Held = false;
        }

        if (GetAsyncKeyState(VK_F5) & 0x8000)
        {
            if (!isF5Held)
            {
                isF5Held = true;
                crosshairColorIndex++; // Naikkan indeks warna

                // Jika sudah melewati batas warna terakhir (5), kembali ke awal (0)
                if (crosshairColorIndex > 5) {
                    crosshairColorIndex = 0;
                }

                // Opsional: Jika ada UI Text untuk warna, update teksnya
                if (hTextCrosshairColor)
                {
                    std::string colorName = "";
                    switch (crosshairColorIndex) {
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
            }
        }
        else
        {
            isF5Held = false;
        }

        bool plusPressed = (GetAsyncKeyState(VK_ADD) & 0x8000) || (GetAsyncKeyState(VK_OEM_PLUS) & 0x8000);
        if (plusPressed)
        {
            if (!isPlusHeld)
            {
                isPlusHeld = true;
                delayMs += 5;
                if (hUIDelay) SetWindowTextA(hUIDelay, std::to_string(delayMs).c_str());
            }
        }
        else { isPlusHeld = false; }

        bool minusPressed = (GetAsyncKeyState(VK_SUBTRACT) & 0x8000) || (GetAsyncKeyState(VK_OEM_MINUS) & 0x8000);
        if (minusPressed)
        {
            if (!isMinusHeld)
            {
                isMinusHeld = true;

                if (delayMs > 5)
                {
                    delayMs -= 5;
                }

                if (hUIDelay) SetWindowTextA(hUIDelay, std::to_string(delayMs).c_str());
            }
        }
        else
        {
            isMinusHeld = false;
        }
    }
}