//
// Created by HichTala on 22/06/25.
//

#include <QFileInfo>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QSettings>
#include <QStackedWidget>
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
	QString deck_ydke_v = settings.value("deck_ydke", "").toString();
	QString deck_ydke_p2_v = settings.value("deck_ydke_p2", "").toString();
	int deck_source_v = remote_deck::saved_source(settings, "");
	int deck_source_p2_v = remote_deck::saved_source(settings, "_p2");
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

	// --- Decklist tab: per player, deck files OR (remote on) a URL / ydke code. ---
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

	auto *browse_layout = new QHBoxLayout();
	auto *label = new QLabel(obs_module_text("deck_list"), this);
	browse_layout->addWidget(label);
	this->browse_button->setMaximumWidth(135);
	browse_layout->addWidget(this->browse_button);
	decklist_tab_layout->addLayout(browse_layout);

	// One block per player: a source picker (shown only when remote decklist
	// is on) and a stacked page with the matching input — the 3 deck files, a
	// URL or a ydke:// code. Each player picks independently.
	auto build_player = [this](QVBoxLayout *parent, const char *title, QComboBox *source, QStackedWidget *stack,
				   QComboBox *const files[3], QLineEdit *url, const QString &url_v, QLineEdit *ydke,
				   const QString &ydke_v, int source_v) {
		auto *header = new QHBoxLayout();
		header->addWidget(new QLabel(obs_module_text(title), this));
		source->addItem(obs_module_text("deck_source_files"));
		source->addItem(obs_module_text("deck_source_url"));
		source->addItem(obs_module_text("deck_source_ydke"));
		source->setCurrentIndex(source_v);
		header->addWidget(source);
		header->addStretch();
		parent->addLayout(header);

		auto *files_page = new QWidget();
		auto *files_layout = new QHBoxLayout(files_page);
		files_layout->setContentsMargins(0, 0, 0, 0);
		for (int i = 0; i < 3; i++)
			files_layout->addWidget(files[i]);
		url->setText(url_v);
		url->setPlaceholderText(obs_module_text("remote_url_ph"));
		ydke->setText(ydke_v);
		ydke->setPlaceholderText(obs_module_text("ydke_ph"));
		stack->addWidget(files_page);
		stack->addWidget(url);
		stack->addWidget(ydke);
		// Keep the row compact instead of soaking up the tab's spare height.
		stack->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
		parent->addWidget(stack);

		connect(source, &QComboBox::currentIndexChanged, this, [this] { UpdateDeckSourceVisibility(); });
	};

	QComboBox *const files_p1[3] = {this->deck_list1, this->deck_list2, this->deck_list3};
	build_player(decklist_tab_layout, "player_1", this->deck_source, this->deck_stack, files_p1, this->deck_url1,
		     deck_url1_v, this->deck_ydke, deck_ydke_v, deck_source_v);

	// Player 2: shown only when two-player mode is enabled (toggle lives in
	// the Feature flags tab).
	auto *p2_layout = new QVBoxLayout(this->player2_section);
	p2_layout->setContentsMargins(0, 0, 0, 0);
	QComboBox *const files_p2[3] = {this->deck_list1_p2, this->deck_list2_p2, this->deck_list3_p2};
	build_player(p2_layout, "player_2", this->deck_source_p2, this->deck_stack_p2, files_p2, this->deck_url1_p2,
		     deck_url1_p2_v, this->deck_ydke_p2, deck_ydke_p2_v, deck_source_p2_v);
	this->player2_section->setVisible(feature_channel_value);
	decklist_tab_layout->addWidget(this->player2_section);

	// Shared remote settings (auth header for URL sources) and the one-off
	// importer. Shown only when the feature is on (toggle in Feature flags).
	{
		auto *remote_layout = new QVBoxLayout(this->remote_section);
		remote_layout->setContentsMargins(0, 0, 0, 0);

		auto *remote_label = new QLabel(obs_module_text("remote_deck_section"), this);
		remote_layout->addWidget(remote_label);

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
	this->feature_card_info->setChecked(settings.value("feature_card_info", false).toBool());
	// Full local card DB defaults ON (offline, no per-card requests).
	this->feature_card_info_db->setChecked(settings.value("feature_card_info_db", true).toBool());
	features_layout->addWidget(this->feature_channel);
	features_layout->addWidget(this->feature_crop);
	features_layout->addWidget(this->feature_debug);
	features_layout->addWidget(this->feature_remote_deck);
	features_layout->addWidget(this->feature_card_info);
	features_layout->addWidget(this->feature_card_info_db);
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
	// Two-player toggle reveals the Player 2 deck block.
	connect(this->feature_channel, &QCheckBox::toggled, this->player2_section, &QWidget::setVisible);
	// Remote toggle reveals the per-player source pickers and remote settings.
	connect(this->feature_remote_deck, &QCheckBox::toggled, this, [this] { UpdateDeckSourceVisibility(); });
	connect(this->import_url_button, SIGNAL(clicked()), SLOT(ImportUrlButtonClicked()));
	connect(this->confidence_slider, &QSlider::valueChanged,
		[confidence_value_label](int value) { confidence_value_label->setText(QString::number(value) + "%"); });

	UpdateDeckSourceVisibility();
	resize(400, 50);
}

void SettingsDialog::UpdateDeckSourceVisibility()
{
	// With remote decklist off every player uses deck files (previous
	// behaviour); the saved source is kept for when it is turned back on.
	const bool remote = this->feature_remote_deck->isChecked();
	this->deck_source->setVisible(remote);
	this->deck_source_p2->setVisible(remote);
	this->deck_stack->setCurrentIndex(remote ? this->deck_source->currentIndex() : remote_deck::SourceFiles);
	this->deck_stack_p2->setCurrentIndex(remote ? this->deck_source_p2->currentIndex() : remote_deck::SourceFiles);
	this->remote_section->setVisible(remote);
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
	settings.setValue("feature_card_info", this->feature_card_info->isChecked());
	settings.setValue("feature_card_info_db", this->feature_card_info_db->isChecked());
	settings.setValue("deck_url1", this->deck_url1->text());
	settings.setValue("deck_url1_p2", this->deck_url1_p2->text());
	settings.setValue("deck_source", this->deck_source->currentIndex());
	settings.setValue("deck_source_p2", this->deck_source_p2->currentIndex());
	settings.setValue("deck_ydke", this->deck_ydke->text().trimmed());
	settings.setValue("deck_ydke_p2", this->deck_ydke_p2->text().trimmed());
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

	// A ydke:// code is decoded directly; anything else is fetched as a URL.
	QString error;
	QString ydk;
	if (url.trimmed().startsWith("ydke://")) {
		ydk = remote_deck::ydke_to_ydk(url, error);
	} else {
		QByteArray body = remote_deck::fetch(url.trimmed(), this->remote_header_name->text(),
						     this->remote_header_value->text(), error);
		ydk = body.isEmpty() ? QString() : remote_deck::to_ydk(body, error);
	}

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
