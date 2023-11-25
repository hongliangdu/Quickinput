#pragma once

#include <windows.h>
#include <atlimage.h>
#include <dwmapi.h> //DwmGetWindowAttribute
#pragma comment(lib, "dwmapi.lib") //DwmGetWindowAttribute

#include "file.h"

namespace CG {
	class Image
	{
	public:

		static void ScreenShot(LPCWSTR path) {
			HDC hScr = GetDC(0);
			CImage image;
			SIZE scr = System::screenSize();;

			image.Create(scr.cx, scr.cy, GetDeviceCaps(hScr, BITSPIXEL));
			BitBlt(image.GetDC(), 0, 0, scr.cx, scr.cy, hScr, 0, 0, SRCCOPY);
			ReleaseDC(0, hScr);
			image.ReleaseDC();
			image.Save(path);
		}

		static bool WindowShot(LPCWSTR path) {
			CImage image;
			HDC hScr = GetDC(0);
			SIZE scr = System::screenVSize();
			HWND wnd = GetForegroundWindow();
			RECT rect = { 0 };
			DwmGetWindowAttribute(wnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));

			if (rect.left < 0) rect.left = 0;
			if (rect.top < 0) rect.top = 0;
			if (rect.right > scr.cx) rect.right = scr.cx;
			if (rect.bottom > scr.cy) rect.bottom = scr.cy;
			if (rect.right < 0) return 0;
			if (rect.bottom < 0) return 0;
			if (rect.left > scr.cx) return 0;
			if (rect.top > scr.cy) return 0;

			if (image.Create(rect.right - rect.left, rect.bottom - rect.top, GetDeviceCaps(hScr, BITSPIXEL))) {
				if (BitBlt(image.GetDC(), 0, 0, rect.right - rect.left, rect.bottom - rect.top, hScr, rect.left, rect.top, SRCCOPY)) {
					ReleaseDC(0, hScr);
					image.ReleaseDC();
					image.Save(path);
					return 1;
				}
			}
			ReleaseDC(0, hScr);
			image.ReleaseDC();
			return 0;
		}

		static bool ClipboardSave(LPCWSTR path) {
			sleep(100);
			OpenClipboard(0);

			if (IsClipboardFormatAvailable(CF_DIB))
			{
				BITMAPFILEHEADER fileHeader = { 0 };
				HANDLE clip = GetClipboardData(CF_DIB);
				GlobalLock(clip);
				DWORD clipSize = (DWORD)GlobalSize(clip);
				DWORD headerSize = sizeof(fileHeader);
				DWORD writeByts = 0;

				if (!clipSize) {
					CloseClipboard();
					return 0;
				}

				fileHeader.bfType = 0x4d42;
				fileHeader.bfSize = clipSize + headerSize;
				fileHeader.bfOffBits = headerSize + headerSize;

				HANDLE file = CreateFileW(path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				WriteFile(file, &fileHeader, headerSize, &writeByts, 0);
				WriteFile(file, clip, clipSize, &writeByts, 0);

				GlobalUnlock(clip);
				CloseHandle(file);
				CloseClipboard();
				if (File::FileState(path)) return 1;
			}
			return 0;
		}
	};
}