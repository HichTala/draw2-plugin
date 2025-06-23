//
// Created by HichTala on 21/06/25.
//

#include "DrawDock.hpp"

#include "SettingsDialog.hpp"

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
}
DrawDock::~DrawDock() = default;

void DrawDock::StartButtonClicked() const
{
	if (this->start_button->isChecked())
		this->start_button->setText("Stop Draw");
	else
		this->start_button->setText("Start Draw");
}

void DrawDock::SettingsButtonClicked()
{
	// QMessageBox::information(this, "Settings", "Settings dialog is not implemented yet.");
	auto *settings_dialog = new SettingsDialog(this);
	settings_dialog->exec();
}