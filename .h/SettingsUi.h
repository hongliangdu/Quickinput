#pragma once
#include "ui_SettingsUi.h"
#include "../static.h"

class SettingsUi : public QWidget
{
	Q_OBJECT

public:
	SettingsUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		WidInit();
		WidEvent();
	}

private:
	Ui::SettingsUiClass ui;
	SettingsData* sets = &Global::qi.set;

	void WidInit()
	{
		ui.hkKey->Mode(2);
		ui.hkRec->Mode(0);
		ui.hkKey->VirtualKey(sets->key & 0xFFFF, sets->key >> 16);
		ui.hkRec->VirtualKey(sets->recKey);

		ui.chbShowTips->setChecked(sets->showTips);
		ui.chbAudFx->setChecked(sets->audFx);
		ui.chbMinMode->setChecked(sets->minMode);
		ui.chbStart->setChecked(Task::Find(L"QuickInput").result);
		ui.chbZoom->setChecked(sets->wndZoom);
	}

	void WidEvent()
	{
		connect(ui.hkKey, SIGNAL(changed()), this, SLOT(OnHkKey()));
		connect(ui.hkRec, SIGNAL(changed()), this, SLOT(OnHkRec()));
		connect(ui.chbShowTips, SIGNAL(clicked()), this, SLOT(OnShowTips()));
		connect(ui.chbAudFx, SIGNAL(clicked()), this, SLOT(OnAud()));
		connect(ui.chbMinMode, SIGNAL(clicked()), this, SLOT(OnMinMode()));
		connect(ui.chbStart, SIGNAL(clicked()), this, SLOT(OnStart()));
		connect(ui.chbZoom, SIGNAL(clicked()), this, SLOT(OnZoom()));
	}

private slots:

	void OnHkKey()
	{
		sets->key = ui.hkKey->virtualKey();
		SaveJson();
	}

	void OnHkRec()
	{
		sets->recKey = ui.hkRec->virtualKey();
		SaveJson();
	}

	void OnAud()
	{
		sets->audFx = ui.chbAudFx->isChecked();
		SaveJson();
	}

	void OnShowTips()
	{
		sets->showTips = ui.chbShowTips->isChecked();
		SaveJson();
	}

	void OnMinMode()
	{
		sets->minMode = ui.chbMinMode->isChecked();
		SaveJson();
	}

	void OnStart()
	{
		if (Task::Find(L"QuickInput").result)
		{
			Task::TaskStruct ts = Task::Delete(L"QuickInput");
			if (!ts.result)
			{
				MsgBox::Error(ts.log.c_str(), L"错误");
			}
		}
		else
		{
			Task::TaskStruct ts = Task::Register(L"QuickInput");
			if (!ts.result)
			{
				MsgBox::Error(ts.log.c_str(), L"错误");
			}
		}
	}

	void OnZoom()
	{
		sets->wndZoom = ui.chbZoom->isChecked();
		SaveJson();
	}
};