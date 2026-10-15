//
// Remote decklist support: fetch card IDs from a URL and normalize them into
// a .ydk file. OBS-free on purpose (so it is unit-testable); callers do logging.
//
#ifndef DRAW_REMOTE_DECK_HPP
#define DRAW_REMOTE_DECK_HPP

#include <QByteArray>
#include <QString>

namespace remote_deck {

// Convert an endpoint response body into valid .ydk text.
// Accepts: JSON array of IDs, JSON object {main,extra,side}, raw .ydk text,
// or loose text with digit runs. Returns "" and sets `error` if no IDs found.
QString to_ydk(const QByteArray &body, QString &error);

// Write `ydk` text to `path` as UTF-8 (truncating). Returns true on success.
bool write_ydk(const QString &path, const QString &ydk);

// HTTP GET `url` with optional single header (ignored if `headerName` empty),
// ~10s timeout, following safe redirects. Returns the body on HTTP 200,
// otherwise "" and sets `error`.
QByteArray fetch(const QString &url, const QString &headerName, const QString &headerValue, QString &error);

} // namespace remote_deck

#endif // DRAW_REMOTE_DECK_HPP
