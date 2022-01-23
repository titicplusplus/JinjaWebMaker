#pragma once

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <unordered_map>
#include <vector>

#include <string>

struct socket_element;

class WebSocket {
	public:
		WebSocket();
		~WebSocket();
		void config_server(int port);

		void wait_client();
		void send_all_client();

		void new_http_request(int socket);
		std::unordered_map<std::string, std::string> get_params(std::string &url);
		std::unordered_map<std::string, std::string> get_params_from_socket(char *buffer, int size);
		std::vector<std::string> info_request(char *buffer, size_t size);
		void stop();
			
	private:
		struct sockaddr_in addr;
		int server_fd;
		bool is_alive;

		std::vector<socket_element> client;
		std::string accept_key;
};
