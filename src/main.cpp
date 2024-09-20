#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <winuser.h>
#include <chrono>
#include <thread>

using namespace std;

const char VERSION[] = "0.1.1";
bool debug_logging = false;
int disable_press_time_ms = 17;
int wait_before_disable_ms = 10;
int isPressed = 0;

void dbgPrint(const char *fmt...)
{
    if (!debug_logging)
    {
        return;
    }
    cout << "[DEBUG]: ";
    printf(fmt);
}

void releaseMouse(HWND hwnd, POINT cPos)
{
    dbgPrint("Releasing mouse left button\n");
    SendMessage(hwnd, WM_LBUTTONUP, 0, MAKELPARAM(cPos.x, cPos.y));
}

void cancel()
{
    dbgPrint("Duplicate detected, cancelling the mouse press...\n");
    POINT cursorPt;
    GetCursorPos(&cursorPt);
    HWND hwnd = WindowFromPoint(cursorPt);
    // Release the mouse first
    releaseMouse(hwnd, cursorPt);
    this_thread::sleep_for(chrono::milliseconds(wait_before_disable_ms));
    EnableWindow(hwnd, false);
    this_thread::sleep_for(chrono::milliseconds(disable_press_time_ms));
    dbgPrint("Reactivating foreground window...\n");
    EnableWindow(hwnd, true);
    cout << "is pressed " << isPressed << endl;
    if (isPressed == 0) {
        POINT cursorPt;
        GetCursorPos(&cursorPt);
        releaseMouse(hwnd, cursorPt);
    } else {
        releaseMouse(hwnd, cursorPt);
        dbgPrint("Button down mouse left button\n");
        POINT currentPt;
        GetCursorPos(&currentPt);
        SendMessage(hwnd, WM_LBUTTONDOWN, 0, MAKELPARAM(currentPt.x, currentPt.y));
    }
}

void loop()
{
    // Default to not pressed.
    SHORT prevKeyState = 0;
    // 0 = not pressed, X = ms since pressed state
    int recentlyPressed = 0;
    while (true)
    {
        SHORT keyState = GetKeyState(VK_LBUTTON);
        if (prevKeyState != keyState)
        {
            if (debug_logging)
            {
                printf("[DEBUG]: Recently pressed: %i\n", recentlyPressed);
            }
            // Key state < 0 means pressed.
            if (keyState < 0)
            {
                dbgPrint("pressed\n");
                isPressed = 1;
                if (recentlyPressed > 0)
                {
                    thread sugoma(cancel);
                    sugoma.detach();
                }
                else
                {
                    // 10 + 1 because we will -1 in the end.
                    recentlyPressed = disable_press_time_ms + 1;
                }
            }
            else
            {
                dbgPrint("not pressed\n");
                isPressed = 0;
            }
            prevKeyState = keyState;
        }
        if (recentlyPressed > 0)
        {
            recentlyPressed--;
        }
        this_thread::sleep_for(chrono::milliseconds(1));
    }
}

BOOL WINAPI CtrlHandler(DWORD fdwCtrlType)
{
    switch (fdwCtrlType)
    {
    default:
        printf("Exiting...\n");
        return FALSE;
    }
}

int main(int argc, char *argv[])
{
    SetConsoleTitleA(LPCSTR("Jane Doe"));
    printf("JaneDeduplicator v%s - https://github.com/teppyboy/JaneDeduplicator\n", VERSION);
    printf("\n!!! EARLY IMPLEMENTATION MAY NOT WORK AT ALL !!!\n\n");
    for (int i = 0; i < argc; i++)
    {
        std::string arg = std::string(argv[i]);
        if (arg == (std::string) "--debug")
        {
            debug_logging = true;
            printf("Debug logging enabled.\n");
        }
        else if (arg.substr(0, 21) == (std::string) "--disable-press-time=")
        {
            printf("Disable press time set to %s\n", arg.substr(21).c_str());
            disable_press_time_ms = std::stoi(arg.substr(21));
        }
        else if (arg.substr(0, 22) == (std::string) "--wait-before-disable=")
        {
            printf("Wait before disable set to %s\n", arg.substr(22).c_str());
            wait_before_disable_ms = std::stoi(arg.substr(22));
        }
        else if (arg.substr(0, 6) == (std::string) "--help")
        {
            printf("Usage: JaneDeduplicate.exe [OPTIONS]\n");
            printf("  --debug: Enable debug logging.\n");
            printf("  --disable-press-time=X: Disable press for X(ms) when duplication detected.\n");
            printf("  --wait-before-disable=X: Wait for X(ms) before disabling.\n");
            printf("  --help: Show this help.\n");
            printf("See https://github.com/teppyboy/JaneDeduplicator#command-line-arguments for more info.\n");
            return 0;
        }
    }
    printf("The program should be running now, press Ctrl + C to exit.\n");
    if (SetConsoleCtrlHandler(CtrlHandler, TRUE))
    {
        loop();
        return 0;
    }
    else
    {
        printf("\nERROR: Could not set control handler");
        return 1;
    }
}
