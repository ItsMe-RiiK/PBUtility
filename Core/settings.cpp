#include "core.hpp"
#include "XOR.hpp"

std::string targetWindow = XOR("Point Blank");
std::string targetExe = XOR("PointBlank.exe");
std::string overlayClassName = XOR("SmartCrosshairOverlay");
std::string overlayWindowName = XOR("SmartCrosshair");
DWORD pbProcessId = 0;
HWND pbGameHwnd = NULL;
std::atomic<bool> isScanningNameTag(false);

bool macroEnabled = false;
int macroMode = 1;
int delayMs = 35;

bool showCrosshair = false;
int crosshairMode = 1;
int crosshairColorIndex = 0;
bool isGameWindowed = true;

bool espNameEnabled = false;
HWND hBtnEspName = NULL;

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

HWND hStatusLabel = NULL;
bool isGameAttached = false;