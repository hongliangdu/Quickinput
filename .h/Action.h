#pragma once
#include "../static.h"

struct wcstring
{
	wchar_t* str = 0;
	uint32 len = 0;
	void operator=(const wchar_t* res) { cpy(res); }
	void cpy(const wchar_t* res)
	{
		emp();
		len = wcslen(res) + 1;
		str = new wchar_t[len];
		wcscpy_s(str, len, res);
	}
	void cpy(const wcstring& res)
	{
		emp();
		len = res.len;
		str = new wchar_t[len];
		wcscpy_s(str, len, res.str);
	}
	void emp()
	{
		if (str) delete[] str;
		len = 0;
	}
};

struct Action;

typedef List<Action> Actions;

class Action
{
public:
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
	struct Mouse { uint32 x = 0; uint32 y = 0; uint32 ex = 0; bool move = 0; };
	struct Text { wcstring str; };
	struct Color { uint32 rgbe = 0; RECT rect = { 0 }; bool unfind = 0; bool move = 0; Actions next; };
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