#pragma once

#include "std.h"

#include <type_traits>

namespace sgd {

struct Config {

	Config(CString jsonSrc);

	Any get(CString name, CAny defaultValue = {}) const;

	template <class T> T get(CString name, const T& defaultValue = {}) const {
		auto v = get(name);
		if (auto* p = as<T>(&v)) return *p;
		if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
			if (auto p = as<double>(&v)) return *p;
		}
		return defaultValue;
	}

	template <class T> Vector<T> getVector(CString name, CVector<T> defaultValue = {}) const {
		auto array = get(name);
		if (auto vp = as<Vector<Any>>(&array)) {
			Vector<T> vs(vp->size());
			for (int i = 0; i < vp->size(); ++i) {
				auto& v = vp->at(i);
				if (auto p = as<T>(&v)) {
					vs[i] = *p;
				}else if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
					if (auto p = as<double>(&v)) vs[i] = (T)*p;
				}
			}
			return vs;
		}
		return defaultValue;
	}

private:
	using AnyMap = Map<String, Any>;

	Any g_config;
};

void setAppConfig(Config* config);

Config* appConfig();

} // namespace sgd
