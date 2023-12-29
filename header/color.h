#pragma once

#include <windows.h>
#include <atlimage.h>
#include "base.h"
#include "string.h"

namespace CG {
	struct Rgb {
		Rgb() {}
		Rgb(byte r, byte g, byte b) { this->r = r, this->g = g, this->b = b; }
		Rgb(COLORREF rgb) { this->r = GetRValue(r), this->g = GetGValue(g), this->b = GetBValue(b); }
		byte b = 0;
		byte g = 0;
		byte r = 0;
	};

	struct Rgba {
		Rgba() {}
		Rgba(byte r, byte g, byte b, byte a) { this->r = r, this->g = g, this->b = b, this->a = a; }
		Rgba(COLORREF rgba) { this->r = GetRValue(r), this->g = GetGValue(g), this->b = GetBValue(b), this->a = GetAValue(a); }
		byte a = 0;
		byte b = 0;
		byte g = 0;
		byte r = 0;
	};

	template<class T>
	class PointMap
	{
		T* _map = 0;
		uint32 _width = 0;
		uint32 _height = 0;

	public:
		PointMap() { }
		PointMap(uint32 width, uint32 height) { create(width, height); }
		~PointMap() { release(); }
		void operator=(const PointMap& pointMap) { copy_s(pointMap); }
		T* operator[](uint32 row) { return _map + (_width * row); }
		T* accessMap() { return _map; }
		T& accessPoint(uint32 row, uint32 col) { return _map[((_width * row) + col)]; }
		uint32 bytes() { return ((_width * _height) * sizeof(T)); }
		uint32 width() { return _width; }
		uint32 height() { return _height; }
		void create(uint32 width = 1, uint32 height = 1) { release(); if (width && height) { _width = width; _height = height; _map = new T[_width * _height]; } }
		void copy(const PointMap& pointMap) { release(); create(pointMap._width, pointMap._height); memcpy_s(_map, _width * _height, pointMap._map, pointMap._width * pointMap._height); }
		void copy_s(const PointMap& pointMap) { release(); create(pointMap._width, pointMap._height); uint32 size = _width * _height; for (uint32 u = 0; u < size; u++) _map[u] = pointMap._map[u]; }
		void fill(const T& point) { for (uint32 u = 0; u < (_width * _height); u++) _map[u] = point; }
		void release() { if (_map) { delete[] _map; _map = 0; } _width = 0; _height = 0; }
	};

	typedef PointMap<Rgb> RgbMap;
	typedef PointMap<Rgba> RgbaMap;

	class Color
	{
	public:

		static COLORREF toColor(LPCWSTR code)
		{
			size_t len = wcslen(code);
			if (len < 1) return 0;
			if (code[0] != '#') return 0;

			WCHAR hex[] = L"0x000000";
			if (len == 4)
			{
				swprintf_s(hex, 9, L"0x0%u0%u0%u0", code[3], code[2], code[1]);
			}
			else if (len == 7)
			{
				swprintf_s(hex, 9, L"0x%u%u%u%u%u%u", code[6], code[5], code[4], code[3], code[2], code[1]);
			}
			return String::HexToNum(hex);
		}

		static std::wstring toCode(COLORREF color)
		{
			std::wstring code = L"#";
			std::wstring hex = String::toHex(GetRValue(color));
			if (hex.length() > 3 && hex.length() > 2)
			{
				code += hex[2];
				code += hex[3];
			}
			else if (hex.length() > 2)
			{
				code += L"0";
				code += hex[2];
			}
			else
			{
				code += L"00";
			}
			hex = String::toHex(GetGValue(color));
			if (hex.length() > 3 && hex.length() > 2)
			{
				code += hex[2];
				code += hex[3];
			}
			else if (hex.length() > 2)
			{
				code += L"0";
				code += hex[2];
			}
			else
			{
				code += L"00";
			}
			hex = String::toHex(GetBValue(color));
			if (hex.length() > 3 && hex.length() > 2)
			{
				code += hex[2];
				code += hex[3];
			}
			else if (hex.length() > 2)
			{
				code += L"0";
				code += hex[2];
			}
			else
			{
				code += L"00";
			}

			return code;
		}

		static bool Compare(COLORREF rgb, COLORREF refer, BYTE extend) { return (InRange(GetRValue(rgb), GetRValue(refer), extend) && InRange(GetGValue(rgb), GetGValue(refer), extend) && InRange(GetBValue(rgb), GetBValue(refer), extend)); }

		// POINT x/y >= 1
		static bool Find(HDC hdc, long x, long y, COLORREF color, BYTE extend = 10) {
			if (x < 1 || y < 1) return 0;
			COLORREF pixel = GetPixel(hdc, x - 1, y - 1);
			return Compare(color, pixel, extend);
		}
		// RECT left/top >= 1, right/bottom >= left/top; return x/y = 0 is false, > 0 is ture & point
		static POINT FindOr(HDC hdc, RECT rect, COLORREF color, BYTE extend = 10) {
			if (rect.right < rect.left) rect.right = rect.left;
			if (rect.bottom < rect.top) rect.bottom = rect.top;

			CImage image;
			COLORREF pixel;
			image.Create(rect.right - rect.left, rect.bottom - rect.top, 24);
			BitBlt(image.GetDC(), 0, 0, rect.right - rect.left, rect.bottom - rect.top, hdc, rect.left, rect.top, SRCCOPY);

			for (int y = 0; y < rect.bottom - rect.top; y++)
			{
				for (int x = 0; x < rect.right - rect.left; x++)
				{
					pixel = image.GetPixel(x, y);
					if (Compare(pixel, color, extend))
					{
						image.ReleaseDC();
						return { rect.left + x, rect.top + y };
					}
				}
			}
			image.ReleaseDC();
			return { 0, 0 };
		}

		// RECT left/top >= 1, right/bottom >= left/top; return x/y = 0 is false, > 0 is ture & point
		static bool FindAnd(HDC hdc, RECT rect, COLORREF color, BYTE extend = 10) {
			if (rect.right < rect.left) rect.right = rect.left;
			if (rect.bottom < rect.top) rect.bottom = rect.top;

			CImage image;
			COLORREF pixel;
			image.Create(rect.right - rect.left, rect.bottom - rect.top, 24);
			BitBlt(image.GetDC(), 0, 0, rect.right - rect.left, rect.bottom - rect.top, hdc, rect.left, rect.top, SRCCOPY);
			long x = 0, y = 0;

			for (int y = 0; y < rect.bottom - rect.top; y++)
			{
				for (int x = 0; x < rect.right - rect.left; x++)
				{
					pixel = image.GetPixel(x, y);
					if (!Compare(pixel, color, extend))
					{
						image.ReleaseDC();
						return 0;
					}
				}
			}
			image.ReleaseDC();
			return 1;
		}

		static COLORREF avg(HDC hdc, RECT rect) {
			if (rect.left < 1 || rect.top < 1) return 0;
			if (rect.right < rect.left) rect.right = rect.left;
			if (rect.bottom < rect.top) rect.bottom = rect.top;

			UINT r = 0, g = 0, b = 0;
			CImage image;
			COLORREF pixel;

			image.Create(rect.right - rect.left + 1, rect.bottom - rect.top + 1, 24);
			BitBlt(image.GetDC(), 0, 0, rect.right - rect.left + 1, rect.bottom - rect.top + 1, hdc, rect.left - 1, rect.top - 1, SRCCOPY);

			long x = 0, y = 0;
			do {
				do {
					pixel = image.GetPixel(x, y);
					r += GetRValue(pixel);
					g += GetGValue(pixel);
					b += GetBValue(pixel);
					y++;
				} while (y <= rect.bottom - rect.top);
				y = 0, x++;
			} while (x <= rect.right - rect.left);

			image.ReleaseDC();

			r /= (rect.right - rect.left + 1) * (rect.bottom - rect.top + 1);
			g /= (rect.right - rect.left + 1) * (rect.bottom - rect.top + 1);
			b /= (rect.right - rect.left + 1) * (rect.bottom - rect.top + 1);

			return RGB(r, g, b);
		}
	};
}