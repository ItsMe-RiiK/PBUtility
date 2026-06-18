#include "core.hpp"
#include "XOR.hpp"
#include <string>

const COLORREF chColors[6] =
{
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

                int r = 2; // Default
                if (isGameWindowed) r = 3;
                else r = 3;

                POINT ketupat[4] =
                {
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
                int len = 10;
                int th = 1;

                int offsetX_H = 0, offsetY_H = 0;
                int offsetX_V = 0, offsetY_V = 0;

                if (isGameWindowed)
                {
                    len = 9;
                    th = 1;
                    offsetX_H = 0; offsetY_H = 1;
                    offsetX_V = 1; offsetY_V = 0;
                }
                else
                {
                    len = 11;
                    th = 1;
                    offsetX_H = 0; offsetY_H = 0;
                    offsetX_V = 1; offsetY_V = 0;
                }

                int cxH = centerX + offsetX_H;
                int cyH = centerY + offsetY_H;

                int cxV = centerX + offsetX_V;
                int cyV = centerY + offsetY_V;

                RECT rectH = { cxH - len, cyH - th, cxH + len + 1, cyH + th };
                RECT rectV = { cxV - th, cyV - len, cxV + th, cyV + len + 1 };

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
    wc.lpszClassName = overlayClassName.c_str();
    wc.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0));
    RegisterClassA(&wc);

    overlayHWND = CreateWindowExA(
        WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW,
        overlayClassName.c_str(), overlayWindowName.c_str(), WS_POPUP,
        0, 0, 40, 40,
        NULL, NULL, wc.hInstance, NULL
    );

    SetLayeredWindowAttributes(overlayHWND, RGB(0, 0, 0), 0, LWA_COLORKEY);

    int lastTargetX = -1;
    int lastTargetY = -1;

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

        if (showCrosshair && pbGameHwnd != NULL && pbGameHwnd == GetForegroundWindow())
        {
            RECT rect;
            GetClientRect(pbGameHwnd, &rect);
            POINT pt = { (rect.right - rect.left) / 2, (rect.bottom - rect.top) / 2 };
            ClientToScreen(pbGameHwnd, &pt);

            LONG windowStyle = GetWindowLong(pbGameHwnd, GWL_STYLE);
            bool currentWindowMode = (windowStyle & WS_CAPTION) != 0;

            if (isGameWindowed != currentWindowMode)
            {
                isGameWindowed = currentWindowMode;
                InvalidateRect(overlayHWND, NULL, TRUE);
                lastTargetX = -1;
            }

            int offsetX = 0;
            int offsetY = 0;

            if (currentWindowMode)
            {
                if (crosshairMode == 1)
                {
                    offsetX = 1; offsetY = 1;
                }
                else if (crosshairMode == 2)
                {
                    offsetX = 1; offsetY = 1;
                }
            }
            else
            {
                if (crosshairMode == 1)
                {
                    offsetX = 0; offsetY = 0;
                }
                else if (crosshairMode == 2)
                {
                    offsetX = 0; offsetY = 1;
                }
            }

            int targetX = pt.x - 20 + offsetX;
            int targetY = pt.y - 20 + offsetY;

            if (targetX != lastTargetX || targetY != lastTargetY)
            {
                SetWindowPos(overlayHWND, HWND_TOPMOST, targetX, targetY, 40, 40, SWP_SHOWWINDOW);
                lastTargetX = targetX;
                lastTargetY = targetY;
            }
            else
            {
                ShowWindow(overlayHWND, SW_SHOWNA);
            }
        }
        else
        {
            ShowWindow(overlayHWND, SW_HIDE);
        }
        Sleep(10);
    }
}