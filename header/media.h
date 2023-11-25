#pragma once

#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")

namespace CG {
	class Media
	{
	public:

		struct AudFx {
			const wchar_t error[MAX_PATH] = L"C:/Windows/Media/Windows Foreground.wav";
			const wchar_t error1[MAX_PATH] = L"C:/Windows/Media/Windows Hardware Fail.wav";
			const wchar_t fail[MAX_PATH] = L"C:/Windows/Media/Windows Hardware Fail";
			const wchar_t warning[MAX_PATH] = L"C:/Windows/Media/Windows Background.wav";
			const wchar_t warning1[MAX_PATH] = L"C:/Windows/Media/Windows Balloon.wav";
			const wchar_t stop[MAX_PATH] = L"C:/Windows/Media/Windows Critical Stop.wav";
			const wchar_t ok[MAX_PATH] = L"C:/Windows/Media/Windows Notify Calendar.wav";
			const wchar_t ok1[MAX_PATH] = L"C:/Windows/Media/Windows Default.wav";
			const wchar_t off[MAX_PATH] = L"C:/Windows/Media/Speech Off.wav";
			const wchar_t on[MAX_PATH] = L"C:/Windows/Media/Speech On.wav";
			const wchar_t sleep[MAX_PATH] = L"C:/Windows/Media/Speech Sleep.wav";
		};

		static void WavePlay(LPCWSTR path, bool sync = 0) {
			if (sync) PlaySoundW(path, NULL, SND_FILENAME | SND_SYNC);
			else PlaySoundW(path, NULL, SND_FILENAME | SND_ASYNC);
		}

		// on the thread
		static void MediaPlay(LPCWSTR path, bool sync = 0) {
			WCHAR cmd[MAX_PATH];
			swprintf_s(cmd, MAX_PATH, L"open \"%s\" alias audio", path);
			mciSendStringW(cmd, 0, 0, 0);
			if (sync) mciSendStringW(L"play audio wait", 0, 0, 0);
			else mciSendStringW(L"play audio", 0, 0, 0);
		}

		// on the thread
		static void MediaPause() { mciSendStringW(L"pause audio", 0, 0, 0); }

		// on the thread
		static void MediaResume() { mciSendStringW(L"resume audio", 0, 0, 0); }

		// on the thread
		static void MediaStop() { mciSendStringW(L"stop audio", 0, 0, 0); }

		// on the thread
		static void MediaClose() { mciSendStringW(L"close audio", 0, 0, 0); }
	};

	static Media::AudFx audfx;
}