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
#include <QComboBox>
#include <obs-module.h>

class SettingsDialog : public QDialog {
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget *parent = nullptr);

private:
	QLineEdit *python_path = new QLineEdit();
	// Deck lists
	QComboBox *deck_list1 = new QComboBox();
	QComboBox *deck_list2 = new QComboBox();
	QComboBox *deck_list3 = new QComboBox();
	QComboBox *model_choice = new QComboBox();
	QSpinBox *minimum_out_of_screen_time = new QSpinBox;
	QSpinBox *minimum_screen_time = new QSpinBox;
	QSlider *confidence_slider = new QSlider(Qt::Horizontal);
	// Opt-in detector-input features (off by default). See feature_flags.h.
	QCheckBox *feature_crop = new QCheckBox(obs_module_text("feature_crop"));
	QCheckBox *feature_rotate = new QCheckBox(obs_module_text("feature_rotate"));
	QCheckBox *feature_debug = new QCheckBox(obs_module_text("feature_debug"));
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
};

#endif //SETTINGSPOPUP_HPP
