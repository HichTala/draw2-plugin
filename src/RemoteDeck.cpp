#include "RemoteDeck.hpp"

#include <QEventLoop>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QRegularExpression>
#include <QStringList>
#include <QTimer>
#include <QUrl>

namespace {

// Pull integer-looking IDs out of a JSON array (numbers or numeric strings).
QStringList ids_from_json_array(const QJsonArray &arr)
{
	QStringList ids;
	for (const QJsonValue v : arr) {
		if (v.isDouble()) {
			ids << QString::number((qint64)v.toDouble());
		} else if (v.isString()) {
			const QString s = v.toString().trimmed();
			static const QRegularExpression digits("^\\d+$");
			if (digits.match(s).hasMatch())
				ids << s;
		}
	}
	return ids;
}

QString build_ydk(const QStringList &main, const QStringList &extra, const QStringList &side)
{
	QString out = "#main\n";
	for (const QString &id : main)
		out += id + "\n";
	out += "#extra\n";
	for (const QString &id : extra)
		out += id + "\n";
	out += "!side\n";
	for (const QString &id : side)
		out += id + "\n";
	return out;
}

} // namespace

namespace remote_deck {

QString to_ydk(const QByteArray &body, QString &error)
{
	const QByteArray trimmed = body.trimmed();
	if (trimmed.isEmpty()) {
		error = "empty response";
		return QString();
	}

	// 1) Try JSON.
	QJsonParseError perr{};
	const QJsonDocument doc = QJsonDocument::fromJson(trimmed, &perr);
	if (perr.error == QJsonParseError::NoError) {
		if (doc.isArray()) {
			const QStringList main = ids_from_json_array(doc.array());
			if (main.isEmpty()) {
				error = "no card IDs found";
				return QString();
			}
			return build_ydk(main, {}, {});
		}
		if (doc.isObject()) {
			const QJsonObject obj = doc.object();
			const QStringList main = ids_from_json_array(obj.value("main").toArray());
			const QStringList extra = ids_from_json_array(obj.value("extra").toArray());
			const QStringList side = ids_from_json_array(obj.value("side").toArray());
			if (main.isEmpty() && extra.isEmpty() && side.isEmpty()) {
				error = "no card IDs found";
				return QString();
			}
			return build_ydk(main, extra, side);
		}
	}

	// 2) Already a .ydk? Pass through — but only if there is at least one ID line.
	const QString text = QString::fromUtf8(trimmed);
	const QString firstLine = text.section('\n', 0, 0).trimmed();
	if (text.contains("#main") || firstLine.startsWith('#') || firstLine.startsWith('!')) {
		static const QRegularExpression idLine("^\\d+$");
		bool hasId = false;
		for (const QString &line : text.split('\n')) {
			if (idLine.match(line.trimmed()).hasMatch()) {
				hasId = true;
				break;
			}
		}
		if (!hasId) {
			error = "no card IDs found";
			return QString();
		}
		return text.endsWith('\n') ? text : text + "\n";
	}

	// 3) Loose text: reject HTML/XML, then take every digit run as a main-deck ID.
	if (text.trimmed().startsWith('<')) {
		error = "unrecognized response format";
		return QString();
	}
	static const QRegularExpression digits("\\d+");
	QStringList main;
	auto it = digits.globalMatch(text);
	while (it.hasNext())
		main << it.next().captured(0);
	if (main.isEmpty()) {
		error = "no card IDs found";
		return QString();
	}
	return build_ydk(main, {}, {});
}

bool write_ydk(const QString &path, const QString &ydk)
{
	QFile f(path);
	if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
		return false;
	const QByteArray bytes = ydk.toUtf8();
	const bool ok = f.write(bytes) == bytes.size();
	f.close();
	return ok;
}

QByteArray fetch(const QString &url, const QString &headerName, const QString &headerValue, QString &error)
{
	QNetworkAccessManager mgr;
	QNetworkRequest req{QUrl(url)};
	req.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::NoLessSafeRedirectPolicy);
	if (!headerName.trimmed().isEmpty())
		req.setRawHeader(headerName.trimmed().toUtf8(), headerValue.toUtf8());

	QNetworkReply *reply = mgr.get(req);

	QEventLoop loop;
	QTimer timer;
	timer.setSingleShot(true);
	QObject::connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
	QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
	timer.start(10000);
	loop.exec();

	if (!reply->isFinished()) {
		reply->abort();
		error = "request timed out";
		reply->deleteLater();
		return {};
	}
	if (reply->error() != QNetworkReply::NoError) {
		error = reply->errorString();
		reply->deleteLater();
		return {};
	}
	const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
	const QByteArray body = reply->readAll();
	reply->deleteLater();
	if (status != 0 && status != 200) {
		error = QString("HTTP %1").arg(status);
		return {};
	}
	return body;
}

} // namespace remote_deck
