#include <core/exports.h>

#include "httplib.h"

using namespace sgd;

void startApp(CString exepath) {

	auto i = exepath.rfind('\\');
	if (i == String::npos) i = exepath.rfind('/');
	if (i == String::npos) SGD_PANIC("Invalid exe path:" + exepath);

	auto file = exepath.substr(i + 1);
	auto root = exepath.substr(0, i);
	auto url = "http://localhost:8080/" + file;

//	sgd::alert("file=" + file + " root=" + root + " url=" + url);

	std::thread([=] {
		httplib::Server server;

		httplib::Headers headers = {{"Cross-Origin-Opener-Policy", "same-origin"},
									{"Cross-Origin-Embedder-Policy", "require-corp"}};

		if(!server.set_mount_point("/", root, std::move(headers))) {
			alert("Server set_mount_point failed");
			SGD_ABORT();
		}

		if(!server.listen("localhost", 8080)) {
			alert("Server listen failed");
			SGD_ABORT();
		}

		alert("OOPS!");
		SGD_ABORT();

	}).detach();

	openBrowser(url);
}

int interpreter() {

	String exepath;
	String execwd;

	for (;;) {

		puts("(gdb)");
		fflush(stdout);

		String cmd;
		if (!std::getline(std::cin, cmd)) return 0;
		// alert(cmd);

		String args;
		auto spc = cmd.find(' ');
		if (spc != String::npos) {
			args = cmd.substr(spc + 1);
			cmd = cmd.substr(0, spc);
		}

		if (cmd == "-file-exec-and-symbols") {
			exepath = dequote(args);
		} else if (cmd == "-environment-cd") {
			execwd = dequote(args);
		} else if (cmd == "-exec-run") {
			startApp(exepath);
		} else if (cmd == "-gdb-exit") {
			return 0;
		}
		puts("^done");
	}
	return 0;
}

int main(int argc, const char* argv[]) {

	if (argc == 2 && !std::strcmp(argv[1], "--version")) {
		puts("GNU gdb (GDB) 11.1");
		return 0;
	}

	if (argc == 2 && !std::strcmp(argv[1], "--interpreter=mi2")) { //
		return interpreter();
	}

	String str;
	for (int i = 0; i < argc; ++i) {
		str += " " + String(argv[i]);
	}
	SGD_PANIC("??? " + str);

	return 1;
}
