#include "RemoteDeck.hpp"

#include <QByteArray>
#include <QString>
#include <cstdio>

static int g_failures = 0;

static void check_eq(const char *name, const QString &got, const QString &want)
{
	if (got != want) {
		g_failures++;
		std::fprintf(stderr, "FAIL %s\n  got:  %s\n  want: %s\n", name, got.toUtf8().constData(),
			     want.toUtf8().constData());
	} else {
		std::fprintf(stderr, "ok   %s\n", name);
	}
}

static void check_true(const char *name, bool cond)
{
	if (!cond) {
		g_failures++;
		std::fprintf(stderr, "FAIL %s\n", name);
	} else {
		std::fprintf(stderr, "ok   %s\n", name);
	}
}

int main()
{
	using remote_deck::to_ydk;
	QString err;

	// JSON array of ints -> main only.
	check_eq("json_array_ints", to_ydk(QByteArray("[12345, 67890]"), err),
		 QString("#main\n12345\n67890\n#extra\n!side\n"));

	// JSON array of strings -> main only.
	check_eq("json_array_strings", to_ydk(QByteArray("[\"12345\",\"67890\"]"), err),
		 QString("#main\n12345\n67890\n#extra\n!side\n"));

	// JSON object with main/extra/side.
	check_eq("json_object_sections", to_ydk(QByteArray("{\"main\":[1,2],\"extra\":[3],\"side\":[4]}"), err),
		 QString("#main\n1\n2\n#extra\n3\n!side\n4\n"));

	// Raw .ydk passes through (must keep its IDs).
	{
		QString out = to_ydk(QByteArray("#main\n111\n#extra\n222\n!side\n"), err);
		check_true("ydk_passthrough_main", out.startsWith("#main"));
		check_true("ydk_passthrough_has111", out.contains("111"));
		check_true("ydk_passthrough_has222", out.contains("222"));
	}

	// Loose text -> main.
	check_eq("loose_text", to_ydk(QByteArray("12345, 67890"), err),
		 QString("#main\n12345\n67890\n#extra\n!side\n"));

	// Empty -> error, empty result.
	err.clear();
	check_true("empty_is_error", to_ydk(QByteArray(""), err).isEmpty() && !err.isEmpty());

	// No digits -> error, empty result.
	err.clear();
	check_true("nodigits_is_error", to_ydk(QByteArray("hello world"), err).isEmpty() && !err.isEmpty());

	// Fix 1: HTML/XML body in loose-text branch must not fabricate a deck.
	err.clear();
	check_true("html_body_is_error",
		   to_ydk(QByteArray("<html><body>error 404</body></html>"), err).isEmpty() && !err.isEmpty());

	// Fix 2: .ydk passthrough with only headers and no numeric IDs is an error.
	err.clear();
	check_true("ydk_passthrough_no_ids_is_error",
		   to_ydk(QByteArray("#main\n#extra\n!side\n"), err).isEmpty() && !err.isEmpty());

	if (g_failures) {
		std::fprintf(stderr, "\n%d failure(s)\n", g_failures);
		return 1;
	}
	std::fprintf(stderr, "\nall passed\n");
	return 0;
}
