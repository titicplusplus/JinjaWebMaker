#include <iostream>
#include <csignal>
#include "webserver.hpp"
#include "create_f.hpp"
#include "export.hpp"

WebServer server;

void end_signal(int signal) {
	std::cout << "signal: " << signal << std::endl;
	server.stop(signal);
}

void help() {
	std::cout << "Usage: webserver, help \n";
	std::cout << "\tcreate NAME           -> to create a new project\n";
	std::cout << "\tserver PORT(optional) -> to start the webserver\n";
	std::cout << "\texport                -> to export the project\n";
	std::cout << std::endl;
}

int main(int argc, char **argv) {

	if (argc == 0) {
		help();
	} else if (argc == 2) {
		if (strcmp(argv[1], "server") == 0) {
			std::signal(SIGINT, end_signal);
			std::signal(SIGTERM, end_signal);

			server.setup_file();
			server.config_server(8080);
			server.start();
		} else if (strcmp(argv[1], "export") == 0) {
			Export();
		} else {
			help();
		}
	} else if (argc == 3) {
		if (strcmp(argv[1], "create") == 0) {
			Create c(argv[2]);
		} else if (strcmp(argv[1], "server") == 0) {
			std::signal(SIGINT, end_signal);
			std::signal(SIGTERM, end_signal);

			server.setup_file();
			server.config_server(std::atoi(argv[2]));
			server.start();

		} else {
			help();
		}
	} else {
		help();
	}


		
	return 0;
}

