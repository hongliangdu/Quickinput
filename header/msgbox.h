#pragma once

#include <windows.h>
#include "string.h"

namespace CG {
	class MsgBox
	{
	public:

		MsgBox() {}

		MsgBox(std::wstring text, LPCWSTR title = L"Message", LONG style = MB_OK)
		{
			MessageBoxW(0, text.c_str(), title, style);
		}

		template<typename T>
		static int Message(T text, LPCWSTR title = L"Message", LONG style = MB_OK) { return MessageBoxW(0, String::toWString(text).c_str(), title, style); }
		static int Warning(LPCWSTR text, LPCWSTR title = L"Warning", LONG style = MB_OK | MB_ICONWARNING | MB_TOPMOST) { return MessageBoxW(0, text, title, style); }
		static int Error(LPCWSTR text, LPCWSTR title = L"Error", LONG style = MB_OK | MB_ICONERROR | MB_TOPMOST) { return MessageBoxW(0, text, title, style); }

		static void Rect(RECT rect) {
			WCHAR str[64];
			swprintf_s(str, 64, L"%ld, %ld, %ld, %ld", rect.left, rect.top, rect.right, rect.bottom);
			MessageBoxW(0, str, L"Rect", MB_TOPMOST);
		}
		/*
		static void Rect(FRECT frect) {
			WCHAR str[64];
			swprintf_s(str, 64, L"%g, %g, %g, %g", frect.left, frect.top, frect.right, frect.bottom);
			MessageBoxW(0, str, L"FRect", MB_TOPMOST);
		}
		*/
		static void Point(POINT point) {
			WCHAR str[64];
			swprintf_s(str, 64, L"%ld, %ld", point.x, point.y);
			MessageBoxW(0, str, L"Point", MB_TOPMOST);
		}
		/*
		static void Point(FPOINT fpoint) {
			WCHAR str[64];
			swprintf_s(str, 64, L"%g, %g", fpoint.x, fpoint.y);
			MessageBoxW(0, str, L"FPoint", MB_TOPMOST);
		}
		*/
	};
}