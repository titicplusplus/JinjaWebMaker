#include "websocket.hpp"
#include "dependencies/WebSocket/sha1/sha1.h"
#include "dependencies/WebSocket/base64/base64.h"

#include <iostream>

struct socket_element {
	std::string host;
	std::string origin;
	std::string key;
	std::string fkey;
	std::string protocol;
	int socket;
};

WebSocket::WebSocket() {
	accept_key = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
}

WebSocket::~WebSocket() {}

void WebSocket::config_server(int port) {
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		std::cout << "socket error" << std::endl;
	}
}

void WebSocket::wait_client() {
	is_alive = true;

	int res = bind(server_fd, (sockaddr*)&addr, sizeof(addr));

	if (res < 0) {
		std::cout << "bind failed: " << res << std::endl;
		return;
	}


	res = listen(server_fd, 3);
	int addrlen = sizeof(addr);

	if (res < 0) {
		std::cout << "listen error" << std::endl;
		return;
	}

	int new_socket = -1;
	while (is_alive && (new_socket = accept(server_fd, (struct sockaddr *)&addr, (socklen_t*)&addrlen))) {
		if (new_socket > 0) {
			std::cout << "New WebSocket socket: " << new_socket << std::endl;
			new_http_request(new_socket);
		}
	}
}

void WebSocket::new_http_request(int socket) {
	client.push_back(socket_element());
	std::size_t end = client.size()-1;


	char buffer[1024] = {0};
	int valread = read(socket, buffer, 1024);

	std::cout << buffer << std::endl;

	std::vector<std::string> value = info_request( buffer, valread );
	auto params = get_params_from_socket(buffer, valread);

	std::string content { "" };

	/**for (auto &f : params) {
		std::cout << f.first << " " << f.second << std::endl;
	}**/


	client[end].socket = socket;
	client[end].host  = params["Host"];
	client[end].origin = params["Origin"];
	client[end].key = params["Sec-WebSocket-Key"];
	client[end].protocol = params["Sec-WebSocket-Version"];



	content += "HTTP/1.1 101 Switching Protocols\r\n";
	content += "Upgrade: websocket\r\n";
	content += "Connection: Upgrade\r\n";

	client[end].fkey = client[end].key + accept_key;

	unsigned char digest[20];

	SHA1 sha;

	sha.Input(accept_key.data(), accept_key.size());
	sha.Result((unsigned *) digest);

	for (int i = 0; i < 20; i += 4) {
		unsigned char c;

		c = digest[i];
		digest[i] = digest[i + 3];
		digest[i + 3] = c;

		c = digest[i + 1];
		digest[i + 1] = digest[i + 2];
		digest[i + 2] = c;
	}

	client[end].fkey = base64_encode((const unsigned char *)digest, 20);
	content += "Sec-WebSocket-Accept: " + client[end].fkey + "\r\n";
	content += "Sec-WebSocket-Version: " + client[end].protocol + "\r\n";
	//content += "Sec-WebSocket-Protocol: chat\r\n";
	content += "\r\n";

	std::cout << "Content:\n" << content << std::endl;


	send( socket, content.c_str(), content.size(), 0 );

	sleep(1);
	
}

void WebSocket::send_all_client() {
	std::string data = "true";
	std::string buffer = "  true";

	buffer[0] = (unsigned char)0x3300;
	buffer[1] = 4;

	for (auto i : client) {
		send(i.socket, data.c_str(), data.size(), 0);
	}

}

std::vector<std::string> WebSocket::info_request(char *buffer, size_t size) {
	size_t i = 0;
	std::string word = "";
	std::vector<std::string> response;
	int detecd_attack = 0;

	while (size > i+2 && buffer[i] != '\r' && buffer[i+1] != '\n') {

		if (buffer[i] == ' ') {
			response.push_back(word);
			word = "";
		}
		else {
			if (buffer[i] == '.') {
				if ((detecd_attack++) < 2) {
					word += buffer[i];
				}
			} else {
				word += buffer[i];
				detecd_attack = 0;
			}
		}

		i++;
	}

	if (word != "") {
		response.push_back(word);
	}

	return response;
}

std::unordered_map<std::string, std::string> WebSocket::get_params_from_socket(char *buffer, int size) {
	int i;
	for (i = 0; i < size; i++) {
		if (buffer[i] == '\n' && buffer[i - 1] == '\r') {
			break;
		}
	}

	std::unordered_map<std::string, std::string> params;
	int step {0};

	std::string attribut { "" };
	std::string value { "" };

	for (; i < size; i++) {
		if (step == 0) {
			if (buffer[i] == ':') {
				step++;
			} else {
				attribut += buffer[i];
			}
		} else if (step == 1) {
			if (buffer[i] != ' ') {
				step++;
				value += buffer[i];
			}
		} else if (step == 2) {
			if (buffer[i] == '\r') {
				step++;
			} else {
				value += buffer[i];
			}
		} else if (step == 3) {
			if (buffer[i] == '\n') {
				step = 0;

				params[attribut] = value;
				attribut = "";
				value = "";
			}
		}
	}

	return params;
}

std::unordered_map<std::string, std::string> WebSocket::get_params(std::string &url) {
	auto it = url.find("?");

	if (it == std::string::npos) {
		return {};
	}

	std::unordered_map<std::string, std::string> params;

	std::string para { "" };
	std::string value { "" };
	int status = 0;

	auto it_r = it;
	++it;
	for (; it != url.size(); ++it) {
		char &v = url[it];
		if (status == 0) {
			if (v != '=') {
				para += v;
			}
			else {
				status = 1;
			}
		}
		else if (status == 1) {
			if (v != '&') {
				value += v;
			}
			else {
				status = 0;
				params[para] = value;

				para = "";
				value = "";
			}
		}
	}
	if (para != "") {
		params[para] = value;
	}

	url = url.substr(0, it_r);
	return params;
}

void WebSocket::stop() {
	shutdown(server_fd, SHUT_RD);
	is_alive = false;
}
