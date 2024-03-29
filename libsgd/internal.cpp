#include "internal.h"

#include <thread>

namespace {

using namespace sgd;

constexpr int maxHandleTypes = 32;

using HandleMap = Map<int, SharedPtr<Shared>>;
using ReverseMap = Map<Shared*, int>;

int g_nextHandle;

HandleMap g_handleMaps[maxHandleTypes];
ReverseMap g_reverseMaps[maxHandleTypes];

HandleTypeInfo* g_handleTypeInfos[maxHandleTypes];

} // namespace

namespace sgd {

HandleTypeInfo::HandleTypeInfo(ObjectType* objectType, HandleType handleType) : objectType(objectType), handleType(handleType) {
}

SGD_Handle createHandle(HandleTypeInfo* type, Shared* shared) {
	auto handle = ++g_nextHandle;
	auto& map = g_handleMaps[(int)type->handleType];
	SGD_ASSERT(map.find(handle) == map.end());
	map.insert(std::make_pair(handle, shared));
	g_reverseMaps[(int)type->handleType].insert(std::make_pair(shared, handle));
	return handle;
}

SGD_Handle getOrCreateHandle(HandleTypeInfo* type, Shared* shared) {
	auto& rmap = g_reverseMaps[(int)type->handleType];
	auto it = rmap.find(shared);
	if (it != rmap.end()) return it->second;
	auto handle = ++g_nextHandle;
	g_handleMaps[(int)type->handleType].insert(std::make_pair(handle, shared));
	rmap.insert(std::make_pair(shared, handle));
	return handle;
}

Shared* resolveHandle(HandleTypeInfo* type, SGD_Handle handle) {
	auto& map = g_handleMaps[(int)type->handleType];
	auto it = map.find(handle);
	if (it == map.end()) error(String("Invalid ") + type->objectType->name+" handle");
	auto shared = it->second;
	if (!shared->is(type->objectType)) error("Invalid handle type");
	return shared;
}

void destroyHandle(HandleTypeInfo* type, SGD_Handle handle) {
	auto& map = g_handleMaps[(int)type->handleType];
	auto it = map.find(handle);
	if (it == map.end()) error("Invalid handle");
	g_reverseMaps[(int)type->handleType].erase(it->second);
	map.erase(it);
}

bool destroyHandle(HandleTypeInfo* type, Shared* shared) {
	auto& rmap = g_reverseMaps[(int)type->handleType];
	auto rit = rmap.find(shared);
	if (rit == rmap.end()) return false;
	g_handleMaps[(int)type->handleType].erase(rit->second);
	rmap.erase(rit);
	return true;
}

void destroyHandles(HandleType handleType) {
	g_handleMaps[(int)handleType].clear();
	g_reverseMaps[(int)handleType].clear();
}

void destroyAllHandles() {
	for(auto& map : g_handleMaps) map.clear();
	for(auto& rmap : g_reverseMaps) rmap.clear();
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
