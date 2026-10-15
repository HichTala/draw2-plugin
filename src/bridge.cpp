//
// Bridge between the C source code (draw.c) and the C++ dock. See bridge.h.
//

#include "bridge.h"

#include "DrawDock.hpp"

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

void draw_dock_set_button_enabled(bool enable)
{
	if (DrawDock::instance) {
		QMetaObject::invokeMethod(DrawDock::instance.data(), "SetButtonEnabled", Qt::QueuedConnection,
					  Q_ARG(bool, enable));
	}
}
