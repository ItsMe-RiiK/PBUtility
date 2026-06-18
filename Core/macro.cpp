#include "core.hpp"
#include "XOR.hpp"
#include <string>

static void SendKey(WORD wVk)
{
    INPUT input = { 0 };
    input.type = INPUT_KEYBOARD;
    input.ki.wVk = wVk;

    SendInput(1, &input, sizeof(INPUT));

    Sleep(20);

    input.ki.dwFlags = KEYEVENTF_KEYUP;
    SendInput(1, &input, sizeof(INPUT));
}

static void SendMouseClick(DWORD dwDown, DWORD dwUp)
{
    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = dwDown;

    SendInput(1, &input, sizeof(INPUT));

    Sleep(20);

    input.mi.dwFlags = dwUp;
    SendInput(1, &input, sizeof(INPUT));
}

void MacroThread()
{
    std::string targetWindow = XOR("Point Blank");

    bool isLeftHeld = false;
    bool isRightHeld = false;

    HWND gameHwnd = NULL;
    int checkCounter = 1000;

    while (true)
    {
        Sleep(1);
        if (checkCounter++ >= 1000)
        {
            gameHwnd = FindWindowA(NULL, targetWindow.c_str());
            checkCounter = 0;
        }

        if (gameHwnd != NULL && gameHwnd == GetForegroundWindow())
        {
            if (macroEnabled)
            {
                if (macroMode == 1) // SCOPE 3Q
                {
                    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
                    {
                        if (!isRightHeld)
                        {
                            isRightHeld = true;
                            Sleep(delayMs + 35);
                            SendMouseClick(MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP);
                            Sleep(delayMs);
                            SendKey(0x33); //3
                            Sleep(delayMs);
                            SendKey(0x51); //Q
                        }
                    }
                    else
                    {
                        isRightHeld = false;
                    }
                }
                else if (macroMode == 2) // 3Q
                {
                    if (GetAsyncKeyState(VK_LBUTTON) & 0x8000)
                    {
                        if (!isLeftHeld)
                        {
                            isLeftHeld = true;
                            Sleep(delayMs);
                            SendKey(0x33);
                            Sleep(delayMs);
                            SendKey(0x51);
                        }
                    }
                    else
                    {
                        isLeftHeld = false;
                    }
                }
            }
        }
        else
        {
            isRightHeld = false;
            isLeftHeld = false;
        }
    }
}