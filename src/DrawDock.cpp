//
// Created by HichTala on 21/06/25.
//

#include "plugin-path.h"
#include "bridge.h"

#include "DrawDock.hpp"
#include "SettingsDialog.hpp"

#include <QSettings>
#include <QFileInfo>
#include <QStringList>
#include <QFont>
#include <QTime>
#include <obs-module.h>

#ifndef _WIN32
#include <csignal>
#include <unistd.h>
#endif

QPointer<DrawDock> DrawDock::instance;

DrawDock::DrawDock(QWidget *parent) : QWidget(parent)
{
	this->parent = parent;

	this->setProperty("class", "dock-widget");

	auto *layout = new QVBoxLayout(this);
	layout->setContentsMargins(5, 4, 5, 4);
	layout->setSpacing(4);

	auto *buttons = new QHBoxLayout();
	buttons->setSpacing(4);

	this->start_button->setText(obs_module_text("start_draw"));
	this->start_button->setCheckable(true);
	this->start_button->setProperty("class", "start-streaming");
	buttons->addWidget(this->start_button);

	this->settings_button->setProperty("class", "icon-gear");
	this->settings_button->setEnabled(true);
	this->settings_button->setToolTip("Settings");
	this->settings_button->setFixedHeight(this->start_button->sizeHint().height());
	this->settings_button->setFixedWidth(this->start_button->sizeHint().height());
	buttons->addWidget(this->settings_button);

	layout->addLayout(buttons);

	this->log_view->setReadOnly(true);
	this->log_view->setMaximumBlockCount(500); // cap memory: keep last 500 lines
	this->log_view->setLineWrapMode(QPlainTextEdit::NoWrap);
	this->log_view->setPlaceholderText(obs_module_text("log_placeholder"));
	this->log_view->document()->setDefaultFont(QFont("Menlo", 10));
	layout->addWidget(this->log_view, 1);

	resize(360, 300);

	connect(start_button, SIGNAL(clicked()), SLOT(StartButtonClicked()));
	connect(settings_button, SIGNAL(clicked()), SLOT(SettingsButtonClicked()));

	instance = this;
}

DrawDock::~DrawDock()
{
	StopPythonDraw();
}

void DrawDock::ResetUi()
{
	this->ready_count = 0;
	this->launched_count = 0;
	this->start_button->setChecked(false);
	RefreshStartButton();
	this->settings_button->setEnabled(true);
}

void DrawDock::AppendLog(const QString &line)
{
	this->log_view->appendPlainText(QTime::currentTime().toString("HH:mm:ss") + "  " + line);
}

void DrawDock::StartButtonClicked()
{
	if (this->start_button->isChecked()) {
		this->start_button->setDisabled(true);
		this->start_button->setText(obs_module_text("starting_draw"));
		this->settings_button->setEnabled(false);
		StartPythonDraw();
	} else {
		StopPythonDraw();
		RefreshStartButton();
		this->settings_button->setEnabled(true);
	}
}

void DrawDock::SettingsButtonClicked()
{
	auto *settings_dialog = new SettingsDialog(this);
	settings_dialog->exec();
}

void DrawDock::StartPythonDraw()
{
	if (this->draw_process[0] || this->draw_process[1])
		return;

	QSettings settings("HichTala", "Draw2");

	QString python_home = settings.value("python_path", "").toString();
#ifdef _WIN32
	QString python_exe = python_home + "/python.exe";
#else
	QString python_exe = python_home + "/bin/python";
#endif
	if (python_home.isEmpty() || !QFileInfo::exists(python_exe)) {
		blog(LOG_ERROR, "Draw2: invalid Python installation '%s' (expected '%s')",
		     python_home.toUtf8().constData(), python_exe.toUtf8().constData());
		AppendLog(QString("✗ ") + obs_module_text("invalid_python_path"));
		QMessageBox::warning(this, "Draw 2", obs_module_text("invalid_python_path"));
		ResetUi();
		return;
	}

	this->ready_count = 0;
	this->launched_count = 0;

	// Opt-in: a second detector (player 2) only when the feature is enabled.
	bool two_players = draw_feature_enabled(FEATURE_CHANNEL);
	AppendLog(two_players ? "▶ Launching detectors for 2 players… (loading models, first run may take a while)"
			      : "▶ Launching the card detector… (loading models, first run may take a while)");

	StartChannel(1, python_exe);
	if (two_players)
		StartChannel(2, python_exe);
}

void DrawDock::StartChannel(int channel, const QString &python_exe)
{
	QSettings settings("HichTala", "Draw2");

	int model_choice = settings.value("model_choice", 0).value<int>();
	int min_out = settings.value("minimum_out_of_screen_time", 25).value<int>();
	int min_screen = settings.value("minimum_screen_time", 6).value<int>();
	int confidence = settings.value("confidence_slider", 5).value<int>();

	// Per-player deck lists: player 1 uses deck_listN, player 2 deck_listN_p2.
	QString sfx = (channel == 2) ? "_p2" : "";
	QString dir = QString::fromUtf8(get_decklists_path()) + "/";
	QString deck_list = dir + settings.value("deck_list1" + sfx, "").toString() + ";" + dir +
			    settings.value("deck_list2" + sfx, "").toString() + ";" + dir +
			    settings.value("deck_list3" + sfx, "").toString() + ";";

	auto *process = new QProcess(this);
	process->setProcessChannelMode(QProcess::MergedChannels);
	this->draw_process[channel - 1] = process;
	this->launched_count++;

	connect(process, &QProcess::readyReadStandardOutput, this, [this, process, channel]() {
		while (process->canReadLine()) {
			QByteArray line = process->readLine().trimmed();
			if (line.isEmpty())
				continue;
			blog(LOG_INFO, "[draw2-backend P%d] %s", channel, line.constData());
			// Debug mode surfaces every backend line; otherwise filter the noisy ones.
			if (draw_feature_enabled(FEATURE_DEBUG) ||
			    (!line.contains("unauthenticated requests") && !line.contains("use_fast") &&
			     !line.contains("Loading weights") && !line.startsWith("mapped size"))) {
				AppendLog(QString("[P%1] ").arg(channel) + QString::fromUtf8(line));
			}
			if (line.contains("Waiting for OBS to start")) {
				ready_count++;
				if (ready_count >= launched_count) {
					start_button->setEnabled(true);
					start_button->setText(obs_module_text("stop_draw"));
				}
			}
		}
	});

	connect(process, &QProcess::errorOccurred, this, [this, channel](QProcess::ProcessError error) {
		blog(LOG_ERROR, "Draw2 backend P%d process error (%d)", channel, (int)error);
		AppendLog(QString("✗ [P%1] backend error (%2)").arg(channel).arg((int)error));
	});

	connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this,
		[this, channel](int code, QProcess::ExitStatus status) {
			blog(LOG_INFO, "Draw2 backend P%d exited (code %d, status %d)", channel, code, (int)status);
			AppendLog(QString("■ [P%1] backend exited (code %2)").arg(channel).arg(code));
			if (draw_process[channel - 1]) {
				draw_process[channel - 1]->deleteLater();
				draw_process[channel - 1] = nullptr;
			}
			// Reset the UI only once both detectors are gone.
			if (!draw_process[0] && !draw_process[1])
				ResetUi();
		});

	// -u: unbuffered so the backend's progress lines reach us immediately.
	QStringList args;

	args << "-u" << "-m" << "draw"
	     << "--model_size" << QString::number(model_choice) << "--deck_list" << deck_list
	     << "--minimum_out_of_screen_time" << QString::number(min_out) << "--minimum_screen_time"
	     << QString::number(min_screen) << "--confidence_threshold" << QString::number(confidence) << "--channel"
	     << QString::number(channel);

	blog(LOG_INFO, "Draw2: launching backend P%d: %s", channel, python_exe.toUtf8().constData());

	// Debug mode: surface the exact launch parameters in the dock log, so the
	// resolved deck paths / channel / thresholds are visible without digging.
	if (draw_feature_enabled(FEATURE_DEBUG)) {
		AppendLog(QString("· [P%1] debug: python=%2").arg(channel).arg(python_exe));
		AppendLog(QString("· [P%1] debug: model=%2 channel=%3 confidence=%4 min_out=%5 min_screen=%6")
				  .arg(channel)
				  .arg(model_choice)
				  .arg(channel)
				  .arg(confidence)
				  .arg(min_out)
				  .arg(min_screen));
		AppendLog(QString("· [P%1] debug: deck_list=%2").arg(channel).arg(deck_list));
	}

	process->start(python_exe, args);
}

void DrawDock::StopPythonDraw()
{
	bool any = false;
	for (int i = 0; i < 2; i++) {
		QProcess *process = this->draw_process[i];
		if (!process)
			continue;
		any = true;
		this->draw_process[i] = nullptr;

		// Deliberate teardown: detach handlers so finished/error don't fire.
		process->disconnect(this);

		if (process->state() != QProcess::NotRunning) {
#ifndef _WIN32
			// SIGINT lets the backend close shared memory cleanly.
			::kill((pid_t)process->processId(), SIGINT);
#else
			process->terminate();
#endif
			if (!process->waitForFinished(5000)) {
				process->kill();
				process->waitForFinished(2000);
			}
		}
		process->deleteLater();
	}
	if (any)
		AppendLog("■ Stopping detectors…");
	this->ready_count = 0;
	this->launched_count = 0;
}

void DrawDock::SetPreviewActive(quintptr source, bool active)
{
	if (active)
		this->previewing_sources.insert(source);
	else
		this->previewing_sources.remove(source);
	RefreshStartButton();
}

// The preview only blocks starting detection. While detectors are starting or
// running (button checked) the button keeps its Starting/Stop state, so it can
// always be used to stop them.
void DrawDock::RefreshStartButton()
{
	if (this->start_button->isChecked())
		return;
	bool previewing = !this->previewing_sources.isEmpty();
	this->start_button->setEnabled(!previewing);
	this->start_button->setText(obs_module_text(previewing ? "preview" : "start_draw"));
}
