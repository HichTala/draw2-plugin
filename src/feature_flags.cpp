//
// Optional, opt-in plugin features. See feature_flags.h.
//

#include "feature_flags.h"

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
