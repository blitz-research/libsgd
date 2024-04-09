#include "fetch.h"

#include "fileio.h"
#include "log.h"
#include "path.h"
#include "stringutil.h"

#include <curl/curl.h>

#include <json11.hpp>

using namespace json11;

namespace sgd {

namespace {

constexpr auto sgdPrefix = "sgd://";

auto g_cacheDir = Path("${HOME}/.sgd");

auto g_cacheFilesPath = g_cacheDir / "cache.json";

std::map<String, String> g_cacheFiles;

size_t g_nextFileId;

bool g_cacheOk = false;

bool initCache() {
	static bool done;
	if (done) return g_cacheOk;
	done = true;

	if (!g_cacheDir.isDir()) {
		std::filesystem::create_directory(g_cacheDir.filePath());
		SGD_ASSERT(g_cacheDir.isDir());
	}

	if (!g_cacheFilesPath.isFile()) {
		saveString("{}", g_cacheFilesPath);
		SGD_ASSERT(g_cacheFilesPath.isFile());
	}

	auto src = loadString(g_cacheFilesPath);
	SGD_ASSERT(src);

	String err;
	auto json = Json::parse(src.result(), err, JsonParse::COMMENTS);
	SGD_ASSERT(json.is_object());

	for (auto& kv : json.object_items()) {
		g_cacheFiles.insert(std::make_pair(kv.first, kv.second.string_value()));
		++g_nextFileId;
	}

	return g_cacheOk = true;
}

void saveCacheState() {
	if (!g_cacheOk) return;

	Json::object obj;
	for (auto& kv : g_cacheFiles) {
		obj.insert(std::make_pair(kv.first, kv.second));
	}
	String src;
	Json(std::move(obj)).dump(src);

	auto r = saveString(src, g_cacheFilesPath);
	SGD_ASSERT(r);
}

Path cacheFile(CString url) {
	if (!startsWith(url, sgdPrefix) || !initCache()) return {};

	auto key = url.substr(6);

	auto it = g_cacheFiles.find(key);
	if (it == g_cacheFiles.end()) return {};

	return g_cacheDir / Path(it->second);
}

void cacheFile(CString url, const void* data, size_t size) {
	if (!g_cacheOk || !startsWith(url, sgdPrefix)) return;

	auto key = url.substr(6);

	SGD_ASSERT(g_cacheFiles.find(key) == g_cacheFiles.end());

	auto file = std::to_string(++g_nextFileId) + Path(url).ext();
	auto path = g_cacheDir / file;

	auto r = saveData(data, size, path);
	SGD_ASSERT(r);

	g_cacheFiles.insert(std::make_pair(key, file));

	saveCacheState();
}

CURL* g_curl;

CURL* openCurl() {
	if (g_curl) return g_curl;

	g_curl = curl_easy_init();
	if (!g_curl) {
		SGD_PANIC("!!! Failed to create CURL instance");
		return nullptr;
	}

	// Important! Should be the default!
	curl_easy_setopt(g_curl, CURLOPT_ACCEPT_ENCODING, "");

	// curl_easy_setopt(g_curl, CURLOPT_VERBOSE, 1);

	return g_curl;
}

String fetch(CString url, curl_write_callback writeFunc, void* writeData) {

	String turl = url;

	if (startsWith(url, sgdPrefix)) {
		static const String SGD_URL = "https://skirmish-dev.net/assets/";
		turl = SGD_URL + url.substr(6);
	}

	auto curl = openCurl();

	curl_easy_setopt(curl, CURLOPT_URL, turl.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunc);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, writeData);

	char error[CURL_ERROR_SIZE] = "";
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error);

	auto result = curl_easy_perform(curl);

	long httpStatus = -1;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatus);
	if (result != CURLcode::CURLE_OK) {
		log() << "### CURL HttpStatus:" << httpStatus;
		log() << "### CURL code:" << curl_easy_strerror(result);
		log() << "### CURL errors:" << error;
		return curl_easy_strerror(result);
	}

	return {};
}

size_t writeStringFunc(char* buffer, size_t size, size_t nmemb, void* datap) {
	size *= nmemb;
	auto text = (String*)datap;
	text->append(buffer, size);
	return size;
}

size_t writeDataFunc(char* buffer, size_t size, size_t nmemb, void* datap) {
	size *= nmemb;
	auto data = (Data*)datap;
	auto datasz = data->size();
	data->resize(datasz + size);
	std::memcpy(data->data() + datasz, buffer, size);
	return size;
}

} // namespace

Expected<String, FetchEx> fetchString(CString url) {
	if (auto cpath = cacheFile(url)) {
		return loadString(cpath).result();
	}

	String result;
	auto err = fetch(url, writeStringFunc, &result);
	if (!err.empty()) return FetchEx(err);

	cacheFile(url, result.data(), result.size());

	return result;
}

Expected<Data, FetchEx> fetchData(CString url) {
	if (auto cpath = cacheFile(url)) {
		return loadData(cpath).result();
	}

	Data result;
	auto err = fetch(url, writeDataFunc, &result);
	if (!err.empty()) return FetchEx(err);

	cacheFile(url, result.data(), result.size());

	return result;
}

} // namespace sgd
