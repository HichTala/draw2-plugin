//
// Created by HichTala on 21/06/25.
//

#ifndef DRAW_DOCK_H
#define DRAW_DOCK_H

#include <QDockWidget>
#include <QWidget>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QApplication>
#include <QIcon>
#include <QStyle>
#include <thread>

class DrawDock : public QWidget {
	Q_OBJECT

public:
	explicit DrawDock(QWidget *parent = nullptr);
	~DrawDock() override;

private:
	QWidget *parent = nullptr;
	QPushButton *start_button = new QPushButton();
	QPushButton *settings_button = new QPushButton();
	std::thread python_thread;
	std::atomic<bool> should_run = false;
	std::atomic<bool> model_ready = false;
	std::atomic<bool> running_flag = false;

private slots:
	void StartButtonClicked();
	void SettingsButtonClicked();
	void StartPythonDraw();
	void StopPythonDraw();
	void initialize_python_interpreter() const;
};

void initialize_python_interpreter();

#endif //DRAW_DOCK_H
