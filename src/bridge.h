//
// Optional, opt-in plugin features.
//
// Features added on top of the upstream plugin are gated behind toggles that
// live in the Draw 2 settings dialog and default to OFF, so the plugin behaves
// exactly like upstream until the user opts in. This header exposes the toggle
// state to both the C source code (draw.c) and the C++ code.
//

#ifndef DRAW_BRIDGE_H
#define DRAW_BRIDGE_H

#pragma once
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Returns whether an optional feature is enabled. Features are opt-in: when the
// key has never been set (or is set to false) this returns false. Backed by the
// shared QSettings("HichTala", "Draw2") store written by the settings dialog.
bool draw_feature_enabled(const char *key);

// Tells the dock whether a Draw Display source is showing its input preview.
// `source` only identifies the caller (each source reports for itself); the
// Start button stays disabled while any source is previewing.
void draw_dock_set_preview(const void *source, bool active);

// QSettings keys for the optional features (keep in sync with SettingsDialog).
#define FEATURE_CHANNEL "feature_channel"
#define FEATURE_CROP "feature_crop"
#define FEATURE_DEBUG "feature_debug"

#ifdef __cplusplus
}
#endif

#endif // DRAW_BRIDGE_H
