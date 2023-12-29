#pragma once
#include <qcolordialog.h>
#include <qevent.h>
#include <qtimer.h>
#include <qbuttongroup.h>
#include "QColorSelection.h"
#include "QPositionSelection.h"
#include "QSelectionBox.h"
#include "QPointView.h"
#include "QRectView.h"
#include "ui_EditUi.h"
#include "../static.h"

class EditUi : public QDialog
{
	Q_OBJECT

public:
	EditUi(Actions& actions) : QDialog()
	{
		this->actions = &actions;
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
	QPointView pv;
	QRectView rv;
	Actions* actions = 0;
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
		connect(ui.tbItem, SIGNAL(currentItemChanged(QTableWidgetItem*, QTableWidgetItem*)), this, SLOT(OnTbSelect(QTableWidgetItem*, QTableWidgetItem*)));
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
	void SetChbColorMove(bool state) { ui.chbColorMove->setChecked(state); }
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

	Action GetKey() {
		Action action(Action::_key);
		if (ui.rbDown->isChecked()) action.key.state = Action::Key::down;
		else if (ui.rbUp->isChecked()) action.key.state = Action::Key::up;
		else if (ui.rbClick->isChecked()) action.key.state = Action::Key::click;
		action.key.vk = ui.hkKey->virtualKey();
		return action;
	}
	Action GetMouse() {
		Action action(Action::_mouse);
		action.mouse.move = ui.rbMove->isChecked();
		int x = ui.etX->text().toInt();
		int y = ui.etY->text().toInt();
		int ex = ui.etMoveRand->text().toInt();
		if (x > posMax) x = posMax;
		if (y > posMax) y = posMax;
		if (ex > posMax) ex = posMax;
		if (x < posMin) x = posMin;
		if (y < posMin) y = posMin;
		action.mouse.x = x;
		action.mouse.y = y;
		action.mouse.ex = ex;
		return action;
	}
	Action GetDelay() {
		Action action(Action::_delay);
		int ms = ui.etTime->text().toInt();
		int ex = ui.etDelayRand->text().toInt();
		if (ms > delayMax) ms = delayMax;
		if (ex > delayMax) ex = delayRandMax;
		action.delay.ms = ms;
		action.delay.ex = ex;
		return action;
	}
	Action GetText() {
		Action action(Action::_text);
		action.text.str.cpy((LPCWSTR)(ui.etText->toPlainText().utf16()));
		return action;
	}
	Action GetColor() {
		Action action(Action::_color);
		action.color.unfind = ui.rbColorNot->isChecked();
		action.color.move = ui.chbColorMove->isChecked();
		{
			int l = ui.etColorL->text().toInt();
			int t = ui.etColorT->text().toInt();
			int r = ui.etColorR->text().toInt();
			int b = ui.etColorB->text().toInt();
			if (l > posMax) l = posMax;
			if (t > posMax) t = posMax;
			if (r > posMax) r = posMax;
			if (b > posMax) b = posMax;
			action.color.rect = { l, t, r, b };
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
			action.color.rgbe = rgbe;
		}
		return action;
	}
	Action GetLoop()
	{
		Action action(Action::_loop);
		int i = 0;
		if (ui.etCount->text() == "") i = 1;
		else i = ui.etCount->text().toInt();
		if (i > loopCountMax) i = loopCountMax;
		action.loop.count = i;
		return action;
	}

	void LoadKey(const Action& action) {
		if (action.key.state == Action::Key::down) ui.rbDown->setCheckable(1);
		else if (action.key.state == Action::Key::up) ui.rbUp->setCheckable(1);
		else if (action.key.state == Action::Key::click) ui.rbClick->setCheckable(1);
		SetHkKey(action.key.vk);
	}
	void LoadMouse(const Action& action) {
		if (action.mouse.move)
		{
			ui.rbMove->setChecked(1);
			OnRbMove(1);
		}
		else
		{
			ui.rbPos->setChecked(1);
			OnRbPos(1);
		}
		SetEtPos({ (long)action.mouse.x, (long)action.mouse.y, (long)action.mouse.ex, 0 });
	}
	void LoadDelay(const Action& action) { SetEtDelay({ (long)action.delay.ms, (long)action.delay.ex }); }
	void LoadText(const Action& action) { ui.etText->setText(QString::fromWCharArray(action.text.str.str)); }
	void LoadColor(const Action& action) { SetRbColorMode(action.color.unfind); SetChbColorMove(action.color.move); SetEtColorRect(action.color.rect); SetEtColorValue(action.color.rgbe); }
	void LoadLoop(const Action& action) { SetEtLoopCount(action.loop.count); }

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
		ui.tbItem->setRowCount(actions->size());
		ui.tbItem->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
		ui.tbItem->verticalHeader()->setDefaultAlignment(Qt::AlignCenter);
		ui.tbItem->verticalHeader()->setDefaultSectionSize(0);

		for (uint32 u = 0; u < actions->size(); u++)
		{
			QString ps;
			switch (actions[0][u].type)
			{
			case Action::_end: ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiEnd)); break;
			
			case Action::_delay:
			{
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiWait));
				ps = QString::number(actions[0][u].delay.ms);
				ps += u8"ㅤㅤ";
				ps += QString::number(actions[0][u].delay.ex);
			}
			break;
			
			case Action::_key:
			{
				if (actions[0][u].key.state == Action::Key::up) ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiUp));
				else if (actions[0][u].key.state == Action::Key::down) ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiDown));
				else if (actions[0][u].key.state == Action::Key::click) ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiClick));
				ps = QString::fromWCharArray(Input::Name(actions[0][u].key.vk));
			}
			break;
			
			case Action::_mouse:
			{
				if (actions[0][u].mouse.move) ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiMove));
				else ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiPos));
				ps = QString::number(actions[0][u].mouse.x);
				ps += u8" - ";
				ps += QString::number(actions[0][u].mouse.y);
				ps += u8"ㅤㅤ";
				ps += QString::number(actions[0][u].mouse.ex);
			}
			break;
			
			case Action::_text:
			{
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiText));
				std::wstring text = actions[0][u].text.str.str;
				ps = QString::fromWCharArray(text.substr(0, 32).c_str());
				if (text.length() > 31) ps += u8"...";
			}
			break;
			
			case Action::_color:
			{
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiColor));
				ps = u8"(";
				ps += QString::number(actions[0][u].color.rect.left);
				ps += u8",";
				ps += QString::number(actions[0][u].color.rect.top);
				ps += u8",";
				ps += QString::number(actions[0][u].color.rect.right);
				ps += u8",";
				ps += QString::number(actions[0][u].color.rect.bottom);
				ps += u8")　(";
				ps += QString::number(GetRValue(actions[0][u].color.rgbe));
				ps += u8",";
				ps += QString::number(GetGValue(actions[0][u].color.rgbe));
				ps += u8",";
				ps += QString::number(GetBValue(actions[0][u].color.rgbe));
				ps += u8",";
				ps += QString::number(GetAValue(actions[0][u].color.rgbe));
				if (actions[0][u].color.unfind) ps += u8")　未找到";
				else
				{
					ps += u8")　找到";
					if (actions[0][u].color.move) ps += u8"并移动";
					else ps += u8"不移动";
				}
			}
			break;
			
			case Action::_loop:
			{
				ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiLoop));
				if (actions[0][u].loop.count) ps = QString::number(actions[0][u].loop.count);
				else ps = u8"无限";
			}
			break;

			case Action::_loopEnd: ui.tbItem->setItem(u, 0, new QTableWidgetItem(UI::qiEndLoop)); break;

			default: ui.tbItem->setItem(u, 0, new QTableWidgetItem(u8"加载失败")); break;
			}
			ui.tbItem->setItem(u, 1, new QTableWidgetItem(ps));
			ui.tbItem->setItem(u, 2, new QTableWidgetItem(QString::fromWCharArray(actions[0][u].mark.c_str())));
			ui.tbItem->item(u, 0)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tbItem->item(u, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
			ui.tbItem->item(u, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
		}
		connect(ui.tbItem, SIGNAL(cellChanged(int, int)), this, SLOT(OnTbChanged(int, int)));
	}

	void GetItem(int32 pos)
	{
		switch (actions[0][pos].type)
		{
		case Action::_delay: LoadDelay(actions[0][pos]); break;
		case Action::_key: LoadKey(actions[0][pos]); break;
		case Action::_mouse: LoadMouse(actions[0][pos]); break;
		case Action::_text: LoadText(actions[0][pos]); break;
		case Action::_color: LoadColor(actions[0][pos]); break;
		case Action::_loop: LoadLoop(actions[0][pos]); break;
		}
	}

	void SetItem(int32 type, int32 pos)
	{
		Action action;
		switch (type)
		{
		case Action::_end: action.type = Action::_end; break;
		case Action::_delay: action = GetDelay(); break;
		case Action::_key: action = GetKey(); break;
		case Action::_mouse: action = GetMouse(); break;
		case Action::_text: action = GetText(); break;
		case Action::_color: action = GetColor(); break;
		case Action::_loop: action = GetLoop(); break;
		case Action::_loopEnd: action.type = Action::_loopEnd; break;
		default: action.type = Action::_none; break;
		}
		if (actions[0][pos].type == Action::_color)
		{
			if (type == Action::_color) action.color.next = actions[0][pos].color.next;
			if (type == Action::_loop) action.loop.next = actions[0][pos].color.next;
		}
		else if (actions[0][pos].type == Action::_loop)
		{
			if (type == Action::_color) action.color.next = actions[0][pos].loop.next;
			if (type == Action::_loop) action.loop.next = actions[0][pos].loop.next;
		}
		actions[0][pos] = action;
	}

	void AddItem(int32 type)
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) pos = actions->size();
		else if ((pos + 1) <= actions->size()) pos++;

		actions->InsNull(pos);
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
			actions->Swp(pos, pos - 1);
			ui.tbItem->setCurrentItem(ui.tbItem->item(pos - 1, 0));
		}
		else if (!up && (pos + 1) < ui.tbItem->rowCount())
		{
			actions->Swp(pos, pos + 1);
			ui.tbItem->setCurrentItem(ui.tbItem->item(pos + 1, 0));
		}
		TbUpdate();
	}

	void showEvent(QShowEvent*)
	{
		ui.lbTitle->setText(windowTitle());
		TbUpdate();
	}

	void mousePressEvent(QMouseEvent* et) { if (et->buttons() & Qt::LeftButton) msPos = et->pos(); }

	void mouseMoveEvent(QMouseEvent* et) { if (et->buttons() & Qt::LeftButton) move(et->pos() + pos() - msPos); }

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
		else if (obj == ui.tbItem->viewport())
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

				actions->Mov(before, after);
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
		actions[0][row].mark = (LPCWSTR)(ui.tbItem->item(row, 2)->text().utf16());
	}

	void OnTbClicked(int row, int column)
	{
		ui.bnColorEdit->setDisabled(1);
		ui.bnLoopEdit->setDisabled(1);

		if (row < 0) return;
		if (column == 2) ui.tbItem->editItem(ui.tbItem->item(row, column));
		else
		{
			pv.hide();
			rv.hide();
			if (actions[0][row].type == Action::_mouse && !actions[0][row].mouse.move)
			{
				POINT pt = AbsToVRel({ (long)actions[0][row].mouse.x, (long)actions[0][row].mouse.y });
				pv.Show(QPoint(pt.x, pt.y));
			}
			if (actions[0][row].type == Action::_color)
			{
				ui.bnColorEdit->setDisabled(0);
				POINT lt = AbsToVRel({ (long)actions[0][row].color.rect.left, (long)actions[0][row].color.rect.top });
				POINT rb = AbsToVRel({ (long)actions[0][row].color.rect.right, (long)actions[0][row].color.rect.bottom });
				rv.Show(QRect(lt.x, lt.y, rb.x - lt.x, rb.y - lt.y));
			}
			else if (actions[0][row].type == Action::_loop) ui.bnLoopEdit->setDisabled(0);
		}
	}

	void OnTbSelect(QTableWidgetItem*, QTableWidgetItem*) { OnTbClicked(ui.tbItem->currentRow(), ui.tbItem->currentColumn()); }

	void OnTbDoubleClick(int row, int column)
	{
		if (row < 0) return;

		GetItem(row);

		changing = 1;
		SetChanging();
	}

	void OnBnClose() { close(); }
	void OnBnKeyAdd() { if (changing) ChangeItem(Action::_key); else AddItem(Action::_key); }
	void OnBnMoveAdd() { if (changing) ChangeItem(Action::_mouse); else AddItem(Action::_mouse); }
	void OnBnDelayAdd() { if (changing) ChangeItem(Action::_delay); else AddItem(Action::_delay); }
	void OnBnLoopAdd() { if (changing) ChangeItem(Action::_loop); else AddItem(Action::_loop); }
	void OnBnTextAdd() { if (changing) ChangeItem(Action::_text); else AddItem(Action::_text); }
	void OnBnColorAdd() { if (changing) ChangeItem(Action::_color); else AddItem(Action::_color); }
	void OnBnEndAdd() { if (changing) ChangeItem(Action::_end); else AddItem(Action::_end); }
	void OnBnEndLoopAdd() { if (changing) ChangeItem(Action::_loopEnd); else AddItem(Action::_loopEnd); }
	void OnBnLoopEdit()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		if (actions[0][pos].type != Action::_loop) return;
		EditUi edit(actions[0][pos].loop.next);
		edit.setWindowTitle(u8"编辑 - 循环");
		edit.exec();
	}
	void OnColorEdit()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		if (actions[0][pos].type != Action::_color) return;
		EditUi edit(actions[0][pos].color.next);
		edit.setWindowTitle(u8"编辑 - 查找颜色");
		edit.exec();
	}
	void OnBnDel()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;
		actions->Del(pos);
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