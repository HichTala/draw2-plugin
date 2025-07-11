//
// Created by HichTala on 22/06/25.
//

#ifndef SETTINGSPOPUP_HPP
#define SETTINGSPOPUP_HPP

#include "DrawDock.hpp"

#include <QDialog>
#include <QFileDialog>
#include <QLineEdit>
#include <QSlider>
#include <QSpinBox>

class SettingsDialog : public QDialog {
	Q_OBJECT

public:
	explicit SettingsDialog(QWidget *parent = nullptr);

private:
	QLineEdit *python_path = new QLineEdit();
	QLineEdit *deck_list = new QLineEdit();
	QSpinBox *minimum_out_of_screen_time = new QSpinBox;
	QSpinBox *minimum_screen_time = new QSpinBox;
	QSlider *confidence_slider = new QSlider(Qt::Horizontal);
	QPushButton *python_browse_button = new QPushButton("Browse");
	QPushButton *browse_button = new QPushButton("Browse");
	QPushButton *ok_button = new QPushButton("&OK");
	QPushButton *cancel_button = new QPushButton("&Cancel");

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
