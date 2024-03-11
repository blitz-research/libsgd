#include "internal.h"

#include <thread>

namespace {

using namespace sgd;

constexpr int maxHandleTypes = 64;

using HandleMap = Map<int, SharedPtr<Shared>>;
using ReverseMap = Map<Shared*, int>;

int g_nextHandle;

HandleMap g_handleMaps[maxHandleTypes];
ReverseMap g_reverseMaps[maxHandleTypes];

} // namespace
namespace sgd {

SGD_Handle getHandle(HandleType type, Shared* shared) {
	auto& rmap = g_reverseMaps[(int)type];
	auto it = rmap.find(shared);
	SGD_ASSERT(it != rmap.end());
	return it->second;
}

SGD_Handle createHandle(HandleType type, Shared* shared) {
	auto handle = ++g_nextHandle;
	auto& map = g_handleMaps[(int)type];
	auto& rmap = g_reverseMaps[(int)type];
	SGD_ASSERT(map.find(handle) == map.end());
	map.insert(std::make_pair(handle, shared));
	rmap.insert(std::make_pair(shared, handle));
	return handle;
}

SGD_Handle getOrCreateHandle(HandleType type, Shared* shared) {
	auto& rmap = g_reverseMaps[(int)type];
	auto it = rmap.find(shared);
	if (it != rmap.end()) return it->second;
	auto handle = ++g_nextHandle;
	g_handleMaps[(int)type].insert(std::make_pair(handle, shared));
	rmap.insert(std::make_pair(shared, handle));
	return handle;
}

Shared* resolveHandle(HandleType type, SGD_Handle handle) {
	auto& map = g_handleMaps[(int)type];
	auto it = map.find(handle);
	return it != map.end() ? it->second : nullptr;
}

void releaseHandle(HandleType type, SGD_Handle handle) {
	auto& map = g_handleMaps[(int)type];
	auto& rmap = g_reverseMaps[(int)type];
	auto it = map.find(handle);
	SGD_ASSERT(it != map.end());
	auto rit = rmap.find(it->second);
	SGD_ASSERT(rit != rmap.end());
	map.erase(it);
	rmap.erase(rit);
}

void error(CString message) {
	sgd_Error(message.c_str());
}

void error(CString error, CString message) {
	sgd::error(error + ": " + message);
}

void error(CString error, CFileioEx& ex) {
	sgd::error(error, ex.message());
}

} // namespace sgd
