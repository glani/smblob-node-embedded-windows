#include <windows.h>

int PressAnyKey( const char *prompt )
{
    DWORD        mode;
    HANDLE       hstdin;
    INPUT_RECORD inrec;
    DWORD        count;
    char         default_prompt[] = "Press a key to continue...";

    /* Set the console mode to no-echo, raw input, */
    /* and no window or mouse events.              */
    hstdin = GetStdHandle( STD_INPUT_HANDLE );
    if (hstdin == INVALID_HANDLE_VALUE
        || !GetConsoleMode( hstdin, &mode )
        || !SetConsoleMode( hstdin, 0 ))
        return 0;

    if (!prompt) prompt = default_prompt;

    /* Instruct the user */
    WriteConsole(
            GetStdHandle( STD_OUTPUT_HANDLE ),
            prompt,
            lstrlen( prompt ),
            &count,
            NULL
    );

    FlushConsoleInputBuffer( hstdin );

    /* Get a single key RELEASE */
    do ReadConsoleInput( hstdin, &inrec, 1, &count );
    while ((inrec.EventType != KEY_EVENT) || inrec.Event.KeyEvent.bKeyDown);

    /* Restore the original console mode */
    SetConsoleMode( hstdin, mode );

    return inrec.Event.KeyEvent.wVirtualKeyCode;
}