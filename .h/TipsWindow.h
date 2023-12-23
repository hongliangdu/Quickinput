#pragma once
#include "../static.h"

struct TipsWindow
{
	static HANDLE thread;
	static HWND wnd;
	static SIZE screen;
	static COLORREF color;
	static std::wstring text;
	static UINT time;

	static LRESULT CALLBACK TipsWndProc(HWND wnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (msg == WM_TIMER)
		{
			if (time < 1001)
			{
				BYTE alpha = (0.255f * (float)time);
				SetLayeredWindowAttributes(wnd, 0, alpha, LWA_ALPHA);
				if (!alpha) KillTimer(wnd, 0);
			}
			time -= 25;
			if (time < 26) time = 0;
		}
		else if (msg == WM_PAINT)
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(wnd, &ps);
			SIZE size = { 0 };

			SelectObject(hdc, Window::setFont(32, 900));
			GetTextExtentExPointW(hdc, text.c_str(), text.length(), 0, 0, 0, &size);

			size.cx += 35, size.cy += 20;

			RECT rect = { (screen.cx / 2) - (size.cx / 2), 0, (screen.cx / 2) + (size.cx / 2), size.cy };
			Window::Rect(wnd, rect);
			rect = { 0, 0, size.cx, size.cy };

			FillRect(hdc, &rect, CreateSolidBrush(color));
			SetBkMode(hdc, TRANSPARENT);
			SetTextColor(hdc, 0);
			TextOutW(hdc, 15, 10, text.c_str(), text.length());

			EndPaint(wnd, &ps);
		}

		return DefWindowProcW(wnd, msg, wParam, lParam);
	}

	static DWORD CALLBACK TipsWindowThread(LPVOID)
	{
		Window::Register(L"TipsWnd", TipsWndProc, 1, RGB(0xCC, 0xEE, 0xFF));
		wnd = Window::Create(L"TipsWnd", L"TipsWnd", WS_POPUP | WS_VISIBLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_NOACTIVATE, 1, 1, 0, 0);
		SetLayeredWindowAttributes(wnd, 0, 0, LWA_ALPHA);
		Window::Redraw(wnd);
		MSG msg;
		while (GetMessageW(&msg, wnd, WM_PAINT, WM_TIMER))
		{
			TranslateMessage(&msg);
			DispatchMessageW(&msg);
		}
		return 0;
	}

	static void Popup(std::wstring text, COLORREF color = RGB(0xCC, 0xEE, 0xFF), UINT time = 1000)
	{
		KillTimer(wnd, 0);
		if (!thread)
		{
			thread = Thread::Start(TipsWindowThread);
			while (!wnd) sleep(1);
		}
		if (!(Window::exStyle(wnd) & WS_EX_NOACTIVATE))
		{
			Window::ExStyle(wnd, WS_EX_NOACTIVATE);
		}
		Window::Top(wnd);
		TipsWindow::text = text;
		TipsWindow::color = color;
		TipsWindow::time = time;
		Window::Redraw(wnd);
		SetTimer(wnd, 0, 10, 0);
	}
	static void Show(std::wstring text, COLORREF color = RGB(0xCC, 0xEE, 0xFF))
	{
		KillTimer(wnd, 0);
		if (!thread)
		{
			thread = Thread::Start(TipsWindowThread);
			while (!wnd) sleep(1);
		}
		if (!(Window::exStyle(wnd) & WS_EX_NOACTIVATE))
		{
			Window::ExStyle(wnd, WS_EX_NOACTIVATE);
		}
		Window::Top(wnd);
		TipsWindow::text = text;
		TipsWindow::color = color;
		Window::Redraw(wnd);
		SetLayeredWindowAttributes(wnd, 0, 255, LWA_ALPHA);
	}
	static void Hide()
	{
		KillTimer(wnd, 0);
		if (!thread)
		{
			thread = Thread::Start(TipsWindowThread);
			while (!wnd) sleep(1);
		}
		if (!(Window::exStyle(wnd) & WS_EX_NOACTIVATE))
		{
			Window::ExStyle(wnd, WS_EX_NOACTIVATE);
		}
		SetLayeredWindowAttributes(wnd, 0, 0, LWA_ALPHA);
	}
};