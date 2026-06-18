#pragma once
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <string>
#include <atomic>

extern bool macroEnabled;
extern int macroMode;
extern int delayMs;
extern bool showCrosshair;
extern int crosshairMode;
extern int crosshairColorIndex;
extern bool isGameWindowed;

extern bool espNameEnabled;
extern HWND hBtnEspName;
extern void ToggleNameTagESP(bool enable);

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

extern HWND hStatusLabel;
extern bool isGameAttached;

extern std::string targetWindow;
extern std::string targetExe;
extern std::string overlayClassName;
extern std::string overlayWindowName;
extern DWORD pbProcessId;
extern HWND pbGameHwnd;
extern std::atomic<bool> isScanningNameTag;