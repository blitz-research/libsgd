#include "internal.h"

namespace sgd {

namespace {

Map<SGD_Handle, SharedPtr<Shared>> g_objects;

Map<Shared*, SGD_Handle> g_handles;

SGD_Handle g_nextHandle;

} // namespace

SGD_Handle createHandle(Shared* shared) {
	auto it = g_handles.find(shared);
	SGD_ASSERT(it == g_handles.end());
	auto handle = ++g_nextHandle;
	g_handles.insert(std::make_pair(shared, handle));
	g_objects.insert(std::make_pair(handle, shared));
	return handle;
}

void destroyHandle(SGD_Handle handle) {
	auto it = g_objects.find(handle);
	SGD_ASSERT(it != g_objects.end());
	g_handles.erase(g_handles.find(it->second));
	g_objects.erase(it);
}

SGD_Handle getHandle(Shared* shared) {
	auto it = g_handles.find(shared);
	SGD_ASSERT(it != g_handles.end());
	return it->second;
}

Shared* getObject(SGD_Handle handle) {
	auto it = g_objects.find(handle);
	SGD_ASSERT(it != g_objects.end());
	return it->second;
}

} // namespace sgd
