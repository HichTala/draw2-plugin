//
// Created by HichTala on 22/06/25.
//

#include <QLabel>
#include <QSettings>

#include "SettingsDialog.hpp"

#include "plugin-path.h"

#include <obs-module.h>
#include <util/base.h>

#ifdef _WIN32
#include <windows.h>
#include <shellapi.h>
#pragma comment(lib, "Shell32.lib")
#endif

void open_folder(const std::string &folder_path)
{
#ifdef _WIN32
	std::string fixed_path = folder_path;
	std::replace(fixed_path.begin(), fixed_path.end(), '/', '\\');

	int size_needed = MultiByteToWideChar(CP_UTF8, 0, fixed_path.c_str(), -1, NULL, 0);
	if (size_needed <= 0) {
		blog(LOG_ERROR, "MultiByteToWideChar failed converting path");
		return;
	}

	std::wstring wpath(size_needed, L'\0');
	MultiByteToWideChar(CP_UTF8, 0, fixed_path.c_str(), -1, &wpath[0], size_needed);
	wpath.resize(size_needed - 1); // Remove trailing null

	HINSTANCE result = ShellExecuteW(NULL, L"explore", wpath.c_str(), NULL, NULL, SW_SHOWNORMAL);
	if ((INT_PTR)result <= 32) {
		blog(LOG_ERROR, "ShellExecuteW failed: %d (path: %ls)", (int)(INT_PTR)result, wpath.c_str());
	}
#else
	std::string command =
#ifdef __APPLE__
		"open \"" + folder_path + "\"";
#else
		"xdg-open \"" + folder_path + "\"";
#endif
	int return_value = system(command.c_str());
	blog(LOG_INFO, "Open command returned %d", return_value);
#endif
}

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
	setWindowTitle("Draw 2 Settings");

	QSettings settings = QSettings("HichTala", "Draw2");

	QString deck_list_path1 = settings.value("deck_list1", "").toString();
	QString deck_list_path2 = settings.value("deck_list2", "").toString();
	QString deck_list_path3 = settings.value("deck_list3", "").toString();
	QString python_path_string = settings.value("python_path", "").toString();
	int minimum_out_of_screen_time_value = settings.value("minimum_out_of_screen_time", 25).value<int>();
	int minimum_screen_time_value = settings.value("minimum_screen_time", 6).value<int>();
	int confidence_value = settings.value("confidence_slider", 1).value<int>();

	auto *layout = new QVBoxLayout(this);

	auto *python_label = new QLabel(obs_module_text("python_path"), this);
	layout->addWidget(python_label);

	auto *python_browse_layout = new QHBoxLayout();
	this->python_path->setText(python_path_string);
	python_browse_layout->addWidget(this->python_path);
	python_browse_layout->addWidget(this->python_browse_button);
	layout->addLayout(python_browse_layout);

	auto *browse_layout = new QHBoxLayout();
	auto *label = new QLabel(obs_module_text("deck_list"), this);
	browse_layout->addWidget(label);
	this->browse_button->setMaximumWidth(125);
	browse_layout->addWidget(this->browse_button);
	layout->addLayout(browse_layout);

	const char *plugin_dir = get_plugin_path();
	QDir dir(((plugin_dir + std::string("/decklists/")).data()));
	QFileInfoList files = dir.entryInfoList(QDir::Files);
	this->deck_list1->setMaximumWidth(125);
	this->deck_list2->setMaximumWidth(125);
	this->deck_list3->setMaximumWidth(125);
	this->deck_list1->addItem(obs_module_text("none"));
	this->deck_list2->addItem(obs_module_text("none"));
	this->deck_list3->addItem(obs_module_text("none"));
	for (const QFileInfo &file : files) {
		this->deck_list1->addItem(file.fileName());
		this->deck_list2->addItem(file.fileName());
		this->deck_list3->addItem(file.fileName());
	}
	int index1 = this->deck_list1->findText(deck_list_path1, Qt::MatchExactly);
	int index2 = this->deck_list2->findText(deck_list_path2, Qt::MatchExactly);
	int index3 = this->deck_list3->findText(deck_list_path3, Qt::MatchExactly);
	if (index1 != -1) {
		this->deck_list1->setCurrentIndex(index1);
	}
	if (index2 != -1) {
		this->deck_list2->setCurrentIndex(index2);
	}
	if (index3 != -1) {
		this->deck_list3->setCurrentIndex(index3);
	}
	auto *decklist_layout = new QHBoxLayout();
	decklist_layout->addWidget(this->deck_list1);
	decklist_layout->addWidget(this->deck_list2);
	decklist_layout->addWidget(this->deck_list3);
	layout->addLayout(decklist_layout);

	this->minimum_out_of_screen_time->setValue(minimum_out_of_screen_time_value);
	auto *minimum_out_of_screen_label = new QLabel(obs_module_text("out_of_screen"), this);
	layout->addWidget(minimum_out_of_screen_label);
	layout->addWidget(this->minimum_out_of_screen_time);

	this->minimum_screen_time->setValue(minimum_screen_time_value);
	auto *minimum_screen_label = new QLabel(obs_module_text("in_screen"), this);
	layout->addWidget(minimum_screen_label);
	layout->addWidget(this->minimum_screen_time);

	auto *confidence_label = new QLabel(obs_module_text("confidence"), this);
	layout->addWidget(confidence_label);
	QHBoxLayout *confidence_layout = new QHBoxLayout;
	this->confidence_slider->setValue(confidence_value);
	auto *confidence_value_label = new QLabel(QString::number(confidence_value) + "%", this);
	confidence_layout->addWidget(confidence_value_label);
	confidence_layout->addWidget(this->confidence_slider);
	layout->addLayout(confidence_layout);

	this->ok_button->setProperty("class", "QPushButton");
	this->cancel_button->setProperty("class", "QPushButton");

	auto *buttons_layout = new QHBoxLayout();
	buttons_layout->addStretch();
	buttons_layout->addWidget(this->cancel_button);
	buttons_layout->addWidget(this->ok_button);
	layout->addLayout(buttons_layout);

	setLayout(layout);
	connect(python_browse_button, SIGNAL(clicked()), SLOT(PythonBrowseButtonClicked()));
	connect(browse_button, SIGNAL(clicked()), SLOT(BrowseButtonClicked()));
	connect(ok_button, SIGNAL(clicked()), SLOT(OkButtonClicked()));
	connect(cancel_button, SIGNAL(clicked()), SLOT(CancelButtonClicked()));
	connect(this->confidence_slider, &QSlider::valueChanged,
		[confidence_value_label](int value) { confidence_value_label->setText(QString::number(value) + "%"); });

	resize(400, 50);
}

void SettingsDialog::PythonBrowseButtonClicked()
{
	QString folderPath =
		QFileDialog::getExistingDirectory(this, "Select Python Installation Folder", QString(),
						  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!folderPath.isEmpty()) {
		this->python_path->setText(folderPath);
	}
}

void SettingsDialog::BrowseButtonClicked()
{
	const char *plugin_dir = get_plugin_path();
	blog(LOG_INFO, "%s", plugin_dir);
	open_folder(plugin_dir + std::string("/decklists/"));
}

void SettingsDialog::OkButtonClicked()
{
	QSettings settings("HichTala", "Draw2");

	settings.setValue("deck_list1", this->deck_list1->currentText());
	settings.setValue("deck_list2", this->deck_list2->currentText());
	settings.setValue("deck_list3", this->deck_list3->currentText());
	settings.setValue("python_path", this->python_path->text());
	settings.setValue("minimum_screen_time", this->minimum_screen_time->value());
	settings.setValue("minimum_out_of_screen_time", this->minimum_out_of_screen_time->value());
	settings.setValue("confidence_slider", this->confidence_slider->value());
	this->close();
}

void SettingsDialog::CancelButtonClicked()
{
	this->close();
}
