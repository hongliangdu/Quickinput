#pragma once

#include <windows.h>
#include <string>
#include <math.h>

namespace CG {
	class String
	{
	public:

		static std::string toString(char val) { char str[] = { val, '\0' }; return str; }
		static std::string toString(wchar_t val) { char str[] = { (char)val, '\0' }; return str; }
		static std::string toString(const char* val) { return val; }
		static std::string toString(const wchar_t* val) {
			int len = WideCharToMultiByte(CP_ACP, 0, val, -1, 0, 0, 0, 0);
			char* cache = new char[len];
			WideCharToMultiByte(CP_ACP, 0, val, -1, cache, len, 0, 0);
			std::string str = cache;
			delete[] cache;
			return str;
		}
		static std::string toString(std::string val) { return val; }
		static std::string toString(std::wstring val) {
			int len = WideCharToMultiByte(CP_ACP, 0, val.c_str(), -1, 0, 0, 0, 0);
			char* cache = new char[len];
			WideCharToMultiByte(CP_ACP, 0, val.c_str(), -1, cache, len, 0, 0);
			std::string str = cache;
			delete[] cache;
			return str;
		}
		static std::string toString(signed char val) { char str[5]; sprintf_s(str, 5, "%d", val); return str; }
		static std::string toString(unsigned char val) { char str[4]; sprintf_s(str, 4, "%u", val); return str; }
		static std::string toString(short val) { char str[7]; sprintf_s(str, 7, "%d", val); return str; }
		static std::string toString(unsigned short val) { char str[6]; sprintf_s(str, 6, "%u", val); return str; }
		static std::string toString(int val) { char str[12]; sprintf_s(str, 12, "%d", val); return str; }
		static std::string toString(unsigned int val) { char str[11]; sprintf_s(str, 11, "%u", val); return str; }
		static std::string toString(long val) { char str[12]; sprintf_s(str, 12, "%ld", val); return str; }
		static std::string toString(unsigned long val) { char str[11]; sprintf_s(str, 11, "%lu", val); return str; }
		static std::string toString(long long val) { char str[22]; sprintf_s(str, 22, "%lld", val); return str; }
		static std::string toString(unsigned long long val) { char str[21]; sprintf_s(str, 21, "%llu", val); return str; }
		static std::string toString(float val) { char str[32]; sprintf_s(str, 32, "%g", val); return str; }
		static std::string toString(double val) { char str[32]; sprintf_s(str, 32, "%g", val); return str; }

		static std::wstring toWString(char val) { wchar_t str[] = { (wchar_t)val, L'\0' }; return str; }
		static std::wstring toWString(wchar_t val) { wchar_t str[] = { val, L'\0' }; return str; }
		static std::wstring toWString(const char* val) {
			int len = MultiByteToWideChar(CP_ACP, 0, val, -1, 0, 0);
			wchar_t* cache = new wchar_t[len];
			MultiByteToWideChar(CP_ACP, 0, val, -1, cache, len);
			std::wstring str = cache;
			delete[] cache;
			return str;
		}
		static std::wstring toWString(std::string val) {
			int len = MultiByteToWideChar(CP_ACP, 0, val.c_str(), -1, 0, 0);
			wchar_t* cache = new wchar_t[len];
			MultiByteToWideChar(CP_ACP, 0, val.c_str(), -1, cache, len);
			std::wstring str = cache;
			delete[] cache;
			return str;
		}
		static std::wstring toWString(std::wstring val) { return val; }
		static std::wstring toWString(const wchar_t* val) { return val; }
		static std::wstring toWString(signed char val) { wchar_t str[5]; swprintf_s(str, 5, L"%d", val); return str; }
		static std::wstring toWString(unsigned char val) { wchar_t str[4]; swprintf_s(str, 4, L"%u", val); return str; }
		static std::wstring toWString(short val) { wchar_t str[7]; swprintf_s(str, 7, L"%d", val); return str; }
		static std::wstring toWString(unsigned short val) { wchar_t str[6]; swprintf_s(str, 6, L"%u", val); return str; }
		static std::wstring toWString(int val) { wchar_t str[12]; swprintf_s(str, 12, L"%d", val); return str; }
		static std::wstring toWString(unsigned int val) { wchar_t str[11]; swprintf_s(str, 11, L"%u", val); return str; }
		static std::wstring toWString(long val) { wchar_t str[12]; swprintf_s(str, 12, L"%ld", val); return str; }
		static std::wstring toWString(unsigned long val) { wchar_t str[11]; swprintf_s(str, 11, L"%lu", val); return str; }
		static std::wstring toWString(long long val) { wchar_t str[22]; swprintf_s(str, 22, L"%lld", val); return str; }
		static std::wstring toWString(unsigned long long val) { wchar_t str[21]; swprintf_s(str, 21, L"%llu", val); return str; }
		static std::wstring toWString(float val) { wchar_t str[32]; swprintf_s(str, 32, L"%g", val); return str; }
		static std::wstring toWString(double val) { wchar_t str[32]; swprintf_s(str, 32, L"%g", val); return str; }

		static std::wstring toLower(std::wstring str)
		{
			std::wstring result;
			for (size_t s = 0; s < str.length(); s++)
			{
				result += towlower(str[s]);
			}
			return result;
		}

		static std::string toLower(std::string str)
		{
			std::string result;
			for (size_t s = 0; s < str.length(); s++)
			{
				result += tolower(str[s]);
			}
			return result;
		}

		static LPCWSTR toWString(int* arr, UINT len) {
			WCHAR str[1024];
			swprintf_s(str, 1024, L"%d", *arr);
			for (UINT n = 1; n < len; n++) {
				swprintf_s(str, 1024, L"%s, %d", str, *(arr + n));
			}
			return str;
		}

		static std::wstring toHex(UINT hex)
		{
			wchar_t result[] = L"0x00000000";
			UINT c = 2, b = 8, n, r;
			do
			{
				n = (BYTE)((hex & ((UINT)0xF << (UINT)((b - 1) * 4u))) >> (UINT)((b - 1) * 4u)), r = 0;
				while (1) { if (r == n) break; r++; }
				if (r)
				{
					if (r < 10) result[c] = L'0' + r;
					else result[c] = 55 + r;
					c++;
				}
				b--;
			} while (b > 0);
			result[c] = L'\0';
			return result;
		}

		static UINT HexToNum(LPCWSTR hex)
		{
			UINT len = (UINT)wcslen(hex);
			if (len < 3 || len > 10) return 0;
			if (hex[0] != '0') return 0;
			if (hex[1] != 'x' && hex[1] != 'X') return 0;

			UINT result = 0;
			UINT c = len - 1, b = 0, r;
			while (c > 1)
			{
				while (1)
				{
					r = 0;
					if (hex[c] >= '0' && hex[c] <= '9')
					{
						if (r == hex[c] - '0') break;
					}
					else if (hex[c] >= 'A' && hex[c] <= 'F')
					{
						if (r == hex[c] - 55) break;
					}
					r++;
				}
				result |= r << (b * 4);
				c--, b++;
			};
			return result;
		}

		static bool Compare(LPCWSTR str1, LPCWSTR str2) { return !wcscmp(str1, str2); }

		static bool Compare(LPCSTR str1, LPCSTR str2) { return !strcmp(str1, str2); }

		static UINT Find(LPCWSTR str, LPCWSTR strFind) {
			for (UINT len = (UINT)wcslen(str), lenF = (UINT)wcslen(strFind), n = 0, f = 0; n < len; n++) {
				if (str[n] == strFind[f]) {
					f++;
					if (f == lenF) return n - lenF + 1;
				}
				else f = 0;
			}
			return 0;
		}
	};
}