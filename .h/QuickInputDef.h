#pragma once
#include <QString>
#include <QFont>
#include <qdesktopwidget.h>
#include <qapplication.h>
#include "TipsWindow.h"
#include "Action.h"
#include "../static.h"

struct QuickInputStruct;

struct Global
{
	static QuickInputStruct qi;
};

struct UI {
	static std::wstring qiOn;
	static std::wstring qiOff;
	static QString qiWait;
	static QString qiDown;
	static QString qiUp;
	static QString qiClick;
	static QString qiPos;
	static QString qiMove;
	static QString qiLoop;
	static QString qiText;
	static QString qiColor;
	static QString qiEnd;
	static QString qiEndLoop;
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
	bool active = 0;
	bool state = 0;
	HWND wnd = 0;
	HANDLE thread = 0;
	std::wstring name;
};

struct SettingsData
{
	uint32 key = 0;
	uint32 recKey = 0;
	bool k1 = 0, k2 = 0;
	bool showTips = 0;
	bool audFx = 0;
	bool minMode = 0;
	bool wndZoom = 0;
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
	Scripts scripts;
	FuncData fun;
	SettingsData set;
	LPVOID rec = 0;

	HDC hdc = GetDC(0);
	SIZE screen = System::screenSize();
	SIZE vscreen = System::screenVSize();

private:
	void (*SetHookState)(bool) = 0;

public:

	void ReScreen()
	{
		screen = System::screenSize();
		QRect qr = QApplication::desktop()->screenGeometry();
		vscreen.cx = qr.width();
		vscreen.cy = qr.height();
	}

	void Ptrs(void (*SetHookState)(bool))
	{
		this->SetHookState = SetHookState;
	}

	void HookState(bool state)
	{
		SetHookState(state);
	}
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
			jItem.Add("rgbe", (uint32)actions[u].color.rgbe);
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
	cfg.Add("key", Global::qi.set.key);
	cfg.Add("recKey", Global::qi.set.recKey);
	cfg.Add("showTips", Global::qi.set.showTips);
	cfg.Add("audFx", Global::qi.set.audFx);
	cfg.Add("minMode", Global::qi.set.minMode);
	cfg.Add("wndZoom", Global::qi.set.wndZoom);
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

		int32 i32 = 0;
		jItem.Get("type", i32);
		if (i32)
		{
			switch (i32)
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
				jItem.Get("rgbe", (uint32&)(action.color.rgbe));
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
	neb::CJsonObject cfg(String::toString(File::TextLoad(L"QuickInput.json")));
	std::string scache;
	cfg.Get("key", Global::qi.set.key);
	cfg.Get("recKey", Global::qi.set.recKey);
	cfg.Get("showTips", Global::qi.set.showTips);
	cfg.Get("audFx", Global::qi.set.audFx);
	cfg.Get("minMode", Global::qi.set.minMode);
	cfg.Get("wndZoom", Global::qi.set.wndZoom);
	cfg.Get("quickClickKey", Global::qi.fun.quickClick.key);
	cfg.Get("quickClickState", Global::qi.fun.quickClick.state);
	cfg.Get("quickClickDelay", Global::qi.fun.quickClick.delay);
	cfg.Get("quickClickMode", Global::qi.fun.quickClick.mode);
	cfg.Get("showClockKey", Global::qi.fun.showClock.key);
	cfg.Get("showClockState", Global::qi.fun.showClock.state);
	cfg.Get("wndActiveState", Global::qi.fun.wndActive.state);
	cfg.Get("wndActiveName", scache); Global::qi.fun.wndActive.name = String::toWString(scache);

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