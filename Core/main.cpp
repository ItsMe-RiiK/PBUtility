#include "core.h"
#include <thread>

void StartMacroEngine()
{
    std::thread t1(MacroThread);
    t1.detach();

    std::thread t2(HotkeyThread);
    t2.detach();

    std::thread t3(CrosshairThread);
    t3.detach();
}