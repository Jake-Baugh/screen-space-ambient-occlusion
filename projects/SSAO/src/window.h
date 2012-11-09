#ifndef _window_h_
#define _window_h_

#include <windows.h>
#include <string>
using namespace std;

#define ERROR_MB(x) MessageBox(NULL, x, L"ERROR", MB_OK | MB_ICONEXCLAMATION);

class Window
{
public:
	Window();
	~Window();

	int getWidth();
	int getHeight();
	HWND getHandle();

	void createWindow(wstring title);
	LRESULT messageHandler(UINT msg, WPARAM wParam, LPARAM lParam);
	void SetWindowCaption(const std::wstring caption);

private:
	HINSTANCE	m_hInstance;
	HWND		m_hWnd;
	wstring		m_windowTitle;

	bool m_minimized;
	bool m_maximized;
	bool m_resizing;

	int m_clientWidth;
	int m_clientHeight;

};

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif
