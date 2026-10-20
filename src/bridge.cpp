//
// Bridge between the C source code (draw.c) and the C++ dock. See bridge.h.
//

#include "bridge.h"

#include <QSettings>
#include <QString>

extern "C" bool draw_feature_enabled(const char *key)
{
	if (!key)
		return false;
	// Same store the settings dialog writes to.
	QSettings settings("HichTala", "Draw2");
	return settings.value(QString::fromUtf8(key), false).toBool();
}

// To use if preview interfere with detection in the future
// void draw_dock_set_preview(const void *source, bool active)
// {
// 	if (DrawDock::instance) {
// 		QMetaObject::invokeMethod(DrawDock::instance.data(), "SetPreviewActive", Qt::QueuedConnection,
// 					  Q_ARG(quintptr, reinterpret_cast<quintptr>(source)), Q_ARG(bool, active));
// 	}
// }
