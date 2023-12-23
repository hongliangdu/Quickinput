#pragma once
#include "../static.h"

class QiItem;

struct QiAction {
	enum {
		none,
		end,
		delay,
		key,
		mouse,
		text,
		color,
		loop,
		loopEnd
	};

	std::wstring mark;
};

struct QiEnd : public QiAction { };

struct QiDelay : public QiAction {
	int32 ms = 0;
	int32 ex = 0;
};

struct QiKey : public QiAction {
	enum {
		up,
		down,
		click
	};
	int32 state = up;
	int32 vk = 0;
};

struct QiMouse : public QiAction {
	enum {
		pos,
		move
	};
	int32 state = pos;
	int32 x = 0;
	int32 y = 0;
	int32 ex = 0;
};

struct QiText : public QiAction {
	std::wstring str;
};

struct QiColor : public QiAction {
	enum {
		find,
		unFind
	};
	bool state = find;
	bool move = 0;
	COLORREF rgbe;
	RECT rect;
	List<QiItem> next;
};

struct QiLoop : public QiAction {
	int32 count = 0;
	List<QiItem> next;
};

struct QiLoopEnd : public QiAction { };

class QiItem {
public:
	typedef int ActionType;

private:
	ActionType _type = QiAction::none;
	QiAction* _action = 0;

public:
	QiItem() { Set(QiAction::none); };
	QiItem(ActionType actionType) { Set(actionType); };
	~QiItem() { Del(); };

	void operator=(const QiItem& item) {
		Set(item.type());
		Cpy(item.action());
	}
	void operator=(const QiEnd& item)
	{
		Set(QiAction::end);
		Cpy(&item);
	}
	void operator=(const QiDelay& item)
	{
		Set(QiAction::delay);
		Cpy(&item);
	}
	void operator=(const QiKey& item)
	{
		Set(QiAction::key);
		Cpy(&item);
	}
	void operator=(const QiMouse& item)
	{
		Set(QiAction::mouse);
		Cpy(&item);
	}
	void operator=(const QiText& item)
	{
		Set(QiAction::text);
		Cpy(&item);
	}
	void operator=(const QiColor& item)
	{
		Set(QiAction::color);
		Cpy(&item);
	}
	void operator=(const QiLoop& item)
	{
		Set(QiAction::loop);
		Cpy(&item);
	}
	void operator=(const QiLoopEnd& item)
	{
		Set(QiAction::loopEnd);
		Cpy(&item);
	}

	void Set(ActionType actionType)
	{
		Del();
		_type = actionType;
		switch (actionType)
		{
		case QiAction::end:
			_action = new QiEnd;
			break;
		case QiAction::delay:
			_action = new QiDelay;
			break;
		case QiAction::key:
			_action = new QiKey;
			break;
		case QiAction::mouse:
			_action = new QiMouse;
			break;
		case QiAction::text:
			_action = new QiText;
			break;
		case QiAction::color:
			_action = new QiColor;
			break;
		case QiAction::loop:
			_action = new QiLoop;
			break;
		case QiAction::loopEnd:
			_action = new QiLoopEnd;
			break;
		}
	}
	void Cpy(const QiAction* item)
	{
		_action->mark = item->mark;
		switch (_type)
		{
		case QiAction::delay:
			((QiDelay*)_action)->ms = ((QiDelay*)(item))->ms;
			((QiDelay*)_action)->ex = ((QiDelay*)(item))->ex;
			break;
		case QiAction::key:
			((QiKey*)_action)->state = ((QiKey*)(item))->state;
			((QiKey*)_action)->vk = ((QiKey*)(item))->vk;
			break;
		case QiAction::mouse:
			((QiMouse*)_action)->state = ((QiMouse*)(item))->state;
			((QiMouse*)_action)->x = ((QiMouse*)(item))->x;
			((QiMouse*)_action)->y = ((QiMouse*)(item))->y;
			((QiMouse*)_action)->ex = ((QiMouse*)(item))->ex;
			break;
		case QiAction::text:
			((QiText*)_action)->str = ((QiText*)(item))->str;
			break;
		case QiAction::color:
			((QiColor*)_action)->state = ((QiColor*)(item))->state;
			((QiColor*)_action)->move = ((QiColor*)(item))->move;
			((QiColor*)_action)->rect = ((QiColor*)(item))->rect;
			((QiColor*)_action)->rgbe = ((QiColor*)(item))->rgbe;
			((QiColor*)_action)->next = ((QiColor*)(item))->next;
			break;
		case QiAction::loop:
			((QiLoop*)_action)->count = ((QiLoop*)(item))->count;
			((QiLoop*)_action)->next = ((QiLoop*)(item))->next;
			break;
		}
	}
	void Del()
	{
		if (_action) {
			delete _action;
			_action = 0;
		}
		_type = QiAction::none;
	}

	const ActionType type() const { return _type; }
	QiAction* action() const { return _action; }
};

static void DelItem(List<QiItem>& item)
{
	for (uint32 u = item.len(); u > 0; u--)
	{
		if (item[u].type() == QiAction::color)
		{
			DelItem(((QiColor*)(item[u].action()))->next);
		}
		else if (item[u].type() == QiAction::loop)
		{
			DelItem(((QiLoop*)(item[u].action()))->next);
		}
		item.Del();
	}
}

#define QiDelayPtr(_QiItem) ((QiDelay*)(_QiItem.action()))
#define QiKeyPtr(_QiItem) ((QiKey*)(_QiItem.action()))
#define QiMousePtr(_QiItem) ((QiMouse*)(_QiItem.action()))
#define QiTextPtr(_QiItem) ((QiText*)(_QiItem.action()))
#define QiColorPtr(_QiItem) ((QiColor*)(_QiItem.action()))
#define QiLoopPtr(_QiItem) ((QiLoop*)(_QiItem.action()))