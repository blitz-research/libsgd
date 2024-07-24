#include "stringutil.h"

namespace sgd {

String replace(CString str, CString find, CString rep) {
	auto r = str;
	for (size_t pos = 0; pos < r.size();) {
		pos = r.find(find, pos);
		if (pos == String::npos) break;
		r.replace(pos, find.size(), rep, 0, rep.size());
		pos += rep.size();
	}
	return r;
}

Vector<String> split(CString str, CString sep) {
	Vector<String> fields;
	for (size_t i = 0; i < str.size();) {
		size_t e = str.find(sep, i);
		if (e == String::npos) {
			fields.push_back(str.substr(i));
			break;
		}
		fields.push_back(str.substr(i, e - i));
		i = e + sep.size();
	}
	return fields;
}

String join(CVector<String> fields, CString sep) {
	if (fields.empty()) return {};
	String str;
	for (const auto& field : fields) {
		if (!str.empty()) str += sep;
		str += field;
	}
	return str;
}

String toUpper(CString cstr) {
	auto str = cstr;
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::toupper(c); });
	return str;
}

String toLower(CString cstr) {
	auto str = cstr;
	std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return std::tolower(c); });
	return str;
}

bool startsWith(CString string, CString prefix) {
	return string.size() >= prefix.size() && string.compare(0, prefix.size(), prefix) == 0;
}

bool endsWith(CString string, CString suffix) {
	return string.size() >= suffix.size() && string.compare(string.size() - suffix.size(), suffix.size(), suffix) == 0;
}

String enquote(CString str) {
	if (startsWith(str, "\"") && endsWith(str, "\"")) return str;
	return "\"" + str + "\"";
}

String dequote(CString str) {
	if (!startsWith(str, "\"") || !endsWith(str, "\"")) return str;
	return str.substr(1, str.size() - 2);
}

bool truthy(CString str) {
	auto tstr = toLower(str);
	return tstr=="1" || tstr == "true" || tstr=="on";
}

} // namespace sgd
