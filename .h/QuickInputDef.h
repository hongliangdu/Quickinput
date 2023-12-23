#pragma once
#include <QString>
#include <QFont>
#include "TipsWindow.h"
#include "QiItem.h"
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
	enum {
		sw,
		down,
		up
	};
	bool state = 0;
	bool block = 0;
	bool k1 = 0, k2 = 0, active = 0;
	uint32 mode = 0;
	uint32 key = 0;
	uint32 a = 0;
	std::wstring name;
	List<QiItem> items;
	HANDLE thread = 0;
};

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
	List<Script> scripts;
	FuncData fun;
	SettingsData set;
	LPVOID rec = 0;

	HDC hdc = GetDC(0);
	SIZE screen = System::screenSize();
	
private:
	void (*SetHookState)(bool) = 0;

public:

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

static void _stdcall SaveItem(neb::CJsonObject& jItems, List<QiItem>& items)
{
	for (uint32 u = 0; u < items.len(); u++)
	{
		neb::CJsonObject jItem;
		neb::CJsonObject jNext;

		std::string str = String::toString(items[u].action()->mark);
		jItem.Add("mark", str);

		switch (items[u].type())
		{
		case QiAction::end:
		{
			jItem.Add("type", QiAction::end);
		}
		break;
		case QiAction::delay:
		{
			jItem.Add("type", QiAction::delay);
			QiDelay* action = QiDelayPtr(items[u]);
			jItem.Add("ms", action->ms);
			jItem.Add("ex", action->ex);
		}
		break;
		case QiAction::key:
		{
			jItem.Add("type", QiAction::key);
			QiKey* action = QiKeyPtr(items[u]);
			jItem.Add("state", action->state);
			jItem.Add("vk", action->vk);
		}
		break;
		case QiAction::mouse:
		{
			jItem.Add("type", QiAction::mouse);
			QiMouse* action = QiMousePtr(items[u]);
			jItem.Add("state", action->state);
			jItem.Add("x", action->x);
			jItem.Add("y", action->y);
			jItem.Add("ex", action->ex);
		}
		break;
		case QiAction::text:
		{
			jItem.Add("type", QiAction::text);
			QiText* action = QiTextPtr(items[u]);
			jItem.Add("text", String::toString(action->str));
		}
		break;
		case QiAction::color:
		{
			jItem.Add("type", QiAction::color);
			QiColor* action = QiColorPtr(items[u]);
			jItem.Add("state", action->state);
			jItem.Add("move", action->move);
			jItem.Add("left", (int32)action->rect.left);
			jItem.Add("top", (int32)action->rect.top);
			jItem.Add("right", (int32)action->rect.right);
			jItem.Add("bottom", (int32)action->rect.bottom);
			jItem.Add("rgbe", (uint32)action->rgbe);

			SaveItem(jNext, action->next);
			jItem.Add("next", jNext);
		}
		break;
		case QiAction::loop:
		{
			jItem.Add("type", QiAction::loop);
			QiLoop* action = QiLoopPtr(items[u]);
			jItem.Add("count", action->count);

			SaveItem(jNext, action->next);
			jItem.Add("next", jNext);
		}
		break;
		case QiAction::loopEnd:
			jItem.Add("type", QiAction::loopEnd);
			break;
		}
		jItems.Add(jItem);
	}
}

static void SaveScript(Script& script)
{
	neb::CJsonObject json;
	neb::CJsonObject jItems;
	SaveItem(jItems, script.items);
	json.Add("state", script.state);
	json.Add("block", script.block);
	json.Add("mode", script.mode);
	json.Add("key", script.key);
	json.Add("a", script.a);
	json.Add("items", jItems);

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

static void _stdcall LoadItem(const neb::CJsonObject jItems, List<QiItem>& items)
{
	for (uint32 u = 0; u < jItems.GetArraySize(); u++)
	{
		QiItem item;
		neb::CJsonObject jItem;
		neb::CJsonObject jNext;
		jItems.Get(u, jItem);

		int32 i32 = 0;
		jItem.Get("type", i32);

		if (i32)
		{
			switch (i32)
			{
			case QiAction::end:
			{
				item.Set(QiAction::end);
			}
			break;
			case QiAction::delay:
			{
				item.Set(QiAction::delay);
				jItem.Get("ms", QiDelayPtr(item)->ms);
				jItem.Get("ex", QiDelayPtr(item)->ex);
			}
			break;
			case QiAction::key:
			{
				item.Set(QiAction::key);
				jItem.Get("state", QiKeyPtr(item)->state);
				jItem.Get("vk", QiKeyPtr(item)->vk);
			}
			break;
			case QiAction::mouse:
			{
				item.Set(QiAction::mouse);
				jItem.Get("state", QiMousePtr(item)->state);
				jItem.Get("x", QiMousePtr(item)->x);
				jItem.Get("y", QiMousePtr(item)->y);
				jItem.Get("ex", QiMousePtr(item)->ex);
			}
			break;
			case QiAction::text:
			{
				item.Set(QiAction::text);
				std::string str;
				jItem.Get("text", str);
				QiTextPtr(item)->str = String::toWString(str);
			}
			break;
			case QiAction::color:
			{
				item.Set(QiAction::color);
				jItem.Get("state", QiColorPtr(item)->state);
				jItem.Get("move", QiColorPtr(item)->move);
				jItem.Get("left", (int32&)(QiColorPtr(item)->rect.left));
				jItem.Get("top", (int32&)(QiColorPtr(item)->rect.top));
				jItem.Get("right", (int32&)(QiColorPtr(item)->rect.right));
				jItem.Get("bottom", (int32&)(QiColorPtr(item)->rect.bottom));
				jItem.Get("rgbe", (uint32&)(QiColorPtr(item)->rgbe));

				jItem.Get("next", jNext);
				LoadItem(jNext, QiColorPtr(item)->next);
			}
			break;
			case QiAction::loop:
			{
				item.Set(QiAction::loop);
				jItem.Get("count", QiLoopPtr(item)->count);

				jItem.Get("next", jNext);
				LoadItem(jNext, QiLoopPtr(item)->next);
			}
			break;
			case QiAction::loopEnd:
			{
				item.Set(QiAction::loopEnd);
			}
				break;
			}
			
			std::string str;
			jItem.Get("mark", str);
			item.action()->mark = String::toWString(str);

			items.Add(item);
		}
	}
}

static void LoadJson()
{
	Global::qi.scripts.Emp();
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

	for (uint32 u = 0; u < files.len(); u++) {
		Global::qi.scripts.Add();
		Script& script = Global::qi.scripts.Get();
		script.name = ((std::wstring)files[u].name).substr(0, wcslen(files[u].name) - 5);

		neb::CJsonObject jScript(String::toString(File::TextLoad(((std::wstring)L"macro\\" + files[u].name), "chinese")));

		jScript.Get("state", script.state);
		jScript.Get("block", script.block);
		jScript.Get("mode", script.mode);
		jScript.Get("key", script.key);
		jScript.Get("a", script.a);

		neb::CJsonObject jItems;
		jScript.Get("items", jItems);

		LoadItem(jItems, script.items);
	}
}

static std::wstring NameFilter(std::wstring name)
{
	for (uint32 n = 0;; n++)
	{
		for (uint32 nx = 0; nx < Global::qi.scripts.len(); nx++)
		{
			std::wstring find = name + L" " + String::toWString(n + 1);
			if (Global::qi.scripts[nx].name == find)
			{
				break;
			}
			if (nx >= Global::qi.scripts.len() - 1)
			{
				return find;
			}
		}
	}
	return L"";
}