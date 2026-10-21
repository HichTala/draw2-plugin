//
// Created by hicham on 13/10/2025.
//

#ifndef DRAW2_OBSPLUGIN_PLUGIN_PATH_H
#define DRAW2_OBSPLUGIN_PLUGIN_PATH_H

#pragma once

const char *get_plugin_path();

// Stable, per-user directory for deck lists (lives in OBS' plugin_config, NOT
// inside the plugin bundle) so user .ydk files survive plugin updates/reinstalls.
const char *get_decklists_path();

// Stable, per-user directory for exported card-info files (current_pN.json,
// events_pN.jsonl, current_card_pN.txt). Lives in OBS' plugin_config.
const char *get_state_path();

#endif //DRAW2_OBSPLUGIN_PLUGIN_PATH_H
