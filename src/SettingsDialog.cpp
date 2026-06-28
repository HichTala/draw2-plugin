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
	int model_choice_int = settings.value("model_choice", 0).toInt();
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

	auto *model_layout = new QHBoxLayout();
	auto *model_label = new QLabel(obs_module_text("Select model size to use:"), this);
	model_layout->addWidget(model_label);
	this->model_choice->setMaximumWidth(125);
	this->model_choice->addItem(obs_module_text("model_size_base"));
	this->model_choice->addItem(obs_module_text("model_size_large"));
	this->model_choice->setCurrentIndex(model_choice_int);
	model_layout->addWidget(this->model_choice);
	layout->addLayout(model_layout);

	auto *browse_layout = new QHBoxLayout();
	auto *label = new QLabel(obs_module_text("deck_list"), this);
	browse_layout->addWidget(label);
	this->browse_button->setMaximumWidth(125);
	browse_layout->addWidget(this->browse_button);
	layout->addLayout(browse_layout);

	QDir dir(get_decklists_path());
	QFileInfoList files = dir.entryInfoList(QDir::Files);
	QComboBox *combos[3] = {this->deck_list1, this->deck_list2, this->deck_list3};
	QString saved[3] = {deck_list_path1, deck_list_path2, deck_list_path3};
	for (int i = 0; i < 3; i++) {
		combos[i]->setMaximumWidth(125);
		combos[i]->addItem(obs_module_text("none"));
		for (const QFileInfo &file : files)
			combos[i]->addItem(file.fileName());
		int idx = combos[i]->findText(saved[i], Qt::MatchExactly);
		if (idx != -1)
			combos[i]->setCurrentIndex(idx);
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

	// Opt-in detector-input features (off by default).
	auto *features_label = new QLabel(obs_module_text("advanced_features"), this);
	layout->addWidget(features_label);
	this->feature_crop->setChecked(settings.value("feature_crop", false).toBool());
	this->feature_rotate->setChecked(settings.value("feature_rotate", false).toBool());
	layout->addWidget(this->feature_crop);
	layout->addWidget(this->feature_rotate);

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
	open_folder(std::string(get_decklists_path()));
}

void SettingsDialog::OkButtonClicked()
{
	QSettings settings("HichTala", "Draw2");

	settings.setValue("deck_list1", this->deck_list1->currentText());
	settings.setValue("deck_list2", this->deck_list2->currentText());
	settings.setValue("deck_list3", this->deck_list3->currentText());
	settings.setValue("model_choice", this->model_choice->currentIndex());
	settings.setValue("python_path", this->python_path->text());
	settings.setValue("minimum_screen_time", this->minimum_screen_time->value());
	settings.setValue("minimum_out_of_screen_time", this->minimum_out_of_screen_time->value());
	settings.setValue("confidence_slider", this->confidence_slider->value());
	settings.setValue("feature_crop", this->feature_crop->isChecked());
	settings.setValue("feature_rotate", this->feature_rotate->isChecked());
	this->close();
}

void SettingsDialog::CancelButtonClicked()
{
	this->close();
}
