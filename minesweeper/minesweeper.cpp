#include <windows.h>
#include <cstdio>
#include "declare.h"


Cell board[CellNum][CellNum];

// Step 1: Define the Window Procedure (Handles all events)
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0); // Posts the WM_QUIT message to our loop
        return 0;
    }
    case WM_ERASEBKGND:return 1;
    case WM_LBUTTONDOWN:
    {
        // 1. Get coordinates
        int xpos = LOWORD(lParam);
        int ypos = HIWORD(lParam);
		
		HandleLeftClick(xpos, ypos);
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
    }
    case WM_RBUTTONDOWN:
    {
		InsertFlag(LOWORD(lParam), HIWORD(lParam));
        InvalidateRect(hwnd, NULL, TRUE);
        return 0;
	}
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        // 1. Get the current window dimensions
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        int width = clientRect.right;
        int height = clientRect.bottom;

        // --- Start Double Buffering ---

        // 2. Create an off-screen memory DC
        HDC hdcMem = CreateCompatibleDC(hdc);

        // 3. Create a bitmap that fits the client area
        HBITMAP hbmScreen = CreateCompatibleBitmap(hdc, width, height);

        // 4. Select the bitmap into the memory DC (hdcMem is now our temporary canvas)
        HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmScreen);

        // 5. Draw everything to the invisible memory canvas (hdcMem)
        //    We need to manually draw the background color first, since we disabled WM_ERASEBKGND
        HBRUSH hBrush = CreateSolidBrush(GetSysColor(COLOR_BTNFACE));
        FillRect(hdcMem, &clientRect, hBrush);
        DeleteObject(hBrush);

        // Call your drawing function, but use the memory DC!
        DrawBoard(hdcMem);

        // 6. BLIT (Block Level Transfer) the finished image to the actual window (hdc)
        BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

        // 7. Clean up GDI objects (REVERSE ORDER of creation/selection)
        SelectObject(hdcMem, hbmOld); // Deselect the bitmap
        DeleteObject(hbmScreen);      // Destroy the temporary bitmap
        DeleteDC(hdcMem);             // Destroy the temporary DC

        // --- End Double Buffering ---

        EndPaint(hwnd, &ps);
        return 0;
    }
    default:

        // Pass all unhandled messages to the default procedure
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

// Step 2: The Entry Point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    // Step 2a: Define the "Blueprint"
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = L"MyGameWindowClass";

    // Step 2b: Register the "Blueprint"
    if (!RegisterClassEx(&wc))
    {
        MessageBox(NULL, L"Window Registration Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2c: Build the Window
    HWND hwnd = CreateWindowEx(
        0,
        L"MyGameWindowClass",
        L"My Minesweeper Game",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CellNum * CellSize + 17, CellNum * CellSize + 40,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL)
    {
        MessageBox(NULL, L"Window Creation Failed!", L"Error", MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // Step 2d: Show the Window
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    // Step 2e: The Message Loop 

	InitBoard(); // Initialize the game board
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        
    }

    return (int)msg.wParam;
}