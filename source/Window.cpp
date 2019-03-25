#include "../headers/Window.h"
#include <iostream>
#ifdef WINDOW

Window::Window(WindowSet ws, LRESULT(CALLBACK* WndProc)(HWND, UINT, WPARAM, LPARAM))
{
	WNDCLASSEX wc;
	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(wc);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "usual window";
	wc.cbWndExtra = NULL;
	wc.cbClsExtra = NULL;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = LoadIcon(NULL, IDI_WINLOGO);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hInstance = GetModuleHandle(NULL);
	wc.hbrBackground = CreateSolidBrush(RGB(255, 255, 255));
	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Error Creating Class", "Error", MB_OK);
		return;
	}
	if (ws.x == -1) {
		ws.x = ws.window_width / 2 - ws.width / 2;
	}
	if (ws.y == -1)
	{
		ws.y = ws.window_height / 2 - ws.height / 2;
	}
	this->hWindow = CreateWindowExA(WS_EX_ACCEPTFILES, "usual window", "DRaw",
		WS_OVERLAPPEDWINDOW,
		ws.x, ws.y,
		ws.width, ws.height,
		(HWND)NULL,
		(HMENU)NULL,
		(HINSTANCE)GetModuleHandle(NULL),
		NULL);
}

HWND Window::GetHWND()
{
	return HWND(this->hWindow);
}

Window::~Window() 
{

}

#endif