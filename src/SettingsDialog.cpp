//
// Created by HichTala on 22/06/25.
//

#include <QLabel>
#include <QSettings>

#include "SettingsDialog.hpp"

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent)
{
	setWindowTitle("Draw 2 Settings");

	QSettings settings = QSettings("HichTala", "Draw2");

	QString deck_list_path = settings.value("deck_list", "").toString();
	QString python_path_string = settings.value("python_path", "").toString();
	int minimum_out_of_screen_time_value = settings.value("minimum_out_of_screen_time", 25).value<int>();
	int minimum_screen_time_value = settings.value("minimum_screen_time", 6).value<int>();
	int confidence_value = settings.value("confidence_slider", 1).value<int>();

	auto *layout = new QVBoxLayout(this);

	auto *python_label = new QLabel("Select Python installation:", this);
	layout->addWidget(python_label);

	auto *python_browse_layout = new QHBoxLayout();
	this->python_path->setText(python_path_string);
	python_browse_layout->addWidget(this->python_path);
	python_browse_layout->addWidget(this->python_browse_button);
	layout->addLayout(python_browse_layout);

	auto *label = new QLabel("Select Deck List to use (leave empty if none):", this);
	layout->addWidget(label);

	auto *browse_layout = new QHBoxLayout();
	this->deck_list->setText(deck_list_path);
	browse_layout->addWidget(this->deck_list);
	browse_layout->addWidget(this->browse_button);
	layout->addLayout(browse_layout);

	this->minimum_out_of_screen_time->setValue(minimum_out_of_screen_time_value);
	auto *minimum_out_of_screen_label = new QLabel("Minimum Out of Screen Time (seconds):", this);
	layout->addWidget(minimum_out_of_screen_label);
	layout->addWidget(this->minimum_out_of_screen_time);

	this->minimum_screen_time->setValue(minimum_screen_time_value);
	auto *minimum_screen_label = new QLabel("Minimum Screen Time (seconds):", this);
	layout->addWidget(minimum_screen_label);
	layout->addWidget(this->minimum_screen_time);

	auto *confidence_label = new QLabel("Confidence Threshold:", this);
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
	QString filePath = QFileDialog::getOpenFileName(this, "Select Deck List File");
	if (!filePath.isEmpty()) {
		this->deck_list->setText(filePath);
	}
}

void SettingsDialog::OkButtonClicked()
{
	QSettings settings("HichTala", "Draw2");

	settings.setValue("deck_list", this->deck_list->text());
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
