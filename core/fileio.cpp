#include "fileio.h"

#include "fetch.h"

#include <fstream>
#include <sstream>

namespace sgd {

namespace {

FileioEx Ex(CString msg, CPath path) {
	return FileioEx(msg + " \"" + path.str() + "\"");
}

} // namespace

Expected<String, FileioEx> loadString(CPath path) {

	if (path.isUrl()) return fetchString(path.url());

	if (!path.isFilePath()) return Ex("Invalid file path",path);

	auto fpath = path.filePath();

	std::error_code ec;
	auto size = std::filesystem::file_size(fpath, ec);
	if (ec) {
		// Read from stream instead here?
		return Ex("IO error examining file",path);
	}
	std::ifstream fs(fpath, std::ios::binary);
	if (!fs.is_open()) return Ex("IO error opening file",path);

	String str(size, '\0');
	if (!fs.read(str.data(), str.size())) return Ex("IO error reading file",path);

	return str;
}

Expected<bool, FileioEx> saveString(CString str, CPath path) {

	if (!path.isFilePath()) return Ex("Invalid file path",path);

	auto fpath = path.filePath();

	std::ofstream fs(fpath, std::ios::binary | std::ios::trunc);
	if (!fs.is_open()) return Ex("IO error opening file",path);

	if (!fs.write((const char*)str.data(), str.size())) return Ex("IO error writing file",path);

	return true;
}

Expected<Data, FileioEx> loadData(CPath path) {

	if (path.isUrl()) return fetchData(path.url());

	if (!path.isFilePath()) return Ex("Invalid file path",path);

	auto fpath = path.filePath();

	std::error_code ec;
	auto size = std::filesystem::file_size(fpath, ec);
	if (ec) {
		// Read from stream instead here?
		return Ex("IO error examining file",path);
	}
	std::ifstream fs(fpath, std::ios::binary);
	if (!fs.is_open()) return Ex("IO error opening file",path);

	Data data(size);
	if (!fs.read((char*)data.data(), data.size())) return Ex("IO error reading file",path);

	return data;
}

Expected<bool, FileioEx> saveData(CData data, CPath path) {

	if (!path.isFilePath()) return Ex("Invalid file path",path);

	auto fpath = path.filePath();

	std::ofstream fs(fpath, std::ios::binary | std::ios::trunc);
	if (!fs.is_open()) return Ex("IO error opening file",path);

	if (!fs.write((const char*)data.data(), data.size())) return Ex("IO error writing file",path);

	return true;
}

Expected<bool, FileioEx> saveData(const void* data, size_t size, CPath path) {

	if (!path.isFilePath()) return Ex("Invalid file path",path);

	auto fpath = path.filePath();

	std::ofstream fs(fpath, std::ios::binary | std::ios::trunc);
	if (!fs.is_open()) return Ex("IO error opening file",path);

	if (!fs.write((const char*)data, size)) return Ex("IO error writing file",path);

	return true;
}

} // namespace sgd
