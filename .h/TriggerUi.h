#pragma once
#include "ui_TriggerUi.h"
#include "../static.h"

class TriggerUi : public QWidget
{
	Q_OBJECT

public:

	TriggerUi(QWidget* parent) : QWidget(parent)
	{
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		WidInit();
		WidEvent();
		LockControl(1);
		TbUpdate();
	}

private:

	const int32 countMax = 9999;

	Ui::TriggerUiClass ui;
	List<Script>& scripts = Global::qi.scripts;

	void WidInit()
	{
		ui.hkTr->Mode(2);
		ui.cmbMode->addItem(u8"切换");
		ui.cmbMode->addItem(u8"按下");
		ui.cmbMode->addItem(u8"松开");
		ui.etCount->setValidator(new QIntValidator(0, countMax, this));

		//Table
		{
			ui.tbItem->setColumnCount(4);
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"名称"));
				ui.tbItem->setHorizontalHeaderItem(0, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"按键"));
				ui.tbItem->setHorizontalHeaderItem(1, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"模式"));
				ui.tbItem->setHorizontalHeaderItem(2, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"状态"));
				ui.tbItem->setHorizontalHeaderItem(3, tbi);
			}

			{
				ui.tbItem->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
				ui.tbItem->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
				ui.tbItem->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Stretch);
				ui.tbItem->setColumnWidth(1, 120);
				ui.tbItem->setColumnWidth(2, 80);
				ui.tbItem->setColumnWidth(3, 60);
			}
		}
	}

	void WidEvent()
	{
		connect(ui.tbItem, SIGNAL(cellClicked(int, int)), this, SLOT(OnTbClicked(int, int)));
		connect(ui.chbBlock, SIGNAL(clicked()), this, SLOT(OnChbBlock()));
		connect(ui.cmbMode, SIGNAL(activated(int)), this, SLOT(OnCmbMode(int)));
		connect(ui.hkTr, SIGNAL(changed()), this, SLOT(OnKeyChanged()));
		connect(ui.etCount, SIGNAL(textEdited(const QString&)), this, SLOT(OnEtCount(const QString&)));
	}

	void ResetControl()
	{
		ui.chbBlock->setChecked(0);
		ui.hkTr->VirtualKey(0);
		ui.etCount->setText("");
	}

	void TbUpdate()
	{
		ui.tbItem->clearMask();
		ui.tbItem->setRowCount(scripts.len());
		ui.tbItem->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.tbItem->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		ui.tbItem->verticalHeader()->setDefaultSectionSize(0);

		for (uint32 u = 0; u < scripts.len(); u++) {
			ui.tbItem->setItem(u, 0, new QTableWidgetItem(QString::fromWCharArray(scripts[u].name.c_str())));
			ui.tbItem->item(u, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//
			QString qs = u8"----";
			if ((scripts[u].key & 0xFFFF) != 0)
			{
				qs = QString::fromWCharArray(Input::Name(scripts[u].key & 0xFFFF));
				if ((scripts[u].key >> 16) != 0)
				{
					qs += u8" + ";
					qs += QString::fromWCharArray(Input::Name(scripts[u].key >> 16));
				}
			}
			ui.tbItem->setItem(u, 1, new QTableWidgetItem(qs));
			ui.tbItem->item(u, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//
			qs = "";
			switch (scripts[u].mode)
			{
			case Script::sw:
				qs = u8"切换";
				break;
			case Script::down:
				qs = u8"按下(";
				if (scripts[u].a) qs += QString::number(scripts[u].a);
				else qs += u8"无限";
				qs += u8")";
				break;
			case Script::up:
				qs = u8"松开(";
				if (scripts[u].a) qs += QString::number(scripts[u].a);
				else qs += u8"无限";
				qs += u8")";
				break;
			}
			ui.tbItem->setItem(u, 2, new QTableWidgetItem(qs));
			ui.tbItem->item(u, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			//
			if (scripts[u].state) qs = UI::trOn;
			else qs = UI::trOff;
			ui.tbItem->setItem(u, 3, new QTableWidgetItem(qs));
			ui.tbItem->item(u, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}
	}

	void LockControl(bool state)
	{
		ui.chbBlock->setDisabled(state);
		ui.cmbMode->setDisabled(state);
		ui.hkTr->setDisabled(state);
		if (state) ui.etCount->setDisabled(state);
	}

private slots:

	void showEvent(QShowEvent*)
	{
		ui.tbItem->setCurrentItem(0);
		TbUpdate();
		ResetControl();
		LockControl(1);
	}

	void OnTbClicked(int row, int column)
	{
		ResetControl();
		LockControl(1);
		if (row < 0) return;

		if (column == 3) scripts[row].state ? scripts[row].state = 0 : scripts[row].state = 1;
		ui.chbBlock->setChecked(scripts[row].block);
		ui.cmbMode->setCurrentIndex(scripts[row].mode);
		ui.hkTr->VirtualKey(scripts[row].key & 0xFFFF, scripts[row].key >> 16);
		if (scripts[row].mode >= Script::down)
		{
			ui.etCount->setText(QString::number(scripts[row].a));
			ui.etCount->setDisabled(0);
		}

		LockControl(0);
		TbUpdate();
	}

	void OnKeyChanged()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		scripts[pos].key = ui.hkTr->virtualKey();

		TbUpdate();
		SaveScript(scripts[pos]);
	}

	void OnChbBlock()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		scripts[pos].block = ui.chbBlock->isChecked();

		TbUpdate();
		SaveScript(scripts[pos]);
	}

	void OnCmbMode(int index)
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		switch (index)
		{
		case Script::sw:
			ui.etCount->setText(u8"无限");
			ui.etCount->setDisabled(1);
			scripts[pos].a = 0;
			scripts[pos].mode = Script::sw;
			break;
		case Script::down:
			ui.etCount->setText("");
			ui.etCount->setDisabled(0);
			scripts[pos].a = 1;
			scripts[pos].mode = Script::down;
			break;
		case Script::up:
			ui.etCount->setText("");
			ui.etCount->setDisabled(0);
			scripts[pos].a = 1;
			scripts[pos].mode = Script::up;
			break;
		}

		TbUpdate();
		SaveScript(scripts[pos]);
	}

	void OnEtCount(const QString& count)
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		int i = count.toInt();
		if (i > countMax) i = countMax;
		scripts[pos].a = i;

		TbUpdate();
		SaveScript(scripts[pos]);
	}
};