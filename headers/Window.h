#ifndef WINDOW
#include <Windows.h>
#define WINDOW
typedef struct {
	int width, height, x, y;
	unsigned int  window_width, window_height;
}WindowSet;
class Window 
{
private:
	HWND hWindow;
	WNDCLASSEX wc;
public:
	Window(WindowSet, LRESULT(CALLBACK* WndProc)(HWND, UINT, WPARAM, LPARAM));
	HWND GetHWND();
	~Window();
};
#endif
