#pragma once
#include <QString>
#include <QFont>
#include <qdesktopwidget.h>
#include <qapplication.h>
#include "D:/#CGDATA/Code/cpp/cg.h"
#include "D:/#CGDATA/Code/cpp/CJsonObject.h"

struct UI {
	static QString syOn;
	static QString syOff;
	static QString syOk;
	static QString syNot;
	static QString syStop;
	static QString syShow;
	static QString syHide;
	static QString syOption;
	static QString syLink;
	static QString syEdit;
	static QString syUp;
	static QString syDown;
	static QString syTurn;
	static QString syLeft;
	static QString syTop;
	static QString syRight;
	static QString syMove;
	static QString syTime;
	static QString syText;
	static QString syLoop;
	static QString syColor;

	static std::wstring qiOn;
	static std::wstring qiOff;
	static QString muOn;
	static QString muOff;
	static QString muShow;
	static QString muHide;
	static QString muExit;
	static QString acWait;
	static QString acDown;
	static QString acUp;
	static QString acClick;
	static QString acPos;
	static QString acMove;
	static QString acLoop;
	static QString acText;
	static QString acColor;
	static QString acEnd;
	static QString acEndLoop;
	static QString trOn;
	static QString trOff;
	static QString etChange;
	static QString etAdd;
	static QString etDel;
	static QString etEdit;
	static QString etFunc;
	static QString etParam;
	static QString rcStart;
	static QString rcStop;
	static QString rcClose;
};

struct Action;
typedef List<Action> Actions;

struct Action
{
	enum
	{
		_none,
		_end,
		_delay,
		_key,
		_mouse,
		_text,
		_color,
		_loop,
		_loopEnd
	};

	struct Delay { uint32 ms; uint32 ex; };
	struct Key { enum { up, down, click }; uint32 vk = 0; uint32 state = up; };
	struct Mouse { int32 x = 0; int32 y = 0; uint32 ex = 0; bool move = 0; };
	struct Text { String::wcstring str; };
	struct Color { Rgba rgbe = 0; RECT rect = { 0 }; bool unfind = 0; bool move = 0; Actions next; };
	struct Loop { uint32 count; Actions next; };

	union
	{
		byte memsize[36];
		Delay delay;
		Key key;
		Mouse mouse;
		Text text;
		Color color;
		Loop loop;
	};

	uint32 type = _none;
	std::wstring mark;

	Action() { Emp(); }
	Action(const uint32 actionType) { Emp(); type = actionType; }
	Action(const Action& action) { Cpy(action); }
	~Action() { Emp(); }

	void operator=(const Action& action) { Cpy(action); }

	void Cpy(const Action& action)
	{
		Emp();
		mark = action.mark;
		switch (action.type)
		{
		case _end: type = _end; break;
		case _delay: type = _delay; delay = action.delay; break;
		case _key: type = _key; key = action.key; break;
		case _mouse: type = _mouse; mouse = action.mouse; break;
		case _text: type = _text; text.str.cpy(action.text.str); break;
		case _color: type = _color; color = action.color; break;
		case _loop: type = _loop; loop = action.loop; break;
		case _loopEnd: type = _loopEnd; break;
		default: type = _none; break;
		}
	}

	void Emp() {
		if (type == _text) text.str.emp();
		type = _none;
		mark = L""; memset(&memsize, 0, 36);
	}
};

struct Script
{
	enum { sw, down, up };
	bool state = 0;
	bool block = 0;
	bool k1 = 0, k2 = 0, active = 0;
	uint32 mode = 0;
	uint32 key = 0;
	uint32 a = 0;
	std::wstring name;
	Actions actions;
	HANDLE thread = 0;
};
typedef List<Script> Scripts;

struct QuickClick
{
	bool state = 0;
	uint32 mode = 0;
	uint32 key = 0;
	uint32 delay = 10;
	HANDLE thread = 0;
};

struct ShowClock
{
	bool state = 0;
	uint32 key = 0;
};

struct WndActive
{
	bool state = 0;
	HWND wnd = 0;
	HANDLE thread = 0;
	std::wstring name;
};

struct SettingsData
{
	uint32 key = 0;
	uint32 recKey = 0;
	bool shift = 0, f12 = 0;
	bool k1 = 0, k2 = 0;
	bool defOn = 0;
	bool showTips = 0;
	bool audFx = 0;
	bool minMode = 0;
	bool zoomBlock = 0;
};

struct FuncData
{
	QuickClick quickClick;
	ShowClock showClock;
	WndActive wndActive;
};

struct QuickInputStruct
{
public:
	bool state = 0;
	bool run = 0;
	Scripts scripts;
	FuncData fun;
	SettingsData set;
	LPVOID rec = 0;

	HDC hdc = GetDC(0);
	SIZE screen = System::screenSize();
	SIZE vscreen = System::screenVSize();

	void ReScreen()
	{
		ReleaseDC(0, hdc);
		hdc = GetDC(0);
		screen = System::screenSize();
		QRect qr = QApplication::desktop()->screenGeometry();
		vscreen.cx = qr.width();
		vscreen.cy = qr.height();
	}
};

struct Global {
	static QuickInputStruct qi;
	static List<byte> trBlock;
	static byte blockRep[255];
};

static POINT RelToAbs(POINT rel) { return { (long)(((double)rel.x / ((double)Global::qi.screen.cx - 1.0)) * 10000.0), (long)(((double)rel.y / ((double)Global::qi.screen.cy - 1.0)) * 10000.0) }; }

static POINT AbsToRel(POINT abs) { return { (long)((double)Global::qi.screen.cx / 10000.0 * (double)abs.x), (long)((double)Global::qi.screen.cy / 10000.0 * (double)abs.y) }; }

static POINT AbsToVRel(POINT abs) { return { (long)((double)Global::qi.vscreen.cx / 10000.0 * (double)abs.x), (long)((double)Global::qi.vscreen.cy / 10000.0 * (double)abs.y) }; }

static void _stdcall SaveItem(neb::CJsonObject& jActions, Actions& actions)
{
	for (uint32 u = 0; u < actions.size(); u++)
	{
		neb::CJsonObject jItem;
		neb::CJsonObject jNext;

		std::string str = String::toString(actions[u].mark);
		jItem.Add("mark", str);

		switch (actions[u].type)
		{
		case Action::_end:
		{
			jItem.Add("type", Action::_end);
		}
		break;
		case Action::_delay:
		{
			jItem.Add("type", Action::_delay);
			jItem.Add("ms", actions[u].delay.ms);
			jItem.Add("ex", actions[u].delay.ex);
		}
		break;
		case Action::_key:
		{
			jItem.Add("type", Action::_key);
			jItem.Add("state", actions[u].key.state);
			jItem.Add("vk", actions[u].key.vk);
		}
		break;
		case Action::_mouse:
		{
			jItem.Add("type", Action::_mouse);
			jItem.Add("move", actions[u].mouse.move);
			jItem.Add("x", actions[u].mouse.x);
			jItem.Add("y", actions[u].mouse.y);
			jItem.Add("ex", actions[u].mouse.ex);
		}
		break;
		case Action::_text:
		{
			jItem.Add("type", Action::_text);
			jItem.Add("text", String::toString(actions[u].text.str.str));
		}
		break;
		case Action::_color:
		{
			jItem.Add("type", Action::_color);
			jItem.Add("unfind", actions[u].color.unfind);
			jItem.Add("move", actions[u].color.move);
			jItem.Add("left", (int32)actions[u].color.rect.left);
			jItem.Add("top", (int32)actions[u].color.rect.top);
			jItem.Add("right", (int32)actions[u].color.rect.right);
			jItem.Add("bottom", (int32)actions[u].color.rect.bottom);
			jItem.Add("rgbe", (uint32)actions[u].color.rgbe.toCOLORREF());
			SaveItem(jNext, actions[u].color.next);
			jItem.Add("next", jNext);
		}
		break;
		case Action::_loop:
		{
			jItem.Add("type", Action::_loop);
			jItem.Add("count", actions[u].loop.count);
			SaveItem(jNext, actions[u].loop.next);
			jItem.Add("next", jNext);
		}
		break;
		case Action::_loopEnd:
			jItem.Add("type", Action::_loopEnd);
			break;
		}
		jActions.Add(jItem);
	}
}

static void SaveScript(Script& script)
{
	neb::CJsonObject json;
	neb::CJsonObject jActions;
	SaveItem(jActions, script.actions);
	json.Add("documen_ charset", std::string("GB2312"));
	json.Add("state", script.state);
	json.Add("block", script.block);
	json.Add("mode", script.mode);
	json.Add("key", script.key);
	json.Add("a", script.a);
	json.Add("actions", jActions);

	std::wstring path = L"macro\\";
	path += script.name;
	path += L".json";
	File::FolderCreate(L"macro");
	File::TextSave(path, String::String::toWString(json.ToString()), "chinese");
}

static void SaveJson()
{
	neb::CJsonObject cfg;
	std::string scache;
	cfg.Add("document_charset", std::string("GB2312"));
	cfg.Add("defOn", Global::qi.set.defOn);
	cfg.Add("key", Global::qi.set.key);
	cfg.Add("recKey", Global::qi.set.recKey);
	cfg.Add("showTips", Global::qi.set.showTips);
	cfg.Add("audFx", Global::qi.set.audFx);
	cfg.Add("minMode", Global::qi.set.minMode);
	cfg.Add("zoomBlock", Global::qi.set.zoomBlock);
	cfg.Add("quickClickKey", Global::qi.fun.quickClick.key);
	cfg.Add("quickClickState", Global::qi.fun.quickClick.state);
	cfg.Add("quickClickDelay", Global::qi.fun.quickClick.delay);
	cfg.Add("quickClickMode", Global::qi.fun.quickClick.mode);
	cfg.Add("showClockKey", Global::qi.fun.showClock.key);
	cfg.Add("showClockState", Global::qi.fun.showClock.state);
	cfg.Add("wndActiveState", Global::qi.fun.wndActive.state);
	scache = String::toString(Global::qi.fun.wndActive.name); cfg.Add("wndActiveName", scache);

	File::TextSave(L"QuickInput.json", String::String::toWString(cfg.ToString()), "chinese");
}

static void _stdcall LoadItem(const neb::CJsonObject jActions, Actions& actions)
{
	for (uint32 u = 0; u < jActions.GetArraySize(); u++)
	{
		Action action;
		neb::CJsonObject jItem;
		neb::CJsonObject jNext;
		jActions.Get(u, jItem);

		uint32 ui32 = 0;
		jItem.Get("type", ui32);
		if (ui32)
		{
			switch (ui32)
			{
			case Action::_end: action.type = Action::_end; break;
			case Action::_delay:
			{
				action.type = Action::_delay;
				jItem.Get("ms", action.delay.ms);
				jItem.Get("ex", action.delay.ex);
			}
			break;
			case Action::_key:
			{
				action.type = Action::_key;
				jItem.Get("state", action.key.state);
				jItem.Get("vk", action.key.vk);
			}
			break;
			case Action::_mouse:
			{
				action.type = Action::_mouse;
				jItem.Get("move", action.mouse.move);
				jItem.Get("x", action.mouse.x);
				jItem.Get("y", action.mouse.y);
				jItem.Get("ex", action.mouse.ex);
			}
			break;
			case Action::_text:
			{
				std::string str;
				jItem.Get("text", str);
				action.type = Action::_text;
				action.text.str = String::toWString(str).c_str();
			}
			break;
			case Action::_color:
			{
				action.type = Action::_color;
				jItem.Get("unfind", action.color.unfind);
				jItem.Get("move", action.color.move);
				jItem.Get("left", (int32&)(action.color.rect.left));
				jItem.Get("top", (int32&)(action.color.rect.top));
				jItem.Get("right", (int32&)(action.color.rect.right));
				jItem.Get("bottom", (int32&)(action.color.rect.bottom));
				jItem.Get("rgbe", ui32); action.color.rgbe.set(ui32);
				jItem.Get("next", jNext);
				LoadItem(jNext, action.color.next);
			}
			break;
			case Action::_loop:
			{
				action.type = Action::_loop;
				jItem.Get("count", action.loop.count);
				jItem.Get("next", jNext);
				LoadItem(jNext, action.loop.next);
			}
			break;
			case Action::_loopEnd: action.type = Action::_loopEnd; break;
			default: action.type = Action::_none; break;
			}

			std::string str;
			jItem.Get("mark", str);
			action.mark = String::toWString(str);
			actions.Add(action);
		}
	}
}

static void LoadJson()
{
	Global::qi.scripts.clear();
	if (File::FileState(L"QuickInput.json"))
	{
		neb::CJsonObject cfg(String::toString(File::TextLoad(L"QuickInput.json")));
		std::string scache;
		cfg.Get("defOn", Global::qi.set.defOn);
		cfg.Get("key", Global::qi.set.key);
		cfg.Get("recKey", Global::qi.set.recKey);
		cfg.Get("showTips", Global::qi.set.showTips);
		cfg.Get("audFx", Global::qi.set.audFx);
		cfg.Get("minMode", Global::qi.set.minMode);
		cfg.Get("zoomBlock", Global::qi.set.zoomBlock);
		cfg.Get("quickClickState", Global::qi.fun.quickClick.state);
		cfg.Get("quickClickKey", Global::qi.fun.quickClick.key);
		cfg.Get("quickClickDelay", Global::qi.fun.quickClick.delay);
		cfg.Get("quickClickMode", Global::qi.fun.quickClick.mode);
		cfg.Get("showClockState", Global::qi.fun.showClock.state);
		cfg.Get("showClockKey", Global::qi.fun.showClock.key);
		cfg.Get("wndActiveState", Global::qi.fun.wndActive.state);
		cfg.Get("wndActiveName", scache); Global::qi.fun.wndActive.name = String::toWString(scache);
	}
	else
	{
		Global::qi.set.defOn = 1;
		Global::qi.set.key = VK_F8;
		Global::qi.set.recKey = VK_F8;
		Global::qi.set.showTips = 1;
		Global::qi.set.audFx = 0;
		Global::qi.set.minMode = 0;
		Global::qi.set.zoomBlock = 0;
		Global::qi.fun.quickClick.state = 1;
		Global::qi.fun.quickClick.key = VK_LBUTTON;
		Global::qi.fun.quickClick.delay = 200;
		Global::qi.fun.quickClick.mode = 0;
		Global::qi.fun.showClock.state = 0;
		Global::qi.fun.showClock.key = VK_MENU;
		Global::qi.fun.wndActive.state = 0;
		Global::qi.fun.wndActive.name = L"";
	}

	File::FindFileStruct files = File::FindFile(L"macro\\*.json");

	for (uint32 u = 0; u < files.size(); u++) {
		Global::qi.scripts.AddNull();
		Script& script = Global::qi.scripts.Get();
		script.name = ((std::wstring)files[u].name).substr(0, wcslen(files[u].name) - 5);

		neb::CJsonObject jScript(String::toString(File::TextLoad(((std::wstring)L"macro\\" + files[u].name), "chinese")));

		jScript.Get("state", script.state);
		jScript.Get("block", script.block);
		jScript.Get("mode", script.mode);
		jScript.Get("key", script.key);
		jScript.Get("a", script.a);

		neb::CJsonObject jActions;
		jScript.Get("actions", jActions);

		LoadItem(jActions, script.actions);
	}
}

static std::wstring NameFilter(std::wstring name)
{
	for (uint32 n = 0;; n++)
	{
		for (uint32 nx = 0; nx < Global::qi.scripts.size(); nx++)
		{
			std::wstring find = name + L" " + String::toWString(n + 1);
			if (Global::qi.scripts[nx].name == find)
			{
				break;
			}
			if (nx >= Global::qi.scripts.size() - 1)
			{
				return find;
			}
		}
	}
	return L"";
}
