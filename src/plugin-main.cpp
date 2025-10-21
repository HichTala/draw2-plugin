/*
Detecting and Recognizing A Wide range of cards (DRAW) v2 - plugin for OBS Studio
Copyright (C) 2025 HichTala hich.tala.phd@gmail.com

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

#include <QWidget>
#include <obs-frontend-api.h>
#include <obs-module.h>
#include <plugin-support.h>
#include <plugin-path.h>

#include "DrawDock.hpp"

extern "C" {
#include "draw.h"
}

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

static const char *module_path = nullptr;

bool obs_module_load(void)
{
	module_path = obs_get_module_data_path(obs_current_module());

	obs_log(LOG_INFO, "Loaded from: %s", module_path);

	auto *main_window = static_cast<QWidget *>(obs_frontend_get_main_window());
	auto *dock = new DrawDock(main_window);

	obs_frontend_add_dock_by_id("drawDock", obs_module_text("Draw 2"), dock);
	obs_register_source(&draw_source);
	obs_log(LOG_INFO, "plugin loaded successfully (version %s)", PLUGIN_VERSION);
	return true;
}

void obs_module_unload(void)
{
	obs_log(LOG_INFO, "plugin unloaded");
}

const char *get_plugin_path()
{
	return module_path;
}