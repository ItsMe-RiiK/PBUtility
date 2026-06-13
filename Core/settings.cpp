#include "core.h"

bool macroEnabled = false;
int macroMode = 1;
int delayMs = 35;

bool showCrosshair = false;
int crosshairMode = 1;
int crosshairColorIndex = 0;
bool isGameWindowed = true;

HWND hUIDelay = NULL;
HWND hUIMacroMode = NULL;
HWND hUICrosshairMode = NULL;
HWND overlayHWND = NULL;

bool lastShowState = false;
int lastModeState = 1;
int lastColorState = 0;

HWND hBtnMacro = NULL, hBtnMode = NULL, hTextMode = NULL, hInputDelay = NULL, hBtnSetDelay = NULL;
HWND hBtnCrosshairMode = NULL, hTextCrosshairMode = NULL, hBtnCrosshairToggle = NULL;
HWND hTextCrosshairColor = NULL;