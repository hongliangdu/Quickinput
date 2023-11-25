#pragma once
#pragma execution_character_set("utf-8")
#include "ui_AboutUi.h"
#include "MoreUi.h"

class AboutUi : public QWidget
{
	Q_OBJECT

public:

	AboutUi(QWidget* parent = 0) : QWidget(parent)
	{
		this->main = parent;
		ui.setupUi(this);
		setParent(parent);
		setWindowFlags(Qt::FramelessWindowHint);
		move(0, 40);

		ControlEvent();
		TextResize();
	}

private:

	Ui::AboutUiClass ui;
	QWidget* main;
	bool moreState = 0;

	void TextResize()
	{
		ui.lbText->setFont(UI::font4);
	}

	void ControlEvent()
	{
		ui.lbText->installEventFilter(this);
	}

private slots:

	bool eventFilter(QObject* obj, QEvent* et)
	{

		if (obj == ui.lbText && et->type() == QEvent::MouseButtonRelease)
		{
			if (!moreState)
			{
				moreState = 1;
				MoreUi* more = new MoreUi(main, &moreState);
				more->show();
			}

			return 1;
		}
		return 0;
	}
};