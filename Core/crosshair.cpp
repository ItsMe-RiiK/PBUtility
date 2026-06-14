#include "core.h"

const char* TARGET_WINDOW_CH = "Point Blank";

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

                // --- Setting Thickness for DOT ---
                int r = 2; // Default
                if (isGameWindowed)
                {
                    r = 3; // Thickness DOT (WINDOW MODE)
                }
                else
                {
                    r = 3; // Thickness DOT (FULLSCREEN MODE)
                }

                POINT ketupat[4] =
                {
                    {
                        centerX, centerY - r
                    },
                    {
                        centerX + r, centerY
                    },
                    {
                        centerX, centerY + r
                    },
                    {
                        centerX - r, centerY
                    }
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
                // default thickness and length
                int len = 10;
                int th = 1;

				// offset var for each line (Horizontal & Vertical) to allow independent adjustment if needed
                int offsetX_H = 0, offsetY_H = 0;
                int offsetX_V = 0, offsetY_V = 0;

				// Settings for Length, Thickness, & Offset position for Crosshair plus based on Window Mode or Fullscreen Mode
                if (isGameWindowed)
                {
                    len = 9;  // Length (WINDOW MODE)
                    th = 1;   // Thickness (WINDOW MODE)

					// Move Horizontal line slightly right in Window mode to better align with typical game crosshair positioning
                    offsetX_H = 0;
                    offsetY_H = 1;

                    // Move Vertical line slightly down in Window mode to better align with typical game crosshair positioning
                    offsetX_V = 1;
                    offsetY_V = 0;
                }
                else
                {
                    len = 11; // Length (FULLSCREEN MODE)
                    th = 1;   // Thickness (FULLSCREEN MODE)

					// Move Horizontal line slightly right in Fullscreen mode to better align with typical game crosshair positioning
                    offsetX_H = 0;
                    offsetY_H = 0;

					// Move Vertical line slightly down in Fullscreen mode to better align with typical game crosshair positioning
                    offsetX_V = 1;
                    offsetY_V = 0;
                }

				// Calculate center positions for Horizontal and Vertical lines with respective offsets
                int cxH = centerX + offsetX_H;
                int cyH = centerY + offsetY_H;

                int cxV = centerX + offsetX_V;
                int cyV = centerY + offsetY_V;

				// Draw Horizontal line
                RECT rectH =
                {
                    cxH - len, cyH - th, cxH + len + 1, cyH + th
                };

				// Draw Vertical line
                RECT rectV =
                {
                    cxV - th, cyV - len, cxV + th, cyV + len + 1
                };

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

            if (isGameWindowed != currentWindowMode)
            {
                isGameWindowed = currentWindowMode;
                InvalidateRect(overlayHWND, NULL, TRUE);
            }

            int offsetX = 0;
            int offsetY = 0;

            // Settings for Position (Overall Coordinates)
            if (currentWindowMode)
            {
                if (crosshairMode == 1)
                { // 1. DOT in WINDOW MODE
                    offsetX = 1;
                    offsetY = 1;
                }
                else if (crosshairMode == 2)
                { // 2. PLUS in WINDOW MODE
                    offsetX = 1;
                    offsetY = 1;
                }
            }
            else
            { // FULLSCREEN / FULL WINDOW MODE
                if (crosshairMode == 1)
                { // 3. DOT in FULLSCREEN MODE
                    offsetX = 0;
                    offsetY = 0;
                }
                else if (crosshairMode == 2)
                { // 4. PLUS in FULLSCREEN MODE
                    offsetX = 0;
                    offsetY = 1;
                }
            }

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