#include <QtWidgets/qapplication.h>
#include <qtextcodec.h>
#include ".h/MainUi.h"
#include "static.h"

byte block = 0;

void SwitchKey(BYTE vk, bool state)
{
	if (vk == VK_SHIFT) Global::qi.set.shift = state;
	else if (vk == VK_F12) Global::qi.set.f12 = state;
	if (Global::qi.set.shift && Global::qi.set.f12) { QiState(0); return; }

	if ((Global::qi.set.key & 0xFFFF) == vk) Global::qi.set.k1 = state;
	if (!(Global::qi.set.key >> 16)) Global::qi.set.k2 = 1;
	else if ((Global::qi.set.key) >> 16 == vk) Global::qi.set.k2 = state;
	if (Global::qi.set.k1 && Global::qi.set.k2)
	{
		if (Global::qi.state) QiState(0);
		else Global::qi.ReScreen(), QiState(1);
		Global::qi.set.k1 = 0, Global::qi.set.k2 = 0;
	}
}

void TriggerKey(BYTE vk, bool state)
{
	std::wstring text;

	if (Global::qi.fun.showClock.state && Global::qi.fun.showClock.key == vk && state)
	{
		System::TimeStruct ts = System::Time();
		text = String::toWString(ts.hour) + L" : " + String::toWString(ts.min) + L" - " + String::toWString(ts.sec);
		TipsWindow::Popup(text);
	}

	if (!Global::qi.state) return;
	if (!Global::qi.fun.wndActive.active) return;

	if (Global::qi.fun.quickClick.state && Global::qi.fun.quickClick.key == vk)
	{
		if (Global::qi.fun.quickClick.mode)
		{
			if (!state)
			{
				if (!Global::qi.fun.quickClick.thread)
				{
					Global::qi.fun.quickClick.thread = Thread::Start(ThreadQuickClick);
					if (Global::qi.set.showTips) TipsWindow::Popup(std::wstring(Input::Name(Global::qi.fun.quickClick.key)) + std::wstring(L"ㅤ连点开始"), RGB(0xFF, 0xAF, 0xEF));
				}
				else
				{
					Thread::Start(ThreadRelease, (LPVOID)Global::qi.fun.quickClick.key);
					TerminateThread(Global::qi.fun.quickClick.thread, 0);
					Global::qi.fun.quickClick.thread = 0;
					if (Global::qi.set.showTips) TipsWindow::Popup(std::wstring(Input::Name(Global::qi.fun.quickClick.key)) + std::wstring(L"ㅤ连点结束"), RGB(0xFF, 0xFF, 0x60));
				}
			}
		}
		else
		{
			if (state)
			{
				if (!Global::qi.fun.quickClick.thread)
				{
					Global::qi.fun.quickClick.thread = Thread::Start(ThreadQuickClick);
					if (Global::qi.set.showTips) TipsWindow::Popup(std::wstring(Input::Name(Global::qi.fun.quickClick.key)) + std::wstring(L"ㅤ连点开始"), RGB(0x20, 0xFF, 0x40));
				}
			}
			else
			{
				if (Global::qi.fun.quickClick.thread)
				{
					Thread::Start(ThreadRelease, (LPVOID)Global::qi.fun.quickClick.key);
					TerminateThread(Global::qi.fun.quickClick.thread, 0);
					Global::qi.fun.quickClick.thread = 0;
					if (Global::qi.set.showTips) TipsWindow::Popup(std::wstring(Input::Name(Global::qi.fun.quickClick.key)) + std::wstring(L"ㅤ连点结束"), RGB(0xFF, 0xFF, 0x60));
				}
			}
		}
	}

	for (uint32 n = 0; n < Global::qi.scripts.size(); n++)
	{
		if (Global::qi.scripts[n].state)
		{
			if ((Global::qi.scripts[n].key & 0xFFFF) == vk) Global::qi.scripts[n].k1 = state, Global::qi.scripts[n].block ? block = vk : 0;
			if ((Global::qi.scripts[n].key >> 16) == 0) Global::qi.scripts[n].k2 = 1;
			else if ((Global::qi.scripts[n].key >> 16) == vk) Global::qi.scripts[n].k2 = state, Global::qi.scripts[n].block ? block = vk : 0;

			if (Global::qi.scripts[n].mode == Script::sw)
			{
				if (Global::qi.scripts[n].k1 && Global::qi.scripts[n].k2)
				{
					if (!Global::qi.scripts[n].thread)
					{
						Global::qi.scripts[n].thread = Thread::Start(ThreadMacro, (LPVOID)n);
						if (Global::qi.set.showTips)
						{
							text = Global::qi.scripts[n].name + L"ㅤ开始";
							TipsWindow::Popup(text, RGB(0xFF, 0xAF, 0xEF));
						}
					}
					else
					{
						TerminateThread(Global::qi.scripts[n].thread, 0);
						Global::qi.scripts[n].thread = 0;
						if (Global::qi.set.showTips)
						{
							text = Global::qi.scripts[n].name + L"ㅤ结束";
							TipsWindow::Popup(text, RGB(0xFF, 0xFF, 0x60));
						}
					}
				}
			}
			else if (Global::qi.scripts[n].mode == Script::down)
			{
				if (Global::qi.scripts[n].k1 && Global::qi.scripts[n].k2)
				{
					if (!Global::qi.scripts[n].thread)
					{
						Global::qi.scripts[n].thread = Thread::Start(ThreadMacro, (LPVOID)n);
						if (Global::qi.set.showTips)
						{
							if (Global::qi.scripts[n].a) text = Global::qi.scripts[n].name + L"ㅤ" + String::toWString(Global::qi.scripts[n].a) + L"次";
							else text = Global::qi.scripts[n].name + L"ㅤ循环";
							TipsWindow::Popup(text, RGB(0x20, 0xFF, 0x40));
						}
					}
				}
				else
				{
					if (Global::qi.scripts[n].thread && Global::qi.scripts[n].a == 0)
					{
						TerminateThread(Global::qi.scripts[n].thread, 0);
						Global::qi.scripts[n].thread = 0;
						if (Global::qi.set.showTips)
						{
							text = Global::qi.scripts[n].name + L"ㅤ结束";
							TipsWindow::Popup(text, RGB(0xFF, 0xFF, 0x60));
						}
					}
				}
			}
			else if (Global::qi.scripts[n].mode == Script::up)
			{
				if (Global::qi.scripts[n].k1 && Global::qi.scripts[n].k2)
				{
					Global::qi.scripts[n].active = 1;
					if (Global::qi.scripts[n].thread && Global::qi.scripts[n].a == 0)
					{
						TerminateThread(Global::qi.scripts[n].thread, 0);
						Global::qi.scripts[n].thread = 0;
						if (Global::qi.set.showTips)
						{
							text = Global::qi.scripts[n].name + L"ㅤ结束";
							TipsWindow::Popup(text, RGB(0xFF, 0xFF, 0x60));
						}
					}
				}
				else if (Global::qi.scripts[n].active)
				{
					Global::qi.scripts[n].active = 0;
					if (!Global::qi.scripts[n].thread)
					{
						Global::qi.scripts[n].thread = Thread::Start(ThreadMacro, (LPVOID)n);
						if (Global::qi.set.showTips)
						{
							if (Global::qi.scripts[n].a) text = Global::qi.scripts[n].name + L"ㅤ" + String::toWString(Global::qi.scripts[n].a) + L"次";
							else text = Global::qi.scripts[n].name + L"ㅤ循环";
							TipsWindow::Popup(text, RGB(0xA0, 0xFF, 0x50));
						}
					}
				}
			}
		}
	}
}

InputHookProc()
{
	if (*exInfo == 214)
	{
		*exInfo = 0;
		return 0;
	}
	else if (vk)
	{
		block = 0;
		
		if (state)
		{
			if (Global::qi.rec)
			{
				if (Global::qi.set.recKey == vk) return 1;
				else if (((RecordUi*)Global::qi.rec)->State()) ((RecordUi*)Global::qi.rec)->AddItems(Input::Convert(vk), state, msPt);
			}
			else
			{
				SwitchKey(Input::Convert(vk), state);
				TriggerKey(Input::Convert(vk), 1);
			}
		}
		else
		{
			if (Global::qi.rec)
			{
				if (Global::qi.set.recKey == Input::Convert(vk)) ((RecordUi*)Global::qi.rec)->EndRec();
				if (Global::qi.set.recKey == vk) return 1;
				else if (((RecordUi*)Global::qi.rec)->State()) ((RecordUi*)Global::qi.rec)->AddItems(Input::Convert(vk), state, msPt);
			}
			else
			{
				SwitchKey(Input::Convert(vk), state);
				TriggerKey(Input::Convert(vk), 0);
			}
		}

		if (block == vk) return 1;
	}

	return 0;
}

int main(int argc, char* argv[])
{
	if (Process::isRunning(File::PathToUrl(Process::runPath()).c_str())) MsgBox::Warning(L"当前文件夹的程序已经运行，若运行更多程序请复制此文件夹", L"提示"), exit(0);
	timeBeginPeriod(1);

	LoadJson();
	InitUI(!Global::qi.set.zoomBlock);
	QApplication app(argc, argv);

	Global::qi.ReScreen();
	TipsWindow::screen = Global::qi.screen;
	TipsWindow::thread = Thread::Start(TipsWindow::TipsWindowThread);

	MainUi wnd;
	if (!Global::qi.set.minMode) wnd.show();
	if (Global::qi.set.defOn) HookState(1), QiState(1);

	app.exec();
	timeEndPeriod(1);
	return 0;
}