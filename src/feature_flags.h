//
// Optional, opt-in plugin features.
//
// Features added on top of the upstream plugin are gated behind toggles that
// live in the Draw 2 settings dialog and default to OFF, so the plugin behaves
// exactly like upstream until the user opts in. This header exposes the toggle
// state to both the C source code (draw.c) and the C++ code.
//

#ifndef DRAW_FEATURE_FLAGS_H
#define DRAW_FEATURE_FLAGS_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Returns whether an optional feature is enabled. Features are opt-in: when the
// key has never been set (or is set to false) this returns false. Backed by the
// shared QSettings("HichTala", "Draw2") store written by the settings dialog.
bool draw_feature_enabled(const char *key);

// QSettings keys for the optional features (keep in sync with SettingsDialog).
#define FEATURE_CROP "feature_crop"
#define FEATURE_ROTATE "feature_rotate"

#ifdef __cplusplus
}
#endif

#endif // DRAW_FEATURE_FLAGS_H
