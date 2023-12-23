#pragma once
#include <qcolordialog.h>
#include <qevent.h>
#include <qtimer.h>
#include <qbuttongroup.h>
#include "QColorSelection.h"
#include "QPositionSelection.h"
#include "QSelectionBox.h"
#include "ui_EditUi.h"
#include "../static.h"

class EditUi: public QDialog
{
	Q_OBJECT
public:
	EditUi(List<QiItem>& items) : QDialog()
	{
		this->items = &items;

		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);
		setAttribute(Qt::WA_TranslucentBackground);

		ui.bnColorEdit->setDisabled(1);
		ui.bnLoopEdit->setDisabled(1);

		WidInit();
		WidEvent();
	}

private:
	const int32 posMin = -10000;
	const int32 posMax = 10000;
	const int32 moveRandMax = 9999;
	const int32 delayMax = 999999;
	const int32 delayRandMax = 9999;
	const int32 loopCountMax = 9999;
	const int32 colorMax = 255;

	Ui::EditUiClass ui;
	List<QiItem>* items = 0;
	QPoint msPos;
	bool changing = 0;
	bool editing = 0;
	bool posCtrl = 0;

	void WidInit()
	{
		ui.hkKey->Mode(0);
		ui.hkKey->VirtualKey(VK_LBUTTON);
		ui.bnColorValue->setStyleSheet(u8"background:qlineargradient(x1:0,y1:0,x2:1,y2:1,stop:0 #FFF,stop:0.5 #F8F,stop:1 #F00);border:none;width:20px;height:20px");
		ui.bnLoopEdit->setText(UI::etEdit);
		ui.bnColorEdit->setText(UI::etEdit);
		ui.bnDel->setText(UI::etDel);

		//RadioButton
		{
			QButtonGroup* keyRbs = new QButtonGroup(this);
			QButtonGroup* moveRbs = new QButtonGroup(this);
			QButtonGroup* colorRbs = new QButtonGroup(this);
			keyRbs->addButton(ui.rbDown);
			keyRbs->addButton(ui.rbUp);
			keyRbs->addButton(ui.rbClick);
			moveRbs->addButton(ui.rbPos);
			moveRbs->addButton(ui.rbMove);
			colorRbs->addButton(ui.rbColorGet);
			colorRbs->addButton(ui.rbColorNot);
			ui.rbDown->setChecked(1);
			ui.rbPos->setChecked(1);
			ui.rbColorGet->setChecked(1);
		}

		//Value
		{
			ui.etX->setValidator(new QIntValidator(0, posMax, this));
			ui.etY->setValidator(new QIntValidator(0, posMax, this));
			ui.etMoveRand->setValidator(new QIntValidator(0, moveRandMax, this));
			ui.etTime->setValidator(new QIntValidator(0, delayMax, this));
			ui.etDelayRand->setValidator(new QIntValidator(0, delayRandMax, this));
			ui.etCount->setValidator(new QIntValidator(0, loopCountMax, this));
			ui.etR->setValidator(new QIntValidator(0, colorMax, this));
			ui.etG->setValidator(new QIntValidator(0, colorMax, this));
			ui.etB->setValidator(new QIntValidator(0, colorMax, this));
			ui.etCX->setValidator(new QIntValidator(0, colorMax, this));
			ui.etColorL->setValidator(new QIntValidator(0, posMax, this));
			ui.etColorT->setValidator(new QIntValidator(0, posMax, this));
			ui.etColorR->setValidator(new QIntValidator(0, posMax, this));
			ui.etColorB->setValidator(new QIntValidator(0, posMax, this));
		}

		//Table
		{
			ui.tbItem->setColumnCount(3);
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"功能"));
				ui.tbItem->setHorizontalHeaderItem(0, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"参数"));
				ui.tbItem->setHorizontalHeaderItem(1, tbi);
			}
			{
				QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"备注"));
				ui.tbItem->setHorizontalHeaderItem(2, tbi);
			}
			ui.tbItem->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
			ui.tbItem->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
			ui.tbItem->verticalHeader()->setDefaultSectionSize(0);
			ui.tbItem->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeMode::Fixed);
			ui.tbItem->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeMode::Interactive);
			ui.tbItem->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeMode::Stretch);
			ui.tbItem->setColumnWidth(0, 100);
			ui.tbItem->setColumnWidth(1, 300);
		}

	}

	void WidEvent()
	{
		ui.tbItem->installEventFilter(this);
		ui.tbItem->viewport()->installEventFilter(this);

		connect(ui.tbItem, SIGNAL(cellChanged(int, int)), this, SLOT(OnTbChanged(int, int)));
		connect(ui.tbItem, SIGNAL(cellClicked(int, int)), this, SLOT(OnTbClicked(int, int)));
		connect(ui.tbItem, SIGNAL(cellDoubleClicked(int, int)), this, SLOT(OnTbDoubleClick(int, int)));
		connect(ui.bnClose, SIGNAL(clicked()), this, SLOT(OnBnClose()));
		connect(ui.bnKeyAdd, SIGNAL(clicked()), this, SLOT(OnBnKeyAdd()));
		connect(ui.bnMoveAdd, SIGNAL(clicked()), this, SLOT(OnBnMoveAdd()));
		connect(ui.bnDelayAdd, SIGNAL(clicked()), this, SLOT(OnBnDelayAdd()));
		connect(ui.bnLoopAdd, SIGNAL(clicked()), this, SLOT(OnBnLoopAdd()));
		connect(ui.bnLoopEdit, SIGNAL(clicked()), this, SLOT(OnBnLoopEdit()));
		connect(ui.bnColorAdd, SIGNAL(clicked()), this, SLOT(OnBnColorAdd()));
		connect(ui.bnColorEdit, SIGNAL(clicked()), this, SLOT(OnColorEdit()));
		connect(ui.bnTextAdd, SIGNAL(clicked()), this, SLOT(OnBnTextAdd()));
		connect(ui.bnDel, SIGNAL(clicked()), this, SLOT(OnBnDel()));
		connect(ui.bnPos, SIGNAL(clicked()), this, SLOT(OnBnPos()));
		connect(ui.bnColorRect, SIGNAL(clicked()), this, SLOT(OnBnColorRect()));
		connect(ui.bnColorValue, SIGNAL(clicked()), this, SLOT(OnBnColorValue()));
		connect(ui.bnEndAdd, SIGNAL(clicked()), this, SLOT(OnBnEndAdd()));
		connect(ui.bnEndLoopAdd, SIGNAL(clicked()), this, SLOT(OnBnEndLoopAdd()));
		connect(ui.rbPos, SIGNAL(toggled(bool)), this, SLOT(OnRbPos(bool)));
		connect(ui.rbMove, SIGNAL(toggled(bool)), this, SLOT(OnRbMove(bool)));
		connect(ui.rbColorGet, SIGNAL(toggled(bool)), this, SLOT(OnRbColor(bool)));
	}

	//------------------------Control Data

	void SetHkKey(BYTE key)
	{
		ui.hkKey->VirtualKey(key);
	}
	void SetEtPos(RECT pt)
	{
		ui.etX->setText(QString::number(pt.left));
		ui.etY->setText(QString::number(pt.top));
		if (pt.right > -1) ui.etMoveRand->setText(QString::number(pt.right));
	}
	void SetEtDelay(POINT pt)
	{
		ui.etTime->setText(QString::number(pt.x));
		ui.etDelayRand->setText(QString::number(pt.y));
	}
	void SetRbColorMode(bool mode) { if (mode) ui.rbColorNot->setChecked(1); else ui.rbColorGet->setChecked(1); }
	void SetChbColorMove(bool state) {  ui.chbColorMove->setChecked(state); }
	void SetEtColorRect(RECT rect)
	{
		ui.etColorL->setText(QString::number(rect.left));
		ui.etColorT->setText(QString::number(rect.top));
		ui.etColorR->setText(QString::number(rect.right));
		ui.etColorB->setText(QString::number(rect.bottom));
	}
	void SetEtColorValue(COLORREF color)
	{
		ui.etR->setText(QString::number(GetRValue(color)));
		ui.etG->setText(QString::number(GetGValue(color)));
		ui.etB->setText(QString::number(GetBValue(color)));
		ui.etCX->setText(QString::number(GetAValue(color)));
	}
	void SetColor(COLORREF color) {
		ui.etR->setText(QString::number(GetRValue(color)));
		ui.etG->setText(QString::number(GetGValue(color)));
		ui.etB->setText(QString::number(GetBValue(color)));
	}
	void SetEtLoopCount(uint32 count) { ui.etCount->setText(QString::number(count)); }

	QiKey GetKey() {
		QiKey item;
		if (ui.rbDown->isChecked()) item.state = QiKey::down;
		else if (ui.rbUp->isChecked()) item.state = QiKey::up;
		else if (ui.rbClick->isChecked()) item.state = QiKey::click;
		item.vk =  ui.hkKey->virtualKey();
		return item;
	}
	QiMouse GetMouse() {
		QiMouse item;
		item.state = ui.rbMove->isChecked();
		int x = ui.etX->text().toInt();
		int y = ui.etY->text().toInt();
		int ex = ui.etMoveRand->text().toInt();
		if (x > posMax) x = posMax;
		if (y > posMax) y = posMax;
		if (ex > posMax) ex = posMax;
		if (x < posMin) x = posMin;
		if (y < posMin) y = posMin;
		item.x = x;
		item.y = y;
		item.ex = ex;
		return item;
	}
	QiDelay GetDelay() {
		QiDelay item;
		int ms = ui.etTime->text().toInt();
		int ex = ui.etDelayRand->text().toInt();
		if (ms > delayMax) ms = delayMax;
		if (ex > delayMax) ex = delayRandMax;
		item.ms = ms;
		item.ex = ex;
		return item;
	}
	QiText GetText() {
		QiText item;
		item.str = (LPCWSTR)ui.etText->toPlainText().utf16();
		return item;
	}
	QiColor GetColor() {
		QiColor item;
		item.state = ui.rbColorNot->isChecked();
		item.move = ui.chbColorMove->isChecked();
		{
			int l = ui.etColorL->text().toInt();
			int t = ui.etColorT->text().toInt();
			int r = ui.etColorR->text().toInt();
			int b = ui.etColorB->text().toInt();
			if (l > posMax) l = posMax;
			if (t > posMax) t = posMax;
			if (r > posMax) r = posMax;
			if (b > posMax) b = posMax;
			item.rect = { l, t, r, b };
		}
		{
			int r = ui.etR->text().toInt();
			int g = ui.etG->text().toInt();
			int b = ui.etB->text().toInt();
			int ex = 0;
			if (ui.etCX->text() == "") ex = 5;
			else ex = ui.etCX->text().toInt();
			if (r > colorMax) r = colorMax;
			if (g > colorMax) g = colorMax;
			if (b > colorMax) b = colorMax;
			if (ex > colorMax) ex = colorMax;
			COLORREF rgbe = RGBA(r, g, b, ex);
			item.rgbe = rgbe;
		}
		return item;
	}
	QiLoop GetLoop() {
		QiLoop item;
		int i = 0;
		if (ui.etCount->text() == "") i = 1;
		else i = ui.etCount->text().toInt();
		if (i > loopCountMax) i = loopCountMax;
		item.count = i;
		return item;
	}

	void LoadKey(const QiKey& item) {
		if (item.state == QiKey::down) ui.rbDown->setCheckable(1);
		else if (item.state == QiKey::up) ui.rbUp->setCheckable(1);
		else if (item.state == QiKey::click) ui.rbClick->setCheckable(1);
		SetHkKey(item.vk);
	}
	void LoadMouse(const QiMouse& item) {
		if (item.state == QiMouse::pos)
		{
			ui.rbPos->setChecked(1);
			OnRbPos(1);
		}
		else if (item.state == QiMouse::move)
		{
			ui.rbMove->setChecked(1);
			OnRbMove(1);
		}
		SetEtPos({ item.x, item.y, item.ex, 0 });
	}
	void LoadDelay(const QiDelay& item) { SetEtDelay({ item.ms, item.ex }); }
	void LoadText(const QiText& item) { ui.etText->setText(QString::fromWCharArray(item.str.c_str())); }
	void LoadColor(const QiColor& item) { SetRbColorMode(item.state); SetChbColorMove(item.move); SetEtColorRect(item.rect); SetEtColorValue(item.rgbe); }
	void LoadLoop(const QiLoop& item) { SetEtLoopCount(item.count); }

	//=================Control Data

	void SetChanging()
	{
		if (changing)
		{
			ui.bnKeyAdd->setText(UI::etChange);
			ui.bnMoveAdd->setText(UI::etChange);
			ui.bnDelayAdd->setText(UI::etChange);
			ui.bnLoopAdd->setText(UI::etChange);
			ui.bnTextAdd->setText(UI::etChange);
			ui.bnColorAdd->setText(UI::etChange);
			ui.bnEndAdd->setText(UI::etChange);
			ui.bnEndLoopAdd->setText(UI::etChange);
		}
		else
		{
			ui.bnKeyAdd->setText(UI::etAdd);
			ui.bnMoveAdd->setText(UI::etAdd);
			ui.bnDelayAdd->setText(UI::etAdd);
			ui.bnLoopAdd->setText(UI::etAdd);
			ui.bnTextAdd->setText(UI::etAdd);
			ui.bnColorAdd->setText(UI::etAdd);
			ui.bnEndAdd->setText(UI::etAdd);
			ui.bnEndLoopAdd->setText(UI::etAdd);
		}
	}

	void TbUpdate()
	{
		disconnect(ui.tbItem, SIGNAL(cellChanged(int, int)), this, SLOT(OnTbChanged(int, int)));
		ui.tbItem->clearMask();
		ui.tbItem->setRowCount(items->len());
		ui.tbItem->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.tbItem->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		ui.tbItem->verticalHeader()->setDefaultSectionSize(0);

		for (uint32 u = 0; u < items->len(); u++)
		{
			QString ps;
			switch (items[0][u].type())
			{
			case QiAction::end:
			{
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiEnd));
			}
				break;
			case QiAction::delay:
			{
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiWait));
				ps = QString::number(QiDelayPtr(items[0][u])->ms);
				ps += u8"ㅤㅤ";
				ps += QString::number(QiDelayPtr(items[0][u])->ex);
			}
				break;
			case QiAction::key:
			{
				if (QiKeyPtr(items[0][u])->state == QiKey::down)
					ui.	tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiDown));
				else if (QiKeyPtr(items[0][u])->state == QiKey::up)
					ui.	tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiUp));
				else if (QiKeyPtr(items[0][u])->state == QiKey::click)
					ui.	tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiClick));
				ps = QString::fromWCharArray(Input::Name(QiKeyPtr(items[0][u])->vk));
			}
				break;
			case QiAction::mouse:
			{
				if (QiMousePtr(items[0][u])->state == QiMouse::pos)
					ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiPos));
				else if (QiMousePtr(items[0][u])->state == QiMouse::move)
					ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiMove));
				ps = QString::number(QiMousePtr(items[0][u])->x);
				ps += u8" - ";
				ps += QString::number(QiMousePtr(items[0][u])->y);
				ps += u8"ㅤㅤ";
				ps += QString::number(QiMousePtr(items[0][u])->ex);
			}
				break;
			case QiAction::text:
			{
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiText));
				ps = QString::fromWCharArray(QiTextPtr(items[0][u])->str.substr(0, 32).c_str());
				if (QiTextPtr(items[0][u])->str.length() > 31) ps += u8"...";
			}
				break;
			case QiAction::color:
			{
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiColor));
				ps = u8"(";
				ps += QString::number(QiColorPtr(items[0][u])->rect.left);
				ps += u8",";
				ps += QString::number(QiColorPtr(items[0][u])->rect.top);
				ps += u8",";
				ps += QString::number(QiColorPtr(items[0][u])->rect.right);
				ps += u8",";
				ps += QString::number(QiColorPtr(items[0][u])->rect.bottom);
				ps += u8")　(";
				ps += QString::number(GetRValue(QiColorPtr(items[0][u])->rgbe));
				ps += u8",";
				ps += QString::number(GetGValue(QiColorPtr(items[0][u])->rgbe));
				ps += u8",";
				ps += QString::number(GetBValue(QiColorPtr(items[0][u])->rgbe));
				ps += u8",";
				ps += QString::number(GetAValue(QiColorPtr(items[0][u])->rgbe));
				if (QiColorPtr(items[0][u])->state == QiColor::find)
				{
					ps += u8")　找到";
					if (QiColorPtr(items[0][u])->move) ps += u8"并移动";
					else ps += u8"不移动";
				}
				else ps += u8")　未找到";
			}
				break;
			case QiAction::loop:
			{
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiLoop));
				if (QiLoopPtr(items[0][u])->count) ps = QString::number(QiLoopPtr(items[0][u])->count);
				else ps = u8"无限";
			}
				break;
			case QiAction::loopEnd:
			{
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiEndLoop));
			}
				break;
			}
			ui.tbItem->setItem(u, 1, new QTableWidgetItem(ps));
			ui.tbItem->setItem(u, 2, new QTableWidgetItem(QString::fromWCharArray(items[0][u].action()->mark.c_str())));
			ui.tbItem->item(u, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tbItem->item(u, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tbItem->item(u, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}
		connect(ui.tbItem, SIGNAL(cellChanged(int, int)), this, SLOT(OnTbChanged(int, int)));
	}

	void GetItem(int32 pos)
	{
		switch (items[0][pos].type())
		{
		case QiAction::delay:
			LoadDelay(*QiDelayPtr(items[0][pos]));
			break;
		case QiAction::key:
			LoadKey(*QiKeyPtr(items[0][pos]));
			break;
		case QiAction::mouse:
			LoadMouse(*QiMousePtr(items[0][pos]));
			break;
		case QiAction::text:
			LoadText(*QiTextPtr(items[0][pos]));
			break;
		case QiAction::color:
			LoadColor(*QiColorPtr(items[0][pos]));
			break;
		case QiAction::loop:
			LoadLoop(*QiLoopPtr(items[0][pos]));
			break;
		}
	}

	void SetItem(int32 type, int32 pos)
	{
		QiItem item;
		switch (type)
		{
		case QiAction::end: item.Set(QiAction::end); break;
		case QiAction::delay: item = GetDelay(); break;
		case QiAction::key: item = GetKey(); break;
		case QiAction::mouse: item = GetMouse(); break;
		case QiAction::text: item = GetText(); break;
		case QiAction::color: item = GetColor(); break;
		case QiAction::loop: item = GetLoop(); break;
		case QiAction::loopEnd: item.Set(QiAction::loopEnd); break;
		}
		if (items[0][pos].type() == QiAction::color)
		{
			if (type == QiAction::color) QiColorPtr(item)->next = QiColorPtr(items[0][pos])->next;
			if (type == QiAction::loop) QiLoopPtr(item)->next = QiColorPtr(items[0][pos])->next;
		}
		else if (items[0][pos].type() == QiAction::loop)
		{
			if (type == QiAction::color) QiColorPtr(item)->next = QiLoopPtr(items[0][pos])->next;
			if (type == QiAction::loop) QiLoopPtr(item)->next = QiLoopPtr(items[0][pos])->next;
		}
		items[0][pos] = item;
	}

	void AddItem(int32 type)
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) pos = items->len();
		else if ((pos + 1) <= items->len()) pos++;
		
		items->Ins(pos);
		SetItem(type, pos);

		TbUpdate();
		ui.tbItem->setCurrentItem(ui.tbItem->item(pos, 0));
	}

	void ChangeItem(int32 type)
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		SetItem(type, pos);

		TbUpdate();
		ui.tbItem->setCurrentItem(0);
		changing = 0;
		SetChanging();
	}

	void MoveItem(bool up)
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		if (up && ((pos - 1) >= 0))
		{
			items->Swp(pos, pos - 1);
			ui.tbItem->setCurrentItem(ui.tbItem->item(pos - 1, 0));
		}
		else if (!up && (pos + 1) < ui.tbItem->rowCount())
		{
			items->Swp(pos, pos + 1);
			ui.tbItem->setCurrentItem(ui.tbItem->item(pos + 1, 0));
		}
		TbUpdate();
	}

	void showEvent(QShowEvent*)
	{
		ui.lbTitle->setText(windowTitle());
		TbUpdate();
	}

	void mousePressEvent(QMouseEvent* et)
	{
		if (et->buttons() & Qt::LeftButton) msPos = et->pos();
	}

	void mouseMoveEvent(QMouseEvent* et)
	{
		if (et->buttons() & Qt::LeftButton) move(et->pos() + pos() - msPos);
	}

private slots:
	bool eventFilter(QObject* obj, QEvent* et)
	{
		if (obj == ui.tbItem)
		{
			if (et->type() == QEvent::KeyPress)
			{
				QKeyEvent* key = static_cast<QKeyEvent*>(et);
				if (key->modifiers() == Qt::ShiftModifier)
				{
					if (key->key() == Qt::Key_Backspace)
					{
						OnBnDel();
						return 1;
					}
					else if (key->key() == Qt::Key_Up)
					{
						MoveItem(1);
						return 1;
					}
					else if (key->key() == Qt::Key_Down)
					{
						MoveItem(0);
						return 1;
					}
				}
			}
		}
		if (obj == ui.tbItem->viewport())
		{
			if (et->type() == QEvent::Drop)
			{
				QDropEvent* drop = static_cast<QDropEvent*>(et);
				QTableWidgetItem* item = ui.tbItem->itemAt(drop->pos());
				int before = 0, after = 0;
				before = ui.tbItem->currentRow();
				if (before < 0) return 0;

				if (item) after = item->row();
				else after = ui.tbItem->rowCount() - 1;

				if (after < 0) return 0;

				items->Mov(before, after);
				TbUpdate();
				ui.tbItem->setCurrentItem(ui.tbItem->item(after, 0));
				return 1;
			}
		}
		return 0;
	}
	
	void OnTbChanged(int row, int column)
	{
		if (row < 0) return;
		if (column != 2) return;
		items[0][row].action()->mark = (LPCWSTR)(ui.tbItem->item(row, 2)->text().utf16());
	}

	void OnTbClicked(int row, int column)
	{
		ui.bnColorEdit->setDisabled(1);
		ui.bnLoopEdit->setDisabled(1);

		if (row < 0) return;
		if (column == 2)
		{
			ui.tbItem->editItem(ui.tbItem->item(row, column));
		}
		else
		{
			if (items[0][row].type() == QiAction::color) ui.bnColorEdit->setDisabled(0);
			else if (items[0][row].type() == QiAction::loop) ui.bnLoopEdit->setDisabled(0);
		}
	}

	void OnTbDoubleClick(int row, int column)
	{
		if (row < 0) return;

		GetItem(row);

		changing = 1;
		SetChanging();
	}

	void OnBnClose()
	{
		hide();
	}

	void OnBnKeyAdd()
	{
		if (changing) ChangeItem(QiAction::key);
		else AddItem(QiAction::key);
	}

	void OnBnMoveAdd()
	{
		if (changing) ChangeItem(QiAction::mouse);
		else AddItem(QiAction::mouse);
	}

	void OnBnDelayAdd()
	{
		if (changing) ChangeItem(QiAction::delay);
		else AddItem(QiAction::delay);
	}

	void OnBnLoopAdd()
	{
		if (changing) ChangeItem(QiAction::loop);
		else AddItem(QiAction::loop);
	}

	void OnBnLoopEdit()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		if (items[0][pos].type() != QiAction::loop) return;
		EditUi edit(QiLoopPtr(items[0][pos])->next);
		edit.setWindowTitle(u8"编辑 - 循环");
		edit.exec();
	}

	void OnBnTextAdd()
	{
		if (changing) ChangeItem(QiAction::text);
		else AddItem(QiAction::text);
	}

	void OnBnColorAdd()
	{
		if (changing) ChangeItem(QiAction::color);
		else AddItem(QiAction::color);
	}

	void OnColorEdit()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		if (items[0][pos].type() != QiAction::color) return;
		EditUi edit(QiColorPtr(items[0][pos])->next);
		edit.setWindowTitle(u8"编辑 - 查找颜色");
		edit.exec();
	}

	void OnBnEndAdd()
	{
		if (changing) ChangeItem(QiAction::end);
		else AddItem(QiAction::end);
	}

	void OnBnEndLoopAdd()
	{
		if (changing) ChangeItem(QiAction::loopEnd);
		else AddItem(QiAction::loopEnd);
	}

	void OnBnDel()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;
		items->Del(pos);
		TbUpdate();
		if ((pos - 1) >= 0) ui.tbItem->setCurrentItem(ui.tbItem->item(pos - 1, 0));

		changing = 0;
		SetChanging();
	}

	void OnBnPos()
	{
		QPositionSelection ps;
		POINT pt;
		ps.Start();
		pt = ps.absPos();
		pt = RelToAbs({ pt.x, pt.y });
		SetEtPos({ pt.x, pt.y, -1 });
	}

	void OnBnColorRect()
	{
		QSelectionBox sb;
		sb.setBoxStyleSheet(u8"background-color:rgba(240,192,224,0.5);border:1px solid #F00;");
		sb.Start();
		RECT rect = sb.absRect();
		POINT lt = RelToAbs({ rect.left, rect.top });
		POINT rb = RelToAbs({ rect.right, rect.bottom });
		RECT r = { lt.x, lt.y, rb.x, rb.y };
		SetEtColorRect({ r.left, r.top, r.right, r.bottom });
	}

	void OnBnColorValue()
	{
		QColorSelection cs;
		QColorDialog cd(cs.Start(), this);
		cd.exec();
		
		SetColor(RGB(cd.currentColor().red(), cd.currentColor().green(), cd.currentColor().blue()));

		QString style = u8"background-color:rgb(";
		style += QString::number(cd.currentColor().red());
		style += u8",";
		style += QString::number(cd.currentColor().green());
		style += u8",";
		style += QString::number(cd.currentColor().blue());
		style += u8");border:none;width:20px;height:20px";
		ui.bnColorValue->setStyleSheet(style);
	}

	void OnRbPos(bool state)
	{
		if (state)
		{
			ui.etX->setValidator(new QIntValidator(0, posMax, this));
			ui.etY->setValidator(new QIntValidator(0, posMax, this));
			if (ui.etX->text().toInt() < 0) ui.etX->setText(u8"0");
			if (ui.etY->text().toInt() < 0) ui.etY->setText(u8"0");
			ui.bnPos->setText(u8"+");
			ui.bnPos->setDisabled(0);
		}
	}

	void OnRbMove(bool state)
	{
		if (state)
		{
			ui.etX->setValidator(new QIntValidator(posMin, posMax, this));
			ui.etY->setValidator(new QIntValidator(posMin, posMax, this));
			ui.bnPos->setText(u8"×");
			ui.bnPos->setDisabled(1);
		}
	}
};