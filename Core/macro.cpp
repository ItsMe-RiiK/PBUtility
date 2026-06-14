#include "core.h"

const char* TARGET_WINDOW = "Point Blank";

static void SendKey(BYTE key)
{
    keybd_event(key, 0, 0, 0);
    Sleep(20);
    keybd_event(key, 0, KEYEVENTF_KEYUP, 0);
}

static void SendMouseClick(DWORD dwDown, DWORD dwUp)
{
    mouse_event(dwDown, 0, 0, 0, 0);
    Sleep(20);
    mouse_event(dwUp, 0, 0, 0, 0);
}

void MacroThread()
{
    bool isLeftHeld = false;
    bool isRightHeld = false;

    while (true)
    {
        Sleep(1);

        HWND hwnd = FindWindowA(NULL, TARGET_WINDOW);

        if (hwnd != NULL && hwnd == GetForegroundWindow())
        {
            if (macroEnabled)
            {
                if (macroMode == 1)
                {
                    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000)
                    {
                        if (!isRightHeld)
                        {
                            isRightHeld = true;
                            Sleep(delayMs + 35);
                            SendMouseClick(MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_LEFTUP);
                            Sleep(delayMs);
                            SendKey(0x33);
                            Sleep(delayMs);
                            SendKey(0x51);
                        }
                    }
                    else
                    {
                        isRightHeld = false;
                    }
                }
                else if (macroMode == 2)
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