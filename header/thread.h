﻿#pragma once

#include <windows.h>
#include <timeapi.h>
#pragma comment(lib,"Winmm.lib")

#define sleep(ms) ::Sleep(ms)

namespace CG {
	class Thread {
	public:

		static void ClockRatio(UINT resolutionRatio)
		{
			timeBeginPeriod(resolutionRatio);
		}

		static void ClockRatioRs(UINT resolutionRatio)
		{
			timeEndPeriod(resolutionRatio);
		}

		// 1hs = 100ns
		static void NanoSleep(int hs)
		{
			HANDLE hTimer = 0;
			LARGE_INTEGER liDueTime;
			liDueTime.QuadPart = -(hs);

			hTimer = CreateWaitableTimerW(0, 1, 0);
			if (hTimer)
			{
				if (SetWaitableTimer(hTimer, &liDueTime, 0, 0, 0, 0))
				{
					WaitForSingleObject(hTimer, INFINITE);
				}
				CloseHandle(hTimer);
			}
		}

		static void Sleep(DWORD ms)
		{
			if (ms) NanoSleep(ms * 10000);
		}

		static HANDLE Start(PTHREAD_START_ROUTINE pfunc, LPVOID lParam = 0) { return CreateThread(0, 0, pfunc, lParam, 0, 0); }

		// return: exitCode
		static DWORD Wait(HANDLE hThread, bool zero = 1) {
			DWORD exitCode;
			WaitForSingleObject(hThread, INFINITE);
			GetExitCodeThread(hThread, &exitCode);
			if (zero) hThread = 0;
			return exitCode;
		}
	};
}