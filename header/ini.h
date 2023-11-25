#pragma once

#include <windows.h>
#include "file.h"

namespace CG {
	class Ini {
		WCHAR path[MAX_PATH];
		WCHAR app[MAX_PATH];
	public:
		Ini() {
			path[0] = L'\0';
			app[0] = L'\0';
		}

		// absolutePath
		Ini(LPCWSTR path, LPCWSTR app) {
			wcscpy_s(this->path, MAX_PATH, path);
			wcscpy_s(this->app, MAX_PATH, app);
		}

		// absolutePath
		void Path(LPCWSTR path) {
			wcscpy_s(this->path, MAX_PATH, path);
		}

		// appName
		void App(LPCWSTR app) {
			wcscpy_s(this->app, MAX_PATH, app);
		}

		// Empty appDate
		bool Empty() {
			if (wcslen(path) < 1) return 0;
			if (wcslen(app) < 1) return 0;
			return WritePrivateProfileStringW(app, 0, 0, path);
		}

		// Delete iniFile
		void Delete() {
			if (wcslen(path) < 1) return;
			File::FileDelete(path);
		}

		// if keyName = 0 will be clear appData, if want to clear appData please call the Ini::Delete()
		bool Write(LPCWSTR keyName, LPCWSTR keyValue) {
			if (wcslen(path) < 1) return 0;
			if (wcslen(app) < 1) return 0;
			File::FileCreate(path);
			return WritePrivateProfileStringW(app, keyName, keyValue, path);
		}

		// if keyName = 0 will be clear appData, if want to clear appData please call the Ini::Delete()
		bool WriteNum(LPCWSTR keyName, int keyValue) {
			if (wcslen(path) < 1) return 0;
			if (wcslen(app) < 1) return 0;
			File::FileCreate(path);
			return WritePrivateProfileStringW(app, keyName, String::toWString(keyValue).c_str(), path);
		}

		LPCWSTR Read(LPCWSTR keyName, LPCWSTR defaultValue = L"") {
			if (wcslen(path) < 1) return 0;
			if (wcslen(app) < 1) return 0;
			WCHAR value[1024];
			GetPrivateProfileStringW(app, keyName, defaultValue, value, 1024, path);
			return value;
		}

		int ReadNum(LPCWSTR keyName, int defaultValue = 0) {
			if (wcslen(path) < 1) return 0;
			if (wcslen(app) < 1) return 0;
			return GetPrivateProfileIntW(app, keyName, defaultValue, path);
		}
	};
}