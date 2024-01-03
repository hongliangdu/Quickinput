#pragma once
#include "static.h"

QuickInputStruct Global::qi;
List<byte> Global::trBlock;
byte Global::blockRep[255];

SIZE  TipsWindow::screen = { 0 };
HANDLE TipsWindow::thread = 0;
HWND TipsWindow::wnd = 0;
UINT TipsWindow::time = 0;
UINT TipsWindow::timeBegin = 0;
COLORREF  TipsWindow::color = 0;
std::wstring  TipsWindow::text;

QString UI::syOn;
QString UI::syOff;
QString UI::syOk;
QString UI::syNot;
QString UI::syStop;
QString UI::syShow;
QString UI::syHide;
QString UI::syOption;
QString UI::syLink;
QString UI::syEdit;
QString UI::syUp;
QString UI::syDown;
QString UI::syTurn;
QString UI::syLeft;
QString UI::syTop;
QString UI::syRight;
QString UI::syMove;
QString UI::syTime;
QString UI::syText;
QString UI::syLoop;
QString UI::syColor;

std::wstring UI::qiOn;
std::wstring UI::qiOff;
QString UI::muOn;
QString UI::muOff;
QString UI::muShow;
QString UI::muHide;
QString UI::muExit;
QString UI::acWait;
QString UI::acDown;
QString UI::acUp;
QString UI::acClick;
QString UI::acPos;
QString UI::acMove;
QString UI::acLoop;
QString UI::acText;
QString UI::acColor;
QString UI::acEnd;
QString UI::acEndLoop;
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