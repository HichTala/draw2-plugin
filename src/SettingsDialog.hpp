//
// Created by HichTala on 22/06/25.
//

#ifndef SETTINGSPOPUP_HPP
#define SETTINGSPOPUP_HPP

#include "DrawDock.hpp"
#include "SettingsDialog.hpp"

#include <QCheckBox>
#include <QDialog>
#include <QFileDialog>
#include <QLineEdit>
#include <QSlider>
#include <QSpinBox>
#include <QStackedWidget>
#include <QComboBox>
#include <obs-module.h>

class SettingsDialog : public QDialog {
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget *parent = nullptr);

private:
	QLineEdit *python_path = new QLineEdit();
	// Player 1 deck lists
	QComboBox *deck_list1 = new QComboBox();
	QComboBox *deck_list2 = new QComboBox();
	QComboBox *deck_list3 = new QComboBox();
	// Player 2 deck lists
	QComboBox *deck_list1_p2 = new QComboBox();
	QComboBox *deck_list2_p2 = new QComboBox();
	QComboBox *deck_list3_p2 = new QComboBox();
	QComboBox *model_choice = new QComboBox();
	QSpinBox *minimum_out_of_screen_time = new QSpinBox;
	QSpinBox *minimum_screen_time = new QSpinBox;
	QSlider *confidence_slider = new QSlider(Qt::Horizontal);
	QWidget *player2_section = new QWidget();
	// Opt-in features (off by default). See bridge.h.
	QCheckBox *feature_channel = new QCheckBox(obs_module_text("feature_channel"));
	QCheckBox *feature_crop = new QCheckBox(obs_module_text("feature_crop"));
	QCheckBox *feature_debug = new QCheckBox(obs_module_text("feature_debug"));
	QCheckBox *feature_remote_deck = new QCheckBox(obs_module_text("feature_remote_deck"));
	// Remote decklist (opt-in): each player picks its own deck source — files,
	// a URL or a ydke:// code — so e.g. Player 1 can be remote and Player 2 local.
	QComboBox *deck_source = new QComboBox();
	QComboBox *deck_source_p2 = new QComboBox();
	QStackedWidget *deck_stack = new QStackedWidget();
	QStackedWidget *deck_stack_p2 = new QStackedWidget();
	QLineEdit *deck_url1 = new QLineEdit();
	QLineEdit *deck_url1_p2 = new QLineEdit();
	QLineEdit *deck_ydke = new QLineEdit();
	QLineEdit *deck_ydke_p2 = new QLineEdit();
	QLineEdit *remote_header_name = new QLineEdit();
	QLineEdit *remote_header_value = new QLineEdit();
	QPushButton *import_url_button = new QPushButton(obs_module_text("import_from_url"));
	QWidget *remote_section = new QWidget();
	void UpdateDeckSourceVisibility();
	QPushButton *python_browse_button = new QPushButton(obs_module_text("browse"));
	QPushButton *browse_button = new QPushButton(obs_module_text("open_folder"));
	QPushButton *ok_button = new QPushButton(obs_module_text("ok"));
	QPushButton *cancel_button = new QPushButton(obs_module_text("cancel"));

	// char *deck_list_path = nullptr;
	// int minimum_out_of_screen_time_value;
	// int minimum_screen_time_value;
	// int confidence_value;

private slots:
	void PythonBrowseButtonClicked();
	void BrowseButtonClicked();
	void OkButtonClicked();
	void CancelButtonClicked();
	void ImportUrlButtonClicked();
};

#endif //SETTINGSPOPUP_HPP
