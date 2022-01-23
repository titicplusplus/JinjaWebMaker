#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <sys/socket.h>
#include "dependencies/inja/inja.hpp"
#include <string>
#include <iostream>
#include <cstring>
#include <unistd.h>

#include <netinet/in.h>
#include <vector>
#include <thread>

#include <unordered_map>
//#include <nlohmann/json.hpp>
#include "dependencies/json/json.hpp"
#include "filewatcher.hpp"

//#include "dependencies/WebSocket/WebSocket.h"

#include "websocket.hpp"

using json = nlohmann::json;

class WebServer {
	public:
		WebServer();
		~WebServer();

		void config_server(int port);

		void start();
		void stop(int signal);
		void new_http_request(int port);

		std::vector<std::string> info_request(char *buffer, size_t size);
		std::string get_content(char *buffer, size_t size);
		
		std::unordered_map<std::string, std::string> get_params(std::string &url);

		std::string open_file(std::string filename);
		std::string content_type(std::string &filenames);

		void setup_file();

	private:
		struct sockaddr_in addr;
		int server_fd;
		bool is_alive;

		inja::Template temp;
		inja::Environment env;

		json data;

		FileWatcher file;

		std::string polling_js;

		WebSocket webSocket;
		std::unique_ptr<std::thread> thread_ptr;

		std::string error404;

		//std::vector<std::thread> threads;
};

#endif // WEBSERVER_HPP
