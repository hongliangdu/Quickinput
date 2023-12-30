#pragma once
#include "static.h"

QuickInputStruct Global::qi;

SIZE  TipsWindow::screen = { 0 };
HANDLE TipsWindow::thread = 0;
HWND TipsWindow::wnd = 0;
UINT TipsWindow::time = 0;
UINT TipsWindow::timeBegin = 0;
COLORREF  TipsWindow::color = 0;
std::wstring  TipsWindow::text;

QString UI::qiWait;
QString UI::qiDown;
QString UI::qiUp;
QString UI::qiClick;
QString UI::qiPos;
QString UI::qiMove;
QString UI::qiLoop;
QString UI::qiText;
QString UI::qiColor;
QString UI::qiEnd;
QString UI::qiEndLoop;
std::wstring UI::qiOn;
std::wstring UI::qiOff;
QString UI::trOn;
QString UI::trOff;
QString UI::etChange;
QString UI::etAdd;
QString UI::etDel;
QString UI::etEdit;
QString UI::etFunc;
QString UI::etParam;
QString UI::rcStart;
QString UI::rcStop;
QString UI::rcClose;