#pragma once
#include <qfiledialog.h>
#include <qtimer.h>
#include "RecordUi.h"
#include "EditUi.h"
#include "ui_MacroUi.h"
#include "../static.h"

class MacroUi : public QWidget
{
	Q_OBJECT

public:
	MacroUi() {}

	MacroUi(QWidget* parent) : QWidget(parent)
	{
		this->main = parent;
		ui.setupUi(this);
		setWindowFlags(Qt::FramelessWindowHint);

		WidInit();
		WidEvent();
		LockControl(1);
		TbUpdate();
	}

private:

	Ui::MacroUiClass ui;
	QWidget* main = 0;
	List<Script>& scripts = Global::qi.scripts;
	QTimer* timer;

	void WidInit()
	{
		timer = new QTimer(this);

		//Table
		{
			ui.tbItem->setColumnCount(1);
			QTableWidgetItem* tbi = new QTableWidgetItem(QString::fromUtf8(u8"名称"));
			ui.tbItem->setHorizontalHeaderItem(0, tbi);
			ui.tbItem->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Stretch);
		}
	}

	void WidEvent()
	{
		connect(timer, SIGNAL(timeout()), this, SLOT(OnTimeOut()));
		connect(ui.tbItem, SIGNAL(cellClicked(int, int)), this, SLOT(OnTbClicked(int, int)));
		connect(ui.etName, SIGNAL(returnPressed()), this, SLOT(OnEtReturn()));
		connect(ui.bnRec, SIGNAL(clicked()), this, SLOT(OnBnRec()));
		connect(ui.bnAdd, SIGNAL(clicked()), this, SLOT(OnBnAdd()));
		connect(ui.bnEdit, SIGNAL(clicked()), this, SLOT(OnBnEdit()));
		connect(ui.bnExp, SIGNAL(clicked()), this, SLOT(OnBnExp()));
		connect(ui.bnImp, SIGNAL(clicked()), this, SLOT(OnBnImp()));
		connect(ui.bnDel, SIGNAL(clicked()), this, SLOT(OnBnDel()));
	}

	void ResetControl()
	{
		ui.etName->setText("");
		ui.tbItem->setCurrentItem(0);
	}

	void LockControl(bool state)
	{
		ui.etName->setDisabled(state);
		ui.bnEdit->setDisabled(state);
		ui.bnExp->setDisabled(state);
		ui.bnDel->setDisabled(state);
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
		}
	}

private slots:

	void showEvent(QShowEvent*)
	{
		ResetControl();
		LockControl(1);
		TbUpdate();
	}

	void OnTimeOut()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;
		timer->stop();
		ui.etName->setDisabled(0);
		ui.etName->setText(QString::fromWCharArray(scripts[pos].name.c_str()));
	}

	void OnTbClicked(int row, int column)
	{
		LockControl(1);
		if (row < 0) return;

		ui.etName->setText(QString::fromWCharArray(scripts[row].name.c_str()));

		LockControl(0);
	}

	void OnEtReturn()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		if (File::NameFilter((LPCWSTR)ui.etName->text().utf16()) == L"")
		{
			timer->setSingleShot(1);
			timer->start(1000);

			ui.etName->setDisabled(1);
			ui.etName->setText("名称不可用");
			return;
		}

		std::wstring path = L"json\\";
		path += (LPCWSTR)ui.etName->text().utf16();
		path += L".json";

		if (File::FileState(path.c_str()))
		{
			timer->setSingleShot(1);
			timer->start(1000);

			ui.etName->setDisabled(1);
			ui.etName->setText("已存在该名称");
			return;
		}

		path = L"json\\" + scripts[pos].name + L".json";
		File::FileDelete(path.c_str());

		scripts[pos].name = (LPCWSTR)ui.etName->text().utf16();
		ui.etName->setText(QString::fromWCharArray(scripts[pos].name.c_str()));

		SaveScript(scripts[pos]);
		LoadJson();
		TbUpdate();
		ResetControl();
		LockControl(1);
	}

	void OnBnRec()
	{
		scripts.Add();
		RecordUi rw(main, scripts.Get().items);
		main->hide();
		Global::qi.HookState(1);
		Global::qi.state = 0;
		Global::qi.rec = &rw;

		std::wstring text = L"按下";
		QKeyEdit ke;
		ke.Mode(2);
		ke.VirtualKey(Global::qi.set.recKey & 0xFFFF, Global::qi.set.recKey >> 16);
		text += ke.Name().toStdWString();
		text += L"开始/停止录制";
		TipsWindow::Show(text, RGB(0x20, 0xFF, 0x20));

		rw.exec();
		main->show();
		Global::qi.HookState(0);

		LoadJson();
		TbUpdate();
		ResetControl();
		LockControl(1);
	}

	void OnBnAdd()
	{
		scripts.Add();
		scripts.Get().name = NameFilter(L"宏");
		scripts.Get().mode = 0;

		SaveScript(scripts.Get());
		LoadJson();
		TbUpdate();
		ResetControl();
		LockControl(1);
	}

	void OnBnEdit()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		EditUi edit(scripts[pos].items);
		edit.setWindowTitle(u8"编辑 - " + QString::fromWCharArray(scripts[pos].name.c_str()));
		edit.exec();

		SaveScript(scripts[pos]);
		LoadJson();
		TbUpdate();
		ResetControl();
		LockControl(1);
	}

	void OnBnExp()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		QString path = QDir::toNativeSeparators(QFileDialog::getSaveFileName(this, u8"导出到", QString::fromWCharArray((scripts[pos].name + L".json").c_str()), u8"Quick input macro (*.json)"));
		if (path == "") return;

		std::wstring srcFile = L"macro\\" + scripts[pos].name + L".json";
		std::wstring newFile = path.toStdWString();

		CopyFileW(srcFile.c_str(), newFile.c_str(), 0);

		LoadJson();
		TbUpdate();
		ResetControl();
		LockControl(1);
	}

	void OnBnImp()
	{
		QString path = QDir::toNativeSeparators(QFileDialog::getOpenFileName(this, u8"导入", QString(), u8"Quick input macro (*.json)"));
		if (path == "") return;

		std::wstring srcFile = path.toStdWString();
		std::wstring newFile = L"macro\\" + File::FileName(srcFile);

		File::FolderCreate(L"macro");
		if (File::FileState(newFile))
		{
			if (MsgBox::Warning(L"文件已存在，是否覆盖？", L"Warning", MB_YESNO | MB_ICONWARNING | MB_TOPMOST) == IDYES)
			{
				CopyFileW(srcFile.c_str(), newFile.c_str(), 0);
			}
		}
		else CopyFileW(srcFile.c_str(), newFile.c_str(), 0);

		LoadJson();
		TbUpdate();
		ResetControl();
		LockControl(1);
	}

	void OnBnDel()
	{
		int pos = ui.tbItem->currentRow();
		if (pos < 0) return;

		std::wstring name = L"macro\\" + scripts[pos].name + L".json";
		scripts.Del(pos);

		File::FileDelete(name.c_str());
		ui.tbItem->setCurrentItem(0);

		LoadJson();
		TbUpdate();
		ResetControl();
		LockControl(1);
	}
};