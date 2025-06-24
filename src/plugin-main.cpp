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

#include <obs-module.h>
#include <plugin-support.h>
#include <obs-frontend-api.h>
#include <QWidget>
#include "DrawDock.hpp"
#include "draw.h"

extern struct obs_source_info draw_filter;
extern struct obs_source_info draw_source;

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

bool obs_module_load(void)
{
	auto *main_window = static_cast<QWidget *>(obs_frontend_get_main_window());
	auto *dock = new DrawDock(main_window);

	obs_frontend_add_dock_by_id("drawDock", obs_module_text("Draw 2"), dock);
	obs_register_source(&draw_filter);
	obs_register_source(&draw_source);
	obs_log(LOG_INFO, "plugin loaded successfully (version %s)", PLUGIN_VERSION);
	return true;
}

void obs_module_unload(void)
{
	obs_log(LOG_INFO, "plugin unloaded");
}
