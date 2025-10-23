//
// Created by HichTala on 21/06/25.
//
#define PY_SSIZE_T_CLEAN

#include "plugin-path.h"

#include <Python.h>
#include <cstdio>
#include <cwchar>
#ifdef _WIN32
#include <windows.h>
#include <tchar.h>
#endif

#include "DrawDock.hpp"

#include "SettingsDialog.hpp"

#include <QSettings>
#include <QStandardPaths>
#include <obs-module.h>

DrawDock::DrawDock(QWidget *parent) : QWidget(parent)
{
	this->parent = parent;

	this->setProperty("class", "dock-widget");

	auto *layout = new QHBoxLayout(this);
	layout->setContentsMargins(5, 4, 5, 4);
	layout->setSpacing(4);

	this->start_button->setText("Start Draw");
	this->start_button->setCheckable(true);
	this->start_button->setProperty("class", "start-streaming");
	layout->addWidget(this->start_button);

	this->settings_button->setProperty("class", "icon-gear");
	this->settings_button->setEnabled(true);
	this->settings_button->setToolTip("Settings");
	this->settings_button->setFixedHeight(this->start_button->sizeHint().height());
	this->settings_button->setFixedWidth(this->start_button->sizeHint().height());
	layout->addWidget(this->settings_button);

	resize(300, 300);

	connect(start_button, SIGNAL(clicked()), SLOT(StartButtonClicked()));
	connect(settings_button, SIGNAL(clicked()), SLOT(SettingsButtonClicked()));

	initialize_python_interpreter();
}
DrawDock::~DrawDock()
{
	if (this->should_run)
		StopPythonDraw();
};

void DrawDock::StartButtonClicked()
{
	if (this->start_button->isChecked()) {
		this->start_button->setText("Start Draw");
		this->start_button->setDisabled(true);
		this->start_button->setText("Starting Draw...");
		StartPythonDraw();
	} else {
		StopPythonDraw();
		this->start_button->setText("Start Draw");
		this->settings_button->setEnabled(true);
	}
}

void DrawDock::SettingsButtonClicked()
{
	auto *settings_dialog = new SettingsDialog(this);
	settings_dialog->exec();
}

void DrawDock::StartPythonDraw()
{
	if (this->running_flag.load())
		return;
	this->running_flag.store(true);
	this->should_run.store(true);
	this->model_ready.store(false);

	this->python_thread = std::thread([this]() {
		PyGILState_STATE gstate = PyGILState_Ensure();
		blog(LOG_INFO, "Starting Draw2 python backend");

		PyObject *pModule = PyImport_ImportModule("draw");

		if (pModule) {
			PyObject *pFunc = PyObject_GetAttrString(pModule, "run");
			if (pFunc && PyCallable_Check(pFunc)) {
				PyObject *args = PyTuple_New(6);
				PyObject *capsule_stop = PyCapsule_New(&this->should_run, "stop_flag", nullptr);
				PyTuple_SetItem(args, 0, capsule_stop);

				PyObject *capsule_ready = PyCapsule_New(&this->model_ready, "model_ready", nullptr);
				PyTuple_SetItem(args, 1, capsule_ready);

				QSettings settings = QSettings("HichTala", "Draw2");

				QByteArray deck_list_path1 = settings.value("deck_list1", "").toString().toUtf8();
				QByteArray deck_list_path2 = settings.value("deck_list2", "").toString().toUtf8();
				QByteArray deck_list_path3 = settings.value("deck_list3", "").toString().toUtf8();
				const char *plugin_dir = get_plugin_path();
				PyTuple_SetItem(args, 2,
						PyUnicode_FromString((plugin_dir + std::string("/decklists/") +
								      std::string(deck_list_path1) + std::string(";") +
								      plugin_dir + std::string("/decklists/") +
								      std::string(deck_list_path2) + std::string(";") +
								      plugin_dir + std::string("/decklists/") +
								      std::string(deck_list_path3) + std::string(";"))
									     .c_str()));

				int minimum_out_of_screen_time_value =
					settings.value("minimum_out_of_screen_time", 25).value<int>();
				PyTuple_SetItem(args, 3, PyLong_FromLong(minimum_out_of_screen_time_value));

				int minimum_screen_time_value = settings.value("minimum_screen_time", 6).value<int>();
				PyTuple_SetItem(args, 4, PyLong_FromLong(minimum_screen_time_value));

				int confidence_value = settings.value("confidence_slider", 5).value<int>();
				PyTuple_SetItem(args, 5, PyLong_FromLong(confidence_value));

				PyObject_CallObject(pFunc, args);
				Py_DECREF(args);

			} else {
				blog(LOG_ERROR, "Failed to find or call start_draw function.");
			}
			Py_XDECREF(pFunc);
			Py_XDECREF(pModule);
		} else {
			blog(LOG_ERROR, "Failed to import draw_module.");
		}
		PyGILState_Release(gstate);
		this->running_flag.store(false);
	});
	std::thread([this]() {
		for (int i = 0; i < 300; ++i) {
			if (this->model_ready.load()) {
				this->start_button->setEnabled(true);
				this->start_button->setText("Stop Draw");
				blog(LOG_INFO, "Draw2 python backend started successfully");
				break;
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
		if (!this->start_button->isEnabled()) {
			this->start_button->setDisabled(true);
			this->start_button->setText("Start Draw");
		}
	}).detach();
}

void DrawDock::StopPythonDraw()
{
	blog(LOG_INFO, "Stopping Draw2 python backend");
	if (!this->running_flag.load())
		return;
	this->should_run.store(false);

	if (this->python_thread.joinable()) {
		this->python_thread.join();
	}
}

void DrawDock::initialize_python_interpreter() const
{
	blog(LOG_INFO, "Initializing Python interpreter ");

	this->start_button->setDisabled(true);
#ifndef _WIN32
	wchar_t pythonPath[512];
#endif
	wchar_t pythonExe[256];
	wchar_t pythonHome[256];
	if (!Py_IsInitialized()) {

		QSettings settings = QSettings("HichTala", "Draw2");
		QByteArray pyHome = settings.value("python_path", "").toString().toUtf8();
		QFileInfo pyHomeInfo(QString::fromUtf8(pyHome));
		if (!pyHomeInfo.exists() || !pyHomeInfo.isDir()) {
			blog(LOG_INFO, "Failed to initialize Python interpreter");
			return;
		}
#ifdef _WIN32
		QByteArray pyExe = pyHome + "/python.exe";
#else
		QString pythonVersion;
		FILE *pipe = popen(
			"python3 -c \"import sys; print(f'{sys.version_info.major}.{sys.version_info.minor}')\"", "r");
		if (!pipe) {
			blog(LOG_ERROR, "Failed to retrieve Python version");
			return;
		}
		char buffer[128];
		if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
			pythonVersion = QString::fromUtf8(buffer).trimmed();
		}
		pclose(pipe);
		QString sitePackagesPath = pyHome + "/lib/python" + pythonVersion + "/site-packages";
		QFileInfo sitePackagesPathInfo(sitePackagesPath);

		if (!sitePackagesPathInfo.exists() || !sitePackagesPathInfo.isDir()) {
			blog(LOG_INFO, "Failed to initialize Python interpreter");
			return;
		}
		QByteArray pyExe = pyHome + "/bin/python";

		wcsncpy(pythonPath, sitePackagesPath.toStdWString().c_str(), sizeof(pythonPath) / sizeof(wchar_t));
		pythonPath[sizeof(pythonPath) / sizeof(wchar_t) - 1] = L'\0';
#endif

		wcsncpy(pythonHome, QString::fromUtf8(pyHome).toStdWString().c_str(),
			sizeof(pythonHome) / sizeof(wchar_t));
		pythonHome[sizeof(pythonHome) / sizeof(wchar_t) - 1] = L'\0';

		wcsncpy(pythonExe, QString::fromUtf8(pyExe).toStdWString().c_str(),
			sizeof(pythonExe) / sizeof(wchar_t));
		pythonExe[sizeof(pythonExe) / sizeof(wchar_t) - 1] = L'\0';

		blog(LOG_INFO, "Python Home: %ls", pythonHome);
		blog(LOG_INFO, "Python Executable: %ls", pythonExe);

		PyConfig config;
		PyConfig_InitPythonConfig(&config);

		PyConfig_SetString(&config, &config.executable, pythonExe);
		PyConfig_SetString(&config, &config.home, pythonHome);

		putenv(("PYTHONHOME=" + std::string(pyHome)).data());
		putenv(("PYTHONPATH=" + std::string(pyHome) + "/python312.zip;" + std::string(pyHome) +
			"/Lib/site-packages;" + std::string(pyHome))
			       .data());

		const char *pyhome_env = getenv("PYTHONHOME");
		const char *pypath_env = getenv("PYTHONPATH");
		const char *lang = getenv("LANG");
		blog(LOG_INFO, "PYTHONHOME env: %s, PYTHONPATH env: %s, LANG: %s", pyhome_env ? pyhome_env : "(null)",
		     pypath_env ? pypath_env : "(null)", lang ? lang : "(null)");

#ifndef _WIN32
		PyConfig_SetString(&config, &config.pythonpath_env, pythonPath);
#endif
		PyStatus status = Py_InitializeFromConfig(&config);
		if (PyStatus_Exception(status) || !Py_IsInitialized()) {

			blog(LOG_INFO, "Failed to initialize Python interpreter: %s", status.err_msg);
			blog(LOG_INFO, "Failed to initialize Python interpreter: %s", status.func);
			PyConfig_Clear(&config);
			if (PyStatus_IsExit(status)) {
				blog(LOG_INFO, "Failed to initialize Python interpreter: %d", status.exitcode);
			}
			return;
		}

		PyConfig_Clear(&config);
	}

	if (Py_IsInitialized()) {
		PyObject *pModule = PyImport_ImportModule("draw");
		if (!pModule) {
			blog(LOG_ERROR, "Failed to import draw_module.");
			return;
		}
		blog(LOG_INFO, "Python interpreter initialized successfully");
		this->start_button->setEnabled(true);
	} else {
		blog(LOG_INFO, "Failed to initialize Python interpreter");
	}
}
