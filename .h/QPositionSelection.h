#pragma once

#include <windows.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <QtWidgets/qdialog.h>
#include <qlabel.h>
#include <qevent.h>
#include <qpainter.h>
#include <qbitmap.h>

class QPositionSelection : public QDialog
{
	Q_OBJECT;

public:

	QPositionSelection(QWidget* parent = 0) : QDialog(parent)
	{
		setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
		setAttribute(Qt::WA_TranslucentBackground);
		setCursor(Qt::CrossCursor);
		lb.setParent(this);
		lb.setMinimumWidth(150);
		lb.setMinimumHeight(30);
		lb.setMaximumWidth(150);
		lb.setMaximumHeight(30);
		lb.setFont(QFont("Microsoft YaHei", 15, 700));
		lb.setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		lb.setStyleSheet("background-color:#000;color:#FFF;");
		lb.hide();
	}

	void Start()
	{
		screen = QApplication::desktop()->screenGeometry();
		setMinimumWidth(screen.width());
		setMinimumHeight(screen.height());
		setMaximumWidth(screen.width());
		setMaximumHeight(screen.height());
		move(0, 0);

		grabMouse();
		exec();
	}

	QPoint relPos()
	{
		return ms;
	}

	POINT absPos()
	{
		return msRel;
	}

private:
	QLabel lb;
	QRect screen;
	QPoint ms;
	POINT msRel;

	void SetText(QMouseEvent* et)
	{
		QPoint lbPos;

		GetCursorPos(&msRel);
		ms = et->globalPos();
		lb.setText(QString::number(msRel.x) + " - " + QString::number(msRel.y));

		if (ms.x() > screen.width() - 210) lbPos.setX(ms.x() - 180);
		else lbPos.setX(ms.x() + 30);
		if (ms.y() > screen.height() - 60) lbPos.setY(ms.y() - 60);
		else lbPos.setY(ms.y() + 30);

		lb.move(lbPos);
		lb.show();
	}

protected:
	void paintEvent(QPaintEvent*) { QPainter pa(this); pa.fillRect(this->rect(), QColor(127, 127, 127, 1)); }

	void mousePressEvent(QMouseEvent* et) { SetText(et); }

	void mouseReleaseEvent(QMouseEvent* et) { releaseMouse(); close(); }

	void mouseMoveEvent(QMouseEvent* et) { SetText(et); }
};