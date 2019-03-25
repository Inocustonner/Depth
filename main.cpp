#include <iostream>
#include <Windows.h>
#include "headers/Window.h"
#define WM_GO_FUCK_THIS_IMAGE 0x101
#define conFieldDesc 0x11000
#define bobFieldDesc 0x11001
HBITMAP hBitmap;
HBITMAP hExample;
HWND conField, bobField;
void ProcessArray(unsigned char * arr, int width, int height, int padding_size, WORD wBitsPerPixel, unsigned short contrast, short border_of_black);
void CompilerBmp(const char *path, unsigned short contrast, short border_of_black);
int LoadBmp(unsigned char ** arr, const char * path, int * sz, int *width, int *height);
void SaveFile(unsigned char *arr, const char *path,  int width, int height, int sz, WORD wBitsPerPixel, const unsigned int& padding_size);
inline void hBmpLoad(HWND hWnd, char * path, unsigned short contrast, unsigned short border_of_black)
{
	CompilerBmp(path, contrast, border_of_black);
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), path, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	hExample = (HBITMAP)LoadImage(GetModuleHandle(NULL), "./temp.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
	InvalidateRect(hWnd, NULL, TRUE);
}

void DrawBmp(HWND hWnd, HBITMAP hBmp, HDC hdc, int x, int y, long width = -1, long height = -1)
{
	BITMAP          bitmap;
	HDC             hdcMem;
	HGDIOBJ         oldBitmap;
	hdcMem = CreateCompatibleDC(hdc);
	oldBitmap = SelectObject(hdcMem, hBmp);
	GetObject(hBmp, sizeof(bitmap), &bitmap);
	if (width + 1) bitmap.bmWidth = width;
	if (height + 1) bitmap.bmHeight = height;
	BitBlt(hdc, x, y, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, oldBitmap);
	DeleteDC(hdcMem);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static unsigned short contrast, border_of_black = 0;
	static char path[MAX_PATH] = { 0 };
	switch (msg) {
	case WM_PAINT: 
	{
		PAINTSTRUCT		ps;
		HDC				hdc = BeginPaint(hWnd, &ps);
		RECT			rect;
		GetWindowRect(hWnd, &rect);
		DrawBmp(hWnd, hBitmap, hdc, 0, 50, rect.right / 2 - 2, rect.bottom - 50);
		DrawBmp(hWnd, hExample, hdc, rect.right / 2 + 4, 50, rect.right / 2 - 2, rect.bottom - 50);

		EndPaint(hWnd, &ps);
		break;
	}
	case WM_DROPFILES:
	{
		//char path[MAX_PATH] = { 0 };
		HDROP hDrop = (HDROP)wParam;
		if (DragQueryFile(hDrop, 0xFFFFFFFF, NULL, NULL) != 1)
		{
			MessageBox(NULL, "Droping multi files not supported", "Error", MB_OK | MB_ICONERROR);
			DragFinish(hDrop);
		}
		if (DragQueryFile(hDrop, 0x0, path, MAX_PATH))
		{
			for (int i = 0; i < strlen(path); i++)
			{
				if (path[i] == '\\') path[i] = '/';
			}
			hBmpLoad(hWnd, path, contrast, border_of_black);
		}
		DragFinish(hDrop);
		break;
	}
	case WM_CREATE:
	{
		HFONT hFont = CreateFont(30, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_OUTLINE_PRECIS,
			CLIP_DEFAULT_PRECIS, CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Arial"));

		HWND conField = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT(""),
			WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_CENTER,
			5, 20, 
			70,30, 
			hWnd, (HMENU)conFieldDesc, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);
		HWND bobField = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("Edit"), TEXT(""),
			WS_CHILD | WS_VISIBLE | ES_NUMBER | ES_CENTER,
			110, 20,
			70, 30,
			hWnd, (HMENU)bobFieldDesc, (HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE), NULL);

		SendMessage(conField, EM_LIMITTEXT, 3, NULL);
		SendMessage(conField, WM_SETFONT, (WPARAM)hFont, TRUE);

		SendMessage(bobField, EM_LIMITTEXT, 3, NULL);
		SendMessage(bobField, WM_SETFONT, (WPARAM)hFont, TRUE);

		HWND hwndButton = CreateWindow(
			"BUTTON",
			"Compile Image",     
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			200,
			20,
			100,
			30,
			hWnd,
			(HMENU)WM_GO_FUCK_THIS_IMAGE,
			(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
			NULL);
		break;
	}
	case WM_COMMAND: 
	{
		if (wParam == WM_GO_FUCK_THIS_IMAGE) {
			char i[4] = { 0 };
			GetWindowText(GetDlgItem(hWnd, conFieldDesc), i, 4);
			contrast = std::atoi(i);
			GetWindowText(GetDlgItem(hWnd, bobFieldDesc), i, 4);
			border_of_black = std::atoi(i);
			if (*path != 0) hBmpLoad(hWnd, path, contrast, border_of_black);
		}
		break;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		DeleteObject(hBitmap);
		return 0L;
	}
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0L;
}

int WINAPI WinMain(
	HINSTANCE hInstance,
	HINSTANCE         hPrevInstance,
	LPSTR             lpCmdLine,
	int               nCmdShow)
{
	WindowSet ws = { 1200, 720, -1, -1, 1440, 900 };
	Window wnd(ws, &WndProc);
	UpdateWindow(wnd.GetHWND());
	ShowWindow(wnd.GetHWND(), nCmdShow);
	MSG msg;
	BOOL bRet;
	while (bRet = GetMessage(&msg, wnd.GetHWND(), NULL, NULL))
	{
		if (-1 == bRet)
		{
			return -1;
		}
		else
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}

int main()
{
	return WinMain(GetModuleHandle(NULL), NULL, NULL, 5);
}

void ProcessArray(unsigned char * arr, int width, int height, int padding_size, WORD wBitsPerPixel, unsigned short contrast, short border_of_black)
{
	WORD wPixelPerBit = wBitsPerPixel / 8;
	unsigned long pixels = height * ((width * wPixelPerBit) + padding_size);
	unsigned char *temp = (unsigned char *)malloc(sizeof(char) * pixels);
	memset(temp, 255, pixels);
	int hw = 0;
	int hh = 1;
	for (unsigned int i = 0; i < pixels; i += 3)
	{
		*(temp + i + 2) = *(temp + i + 1) = *(temp + i) = 255 * !((*(arr + i) +*(arr + i - (width)* hw * 3) + *(arr + i + (width)* hh * 3) + *(arr + i + 3) + *(arr + i - 3)) / 5 - *(arr + i) >= contrast |
			(*(arr + i + 1) + *(arr + i + 1 - width * hw * 3) + *(arr + i + 1 + width * hh * 3) + *(arr + i + 1 + 3) + *(arr + i + 1 - 3)) / 5 - *(arr + i + 1) >= contrast |
			(*(arr + i + 2) + *(arr + i + 2 - width * hw * 3) + *(arr + i + 2 + width * hh * 3) + *(arr + i + 2 + 3) + *(arr + i + 2 - 3)) / 5 - *(arr + i + 2) >= contrast);
		*(temp + i + 2) = *(temp + i + 1) = *(temp + i) = *(temp + i) * !((*(arr + i)
			+ *(arr + i + 1)
			+ *(arr + i + 2)
			+ *(arr + i + (width)* hh * 3)
			+ *(arr + i - (width)* hw * 3)) / 5
			<= border_of_black);
		hw = width * 3 <= i;
		hh = (i / (width * 3) == height - 1) ^ 1;
	}
	//TODO: Anti aliasing
	std::memcpy(arr, temp, pixels);
	free(temp);
}

int LoadBmp(unsigned char ** arr, const char * path, int * sz, int *width, int *height) {
	BITMAPFILEHEADER bmpf;
	BITMAPINFOHEADER bmpi;
	DWORD byteread;
	HANDLE hfile = CreateFile(
		path,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);

	if (hfile == NULL) 
	{
		MessageBox(NULL, "Unnable to open file", "Error", MB_OK);
		return 1;
	}

	if (!ReadFile(hfile, &bmpf, sizeof(BITMAPFILEHEADER), &byteread, NULL)) 
	{
		MessageBox(NULL, "Unnable to read file header", "Error", MB_OK);
		CloseHandle(hfile);
		return 2;
	}

	if (!ReadFile(hfile, &bmpi, sizeof(BITMAPINFOHEADER), &byteread, NULL)) {
		MessageBox(NULL, "Unnable to read info header", "Error", MB_OK);
		CloseHandle(hfile);
		return 3;
	}

	if (bmpf.bfType != 'MB') 
	{
		MessageBox(NULL, "Is not .bmp actually", "Error", MB_OK);
		CloseHandle(hfile);
		return 4;
	}

	if (bmpi.biCompression != BI_RGB) 
		return 5;

	if (bmpi.biBitCount != 24)
	{
		MessageBox(NULL, "The image must be in 24 - bit format", "Error", MB_OK);
		CloseHandle(hfile);
		return 6;
	}

	*width = bmpi.biWidth;
	*height = bmpi.biHeight;

	*sz = bmpf.bfSize - bmpf.bfOffBits;

	*arr = (BYTE *)malloc(*sz);

	SetFilePointer(hfile, bmpf.bfOffBits, NULL, FILE_BEGIN);

	if (!ReadFile(hfile, *arr, *sz, &byteread, NULL))
	{
		MessageBox(NULL, "Error reading file bytes", "Error", MB_OK);
		free(*arr);
		CloseHandle(hfile);
		return 7;
	}
	CloseHandle(hfile);
	return 0;
}

void SaveFile(unsigned char *arr, const char *path,int width, int height, int sz, WORD wBitsPerPixel, const unsigned int& padding_size) {
	DWORD bytewrited;
	unsigned long headers_size = sizeof(BITMAPFILEHEADER) +
		sizeof(BITMAPINFOHEADER);

	unsigned long pixel_data_size = height * ((width * (wBitsPerPixel / 8)) + padding_size);

	BITMAPINFOHEADER bmpi = { 0 };

	bmpi.biSize = sizeof(BITMAPINFOHEADER);

	bmpi.biBitCount = wBitsPerPixel;

	bmpi.biClrImportant = 0;
 
	bmpi.biClrUsed = 0;
 
	bmpi.biCompression = BI_RGB;

	bmpi.biHeight = height;
 
	bmpi.biWidth = width;

	bmpi.biPlanes = 1;
  
	bmpi.biSizeImage = pixel_data_size;

	BITMAPFILEHEADER bmpf = { 0 };

	bmpf.bfType = 0x4D42;
 
	bmpf.bfOffBits = headers_size;

	bmpf.bfSize = headers_size + pixel_data_size;

	HANDLE hfile = CreateFile(path,
		GENERIC_WRITE,
		FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hfile == INVALID_HANDLE_VALUE) 
	{
		MessageBox(NULL, "Unnable to create file", "Error", MB_OK);
		return;
	}

	if (!WriteFile(hfile, &bmpf, sizeof(bmpf), &bytewrited, NULL))
	{
		MessageBox(NULL, "Unnable to write file header", "Error", MB_OK);
		CloseHandle(hfile);
		return;
	}

	if (!WriteFile(hfile, &bmpi, sizeof(bmpi), &bytewrited, NULL)) 
	{
		MessageBox(NULL, "Unnable to write file info header", "Error", MB_OK);
		CloseHandle(hfile);
		return;
	}

	if (!WriteFile(hfile, arr, bmpi.biSizeImage, &bytewrited, NULL)) 
	{
		MessageBox(NULL, "Unnable to write RGB file data", "Error", MB_OK);
		CloseHandle(hfile);
		return;
	}
	CloseHandle(hfile);
}

void CompilerBmp(const char *path, unsigned short contrast, short border_of_black)
{
	unsigned char *buffer = { 0 };
	int sz, width, height, padding = 0;
	LoadBmp(&buffer, path, &sz, &width, &height);
	padding = (4 - ((width * 3) % 4)) % 4;
	ProcessArray(buffer,
		width,
		height,
		padding,
		24,
		contrast,
		border_of_black);
	SaveFile(buffer, "./temp.bmp", width, height, sz, 24, padding);
	free(buffer);
}


/*if ((*(arr + i) + *(arr + i - (width)* hw * 3) + *(arr + i + (width)* hh * 3) + *(arr + i + 3) + *(arr + i - 3)) / 5 - *(arr + i) >= contrast |
(*(arr + i + 1) + *(arr + i + 1 - width * hw * 3) + *(arr + i + 1 + width * hh * 3) + *(arr + i + 1 + 3) + *(arr + i + 1 - 3)) / 5 - *(arr + i + 1) >= contrast |
(*(arr + i + 2) + *(arr + i + 2 - width * hw * 3) + *(arr + i + 2 + width * hh * 3) + *(arr + i + 2 + 3) + *(arr + i + 2 - 3)) / 5 - *(arr + i + 2) >= contrast
)
{
	*(temp + i) = 0; *(temp + i + 1) = 0; *(temp + i + 2) = 0;
}
else if ((*(arr + i)
			+ *(arr + i + 1)
			+ *(arr + i + 2)
			+ *(arr + i + (width)* hh * 3)
			+ *(arr + i - (width)* hw * 3)) / 5
			<= border_of_black)
		{
			*(temp + i) = 0; *(temp + i + 1) = 0; *(temp + i + 2) = 0;
		}
*/