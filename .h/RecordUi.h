#pragma once
#include <qevent.h>
#include "ui_RecordUi.h"
#include "../static.h"

class RecordUi : public QDialog
{
	Q_OBJECT

public:
	RecordUi(QWidget* main, List<QiItem>& items) : QDialog(main)
	{
		this->main = main;
		this->items = &items;
		ui.setupUi(this);
		setParent(0);
		setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
		setMouseTracking(true);

		ui.bnStart->setText(UI::rcStart);
		ui.bnClose->setText(UI::rcClose);

		WidEvent();
	}

	void AddItems(byte vk, bool state, POINT pt = { 0 })
	{
		if (!begin)
		{
			tim = clock();
			begin = 1;
		}

		// delay
		items->Add(QiItem(QiAction::delay));
		QiDelayPtr(items->Get())->ms = clock() - tim;
		tim = clock();

		// pos
		if (Input::Type(vk))
		{
			items->Add(QiItem(QiAction::mouse));
			POINT abs = RelToAbs(pt);
			QiMousePtr(items->Get())->x = abs.x;
			QiMousePtr(items->Get())->y = abs.y;
		}

		// key
		items->Add(QiItem(QiAction::key));
		QiKeyPtr(QiItem(QiAction::key))->vk = vk;
		if (state) QiKeyPtr(QiItem(QiAction::key))->state= QiKey::down;
		else QiKeyPtr(QiItem(QiAction::key))->state= QiKey::up;
	}

	bool State() { return start; }

	void EndRec()
	{
		if (start)
		{
			items->Add();
			items->Add();
		}
		else
		{
			items->Emp();
		}
		OnBnStart();
	}

private:
	Ui::RecordUiClass ui;
	QWidget* main = 0;
	List<QiItem>* items = 0;
	long long tim = 0;
	bool start = 0, begin = 0;

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
			items->Del();
			items->Del();
			items->Del();
			items->Del();
			items->Del();
			items->Del();

			Global::qi.scripts.Get().name = NameFilter(L"录制");
			SaveScript(Global::qi.scripts.Get());
			close();
		}
		else
		{
			TipsWindow::Hide();
			items->Emp();
			ui.bnStart->setText(UI::rcStop);
			start = 1;
			Global::qi.rec = this;
		}
	}

	void OnBnClose()
	{
		TipsWindow::Hide();
		Global::qi.rec = 0;
		Global::qi.scripts.Del();
		main->show();
		close();
	}
};