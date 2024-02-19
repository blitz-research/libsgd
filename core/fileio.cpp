#include "fileio.h"

#include "log.h"

#include <fstream>
#include <sstream>

#if SGD_OS_WINDOWS
#include <Windows.h>
#undef min
#undef max
#endif

namespace sgd {

Expected<String, FileioEx> loadString(CPath path) {
	auto fpath = path.resolve();

	std::error_code ec;
	auto size = std::filesystem::file_size(fpath, ec);
	if (ec) {
		// Read from stream instead here?
		return FileioEx{"IO error examining file"};
	}
	std::ifstream fs(fpath, std::ios::binary);
	if (!fs.is_open()) return FileioEx{"IO error opening file"};

	String str(size, '\0');
	if (!fs.read(str.data(), str.size())) return FileioEx{"IO error reading file"};

	return str;
}

Expected<bool, FileioEx> saveString(CString str, CPath path) {
	auto fpath = path.resolve();

	std::ofstream fs(fpath, std::ios::binary | std::ios::trunc);
	if (!fs.is_open()) return FileioEx{"IO error opening file"};

	if (!fs.write((const char*)str.data(), str.size())) return FileioEx{"IO error writing file"};

	return true;
}

Expected<Data, FileioEx> loadData(CPath path) {
	auto fpath = path.resolve();

	std::error_code ec;
	auto size = std::filesystem::file_size(fpath, ec);
	if (ec) {
		// Read from stream instead here?
		return FileioEx{"IO error examining file"};
	}
	std::ifstream fs(fpath, std::ios::binary);
	if (!fs.is_open()) return FileioEx{"IO error opening file"};

	Data data(size);
	if (!fs.read((char*)data.data(), data.size())) return FileioEx{"IO error reading file"};

	return data;
}

Expected<bool, FileioEx> saveData(CData data, CPath path) {
	auto fpath = path.resolve();

	std::ofstream fs(fpath, std::ios::binary | std::ios::trunc);
	if (!fs.is_open()) return FileioEx{"IO error opening file"};

	if (!fs.write((const char*)data.data(), data.size())) return FileioEx{"IO error writing file"};

	return true;
}

#if SGD_OS_WINDOWS
void openURL(CString url) {
	CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
	ShellExecute(HWND_DESKTOP, nullptr, url.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
}
#endif

} // namespace sgd
