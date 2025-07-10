//
// Created by HichTala on 21/06/25.
//
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "DrawDock.hpp"

#include "SettingsDialog.hpp"

#include <util/base.h>

void initialize_python_interpreter()
{
	if (!Py_IsInitialized()) {
		Py_Initialize();
	}
	PyRun_SimpleString(R"(
import sys
sys.path.insert(0, '/home/hicham/miniconda3/envs/phd/lib/python3.12/site-packages')
)");
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
	if (this->should_run) StopPythonDraw();
};

void DrawDock::StartButtonClicked()
{
	if (this->start_button->isChecked()) {
		StartPythonDraw();
		this->start_button->setText("Stop Draw");
	} else {
		StopPythonDraw();
		this->start_button->setText("Start Draw");
	}
}

void DrawDock::SettingsButtonClicked()
{
	// QMessageBox::information(this, "Settings", "Settings dialog is not implemented yet.");
	auto *settings_dialog = new SettingsDialog(this);
	settings_dialog->exec();
}

void DrawDock::StartPythonDraw()
{
	if (this->running_flag.load())
		return;
	this->running_flag.store(true);
	this->should_run.store(true);

	this->python_thread = std::thread([this]() {
		PyGILState_STATE gstate = PyGILState_Ensure();
		blog(LOG_INFO, "StartPythonDraw");

		PyObject *pName = PyUnicode_FromString("draw");
		PyObject *pModule = PyImport_ImportModule("draw");
		Py_DECREF(pName);

		if (pModule) {
			PyObject *pFunc = PyObject_GetAttrString(pModule, "run");
			if (pFunc && PyCallable_Check(pFunc)) {
				PyObject *args = PyTuple_New(1);
				PyObject *capsule = PyCapsule_New(&this->should_run, "stop_flag", nullptr);
				PyTuple_SetItem(args, 0, capsule);

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
}

void DrawDock::StopPythonDraw()
{
	if (!this->running_flag.load())
		return;
	this->should_run.store(false);

	if (this->python_thread.joinable()) {
		this->python_thread.join();
	}
}