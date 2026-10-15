//
// Created by HichTala on 22/06/25.
//

#include <QFileInfo>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QSettings>
#include <QTabWidget>

#include "SettingsDialog.hpp"
#include "RemoteDeck.hpp"

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
	QString deck_list_path1_p2 = settings.value("deck_list1_p2", "").toString();
	QString deck_list_path2_p2 = settings.value("deck_list2_p2", "").toString();
	QString deck_list_path3_p2 = settings.value("deck_list3_p2", "").toString();
	bool feature_channel_value = settings.value("feature_channel", false).toBool();
	int model_choice_int = settings.value("model_choice", 0).toInt();
	QString python_path_string = settings.value("python_path", "").toString();
	int minimum_out_of_screen_time_value = settings.value("minimum_out_of_screen_time", 25).value<int>();
	int minimum_screen_time_value = settings.value("minimum_screen_time", 6).value<int>();
	int confidence_value = settings.value("confidence_slider", 1).value<int>();
	bool feature_remote_deck_value = settings.value("feature_remote_deck", false).toBool();
	QString deck_url1_v = settings.value("deck_url1", "").toString();
	QString deck_url1_p2_v = settings.value("deck_url1_p2", "").toString();
	QString remote_header_name_v = settings.value("remote_header_name", "").toString();
	QString remote_header_value_v = settings.value("remote_header_value", "").toString();

	// The dialog is split into tabs so it stays within the screen as more
	// settings and opt-in features are added: "General" holds the core
	// detection settings, "Decklist" the deck files (per player) and optional
	// remote URLs, and "Feature flags" the opt-in toggles.
	auto *tabs = new QTabWidget(this);

	// --- General tab: core detection settings. ---
	auto *general_page = new QWidget();
	auto *layout = new QVBoxLayout(general_page);

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
	layout->addStretch();

	// --- Decklist tab: deck files (per player) OR a remote URL per player. ---
	auto *decklist_page = new QWidget();
	auto *decklist_tab_layout = new QVBoxLayout(decklist_page);

	QDir dir(get_decklists_path());
	QFileInfoList files = dir.entryInfoList(QDir::Files);
	QComboBox *combos[6] = {this->deck_list1,    this->deck_list2,    this->deck_list3,
				this->deck_list1_p2, this->deck_list2_p2, this->deck_list3_p2};
	QString saved[6] = {deck_list_path1,    deck_list_path2,    deck_list_path3,
			    deck_list_path1_p2, deck_list_path2_p2, deck_list_path3_p2};
	for (int i = 0; i < 6; i++) {
		combos[i]->setMaximumWidth(125);
		combos[i]->addItem(obs_module_text("none"));
		for (const QFileInfo &file : files)
			combos[i]->addItem(file.fileName());
		int idx = combos[i]->findText(saved[i], Qt::MatchExactly);
		if (idx != -1)
			combos[i]->setCurrentIndex(idx);
	}

	// File selectors: hidden while remote decklist is on.
	auto *file_layout = new QVBoxLayout(this->file_section);
	file_layout->setContentsMargins(0, 0, 0, 0);

	auto *browse_layout = new QHBoxLayout();
	auto *label = new QLabel(obs_module_text("deck_list"), this);
	browse_layout->addWidget(label);
	this->browse_button->setMaximumWidth(125);
	browse_layout->addWidget(this->browse_button);
	file_layout->addLayout(browse_layout);

	auto *p1_label = new QLabel(obs_module_text("player_1"), this);
	file_layout->addWidget(p1_label);
	auto *decklist_layout = new QHBoxLayout();
	decklist_layout->addWidget(this->deck_list1);
	decklist_layout->addWidget(this->deck_list2);
	decklist_layout->addWidget(this->deck_list3);
	file_layout->addLayout(decklist_layout);

	// Player 2 deck lists: shown only when two-player mode is enabled (toggle
	// lives in the Feature flags tab).
	auto *p2_layout = new QVBoxLayout(this->player2_section);
	p2_layout->setContentsMargins(0, 0, 0, 0);
	auto *p2_label = new QLabel(obs_module_text("player_2"), this);
	p2_layout->addWidget(p2_label);
	auto *decklist_layout_p2 = new QHBoxLayout();
	decklist_layout_p2->addWidget(this->deck_list1_p2);
	decklist_layout_p2->addWidget(this->deck_list2_p2);
	decklist_layout_p2->addWidget(this->deck_list3_p2);
	p2_layout->addLayout(decklist_layout_p2);
	this->player2_section->setVisible(feature_channel_value);
	file_layout->addWidget(this->player2_section);

	this->file_section->setVisible(!feature_remote_deck_value);
	decklist_tab_layout->addWidget(this->file_section);

	// Remote decklist section: one URL per player, replacing the file
	// selectors. Shown only when the feature is on (toggle in Feature flags).
	{
		auto *remote_layout = new QVBoxLayout(this->remote_section);
		remote_layout->setContentsMargins(0, 0, 0, 0);

		auto *remote_label = new QLabel(obs_module_text("remote_deck_section"), this);
		remote_layout->addWidget(remote_label);

		this->deck_url1->setText(deck_url1_v);
		this->deck_url1->setPlaceholderText(obs_module_text("remote_url_ph"));
		auto *p1_url_layout = new QHBoxLayout();
		p1_url_layout->addWidget(new QLabel(obs_module_text("player_1"), this));
		p1_url_layout->addWidget(this->deck_url1);
		remote_layout->addLayout(p1_url_layout);

		// Player 2 URL: shown only when two-player mode is enabled.
		auto *p2_url_layout = new QHBoxLayout(this->remote_p2_row);
		p2_url_layout->setContentsMargins(0, 0, 0, 0);
		this->deck_url1_p2->setText(deck_url1_p2_v);
		this->deck_url1_p2->setPlaceholderText(obs_module_text("remote_url_ph"));
		p2_url_layout->addWidget(new QLabel(obs_module_text("player_2"), this));
		p2_url_layout->addWidget(this->deck_url1_p2);
		this->remote_p2_row->setVisible(feature_channel_value);
		remote_layout->addWidget(this->remote_p2_row);

		this->remote_header_name->setText(remote_header_name_v);
		this->remote_header_name->setPlaceholderText(obs_module_text("remote_header_name_ph"));
		this->remote_header_value->setText(remote_header_value_v);
		this->remote_header_value->setPlaceholderText(obs_module_text("remote_header_value_ph"));
		auto *header_layout = new QHBoxLayout();
		header_layout->addWidget(this->remote_header_name);
		header_layout->addWidget(this->remote_header_value);
		remote_layout->addLayout(header_layout);

		remote_layout->addWidget(this->import_url_button);
	}
	this->remote_section->setVisible(feature_remote_deck_value);
	decklist_tab_layout->addWidget(this->remote_section);
	decklist_tab_layout->addStretch();

	// --- Feature flags tab: opt-in toggles (off by default). ---
	auto *features_page = new QWidget();
	auto *features_layout = new QVBoxLayout(features_page);
	auto *features_label = new QLabel(obs_module_text("advanced_features"), this);
	features_layout->addWidget(features_label);
	this->feature_channel->setChecked(feature_channel_value);
	this->feature_crop->setChecked(settings.value("feature_crop", false).toBool());
	this->feature_debug->setChecked(settings.value("feature_debug", false).toBool());
	this->feature_remote_deck->setChecked(feature_remote_deck_value);
	features_layout->addWidget(this->feature_channel);
	features_layout->addWidget(this->feature_crop);
	features_layout->addWidget(this->feature_debug);
	features_layout->addWidget(this->feature_remote_deck);
	features_layout->addStretch();

	tabs->addTab(general_page, obs_module_text("general_tab"));
	tabs->addTab(decklist_page, obs_module_text("decklist_tab"));
	tabs->addTab(features_page, obs_module_text("features_tab"));

	this->ok_button->setProperty("class", "QPushButton");
	this->cancel_button->setProperty("class", "QPushButton");

	auto *buttons_layout = new QHBoxLayout();
	buttons_layout->addStretch();
	buttons_layout->addWidget(this->cancel_button);
	buttons_layout->addWidget(this->ok_button);

	auto *root = new QVBoxLayout(this);
	root->addWidget(tabs);
	root->addLayout(buttons_layout);
	setLayout(root);
	connect(python_browse_button, SIGNAL(clicked()), SLOT(PythonBrowseButtonClicked()));
	connect(browse_button, SIGNAL(clicked()), SLOT(BrowseButtonClicked()));
	connect(ok_button, SIGNAL(clicked()), SLOT(OkButtonClicked()));
	connect(cancel_button, SIGNAL(clicked()), SLOT(CancelButtonClicked()));
	// Two-player toggle reveals the Player 2 deck file row and remote URL row.
	connect(this->feature_channel, &QCheckBox::toggled, this, [this](bool on) {
		this->player2_section->setVisible(on);
		this->remote_p2_row->setVisible(on);
	});
	// Remote toggle swaps the file selectors for the URL fields.
	connect(this->feature_remote_deck, &QCheckBox::toggled, this, [this](bool on) {
		this->file_section->setVisible(!on);
		this->remote_section->setVisible(on);
	});
	connect(this->import_url_button, SIGNAL(clicked()), SLOT(ImportUrlButtonClicked()));
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
	settings.setValue("deck_list1_p2", this->deck_list1_p2->currentText());
	settings.setValue("deck_list2_p2", this->deck_list2_p2->currentText());
	settings.setValue("deck_list3_p2", this->deck_list3_p2->currentText());
	settings.setValue("feature_channel", this->feature_channel->isChecked());
	settings.setValue("model_choice", this->model_choice->currentIndex());
	settings.setValue("python_path", this->python_path->text());
	settings.setValue("minimum_screen_time", this->minimum_screen_time->value());
	settings.setValue("minimum_out_of_screen_time", this->minimum_out_of_screen_time->value());
	settings.setValue("confidence_slider", this->confidence_slider->value());
	settings.setValue("feature_crop", this->feature_crop->isChecked());
	settings.setValue("feature_debug", this->feature_debug->isChecked());
	settings.setValue("feature_remote_deck", this->feature_remote_deck->isChecked());
	settings.setValue("deck_url1", this->deck_url1->text());
	settings.setValue("deck_url1_p2", this->deck_url1_p2->text());
	settings.setValue("remote_header_name", this->remote_header_name->text());
	settings.setValue("remote_header_value", this->remote_header_value->text());
	this->close();
}

void SettingsDialog::CancelButtonClicked()
{
	this->close();
}

void SettingsDialog::ImportUrlButtonClicked()
{
	bool ok = false;
	QString url = QInputDialog::getText(this, obs_module_text("import_from_url"),
					    obs_module_text("import_url_prompt"), QLineEdit::Normal, QString(), &ok);
	if (!ok || url.trimmed().isEmpty())
		return;

	QString name = QInputDialog::getText(this, obs_module_text("import_from_url"),
					     obs_module_text("import_name_prompt"), QLineEdit::Normal, QString(), &ok);
	if (!ok || name.trimmed().isEmpty())
		return;

	QString error;
	QByteArray body = remote_deck::fetch(url.trimmed(), this->remote_header_name->text(),
					     this->remote_header_value->text(), error);
	QString ydk = body.isEmpty() ? QString() : remote_deck::to_ydk(body, error);

	if (ydk.isEmpty()) {
		QMessageBox::warning(this, obs_module_text("import_from_url"),
				     QString(obs_module_text("import_failed")) + " " + error);
		return;
	}

	QString filename = QFileInfo(name.trimmed()).fileName();
	if (filename.isEmpty()) {
		QMessageBox::warning(this, obs_module_text("import_from_url"),
				     QString(obs_module_text("import_failed")) + " " + name.trimmed());
		return;
	}
	if (!filename.endsWith(".ydk"))
		filename += ".ydk";
	QString path = QString::fromUtf8(get_decklists_path()) + "/" + filename;
	if (!remote_deck::write_ydk(path, ydk)) {
		QMessageBox::warning(this, obs_module_text("import_from_url"),
				     QString(obs_module_text("import_failed")) + " " + path);
		return;
	}

	// Make the new file selectable in every combo right away.
	QComboBox *combos[6] = {this->deck_list1,    this->deck_list2,    this->deck_list3,
				this->deck_list1_p2, this->deck_list2_p2, this->deck_list3_p2};
	for (QComboBox *c : combos) {
		if (c->findText(filename, Qt::MatchExactly) == -1)
			c->addItem(filename);
	}
	QMessageBox::information(this, obs_module_text("import_from_url"), obs_module_text("import_ok"));
}
