#pragma once

#include <windows.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <QtWidgets/qdialog.h>
#include <qevent.h>
#include <qpainter.h>
#include <qbitmap.h>

class QColorBox : public QWidget
{
	Q_OBJECT
public:
	QColorBox(QWidget* parent = 0) : QWidget(parent) {}
	void setColor(QColor color) { this->color = color; repaint(); }
private:
	QColor color;
protected:
	void paintEvent(QPaintEvent*) { QPainter pa(this); pa.fillRect(this->rect(), color); }
};

class QColorSelection : public QDialog
{
	Q_OBJECT;

public:

	QColorSelection(QWidget* parent = 0) : QDialog(parent)
	{
		setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
		setAttribute(Qt::WA_TranslucentBackground);
		setCursor(Qt::CrossCursor);
		colorBox.setParent(this);
		colorBox.setMinimumWidth(20);
		colorBox.setMinimumHeight(20);
		colorBox.setMaximumWidth(20);
		colorBox.setMaximumHeight(20);
	}

	QColor Start()
	{
		screen = QApplication::desktop()->screenGeometry();
		setMinimumWidth(screen.width());
		setMinimumHeight(screen.height());
		setMaximumWidth(screen.width());
		setMaximumHeight(screen.height());
		move(0, 0);

		hdc = GetDC(0);
		colorBox.hide();
		grabMouse();
		exec();
		return color;
	}

private:
	QColorBox colorBox;
	QColor color;
	QRect screen;
	HDC hdc;
	COLORREF rgb;

	void SetColor(QMouseEvent* et)
	{
		QPoint ms = et->globalPos();
		QPoint boxPos;
		POINT pt;
		GetCursorPos(&pt);
		rgb = GetPixel(hdc, pt.x, pt.y);

		if (ms.x() > screen.width() - 80) boxPos.setX(ms.x() - 30);
		else boxPos.setX(ms.x() + 30);
		if (ms.y() > screen.height() - 80) boxPos.setY(ms.y() - 30);
		else boxPos.setY(ms.y() + 30);

		color = qRgb(GetRValue(rgb), GetGValue(rgb), GetBValue(rgb));
		colorBox.setColor(color);
		colorBox.move(boxPos);
		colorBox.show();
	}

protected:
	void paintEvent(QPaintEvent*) { QPainter pa(this); pa.fillRect(this->rect(), QColor(127, 127, 127, 1)); }

	void mousePressEvent(QMouseEvent* et) { SetColor(et); }

	void mouseReleaseEvent(QMouseEvent* et) { releaseMouse(); close(); }

	void mouseMoveEvent(QMouseEvent* et) { SetColor(et); }
};