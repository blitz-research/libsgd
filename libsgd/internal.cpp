#include "internal.h"

#include <thread>

namespace sgdx {

namespace {

std::mutex g_eventQueueMutex;
Deque<SGD_Event> g_eventQueue;

using HandleMap = Map<SGD_Handle, SharedPtr<Shared>>;
using ReverseMap = Map<Shared*, SGD_Handle>;

SGD_Handle g_nextHandle;
HandleMap g_handleMap;
ReverseMap g_reverseMap;

} // namespace

void error(CString message) {
	sgd_Error(message.c_str());
}

void error(CString error, CString message) {
	sgdx::error(error + ": " + message);
}

void error(CString error, CFileioEx& ex) {
	sgdx::error(error, ex.message());
}

void postEvent(SGD_Event event) {
	std::lock_guard<std::mutex> lock(g_eventQueueMutex);
	g_eventQueue.push_back(event);
}

void getEventQueue(Deque<SGD_Event>& queue) {
	std::lock_guard<std::mutex> lock(g_eventQueueMutex);
	std::swap(queue, g_eventQueue);
}

SGD_Handle createHandle(Shared* shared) {
	auto handle = ++g_nextHandle;
//	SGD_LOG << "Bind shared"<<shared<<"to handle"<<handle;
	SGD_ASSERT(g_handleMap.find(handle) == g_handleMap.end());
	g_handleMap.insert(std::make_pair(handle, shared));
	SGD_ASSERT(g_reverseMap.find(shared) == g_reverseMap.end());
	g_reverseMap.insert(std::make_pair(shared, handle));
	return handle;
}

SGD_Handle getHandle(Shared* shared) {
	SGD_ASSERT(g_reverseMap.find(shared) != g_reverseMap.end());
	return g_reverseMap.find(shared)->second;
}

SGD_Handle getOrCreateHandle(Shared* shared) {
	auto it = g_reverseMap.find(shared);
	if (it != g_reverseMap.end()) return it->second;
	auto handle = ++g_nextHandle;
	SGD_ASSERT(g_handleMap.find(handle) == g_handleMap.end());
	g_handleMap.insert(std::make_pair(handle, shared));
	SGD_ASSERT(g_reverseMap.find(shared) == g_reverseMap.end());
	g_reverseMap.insert(std::make_pair(shared, handle));
	return handle;
}

Shared* resolveHandle(SGD_Handle handle) {
	auto it = g_handleMap.find(handle);
	if (it == g_handleMap.end()) SGD_ERROR("Invalid handle");
	return it->second;
}

void releaseHandle(SGD_Handle handle) {
	auto it = g_handleMap.find(handle);
	if (it == g_handleMap.end()) SGD_ERROR("Invalid handle");
	auto revit = g_reverseMap.find(it->second);
	SGD_ASSERT(revit != g_reverseMap.end());
	g_reverseMap.erase(revit);
	g_handleMap.erase(it);
}

void releaseHandle(Shared* shared) {
	auto revit = g_reverseMap.find(shared);
	SGD_ASSERT(revit != g_reverseMap.end());
	auto it = g_handleMap.find(revit->second);
	SGD_ASSERT(it != g_handleMap.end());
	g_reverseMap.erase(revit);
	g_handleMap.erase(it);
}

void releaseAllHandles(ObjectType* ofType) {
	Vector<SGD_Handle> todo;
	for (auto& it : g_handleMap) {
		if (it.second->dynamicType() != ofType) continue;
		todo.push_back(it.first);
	}
	for (auto handle : todo) {
		releaseHandle(handle);
	}
}

void releaseAllHandles() {
	g_reverseMap.clear();
	g_handleMap.clear();
}

} // namespace sgdx
