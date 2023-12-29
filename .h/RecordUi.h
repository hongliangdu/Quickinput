#pragma once
#include <qevent.h>
#include "ui_RecordUi.h"
#include "../static.h"

class RecordUi : public QDialog
{
	Q_OBJECT

public:
	RecordUi(Actions& actions) : QDialog()
	{
		this->actions = &actions;
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setMouseTracking(true);

		ui.bnStart->setText(UI::rcStart);
		ui.bnClose->setText(UI::rcClose);

		WidEvent();
	}

	void AddItems(const byte vk, const bool state, POINT pt = { 0 })
	{
		Action action;

		if (!begin)
		{
			tim = clock();
			begin = 1;
		}
		else
		{
			// delay
			action.type = Action::_delay;
			action.delay.ms = clock() - tim;
			actions->Add(action);
			tim = clock();
		}

		// pos
		if (Input::Type(vk))
		{
			pt = RelToAbs(pt);
			action.Emp();
			action.type = Action::_mouse;
			action.mouse.x = pt.x;
			action.mouse.y = pt.y;
			actions->Add(action);
		}

		// key
		action.Emp();
		action.type = Action::_key;
		action.key.vk = vk;
		if (state) action.key.state = Action::Key::down;
		else action.key.state = Action::Key::up;
		actions->Add(action);
	}

	bool State() const { return start; }

	void EndRec()
	{
		click = 1;
		OnBnStart();
		click = 0;
	}

private:
	Ui::RecordUiClass ui;
	Actions* actions = 0;
	long long tim = 0;
	bool start = 0, begin = 0, click = 0;

	void WidEvent()
	{
		connect(ui.bnStart, SIGNAL(clicked()), this, SLOT(OnBnStart()));
		connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));
	}

	void mouseMoveEvent(QMouseEvent* et)
	{
		if (et->buttons() & Qt::LeftButton) move(et->pos() + pos() - QPoint(5, 15));
	}

public slots:
	void OnBnStart()
	{
		if (start)
		{
			Global::qi.rec = 0;
			if (!click) actions->DelBack(6);

			Global::qi.scripts.Get().name = NameFilter(L"录制");
			SaveScript(Global::qi.scripts.Get());
			close();
		}
		else
		{
			TipsWindow::Hide();
			actions->clear();
			ui.bnStart->setText(UI::rcStop);
			start = 1;
			Global::qi.rec = this;
		}
	}

	void OnBnClose()
	{
		TipsWindow::Hide();
		Global::qi.rec = 0;
		Global::qi.scripts.DelBack();
		close();
	}
};