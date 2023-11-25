#pragma once
#pragma execution_character_set("utf-8")
#include <qdesktopservices.h>
#include <qurl.h>
#include "ui_MoreUi.h"
#include "QuickInputDef.h"

class MoreUi : public QDialog
{
	Q_OBJECT

public:

	MoreUi(QWidget* parent = 0, bool* pThis = 0) : QDialog(parent)
	{
		this->state = pThis;
		ui.setupUi(this);
		setParent(0);
		setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
		FontResize();
	}

private:

	Ui::MoreUiClass ui;
	QLabel* bg;
	bool* state = 0;

	void FontResize()
	{
		ui.lbHowUse->setFont(UI::font3);
		ui.lbLog->setFont(UI::font3);
		ui.lbHowUseText->setFont(UI::font1);
		ui.lbLogText->setFont(UI::font1);
	}

private slots:

	void closeEvent(QCloseEvent*)
	{
		*state = 0;
	}
};