#include <QtWidgets/qapplication.h>
#include <qtextcodec.h>
#include ".h/MainUi.h"
#include ".h/TipsWindow.h"
#include "static.h"

void InitUI(bool zoom)
{
	if (zoom && System::screenSize().cy > 1200) qputenv("QT_SCALE_FACTOR", QByteArray::number(System::screenZoomRote(), 10, 1));

	{
		if (System::Version().dwMajorVersion >= 10)
		{
			UI::qiOn = L"已启用✅";
			UI::qiOff = L"已禁用⛔";
			UI::qiWait = u8"等待⏳";
			UI::qiDown = u8"按下⬇";
			UI::qiUp = u8"松开⬆";
			UI::qiClick = u8"点击🔃";
			UI::qiMove = u8"移动🔜";
			UI::qiPos = u8"位置🔝";
			UI::qiText = u8"复制文本🅰";
			UI::qiLoop = u8"循环♾️";
			UI::qiColor = u8"查找颜色🌈";
			UI::qiEnd = u8"结束宏🛑";
			UI::qiEndLoop = u8"结束循环🛑";
			UI::trOn = u8"启用✅";
			UI::trOff = u8"禁用⛔";
			UI::etFunc = u8"功能⚙";
			UI::etParam = u8"参数🔗";
			UI::etAdd = u8"添加✔️";
			UI::etDel = u8"删除❌";
			UI::etChange = u8"修改⭕";
			UI::etEdit = u8"编辑🔧";
			UI::rcStart = u8"开始⭕";
			UI::rcStop = u8"停止⭕";
			UI::rcClose = u8"取消❌";
		}
		else
		{
			UI::qiOn = L"已启用✔";
			UI::qiOff = L"已禁用✘";
			UI::qiWait = u8"等待☽";
			UI::qiDown = u8"按下↓";
			UI::qiUp = u8"松开↑";
			UI::qiClick = u8"点击◈";
			UI::qiMove = u8"移动↘";
			UI::qiPos = u8"位置↗";
			UI::qiText = u8"复制文本";
			UI::qiLoop = u8"循环↩";
			UI::qiColor = u8"查找颜色☀";
			UI::qiEnd = u8"结束宏Ⓢ";
			UI::qiEndLoop = u8"结束循环Ⓢ";
			UI::trOn = u8"启用✔";
			UI::trOff = u8"禁用✘";
			UI::etFunc = u8"功能✱";
			UI::etParam = u8"参数※";
			UI::etAdd = u8"添加✔";
			UI::etDel = u8"删除✘";
			UI::etChange = u8"修改◈";
			UI::etEdit = u8"编辑▲";
			UI::rcStart = u8"开始✔";
			UI::rcStop = u8"停止✔";
			UI::rcClose = u8"取消✘";
		}
	}
}

uint8 ExcItem(Action& item)
{
	if (!Global::qi.state) return 1;
	switch (item.type)
	{
	case Action::_end: return 1;

	case Action::_delay:
	{
		if (item.delay.ex)
		{
			int32 ms;
			ms = item.delay.ms + (Rand(item.delay.ex, item.delay.ex - (item.delay.ex * 2)));
			if (ms < 0) ms = 0;
			Thread::Sleep(ms);
		}
		else Thread::Sleep(item.delay.ms);
		return 0;
	}

	case Action::_key:
	{
		if (item.key.state == Action::Key::up) Input::State(item.key.vk, 0, 214);
		else if (item.key.state == Action::Key::down) Input::State(item.key.vk, 1, 214);
		else if (item.key.state == Action::Key::click)
		{
			Thread::Sleep(10);
			Input::State(item.key.vk, 1, 214);
			Thread::Sleep(10);
			Input::State(item.key.vk, 0, 214);
			Thread::Sleep(10);
		}
		return 0;
	}

	case Action::_mouse:
	{
		Thread::Sleep(10);
		if (item.mouse.ex)
		{
			POINT pt = { 0 };
			pt.x = item.mouse.x + (Rand(item.mouse.ex, (~item.mouse.ex + 1)));
			pt.y = item.mouse.y + (Rand(item.mouse.ex, (~item.mouse.ex + 1)));
			if (item.mouse.move) Input::Move(pt.x, pt.y);
			else Input::MoveToA(pt.x * 6.5535, pt.y * 6.5535);
		}
		else {
			if (item.mouse.move) Input::Move(item.mouse.x, item.mouse.y);
			else Input::MoveToA(item.mouse.x * 6.5535, item.mouse.y * 6.5535);
		}
		Thread::Sleep(10);
		return 0;
	}

	case Action::_text: System::ClipBoardText(item.text.str.str); return 0;

	case Action::_color:
	{
		RECT rect = { 0 };
		POINT pt = { 0 };
		pt = AbsToRel({ item.color.rect.left, item.color.rect.top });
		rect.left = pt.x, rect.top = pt.y;
		pt = AbsToRel({ item.color.rect.right, item.color.rect.bottom });
		rect.right = pt.x, rect.bottom = pt.y;
		POINT pos = Color::FindOr(Global::qi.hdc, rect, item.color.rgbe, GetAValue(item.color.rgbe));
		if (item.color.unfind)
		{
			if (!pos.x)
			{
				for (uint32 u = 0; u < item.color.next.size(); u++)
				{
					uint8 r = ExcItem(item.color.next[u]);
					if (r) return r;
				}
			}
		}
		else
		{
			if (pos.x)
			{
				for (uint32 u = 0; u < item.color.next.size(); u++)
				{
					uint8 r = ExcItem(item.color.next[u]);
					if (r) return r;
				}
				if (item.color.move == 1) Input::MoveTo(pos.x, pos.y);
			}
		}
		return 0;
	}

	case Action::_loop:
	{
		if (item.loop.count)
		{
			for (uint32 u = 0; u < item.loop.count; u++)
			{
				for (uint32 ux = 0; ux < item.loop.next.size(); ux++)
				{
					uint8 r = ExcItem(item.loop.next[u]);
					if (r == 1) return r;
					else if (r == 2) return 0;
				}
			}
		}
		else
		{
			while (1)
			{
				for (uint32 u = 0; u < item.loop.next.size(); u++)
				{
					uint8 r = ExcItem(item.loop.next[u]);
					if (r == 1) return 1;
					else if (r == 2) return 0;
				}
			}
		}
		return 0;
	}

	case Action::_loopEnd: return 2;
	}
	return 0;
}

DWORD CALLBACK ThreadQuickClick(LPVOID)
{
	srand(clock());
	uint32 b = 0;
	uint32 e = 0;
	if (Global::qi.fun.quickClick.delay > 99) b = 50, e = Global::qi.fun.quickClick.delay - 50;
	else if (Global::qi.fun.quickClick.delay > 1) b = Global::qi.fun.quickClick.delay / 2, e = b;
	else e = Global::qi.fun.quickClick.delay;

	while (Global::qi.state)
	{
		Input::State(Global::qi.fun.quickClick.key, 1, 214);
		Thread::Sleep(b);
		Input::State(Global::qi.fun.quickClick.key, 0, 214);
		Thread::Sleep(e);
	}
	return 0;
}
DWORD CALLBACK ThreadMacro(LPVOID lParam)
{
	srand(clock());
	uint32 pos = (UINT)lParam;
	uint32 count = Global::qi.scripts[pos].a;
	uint32 n = 0;
	while (Global::qi.state)
	{
		if (count)
		{
			n++;
			if (n > count) break;
		}
		for (uint32 n = 0; n < Global::qi.scripts[pos].actions.size() && Global::qi.state; n++)
		{
			if (ExcItem(Global::qi.scripts[pos].actions[n]))
			{
				Global::qi.scripts[pos].thread = 0;
				return 0;
			}
		}
	}
	Global::qi.scripts[pos].thread = 0;
	return 0;
}
DWORD CALLBACK ThreadRelease(LPVOID key)
{
	Input::State((BYTE)key, 0, 214);
	return 0;
}

byte block = 0;

void SwitchKey(BYTE vk, bool state)
{
	if (vk == VK_SHIFT) Global::qi.set.shift = state;
	else if (vk == VK_F12) Global::qi.set.f12 = state;
	if (Global::qi.set.shift && Global::qi.set.f12)
	{
		QiState(0);
		return;
	}

	byte k1 = Global::qi.set.key & 0xFFFF;
	byte k2 = Global::qi.set.key >> 16;
	if (k1 == vk) Global::qi.set.k1 = state;
	if (!k2) Global::qi.set.k2 = 1;
	else if (k2 == vk) Global::qi.set.k2 = state;
	if (Global::qi.set.k1 && Global::qi.set.k2)
	{
		if (Global::qi.state) QiState(0);
		else QiState(1);
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
					if (Global::qi.set.showTips) TipsWindow::Popup(std::wstring(Input::Name(Global::qi.fun.quickClick.key)) + std::wstring(L"ㅤ连点开始"));
				}
				else
				{
					Thread::Start(ThreadRelease, (LPVOID)Global::qi.fun.quickClick.key);
					TerminateThread(Global::qi.fun.quickClick.thread, 0);
					Global::qi.fun.quickClick.thread = 0;
					if (Global::qi.set.showTips) TipsWindow::Popup(std::wstring(Input::Name(Global::qi.fun.quickClick.key)) + std::wstring(L"ㅤ连点结束"));
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
					if (Global::qi.set.showTips) TipsWindow::Popup(std::wstring(Input::Name(Global::qi.fun.quickClick.key)) + std::wstring(L"ㅤ连点开始"));
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
							TipsWindow::Popup(text, RGB(0xFF, 0xAA, 0xFF));
						}
					}
					else
					{
						TerminateThread(Global::qi.scripts[n].thread, 0);
						Global::qi.scripts[n].thread = 0;
						if (Global::qi.set.showTips)
						{
							text = Global::qi.scripts[n].name + L"ㅤ结束";
							TipsWindow::Popup(text, RGB(0xFF, 0xAA, 0x00));
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
							TipsWindow::Popup(text, RGB(0x60, 0x80, 0xFF));
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
							TipsWindow::Popup(text, RGB(0xFF, 0xAA, 0x00));
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
							TipsWindow::Popup(text, RGB(0xFF, 0xAA, 0x00));
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
							TipsWindow::Popup(text, RGB(0x40, 0xFF, 0x60));
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
	if (Process::isRunning(File::PathToUrl(Process::runPath()).c_str()))
	{
		MsgBox::Warning(L"当文件夹的程序已经运行，若运行更多程序请复制此文件夹", L"提示");
		exit(0);
	}

	timeBeginPeriod(1);

	LoadJson();
	InitUI(!Global::qi.set.zoomBlock);
	QApplication app(argc, argv);

	Global::qi.ReScreen();
	TipsWindow::screen = Global::qi.screen;
	TipsWindow::thread = Thread::Start(TipsWindow::TipsWindowThread);

	MainUi wnd;
	if (!Global::qi.set.minMode) wnd.show();

	app.exec();

	timeEndPeriod(1);
	return 0;
}