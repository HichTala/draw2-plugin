//
// Created by HichTala on 21/06/25.
//
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "DrawDock.hpp"

#include "SettingsDialog.hpp"

#include <QSettings>
#include <QStandardPaths>
#include <obs-module.h>

void initialize_python_interpreter()
{
	blog(LOG_INFO, "Initializing Python interpreter ");
	if (!Py_IsInitialized()) {

		QSettings settings = QSettings("HichTala", "Draw2");
		QByteArray pyHome = settings.value("python_path", "").toString().toUtf8();
		QString sitePackagesPath = pyHome + "/lib/python3.12/site-packages";

		blog(LOG_INFO, "Initializing Python interpreter with home: %s", pyHome.toStdString().c_str());
		blog(LOG_INFO, "Initializing Python interpreter with site packages: %s",
		     sitePackagesPath.toStdString().c_str());

		qputenv("PYTHONHOME", QByteArray(pyHome));
		qputenv("PYTHONPATH", QByteArray(sitePackagesPath.toUtf8()));

		Py_Initialize();
		// PyRun_SimpleString(R"(import sys)");
		// PyRun_SimpleString(R"(sys.path.insert(0, '/home/hicham/miniconda3/envs/phd/lib/python3.12/site-packages'))");
		// PyRun_SimpleString(R"(sys.path.insert(0, '/home/hicham/miniconda3/envs/phd/lib/python3.12/site-packages'))");
	}
}

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
		this->start_button->setDisabled(true);
		this->start_button->setText("Starting Draw...");
		this->settings_button->setDisabled(true);
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

		PyObject *pName = PyUnicode_FromString("draw");
		PyObject *pModule = PyImport_ImportModule("draw");
		Py_DECREF(pName);

		if (pModule) {
			PyObject *pFunc = PyObject_GetAttrString(pModule, "run");
			if (pFunc && PyCallable_Check(pFunc)) {
				PyObject *args = PyTuple_New(6);
				PyObject *capsule_stop = PyCapsule_New(&this->should_run, "stop_flag", nullptr);
				PyTuple_SetItem(args, 0, capsule_stop);

				PyObject *capsule_ready = PyCapsule_New(&this->model_ready, "model_ready", nullptr);
				PyTuple_SetItem(args, 1, capsule_ready);

				QSettings settings = QSettings("HichTala", "Draw2");

				QByteArray deck_list_path = settings.value("deck_list", "").toString().toUtf8();
				PyTuple_SetItem(args, 2, PyUnicode_FromString(deck_list_path.constData()));

				int minimum_out_of_screen_time_value =
					settings.value("minimum_out_of_screen_time", 25).value<int>();
				PyTuple_SetItem(args, 3, PyLong_FromLong(minimum_out_of_screen_time_value));

				int minimum_screen_time_value = settings.value("minimum_screen_time", 6).value<int>();
				PyTuple_SetItem(args, 4, PyLong_FromLong(minimum_screen_time_value));

				int confidence_value = settings.value("confidence_slider", 1).value<int>();
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
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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
