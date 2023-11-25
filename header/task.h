#pragma once

#include "file.h"
#include "thread.h"
#include "process.h"
#include "system.h"
#include <string>
#include <windows.h>
#include <comdef.h>
#include <wincred.h>
#include <taskschd.h>
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "credui.lib")
#pragma comment(lib, "taskschd.lib")

namespace CG {
	class Task {
	public:
		Task() {}

		Task(std::wstring taskName, std::wstring exePath = Process::exePath(), std::wstring runPath = Process::runPath()) {
			Register(taskName, exePath, runPath);
		}

		std::wstring getLog() { return ts.log; }

		// return 0: error, return 1: task registered
		bool Register(std::wstring taskName, std::wstring exePath = Process::exePath(), std::wstring runPath = File::PathLast(Process::exePath().c_str())) {
			ts.taskName = taskName;
			ts.exePath = exePath;
			ts.runPath = runPath;
			ts.operate = 1;

			ts.thread = Thread::Start(TaskOperate, &ts);
			return Thread::Wait(ts.thread);
		}

		// return 0: no task or error, return 1: task Found
		bool Find(std::wstring taskName) {
			ts.taskName = taskName;
			ts.operate = 0;

			ts.thread = Thread::Start(TaskOperate, &ts);
			return Thread::Wait(ts.thread);
		}

		// return 0: no task or error, return 1: task deleted
		bool Delete(std::wstring taskName) {
			ts.taskName = taskName;
			ts.operate = 2;

			ts.thread = Thread::Start(TaskOperate, &ts);
			return Thread::Wait(ts.thread);
		}

		void Emplog() { ts.log = L""; }

	private:

		// 0: Find, 1: Register, 2: Delete
		struct TaskStruct {
			BYTE operate = 0;
			HANDLE thread = 0;
			std::wstring log;
			std::wstring taskName;
			std::wstring exePath;
			std::wstring runPath;
		};

		TaskStruct ts;

		static DWORD CALLBACK TaskOperate(LPVOID lParam) {
			TaskStruct* ts = (TaskStruct*)lParam;

			HRESULT hr = 0;
			ITaskService* pTaskService = 0;
			ITaskFolder* pTaskFolder = 0;

			WCHAR taskPath[MAX_PATH];
			WCHAR userName[MAX_PATH];
			swprintf_s(taskPath, MAX_PATH, L"\\%s", ts->taskName.c_str());
			swprintf_s(userName, MAX_PATH, L"%s\\%s", System::environmentVariable(L"COMPUTERNAME").c_str(), System::environmentVariable(L"USERNAME").c_str());


			ts->log += L"Initialize: \n";

			hr = CoInitializeEx(0, COINIT_MULTITHREADED);
			if (FAILED(hr)) {
				if (hr == S_FALSE) {
					ts->log += L"  warning: CoInitializeEx\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";
				}
				else if (hr == RPC_E_CHANGED_MODE) {
					ts->log += L"error: CoInitializeEx\n";
					ts->log += L"  result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					return 0;
				}
			}

			hr = CoInitializeSecurity(0, -1, 0, 0, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, 0, 0, 0);
			if (FAILED(hr)) {
				if (hr == RPC_E_TOO_LATE) {
					ts->log += L"  warning: CoInitializeSecurity\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";
				}
				else {
					ts->log += L"  error: CoInitializeSecurity\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";
					CoUninitialize();
					return 0;
				}
			}

			hr = CoCreateInstance(CLSID_TaskScheduler, 0, CLSCTX_INPROC_SERVER, IID_ITaskService, (void**)&pTaskService);
			if (FAILED(hr)) {
				ts->log += L"  error: CoCreateInstance\n";
				ts->log += L"    result: ";
				ts->log += String::toWString(HRESULT_CODE(hr));
				ts->log += L"\n";

				CoUninitialize();
				return 0;
			}

			hr = pTaskService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
			if (FAILED(hr)) {
				ts->log += L"  error: pTaskService->Connect\n";
				ts->log += L"    result: ";
				ts->log += String::toWString(HRESULT_CODE(hr));
				ts->log += L"\n";

				CoUninitialize();
				return 0;
			}

			hr = pTaskService->GetFolder(_bstr_t(L"\\"), &pTaskFolder);
			if (FAILED(hr)) {
				ts->log += L"  error: pTaskService->GetFolder\n";
				ts->log += L"    result: ";
				ts->log += String::toWString(HRESULT_CODE(hr));
				ts->log += L"\n";

				CoUninitialize();
				return 0;
			}

			// 0: Find, 1: Register, 2: Delete
			if (ts->operate == 0) {
				ts->log += L"Find: \n";

				IRegisteredTask* pRegisteredTask = 0;
				hr = pTaskFolder->GetTask(_bstr_t(taskPath), &pRegisteredTask);
				if (FAILED(hr))
				{
					ts->log += L"  warning: pTaskFolder->GetTask\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				pRegisteredTask->Release();
				pTaskFolder->Release();
				CoUninitialize();

				return 1;
			}
			else if (ts->operate == 1) {
				ts->log += L"Register: \n";

				pTaskFolder->DeleteTask(_bstr_t(ts->taskName.c_str()), 0);

				ITaskDefinition* pTask = 0;
				hr = pTaskService->NewTask(0, &pTask);
				if (FAILED(hr)) {
					ts->log += L"  error: pTaskService->NewTask\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				IRegistrationInfo* pRegInfo = 0;
				hr = pTask->get_RegistrationInfo(&pRegInfo);
				if (FAILED(hr)) {
					ts->log += L"  error: pTask->get_RegistrationInfo\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pTask->Release();
					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				hr = pRegInfo->put_Author(_bstr_t(L"No set"));
				pRegInfo->Release();
				if (FAILED(hr)) {
					ts->log += L"warning: pRegInfo->put_Author\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";
				}

				IPrincipal* pPrincipal = 0;
				hr = pTask->get_Principal(&pPrincipal);
				if (FAILED(hr)) {
					ts->log += L"  error: get_Principal\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pTask->Release();
					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				hr = pPrincipal->put_Id(_bstr_t(L"Principal 1"));
				if (FAILED(hr)) {
					ts->log += L"warning: pPrincipal->put_Id\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";
				}

				hr = pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
				if (FAILED(hr)) {
					ts->log += L"  error: pPrincipal->put_LogonType\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pPrincipal->Release();
					pTask->Release();
					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_HIGHEST);
				pPrincipal->Release();
				if (FAILED(hr)) {
					ts->log += L"  error: pPrincipal->put_RunLevel\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pTask->Release();
					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				ITaskSettings* pSettings = 0;
				hr = pTask->get_Settings(&pSettings);
				if (FAILED(hr)) {
					ts->log += L"  error: pTask->get_Settings\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pTask->Release();
					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				hr = pSettings->put_StartWhenAvailable(VARIANT_TRUE);
				pSettings->Release();
				if (FAILED(hr)) {
					ts->log += L"  error: pSettings->put_StartWhenAvailable\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pTask->Release();
					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				ITriggerCollection* pTriggerCollection = 0;
				hr = pTask->get_Triggers(&pTriggerCollection);
				if (FAILED(hr)) {
					ts->log += L"  error: pTask->get_Triggers\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pTask->Release();
					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				ITrigger* pTrigger = 0;
				hr = pTriggerCollection->Create(TASK_TRIGGER_LOGON, &pTrigger);
				pTriggerCollection->Release();
				if (FAILED(hr)) {
					ts->log += L"  error: pTriggerCollection->Create\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pTask->Release();
					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				ILogonTrigger* pLogonTrigger = 0;
				hr = pTrigger->QueryInterface(IID_ILogonTrigger, (void**)&pLogonTrigger);
				pTrigger->Release();
				if (FAILED(hr)) {
					ts->log += L"  error: pTrigger->QueryInterface\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pTask->Release();
					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				hr = pLogonTrigger->put_Id(_bstr_t(L"Logon trigger"));
				if (FAILED(hr)) {
					ts->log += L"warning: pLogonTrigger->put_Id\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";
				}

				hr = pLogonTrigger->put_UserId(_bstr_t(userName));
				if (FAILED(hr)) {
					ts->log += L"  error: pLogonTrigger->put_UserId\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pTask->Release();
					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				hr = pLogonTrigger->put_Delay(_bstr_t(L"PT0S"));
				pLogonTrigger->Release();
				if (FAILED(hr)) {
					ts->log += L"  error: pLogonTrigger->put_Delay\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pTask->Release();
					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				IActionCollection* pActionCollection = 0;
				hr = pTask->get_Actions(&pActionCollection);
				if (FAILED(hr)) {
					ts->log += L"  error: pTask->get_Actions\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pTask->Release();
					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				IAction* pAction = 0;
				hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
				pActionCollection->Release();
				if (FAILED(hr)) {
					ts->log += L"  error: pActionCollection->Create\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pTask->Release();
					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				IExecAction* pExecAction = 0;
				hr = pAction->QueryInterface(IID_IExecAction, (void**)&pExecAction);
				pAction->Release();
				if (FAILED(hr)) {
					ts->log += L"  error: pAction->QueryInterface\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pTask->Release();
					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				hr = pExecAction->put_Path(_bstr_t(ts->exePath.c_str()));
				if (FAILED(hr)) {
					ts->log += L"  error: pExecAction->put_Path\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pExecAction->Release();
					pTask->Release();
					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				hr = pExecAction->put_WorkingDirectory(_bstr_t(ts->runPath.c_str()));
				pExecAction->Release();
				if (FAILED(hr)) {
					ts->log += L"  error: pExecAction->put_WorkingDirectory\n";
					ts->log += L"    result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pTask->Release();
					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				IRegisteredTask* pRegisteredTask = 0;
				hr = pTaskFolder->RegisterTaskDefinition(_bstr_t(ts->taskName.c_str()), pTask, TASK_CREATE_OR_UPDATE, _variant_t(), _variant_t(), TASK_LOGON_INTERACTIVE_TOKEN, _variant_t(L""), &pRegisteredTask);
				if (FAILED(hr)) {
					ts->log += L"  error: pTaskFolder->RegisterTaskDefinition\n";
					ts->log += L"      result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";

					pTask->Release();
					pTaskFolder->Release();
					CoUninitialize();
					return 0;
				}

				pTask->Release();
				pRegisteredTask->Release();
				pTaskFolder->Release();
				CoUninitialize();
				return 1;
			}
			else if (ts->operate == 2) {
				ts->log += L"Delete: \n";

				hr = pTaskFolder->DeleteTask(_bstr_t(ts->taskName.c_str()), 0);
				if (FAILED(hr)) {
					ts->log += L"  warning: pTaskFolder->RegisterTaskDefinition\n";
					ts->log += L"      result: ";
					ts->log += String::toWString(HRESULT_CODE(hr));
					ts->log += L"\n";
					return 0;
				}
				return 1;
			}

			return 0;
		}
	};
}