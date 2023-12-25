#pragma once

#include <windows.h>
#include <TlHelp32.h>

#include "base.h"
#include "file.h"

namespace CG {
	class Process {
	public:

		typedef List<DWORD> PIDS;

		static std::wstring runPath() { return _wgetcwd(0, 0); }

		static std::wstring exePath() { WCHAR path[MAX_PATH]; GetModuleFileNameW(0, path, MAX_PATH); return path; }

		static std::wstring exeName() { return File::FileName(exePath()); }

		static bool isRunning(LPCWSTR instanceName) { CreateMutexW(0, 0, instanceName); if (GetLastError() == ERROR_ALREADY_EXISTS) return 1; return 0; }
		static void RunOnce(LPCWSTR instanceName) { CreateMutexW(0, 0, instanceName); if (GetLastError() == ERROR_ALREADY_EXISTS) exit(0); }

		// include .exe, return pid
		static DWORD State(const std::wstring exe) {
			PROCESSENTRY32W pe = { 0 };
			pe.dwSize = sizeof(PROCESSENTRY32W);
			HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

			Process32FirstW(hProcess, &pe);
			do {
				if (String::toLower(pe.szExeFile) == String::toLower(exe)) return pe.th32ProcessID;
			} while (Process32NextW(hProcess, &pe));
			return 0;
		}

		// include .exe, return pids
		static PIDS Find(LPCWSTR exe) {
			PIDS pids;
			PROCESSENTRY32W pe = { 0 };
			pe.dwSize = sizeof(PROCESSENTRY32W);
			HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

			Process32FirstW(hProcess, &pe);
			do {
				if (String::toLower(pe.szExeFile) == String::toLower(exe)) pids.Add(pe.th32ProcessID);
			} while (Process32NextW(hProcess, &pe));
			return pids;
		}

		static HANDLE Start(const std::wstring cmdLine, const bool showWindow = 1, const DWORD creationFlags = CREATE_NEW_CONSOLE, std::wstring workPath = L"") {
			STARTUPINFO si = { 0 };
			si.cb = sizeof(STARTUPINFO);
			PROCESS_INFORMATION pi = { 0 };
			LPWSTR cmd = new WCHAR[cmdLine.length() + 1];

			wcscpy_s(cmd, cmdLine.length() + 1, cmdLine.c_str());
			if (workPath == L"")
			{
				File::CmdLine cmdLines = File::toCmdLine(cmdLine);
				if (cmdLines.size()) workPath = File::PathLast(cmdLines[0]);
				if (!File::FolderState(workPath)) workPath = runPath();
			}
			if (!showWindow) {
				si.dwFlags = STARTF_USESHOWWINDOW;
				si.wShowWindow = SW_HIDE;
			}
			if (CreateProcessW(0, cmd, 0, 0, 0, creationFlags, 0, workPath.c_str(), &si, &pi)) {
				CloseHandle(&pi.hThread);
				CloseHandle(&pi.hProcess);
			}
			delete[] cmd;
			return 0;
		}

		static void Close(const std::wstring exe) {
			PROCESSENTRY32W pe = { 0 };
			pe.dwSize = sizeof(PROCESSENTRY32W);
			HANDLE hProcShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

			Process32FirstW(hProcShot, &pe);
			do {
				if (String::toLower(pe.szExeFile) == String::toLower(exe)) {
					TerminateProcess(OpenProcess(PROCESS_ALL_ACCESS, 0, pe.th32ProcessID), 0);
				}
			} while (Process32NextW(hProcShot, &pe));
		}
	};
}