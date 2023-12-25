#pragma once
#include <QtWidgets/qwidget.h>
#include <qlabel.h>
#include <qevent.h>
#include <qpainter.h>
#include <qdesktopwidget.h>
#include <qapplication.h>

class QPointView : public QWidget
{
	Q_OBJECT;

public:

	QPointView(QWidget* parent = 0) : QWidget(parent)
	{
		setAttribute(Qt::WA_TransparentForMouseEvents);
		setWindowFlags(Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint);
		setAttribute(Qt::WA_TranslucentBackground);
	}

	void Point(QPoint pt) { this->pt = pt; }
	void Size(quint32 size) { this->size = size; }
	void Weight(quint32 weight) { this->weight = weight; }
	void Color(QColor color) { this->color = color; }

	void Show(QPoint pt = QPoint(), const quint32 size = 20, const quint32 weight = 2, QColor color = QColor(255, 0, 0))
	{
		screen = QApplication::desktop()->screenGeometry();
		setMinimumWidth(screen.width());
		setMinimumHeight(screen.height());
		setMaximumWidth(screen.width());
		setMaximumHeight(screen.height());
		move(0, 0);
		Point(pt), Size(size), Weight(weight), Color(color);
		show();
	}

private:
	QRect screen;
	QPoint pt;
	QColor color = QColor(255, 0, 0);
	quint32 weight = 2;
	quint32 size = 20;

protected:
	void paintEvent(QPaintEvent*) {
		QPainter ptr(this);
		QPen pen(color);
		pen.setWidth(weight);
		ptr.setPen(pen);
		ptr.drawLine(QPoint(pt.x() - size, pt.y()), QPoint(pt.x() + size, pt.y()));
		ptr.drawLine(QPoint(pt.x(), pt.y() - size), QPoint(pt.x(), pt.y() + size));
	}
};