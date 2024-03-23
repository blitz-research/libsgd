#include "fileio.h"

#include "log.h"
#include "stringutil.h"
#include "fetch.h"

#include <fstream>
#include <sstream>

#if SGD_OS_WINDOWS
#include <Windows.h>
#undef min
#undef max
#endif

namespace sgd {

Expected<String, FileioEx> loadString(CPath path) {

	if(path.isUrl()) {
		auto r = fetchString(path.str());
		if(!r) return FileioEx(r.error().message());
		return r.result();
	}

	if(!path.isValidFilePath()) return FileioEx("Invalid file path");

	auto fpath = path.filePath();

	std::error_code ec;
	auto size = std::filesystem::file_size(fpath, ec);
	if (ec) {
		// Read from stream instead here?
		return FileioEx{"IO error examining file '"+path.str()+"'"};
	}
	std::ifstream fs(fpath, std::ios::binary);
	if (!fs.is_open()) return FileioEx{"IO error opening file"};

	String str(size, '\0');
	if (!fs.read(str.data(), str.size())) return FileioEx{"IO error reading file"};

	return str;
}

Expected<bool, FileioEx> saveString(CString str, CPath path) {

	if(!path.isValidFilePath()) return FileioEx("Invalid file path");

	auto fpath = path.filePath();

	std::ofstream fs(fpath, std::ios::binary | std::ios::trunc);
	if (!fs.is_open()) return FileioEx{"IO error opening file"};

	if (!fs.write((const char*)str.data(), str.size())) return FileioEx{"IO error writing file"};

	return true;
}

Expected<Data, FileioEx> loadData(CPath path) {

	if(path.isUrl()) {
		auto r = fetchData(path.str());
		if(!r) return FileioEx(r.error().message());
		return r.result();
	}

	if(!path.isValidFilePath()) return FileioEx("Invalid file path");

	auto fpath = path.filePath();

	std::error_code ec;
	auto size = std::filesystem::file_size(fpath, ec);
	if (ec) {
		// Read from stream instead here?
		return FileioEx{"IO error examining file '"+path.str()+"'"};
	}
	std::ifstream fs(fpath, std::ios::binary);
	if (!fs.is_open()) return FileioEx{"IO error opening file"};

	Data data(size);
	if (!fs.read((char*)data.data(), data.size())) return FileioEx{"IO error reading file"};

	return data;
}

Expected<bool, FileioEx> saveData(CData data, CPath path) {

	if(!path.isValidFilePath()) return FileioEx("Invalid file path");

	auto fpath = path.filePath();

	std::ofstream fs(fpath, std::ios::binary | std::ios::trunc);
	if (!fs.is_open()) return FileioEx{"IO error opening file"};

	if (!fs.write((const char*)data.data(), data.size())) return FileioEx{"IO error writing file"};

	return true;
}

Expected<bool, FileioEx> saveData(const void *data, size_t size, CPath path) {

	if(!path.isValidFilePath()) return FileioEx("Invalid file path");

	auto fpath = path.filePath();

	std::ofstream fs(fpath, std::ios::binary | std::ios::trunc);
	if (!fs.is_open()) return FileioEx{"IO error opening file"};

	if (!fs.write((const char*)data, size)) return FileioEx{"IO error writing file"};

	return true;
}

} // namespace sgd
