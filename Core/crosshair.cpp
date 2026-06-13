#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "core.h"

const char* TARGET_WINDOW_CH = "Point Blank";

const COLORREF chColors[6] = {
    RGB(0, 255, 0),
    RGB(255, 0, 0),
    RGB(0, 255, 255),
    RGB(255, 255, 0),
    RGB(255, 0, 255),
    RGB(255, 255, 255)
};

static LRESULT CALLBACK OverlayProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (uMsg == WM_PAINT)
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        int centerX = 20;
        int centerY = 20;

        if (showCrosshair)
        {
            COLORREF currentColor = chColors[crosshairColorIndex];

            if (crosshairMode == 1) // 1. DOT
            {
                HBRUSH hBrush = CreateSolidBrush(currentColor);
                HPEN hPen = CreatePen(PS_SOLID, 1, currentColor);

                HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
                HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

                int r = 2;

                POINT ketupat[4] = {
                    { centerX, centerY - r },
                    { centerX + r, centerY },
                    { centerX, centerY + r },
                    { centerX - r, centerY }
                };

                Polygon(hdc, ketupat, 4);

                SelectObject(hdc, hOldBrush);
                SelectObject(hdc, hOldPen);
                DeleteObject(hBrush);
                DeleteObject(hPen);
            }
            else if (crosshairMode == 2) // 2. PLUS
            {
                HBRUSH hBrush = CreateSolidBrush(currentColor);

                int lineLen = isGameWindowed ? 10 : 11;

                RECT rectH = { centerX - lineLen, centerY - 1, centerX + lineLen + 1, centerY + 1 };
                RECT rectV = { centerX - 1, centerY - lineLen, centerX + 1, centerY + lineLen + 1 };

                FillRect(hdc, &rectH, hBrush);
                FillRect(hdc, &rectV, hBrush);
                DeleteObject(hBrush);
            }
        }
        EndPaint(hwnd, &ps);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CrosshairThread()
{
    WNDCLASSA wc = { 0 };
    wc.lpfnWndProc = OverlayProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = "SmartCrosshairOverlay";
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
    RegisterClassA(&wc);

    overlayHWND = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        wc.lpszClassName, "SmartCrosshair", WS_POPUP,
        0, 0, 40, 40,
        NULL, NULL, wc.hInstance, NULL
    );

    SetLayeredWindowAttributes(overlayHWND, RGB(0, 0, 0), 0, LWA_COLORKEY);

    while (true)
    {
        MSG msg;
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (showCrosshair != lastShowState || crosshairMode != lastModeState || crosshairColorIndex != lastColorState)
        {
            InvalidateRect(overlayHWND, NULL, TRUE);
            lastShowState = showCrosshair;
            lastModeState = crosshairMode;
            lastColorState = crosshairColorIndex;
        }

        HWND gameHwnd = FindWindowA(NULL, TARGET_WINDOW_CH);

        if (showCrosshair && gameHwnd != NULL && gameHwnd == GetForegroundWindow())
        {
            RECT rect;
            GetClientRect(gameHwnd, &rect);
            POINT pt = { (rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2 };
            ClientToScreen(gameHwnd, &pt);

            LONG windowStyle = GetWindowLong(gameHwnd, GWL_STYLE);
            bool currentWindowMode = (windowStyle & WS_CAPTION) != 0;

            if (isGameWindowed != currentWindowMode) {
                isGameWindowed = currentWindowMode;
                InvalidateRect(overlayHWND, NULL, TRUE);
            }

            int offsetX = currentWindowMode ? 1 : 0;
            int offsetY = currentWindowMode ? 1 : 0;

            int targetX = pt.x - 20 + offsetX;
            int targetY = pt.y - 20 + offsetY;

            SetWindowPos(overlayHWND, HWND_TOPMOST, targetX, targetY, 40, 40, SWP_SHOWWINDOW);
        }
        else
        {
            ShowWindow(overlayHWND, SW_HIDE);
        }
        Sleep(10);
    }
}