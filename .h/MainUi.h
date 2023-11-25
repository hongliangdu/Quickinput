#pragma once
#pragma execution_character_set("utf-8")
#include <qsystemtrayicon.h>
#include "ui_MainUi.h"
#include "MacroUi.h"
#include "TriggerUi.h"
#include "FuncUi.h"
#include "SettingsUi.h"
#include "AboutUi.h"
#include "QuickInputDef.h"

class MainUi : public QMainWindow
{
	Q_OBJECT

public:

	MainUi(QWidget* parent = 0, QuickInputStruct* qis = 0) : QMainWindow(parent)
	{
		this->qis = qis;
		ui.setupUi(this);

		{
			tray = new QSystemTrayIcon(this);
			tray->setIcon(QIcon(":/icon.png"));
			tray->show();

			wm = new MacroUi(this, qis);
			wt = new TriggerUi(this, qis);
			wf = new FuncUi(this, qis);
			ws = new SettingsUi(this, qis);
			wa = new AboutUi(this);
		}

		{
			connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(OnTrayClick(QSystemTrayIcon::ActivationReason)));
			connect(ui.bnScript, SIGNAL(clicked()), this, SLOT(OnBnScript()));
			connect(ui.bnTrigger, SIGNAL(clicked()), this, SLOT(OnBnTrigger()));
			connect(ui.bnFunc, SIGNAL(clicked()), this, SLOT(OnBnFunc()));
			connect(ui.bnSettings, SIGNAL(clicked()), this, SLOT(OnBnSettings()));
			connect(ui.bnAbout, SIGNAL(clicked()), this, SLOT(OnBnAbout()));
		}

		ShowWnd(wm);
		FontResize();
	}

private:

	Ui::MainUiClass ui;
	QSystemTrayIcon* tray = 0;

	QWidget* wm = 0;
	QWidget* wt = 0;
	QWidget* wf = 0;
	QWidget* ws = 0;
	QWidget* wa = 0;

	QuickInputStruct* qis = 0;
	QString styleOn = "QPushButton{background-color:#CCEEFF;padding:10px;border:none;}";
	QString styleOff = "QPushButton{background-color:#AADDEE;padding:10px;border:none;}QPushButton:hover{background-color:#C0E2F2;}";

	void FontResize()
	{
		ui.bnScript->setFont(UI::font4);
		ui.bnTrigger->setFont(UI::font4);
		ui.bnFunc->setFont(UI::font4);
		ui.bnSettings->setFont(UI::font4);
		ui.bnAbout->setFont(UI::font4);
	}

	void ShowWnd(QWidget* wnd)
	{
		if (!wnd) return;
		if (wm) wm->hide();
		if (wt) wt->hide();
		if (wf) wf->hide();
		if (ws) ws->hide();
		if (wa) wa->hide();

		ui.bnScript->setStyleSheet(styleOff);
		ui.bnTrigger->setStyleSheet(styleOff);
		ui.bnFunc->setStyleSheet(styleOff);
		ui.bnSettings->setStyleSheet(styleOff);
		ui.bnAbout->setStyleSheet(styleOff);

		if (wnd == wm) ui.bnScript->setStyleSheet(styleOn);
		else if (wnd == wt) ui.bnTrigger->setStyleSheet(styleOn);
		else if (wnd == wf) ui.bnFunc->setStyleSheet(styleOn);
		else if (wnd == ws) ui.bnSettings->setStyleSheet(styleOn);
		else if (wnd == wa) ui.bnAbout->setStyleSheet(styleOn);

		wnd->show();
	}

private slots:
	void changeEvent(QEvent* et)
	{
		if (et->type() == QEvent::WindowStateChange)
		{
			if (windowState() == Qt::WindowNoState)
			{
				qis->state = 0;
				qis->HookState(0);
			}
			else if (windowState() == Qt::WindowMinimized)
			{
				qis->HookState(1);
				if (qis->set.minMode) hide();
			}
		}
	}

	void OnTrayClick(QSystemTrayIcon::ActivationReason reason)
	{
		if (reason == QSystemTrayIcon::Trigger)
		{
			qis->state = 0;
			qis->HookState(0);
			setWindowState(Qt::WindowNoState);
			show();
		}
	}

	void OnBnScript()
	{
		ShowWnd(wm);
	}
	void OnBnTrigger()
	{
		ShowWnd(wt);
	}
	void OnBnFunc()
	{
		ShowWnd(wf);
	}
	void OnBnSettings()
	{
		ShowWnd(ws);
	}
	void OnBnAbout()
	{
		ShowWnd(wa);
	}
};