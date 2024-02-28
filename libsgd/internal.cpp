#include "internal.h"

namespace {

using namespace sgd;

constexpr int maxHandleTypes = 64;

using HandleMap = Map<int, SharedPtr<Shared>>;
using ReverseMap = Map<Shared*, int>;

int g_nextHandle;

HandleMap g_handleMaps[maxHandleTypes];
ReverseMap g_reverseMaps[maxHandleTypes];

void(SGD_DECL *g_errorHandler)(SGD_String error, void* context);

void* g_errorContext;

} // namespace

void SGD_DECL sgd_SetErrorHandler(void(SGD_DECL *handler)(SGD_String error, void* context), void* context) {
	g_errorHandler = handler;
	g_errorContext = context;
}

void SGD_DECL sgd_Error(SGD_String error) {
	if(g_errorHandler) {
		g_errorHandler(error, g_errorContext);
	}else {
		sgd::alert(sgd::String("Unhandled SGD error: ")+error);
	}
	std::exit(1);
}

namespace sgd {

void error(CString message) {
	sgd_Error(message.c_str());
}

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

Shared* resolveHandle(HandleType type, SGD_Handle handle) {
	auto& map = g_handleMaps[(int)type];
	auto it = map.find(handle);
	return it!= map.end() ? it->second : nullptr;
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

} // namespace sgd
