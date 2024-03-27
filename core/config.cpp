#include "config.h"
#include "log.h"

#include "json11.hpp"

namespace sgd {

namespace {

Config* g_appConfig = new Config("{}");

Any toAny(const json11::Json& jv) {
	if (jv.is_null()) return {};
	if (jv.is_bool()) return jv.bool_value();
	if (jv.is_string()) return jv.string_value();
	if (jv.is_number()) return jv.number_value();
	if (jv.is_array()) {
		auto& js = jv.array_items();
		Vector<Any> vs(js.size());
		for (int i = 0; i < js.size(); ++i) {
			vs[i] = toAny(js[i]);
		}
		return vs;
	}
	if (jv.is_object()) {
		auto& js = jv.object_items();
		Map<String, Any> vs;
		for (auto kv : js) {
			vs[kv.first] = toAny(kv.second);
		}
		return vs;
	}
	SGD_ABORT();
}

} // namespace

Config::Config(CString jsonSrc) {

	String err;
	auto json = json11::Json::parse(jsonSrc, err, json11::JsonParse::COMMENTS);
	if (!json.is_object()) {
		log() << "Error parsing json:" << err;
		SGD_ABORT();
		return;
	}
	g_config = toAny(json.object_items());
}

Any Config::get(CString name, CAny defaultValue) const {

	auto obj = as<AnyMap>(&g_config);

	for (size_t i = 0; i < name.size();) {
		auto e = name.find('.', i);
		auto f = name.substr(i, e != String::npos ? e - i : e);
		auto it = obj->find(f);
		if (it == obj->end()) {
			// Can't find field
			return defaultValue;
		}
		if (e == String::npos) return it->second;
		obj = as<AnyMap>(&it->second);
		i = e + 1;
	}
	return defaultValue;
}

void setAppConfig(Config* config) {
	g_appConfig = config;
}

Config* appConfig() {
	return g_appConfig;
}

} // namespace sgd
