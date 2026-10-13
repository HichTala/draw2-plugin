//
// Created by HichTala on 21/06/25.
//

#ifndef DRAW_DOCK_H
#define DRAW_DOCK_H

#include <QDockWidget>
#include <QWidget>
#include <QPointer>
#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QApplication>
#include <QIcon>
#include <QStyle>
#include <QProcess>

class DrawDock : public QWidget {
	Q_OBJECT

public:
	explicit DrawDock(QWidget *parent = nullptr);
	~DrawDock() override;
	static QPointer<DrawDock> instance;

private:
	QWidget *parent = nullptr;
	QPushButton *start_button = new QPushButton();
	QPushButton *settings_button = new QPushButton();

	// The Python backend runs as a separate process (it is NOT embedded in
	// OBS). Embedding libpython clashes with OBS' own scripting symbols on
	// macOS, so we spawn the interpreter and communicate through the shared
	// memory the Draw Display source already uses.
	// One backend process per player/channel (index 0 == channel 1).
	QProcess *draw_process[2] = {nullptr, nullptr};
	int launched_count = 0;
	int ready_count = 0;
	QPlainTextEdit *log_view = new QPlainTextEdit();

	void ResetUi();
	void AppendLog(const QString &line);
	void StartChannel(int channel, const QString &python_exe);

public slots:
	void SetButtonEnabled(bool clickable) const;

private slots:
	void StartButtonClicked();
	void SettingsButtonClicked();
	void StartPythonDraw();
	void StopPythonDraw();
};

#endif //DRAW_DOCK_H
