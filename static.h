#pragma once
#include "D:/#CGDATA/Code/cpp/cg.h"
#include "D:/#CGDATA/Code/cpp/CJsonObject.h"
#include ".h/QuickInputDef.h"
#include ".h/TipsWindow.h"

static void InitUI(bool zoom)
{
	if (zoom && System::screenSize().cy > 1200) qputenv("QT_SCALE_FACTOR", QByteArray::number(System::screenZoomRote(), 10, 1));
	{
		if (System::Version().dwMajorVersion >= 10)
		{
			UI::syOn = u8"✅";
			UI::syOff = u8"⛔";
			UI::syOk = u8"⭕";
			UI::syNot = u8"❌";
			UI::syStop = u8"🛑";
			UI::syShow = u8"🔼";
			UI::syHide = u8"🔽";
			UI::syOption = u8"⚙";
			UI::syLink = u8"🔗";
			UI::syEdit = u8"🔧";
			UI::syUp = u8"⬆️";
			UI::syDown = u8"⬇️";
			UI::syTurn = u8"🔃";
			UI::syLeft = u8"🔙";
			UI::syTop = u8"🔝";
			UI::syRight = u8"🔜";
			UI::syMove = u8"🔛";
			UI::syTime = u8"⏳";
			UI::syText = u8"🅰️";
			UI::syLoop = u8"♾️";
			UI::syColor = u8"🌈";
		}
		else
		{
			UI::syOn = u8"✔";
			UI::syOff = u8"✘";
			UI::syOk = u8"✔";
			UI::syNot = u8"✘";
			UI::syStop = u8"Ⓢ";
			UI::syShow = u8"▲";
			UI::syOption = u8"✱";
			UI::syLink = u8"※";
			UI::syEdit = u8"▲";
			UI::syUp = u8"↑";
			UI::syDown = u8"↓";
			UI::syTurn = u8"◈";
			UI::syLeft = u8"←";
			UI::syTop = u8"↑";
			UI::syRight = u8"→";
			UI::syMove = u8"↘";
			UI::syTime = u8"☽";
			UI::syText = u8"A";
			UI::syLoop = u8"↩";
			UI::syColor = u8"☀";
		}
		{
			UI::qiOn = (File::PathLast(Process::runPath()) + std::wstring(L"　已启用" + UI::syOn.toStdWString()));
			UI::qiOff = (File::PathLast(Process::runPath()) + std::wstring(L"　已禁用" + UI::syOff.toStdWString()));
			UI::muOn = (QString::fromUtf8(u8"启用") + UI::syOn);
			UI::muOff = (QString::fromUtf8(u8"禁用") + UI::syOff);
			UI::muShow = (QString::fromUtf8(u8"显示") + UI::syShow);
			UI::muHide = (QString::fromUtf8(u8"隐藏") + UI::syHide);
			UI::muExit = (QString::fromUtf8(u8"退出") + UI::syStop);
			UI::acDown = (QString::fromUtf8(u8"按下") + UI::syDown);
			UI::acUp = (QString::fromUtf8(u8"松开") + UI::syUp);
			UI::acClick = (QString::fromUtf8(u8"点击") + UI::syTurn);
			UI::acPos = (QString::fromUtf8(u8"位置") + UI::syLeft);
			UI::acMove = (QString::fromUtf8(u8"移动") + UI::syMove);
			UI::acWait = (QString::fromUtf8(u8"等待") + UI::syTime);
			UI::acText = (QString::fromUtf8(u8"复制") + UI::syText);
			UI::acLoop = (QString::fromUtf8(u8"循环") + UI::syLoop);
			UI::acColor = (QString::fromUtf8(u8"找色") + UI::syColor);
			UI::acEnd = (QString::fromUtf8(u8"结束") + UI::syStop);
			UI::acEndLoop = (QString::fromUtf8(u8"结束循环") + UI::syStop);
			UI::trOn = (QString::fromUtf8(u8"启用") + UI::syOn);
			UI::trOff = (QString::fromUtf8(u8"禁用") + UI::syOff);
			UI::etFunc = (QString::fromUtf8(u8"动作") + UI::syOption);
			UI::etParam = (QString::fromUtf8(u8"参数") + UI::syLink);
			UI::etAdd = (QString::fromUtf8(u8"添加") + UI::syOk);
			UI::etDel = (QString::fromUtf8(u8"删除") + UI::syNot);
			UI::etChange = (QString::fromUtf8(u8"修改") + UI::syOk);
			UI::etEdit = (QString::fromUtf8(u8"编辑") + UI::syEdit);
			UI::rcStart = (QString::fromUtf8(u8"开始") + UI::syOk);
			UI::rcStop = (QString::fromUtf8(u8"停止") + UI::syOk);
			UI::rcClose = (QString::fromUtf8(u8"取消") + UI::syNot);
		}
	}
}

static uint8 ExcItem(Action& item)
{
	if (!Global::qi.run) return 1;
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
		RgbMap rgbMap;
		Image::HdcRgbmap(Global::qi.hdc, Global::qi.screen, rgbMap);
		Color::FindOrStatus result = Color::FindOr(rgbMap, rect, item.color.rgbe.toRgb(), item.color.rgbe.a);
		if (item.color.unfind)
		{
			if (!result.find)
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
			if (result.find)
			{
				if (item.color.move == 1) Input::MoveTo(result.pt.x, result.pt.y);
				for (uint32 u = 0; u < item.color.next.size(); u++)
				{
					uint8 r = ExcItem(item.color.next[u]);
					if (r) return r;
				}
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
					uint8 r = ExcItem(item.loop.next[ux]);
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
static DWORD CALLBACK ThreadQuickClick(LPVOID)
{
	srand(clock());
	uint32 b = 0;
	uint32 e = 0;
	if (Global::qi.fun.quickClick.delay > 99) b = 50, e = Global::qi.fun.quickClick.delay - 50;
	else if (Global::qi.fun.quickClick.delay > 1) b = Global::qi.fun.quickClick.delay / 2, e = b;
	else e = Global::qi.fun.quickClick.delay;

	while (Global::qi.run)
	{
		Input::State(Global::qi.fun.quickClick.key, 1, 214);
		Thread::Sleep(b);
		Input::State(Global::qi.fun.quickClick.key, 0, 214);
		Thread::Sleep(e);
	}
	return 0;
}
static DWORD CALLBACK ThreadMacro(LPVOID lParam)
{
	srand(clock());
	uint32 pos = (UINT)lParam;
	uint32 count = Global::qi.scripts[pos].a;
	uint32 n = 0;
	while (Global::qi.run)
	{
		if (count)
		{
			n++;
			if (n > count) break;
		}
		for (uint32 n = 0; n < Global::qi.scripts[pos].actions.size() && Global::qi.run; n++)
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
static DWORD CALLBACK ThreadRelease(LPVOID key)
{
	Input::State((BYTE)key, 0, 214);
	return 0;
}
static DWORD CALLBACK ThreadWndActive(LPVOID)
{
	while (Global::qi.state)
	{
		Global::qi.fun.wndActive.wnd = FindWindowW(0, Global::qi.fun.wndActive.name.c_str());
		if (Global::qi.fun.wndActive.wnd)
		{
			bool active = (GetForegroundWindow() == Global::qi.fun.wndActive.wnd);
			if (!Global::qi.run && active)
			{
				Global::qi.run = 1;
				if (Global::qi.set.showTips) TipsWindow::Popup(L"已启用 - 窗口内", RGB(0xA0, 0xFF, 0xC0));
			}
			else if (Global::qi.run && !active)
			{
				Global::qi.run = 0;
				if (Global::qi.set.showTips) TipsWindow::Popup(L"已禁用 - 窗口外", RGB(0xFF, 0x80, 0x80));
			}
		}
		else if (Global::qi.run)
		{
			Global::qi.run = 0;
			if (Global::qi.set.showTips) TipsWindow::Popup(L"已禁用 - 窗口外", RGB(0xFF, 0x80, 0x80));
		}
		sleep(100);
	}
	Global::qi.fun.wndActive.thread = 0;
	return 0;
}

static void UpdateBlock()
{
	Global::trBlock.clear();
	for (uint32 u = 0; u < Global::qi.scripts.size(); u++)
	{
		if (Global::qi.scripts[u].state && Global::qi.scripts[u].block)
		{
			if ((Global::qi.scripts[u].key & 0xFFFF)) Global::trBlock.Add((Global::qi.scripts[u].key & 0xFFFF));
			if (Global::qi.scripts[u].key >> 16) Global::trBlock.Add(Global::qi.scripts[u].key >> 16);
		}
	}
	memset(Global::blockRep, 0, sizeof(Global::blockRep));
}
static void HookState(bool state)
{
	if (state)
	{
		if (!InputHook::Start(InputHook::all)) MsgBox::Error(L"创建输入Hook失败，检查是否管理员身份运行 或 是否被安全软件拦截。");
	}
	else InputHook::Stop(InputHook::all);
}
static void QiState(bool state)
{
	if (state)
	{
		UpdateBlock();
		Global::qi.state = 1;
		Global::qi.ReScreen();
		TipsWindow::screen = Global::qi.screen;
		TipsWindow::Popup(UI::qiOn);
		if (Global::qi.set.audFx)Media::WavePlay(audfx_on);
		if (Global::qi.fun.wndActive.state) { if (!Global::qi.fun.wndActive.thread) Global::qi.fun.wndActive.thread = Thread::Start(ThreadWndActive); }
		else Global::qi.run = 1;
	}
	else
	{
		Global::qi.state = 0, Global::qi.run = 0;
		TipsWindow::Popup(UI::qiOff, RGB(0xFF, 0x50, 0x50));
		if (Global::qi.set.audFx)Media::WavePlay(audfx_off);
	}
}