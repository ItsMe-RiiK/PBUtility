#pragma once
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>

extern bool macroEnabled;
extern int macroMode;
extern int delayMs;
extern bool showCrosshair;
extern int crosshairMode;
extern int crosshairColorIndex;
extern bool isGameWindowed;

extern HWND hUIDelay;
extern HWND hUIMacroMode;
extern HWND hUICrosshairMode;
extern HWND overlayHWND;

extern bool lastShowState;
extern int lastModeState;
extern int lastColorState;

extern void StartMacroEngine();
extern void MacroThread();
extern void HotkeyThread();
extern void CrosshairThread();

extern HWND hBtnMacro, hBtnMode, hTextMode, hInputDelay, hBtnSetDelay;
extern HWND hBtnCrosshairMode, hTextCrosshairMode, hBtnCrosshairToggle;
extern HWND hTextCrosshairColor;