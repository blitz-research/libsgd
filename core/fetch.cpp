#include "fetch.h"

#include "log.h"
#include "stringutil.h"

#include <curl/curl.h>
#include <thread>

namespace sgd {

namespace {

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

String fetch(String url, curl_write_callback writeFunc, void* writeData) {

	if (startsWith(url, "sgd://")) {
		static const String SGD_URL = "https://skirmish-dev.net/assets/";
		url = SGD_URL + url.substr(6);
		log() << "### Remapped url:" << url;
	}

	auto curl = openCurl();

	curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunc);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, writeData);

	char error[CURL_ERROR_SIZE] = "";
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error);

	auto result = curl_easy_perform(curl);

	long httpStatus = -1;
	curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &httpStatus);
	if (result != CURLcode::CURLE_OK) {
		// if (httpStatus < 200 || httpStatus >= 300) log() << "### CURL HttpStatus:" << httpStatus;
		log() << "### CURL HttpStatus:" << httpStatus;
		log() << "### CURL code:" << curl_easy_strerror(result);
		log() << "### CURL errors:" << error;
	}

	if (result) String(curl_easy_strerror(result));

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
	String result;
	auto err = fetch(url, writeStringFunc, &result);
	if (!err.empty()) return FetchEx(err);
	return result;
}

Expected<Data, FetchEx> fetchData(CString url) {
	Data result;
	auto err = fetch(url, writeDataFunc, &result);
	if (!err.empty()) return FetchEx(err);
	return result;
}

} // namespace sgd
