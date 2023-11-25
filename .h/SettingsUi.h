#pragma once
#pragma execution_character_set("utf-8")
#include "ui_SettingsUi.h"
#include "QuickInputDef.h"

class SettingsUi : public QWidget
{
	Q_OBJECT

public:

	SettingsUi(QWidget* parent = 0, QuickInputStruct* qis = 0) : QWidget(parent)
	{
		this->qis = qis;

		ui.setupUi(this);
		setParent(parent);
		setWindowFlags(Qt::FramelessWindowHint);
		move(0, 40);

		ControlInit();
		ControlEvent();
		FontResize();
	}

private:

	Ui::SettingsUiClass ui;

	QuickInputStruct* qis;
	CG::Task task;

	void FontResize()
	{
		ui.lbSettings->setFont(UI::font3);
		ui.lbSwitchKey->setFont(UI::font2);
		ui.lbShowTips->setFont(UI::font2);
		ui.lbMinMode->setFont(UI::font2);
		ui.lbStart->setFont(UI::font2);
		ui.lbZoom->setFont(UI::font2);
		ui.hkSwitchKey->setFont(UI::font1);
	}

	void ControlInit()
	{
		ui.hkSwitchKey->Mode(2);
		ui.hkSwitchKey->VirtualKey(qis->set.key);
		ui.hkSwitchKey->VirtualKey(qis->set.key & 0xFFFF, qis->set.key >> 16);

		ui.chbShowTips->setChecked(qis->set.showTips);
		ui.chbMinMode->setChecked(qis->set.minMode);
		ui.chbStart->setChecked(task.Find(L"QuickInput"));
		ui.chbZoom->setChecked(qis->set.wndZoom);
	}

	void ControlEvent()
	{
		connect(ui.hkSwitchKey, SIGNAL(changed()), this, SLOT(OnHkChanged()));
		connect(ui.chbShowTips, SIGNAL(clicked()), this, SLOT(OnShowTips()));
		connect(ui.chbMinMode, SIGNAL(clicked()), this, SLOT(OnMinMode()));
		connect(ui.chbStart, SIGNAL(clicked()), this, SLOT(OnStart()));
		connect(ui.chbZoom, SIGNAL(clicked()), this, SLOT(OnZoom()));
	}

private slots:

	void OnHkChanged()
	{
		qis->set.key = ui.hkSwitchKey->virtualKey();
		SaveJson(qis);
	}

	void OnShowTips()
	{
		qis->set.showTips = ui.chbShowTips->isChecked();
		SaveJson(qis);
	}

	void OnMinMode()
	{
		qis->set.minMode = ui.chbMinMode->isChecked();
		SaveJson(qis);
	}

	void OnStart()
	{
		if (task.Find(L"QuickInput"))
		{
			if (!task.Delete(L"QuickInput"))
			{
				MsgBox::Error(task.getLog().c_str(), L"错误");
			}
		}
		else
		{
			if (!task.Register(L"QuickInput"))
			{
				MsgBox::Error(task.getLog().c_str(), L"错误");
			}
		}
		task.Emplog();
	}

	void OnZoom()
	{
		qis->set.wndZoom = ui.chbZoom->isChecked();
		SaveJson(qis);
	}
};