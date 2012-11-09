#include "Window.h"

/**
	Once a WM_CREATE command is received, reroute messages to the 
	created window.
**/
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static Window* appWindow = 0;

	switch(msg)
	{
	case WM_CREATE:
		CREATESTRUCT* cs = (CREATESTRUCT*)lParam;
		appWindow = (Window*)cs->lpCreateParams;
		return 0;
	}

	if(appWindow)
		return appWindow->messageHandler(msg, wParam, lParam);
	else 
		return DefWindowProc(hWnd, msg, wParam, lParam);
}

/**
	Initialize Window's private member variables
**/
Window::Window()
	: m_clientWidth(1600), m_clientHeight(900), m_hInstance(0), m_hWnd(0),
	m_maximized(false), m_minimized(false), m_resizing(false)
{}

Window::~Window()
{}

int Window::getWidth() { return m_clientWidth; }
int Window::getHeight() { return m_clientHeight; }
HWND Window::getHandle() { return m_hWnd; }

/**
	Create a Win32 window
**/
void Window::createWindow(wstring title)
{
	WNDCLASSEX wcex;

	m_windowTitle = title;
	m_hInstance = GetModuleHandle(NULL);

	wcex.cbSize			= sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= m_hInstance;
	wcex.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= m_windowTitle.c_str();
	wcex.hIconSm		= 0;

	if(!RegisterClassEx(&wcex))
	{
		ERROR_MB(L"Failed to Register WndClass");
		PostQuitMessage(0);
	}

	UINT styleBorderless = WS_EX_TOPMOST | WS_POPUP;
	UINT styleNormal = WS_OVERLAPPEDWINDOW;

	RECT R = {0, 0, m_clientWidth, m_clientHeight};
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	m_hWnd = CreateWindow(m_windowTitle.c_str(), m_windowTitle.c_str(), 
			 styleNormal, CW_USEDEFAULT, CW_USEDEFAULT, m_clientWidth, m_clientHeight, 
			 0, 0, m_hInstance, this);
	if(!m_hWnd)
	{
		ERROR_MB(L"Create Window Failed");
		PostQuitMessage(0);
	}

	ShowWindow(m_hWnd, SW_SHOW);
	SetForegroundWindow(m_hWnd);
	SetFocus(m_hWnd);
	UpdateWindow(m_hWnd);
	m_maximized = true;
	
}

/** 
	Process messages for the Window
**/
LRESULT Window::messageHandler(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_MOUSEMOVE:
		return 0;
	case WM_LBUTTONDOWN:
		return 0;
	case WM_RBUTTONDOWN:
		return 0;
	case WM_LBUTTONUP:
		return 0;
	case WM_RBUTTONUP:
		return 0;
	case WM_KEYDOWN:
		return 0;
	case WM_KEYUP:
		return 0;
	case WM_SIZE: //User resizes window
		m_clientWidth = LOWORD(lParam);
		m_clientHeight = HIWORD(lParam);
		switch(wParam)
		{
		case SIZE_MINIMIZED:
			m_minimized = true;
			m_maximized = false;
			break;
		case SIZE_MAXIMIZED:
			m_minimized = false;
			m_maximized = true;
			break;
		case SIZE_RESTORED:
			if(m_minimized) //check previous state
			{
				m_minimized = false;
			}	
			else if(m_maximized)
			{
				m_maximized = false;
			}
			else if(m_resizing) 
			{ /*do nothing while user is still resizing*/ }
			else 
			{
			}
			break;
		}
		return 0;
	case WM_ENTERSIZEMOVE: //Users grabs resize bars
		m_resizing = true;
		return 0;
	case WM_EXITSIZEMOVE: //User releases resize bars
		m_resizing = false;
	//	m_eventManager.OnResize(m_clientWidth, m_clientHeight);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_GETMINMAXINFO: //Prevent window from becoming too small
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
		return 0;
	case WM_SYSCOMMAND:
		switch(wParam)
		{
		case SC_CLOSE:  //Application being closed
			break;
		case SC_SCREENSAVE: //ScreenSaver
			break;
		case SC_MONITORPOWER: //Power saving mode
			break;
		}
		break;
	}

	return DefWindowProc(m_hWnd, msg, wParam, lParam);
}


void Window::SetWindowCaption(const std::wstring caption)
{
	SetWindowText(m_hWnd, caption.c_str());
}
