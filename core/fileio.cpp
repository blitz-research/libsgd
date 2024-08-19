#include "fileio.h"

#include "fetch.h"
#include "log.h"

#include <fstream>
#include <sstream>

#define SGD_PATHEX(MSG, PATH) SGD_FILEIOEX(sgd::String(MSG) + " \"" + (PATH).str() + "\"")

namespace sgd {

FileioEx::FileioEx(String message, const char* file, int line) : m_message(std::move(message)) {
	SGD_LOG << "File IO Error:"<<message << "file:" << file << "line:" << line;
}

Expected<String, FileioEx> loadString(CPath path) {

	if (path.isUrl()) return fetchString(path.url());

	if (!path.isFilePath()) return SGD_PATHEX("Invalid file path", path);

	auto fpath = path.filePath();

	std::error_code ec;
	auto size = std::filesystem::file_size(fpath, ec);
	if (ec) {
		// Read from stream instead here?
		return SGD_PATHEX("IO error examining file", path);
	}
	std::ifstream fs(fpath, std::ios::binary);
	if (!fs.is_open()) return SGD_PATHEX("IO error opening file", path);

	String str(size, '\0');
	if (!fs.read(str.data(), str.size())) return SGD_PATHEX("IO error reading file", path);

	return str;
}

Expected<bool, FileioEx> saveString(CString str, CPath path) {

	if (!path.isFilePath()) return SGD_PATHEX("Invalid file path", path);

	auto fpath = path.filePath();

	std::ofstream fs(fpath, std::ios::binary | std::ios::trunc);
	if (!fs.is_open()) return SGD_PATHEX("IO error opening file", path);

	if (!fs.write((const char*)str.data(), str.size())) return SGD_PATHEX("IO error writing file", path);

	return true;
}

Expected<Data, FileioEx> loadData(CPath path) {

	if (path.isUrl()) return fetchData(path.url());

	if (!path.isFilePath()) return SGD_PATHEX("Invalid file path", path);

	auto fpath = path.filePath();

	std::error_code ec;
	auto size = std::filesystem::file_size(fpath, ec);
	if (ec) {
		// Read from stream instead here?
		return SGD_PATHEX("IO error examining file", path);
	}
	std::ifstream fs(fpath, std::ios::binary);
	if (!fs.is_open()) return SGD_PATHEX("IO error opening file", path);

	Data data(size);
	if (!fs.read((char*)data.data(), data.size())) return SGD_PATHEX("IO error reading file", path);

	return data;
}

Expected<bool, FileioEx> saveData(CData data, CPath path) {

	if (!path.isFilePath()) return SGD_PATHEX("Invalid file path", path);

	auto fpath = path.filePath();

	std::ofstream fs(fpath, std::ios::binary | std::ios::trunc);
	if (!fs.is_open()) return SGD_PATHEX("IO error opening file", path);

	if (!fs.write((const char*)data.data(), data.size())) return SGD_PATHEX("IO error writing file", path);

	return true;
}

Expected<bool, FileioEx> saveData(const void* data, size_t size, CPath path) {

	if (!path.isFilePath()) return SGD_PATHEX("Invalid file path", path);

	auto fpath = path.filePath();

	std::ofstream fs(fpath, std::ios::binary | std::ios::trunc);
	if (!fs.is_open()) return SGD_PATHEX("IO error opening file", path);

	if (!fs.write((const char*)data, size)) return SGD_PATHEX("IO error writing file", path);

	return true;
}

} // namespace sgd
