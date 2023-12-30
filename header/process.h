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

		static bool isRunning(LPCWSTR mutexName) { HANDLE handle = CreateMutexW(0, 0, mutexName); if (GetLastError() == ERROR_ALREADY_EXISTS) { CloseHandle(handle); return 1; } CloseHandle(handle); return 0; }
		static void RunOnce(LPCWSTR mutexName) { HANDLE handle = CreateMutexW(0, 0, mutexName); if (GetLastError() == ERROR_ALREADY_EXISTS) { CloseHandle(handle); exit(0); } }

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

		static bool Open(std::wstring file, UINT showCmd = SW_NORMAL, LPCWSTR cmdLine = 0, LPCWSTR workPath = 0) {
			std::wstring _workPath;
			if (workPath) _workPath = workPath; else { _workPath = File::PathLast(File::ParseCmd(file, 0).path); if (!File::FolderState(_workPath)) _workPath = runPath(); }
			return ShellExecuteW(0, L"open", file.c_str(), cmdLine, _workPath.c_str(), showCmd);
		}
		static bool Open(File::CmdLine cl, UINT showCmd = SW_NORMAL, LPCWSTR workPath = 0) {
			std::wstring path = cl.path;
			if (cl.param.size())
			{ 
				std::wstring cmd; for (uint32 u = 0; u < cl.param.size(); u++) cmd += std::wstring(L" ") + cl.param[u];
				return Open(path, showCmd, cmd.c_str());
			}
			return Open(path, showCmd, 0, workPath);
		}

		static bool Start(std::wstring cmdLine, UINT showCmd = SW_NORMAL, LPCWSTR workPath = 0, DWORD creationFlags = CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT) {
			STARTUPINFO si = { 0 };
			si.cb = sizeof(STARTUPINFO);
			PROCESS_INFORMATION pi = { 0 };
			std::wstring _workPath;
			if (workPath) _workPath = workPath; else { _workPath = File::PathLast(File::ParseCmd(cmdLine, 0).path); if (!File::FolderState(_workPath)) _workPath = runPath(); }
			if (showCmd != SW_NORMAL) si.dwFlags = STARTF_USESHOWWINDOW, si.wShowWindow = showCmd;
			if (CreateProcessW(0, (LPWSTR)cmdLine.c_str(), 0, 0, 0, creationFlags, 0, _workPath.c_str(), &si, &pi)) {
				CloseHandle(&pi.hThread);
				CloseHandle(&pi.hProcess);
				return 1;
			}
			else return 0;
		}
		static bool Start(File::CmdLine cl, UINT showCmd = SW_NORMAL, LPCWSTR workPath = 0, DWORD creationFlags = CREATE_NEW_CONSOLE | CREATE_UNICODE_ENVIRONMENT)
		{
			std::wstring cmd = cl.path; for (uint32 u = 0; u < cl.param.size(); u++) cmd += std::wstring(L" ") + cl.param[u];
			return Start(cl.path, showCmd, workPath, creationFlags);
		}

		static void Close(std::wstring exe) {
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