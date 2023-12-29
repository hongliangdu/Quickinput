#pragma once
#include <qvalidator.h>
#include "ui_FuncUi.h"
#include "../static.h"

class FuncUi : public QWidget
{
	Q_OBJECT

public:

	FuncUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		WidInit();
		WidEvent();
	}

private:
	Ui::FuncUiClass ui;
	FuncData* func = &Global::qi.fun;

	void WidInit()
	{
		ui.chbQkClick->setChecked(func->quickClick.state);
		ui.chbClock->setChecked(func->showClock.state);
		ui.chbWndActive->setChecked(func->wndActive.state);

		ui.etQkDelay->setText(QString::number(func->quickClick.delay));
		ui.etQkDelay->setValidator(new QIntValidator(0, 99999, this));
		ui.etWndActive->setText(QString::fromWCharArray(func->wndActive.name.c_str()));

		ui.hkQkClick->Mode(0);
		ui.hkQkClick->VirtualKey(func->quickClick.key);
		ui.hkClock->Mode(0);
		ui.hkClock->VirtualKey(func->showClock.key);

		ui.cmbMode->addItem(u8"按下");
		ui.cmbMode->addItem(u8"切换");
		ui.cmbMode->setCurrentIndex(func->quickClick.mode);

	}

	void WidEvent()
	{
		connect(ui.hkQkClick, SIGNAL(changed()), this, SLOT(OnQkClickKey()));
		connect(ui.etQkDelay, SIGNAL(textEdited(const QString&)), this, SLOT(OnQkDelay(const QString&)));
		connect(ui.hkClock, SIGNAL(changed()), this, SLOT(OnShowClock()));
		connect(ui.chbQkClick, SIGNAL(stateChanged(int)), this, SLOT(OnQkClick(int)));
		connect(ui.chbClock, SIGNAL(stateChanged(int)), this, SLOT(OnClock(int)));
		connect(ui.chbWndActive, SIGNAL(stateChanged(int)), this, SLOT(OnWndActive(int)));
		connect(ui.cmbMode, SIGNAL(activated(int)), this, SLOT(OnCmbMode(int)));
		connect(ui.bnWndActive, SIGNAL(clicked()), this, SLOT(OnBnWndActive()));
	}

private slots:

	void OnQkClickKey()
	{
		func->quickClick.key = ui.hkQkClick->virtualKey();
		SaveJson();
	}

	void OnQkDelay(const QString& text)
	{
		func->quickClick.delay = text.toInt();
		SaveJson();
	}

	void OnShowClock()
	{
		func->showClock.key = ui.hkClock->virtualKey();
		SaveJson();
	}

	void OnQkClick(int state)
	{
		func->quickClick.state = state;
		SaveJson();
	}

	void OnClock(int state)
	{
		func->showClock.state = state;
		SaveJson();
	}

	void OnCmbMode(int row)
	{
		func->quickClick.mode = row;
		SaveJson();
	}

	void OnWndActive(int state)
	{
		func->wndActive.state = state;
		SaveJson();
	}

	void OnBnWndActive()
	{
		parentWidget()->hide();
		TipsWindow::Show(L"按回车开始，切换到对应窗口并再按回车", RGB(0x20, 0xFF, 0x20));
		while (!Input::state(VK_RETURN)) sleep(10);
		while (Input::state(VK_RETURN)) sleep(10);

		while (!Input::state(VK_RETURN))
		{
			func->wndActive.wnd = GetForegroundWindow();
			func->wndActive.name = Window::text(func->wndActive.wnd);
			TipsWindow::Show(func->wndActive.name, RGB(0xCC, 0xEE, 0xFF));
			sleep(50);
		}
		TipsWindow::Popup(func->wndActive.name, RGB(0x20, 0xFF, 0x20));
		ui.etWndActive->setText(QString::fromWCharArray(func->wndActive.name.c_str()));
		parentWidget()->show();

		SaveJson();
	}
};