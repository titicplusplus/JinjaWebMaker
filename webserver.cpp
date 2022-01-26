#include "webserver.hpp"
#include <future>
#include <thread>
#include <fstream>

#ifndef __has_include
	static_assert(false, "__has_include not supported");
#else
#  if __has_include(<experimental/filesystem>)
#    include <experimental/filesystem>
	namespace fs = std::experimental::filesystem;
#  elif __has_include(<filesystem>)
#    include <filesystem>
	namespace fs = std::filesystem;
#  elif __has_include(<boost/filesystem.hpp>)
#    include <boost/filesystem.hpp> 
	namespace fs = boost::filesystem;
#  endif
#endif

WebServer::WebServer() {
	//setup_file();

	/**std::ifstream polling { "p.js" };

	polling_js = "";
	std::string line { "\n" };

	while (std::getline(polling, line)) {
		polling_js += line + "\n";
	}

	polling.close();**/

	polling_js = R"(<script type = "text/javascript">var polling = true;function reqListener () {if (this.responseText.length == 6) {window.location.reload();}}function delay(milliseconds){return new Promise(resolve => {setTimeout(resolve, milliseconds);});}async function Polling() {await delay(3000);while (polling) {await delay(1000);var oReq = new XMLHttpRequest();oReq.addEventListener("load", reqListener);oReq.open("GET", "/new");oReq.contentType = "text/plain";oReq.send();}}Polling();</script>
	)";

	error404 = "<html><head><title>Error: 404</title>" + polling_js + "</head><body><h1>Error: 404</h1></body></html>";
}

void WebServer::setup_file() {
	std::ifstream json_config { "data.json" };
	json_config >> data;
	json_config.close();

	std::vector<std::string> includes_file { file.load(data) };

	for (const auto& file: includes_file) {
		std::cout << file << std::endl;
		inja::Template temp = env.parse_template("includes/" + file);
		env.include_template(file, temp);
	}
}

void WebServer::config_server(int port) {
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
		std::cout << "socket error" << std::endl;
	}
	std::cout << "Server listening on http://127.0.0.1:" << port << std::endl;

	webSocket.config_server(port+1);
}

void WebServer::start() {
	int new_socket;
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

	thread_ptr = std::unique_ptr<std::thread>(new std::thread(&WebSocket::wait_client, &webSocket));

	while (is_alive && (new_socket = accept(server_fd, (struct sockaddr *)&addr, (socklen_t*)&addrlen))) {
		if (new_socket > 0) {
			std::async(std::launch::async, [this, new_socket] {
				new_http_request(new_socket);
			});
		}
	}
}

void WebServer::new_http_request(int port) {
	char buffer[1024] = {0};
	int valread = read(port, buffer, 1024);

	std::vector<std::string> value = info_request( buffer, valread );
	auto params = get_params( value[1] );

	//if (value[1] != "/new")
		//std::cout << buffer << std::endl; 

	if (value[0] == "POST") {
		//std::cout << get_content(buffer, valread) << std::endl;
	}

	std::string content { "" };

	//////////////////////////////////
	
	if (value[1] == "/") {
		content = open_file("index.jinja2.html");

		auto pos = content.find("</title>");

		if (pos != std::string::npos)
			content.insert(pos+8, polling_js);
	} else if (value[1] == "/new") {

		if (file.json_change()) {
			content = "true";

			setup_file();
		}
		else {
			content = file.new_modif();

			if (content == "true") {
				setup_file();
			}
		}
	} else {
		content = open_file(value[1].substr(1));
	}

	if (value[1].size() > 5 && value[1].substr(value[1].size() - 5, value[1].size()) == ".html") {
		auto pos = content.find("</title>");

		if (pos != std::string::npos)
			content.insert(pos+8, polling_js);
	}


	//////////////////////////////////

	if (content == "!:") {
		content = "No file found!";
		std::string rep = "HTTP/1.0 404 Not Found\r\nContent-Length:" + 
			std::to_string(error404.size()) + "\r\n\r\n" + error404;

		send( port, rep.c_str(), rep.size(), 0 );
	} else {
		std::string rep = "HTTP/1.0 200 OK\r\nContent-Length: " +  
			std::to_string(content.size()) + "\r\n" + content_type(value[1]) + "\r\n\r\n" + content;

		send( port, rep.c_str(), rep.size(), 0 );
	}
}

std::string WebServer::open_file(std::string filename) {
	if (fs::exists(filename)) {

		if (filename.find(".jinja2.") != std::string::npos) {
			try {
				temp = env.parse_template(filename);

				return env.render(temp, data["variables"]);
			} catch (const std::exception& e) {
				std::cout << e.what() << std::endl;
				return "!:";
			}
		} else {
			std::ifstream file { filename };

			if (file) {
				std::string line { "" };
				std::string html { "" };

				while (std::getline(file, line)) {
					html += line + "\n";
				}

				file.close();
				return html;
			} else {
				return "!:";
			}
		}
	} else {
		return "!:";
	}
}

std::string WebServer::content_type(std::string &filename) {
	std::string ext { "" };
	
	if (filename.size() < 2) {
		return "content-type: text/html; charset=utf-8\r\n";
	}

	std::size_t i { filename.size() -2};

	for (; i >= 0; i--) {
		if (filename[i] == '.') {
			break;	
		} else if (filename[i] == '/') {
			return "content-type: text/plain; charset=utf-8\r\n";
		}
	}

	ext = filename.substr(i+1);

	if (ext == "html") {
		return "content-type: text/html; charset=utf-8\r\n";
	} else if (ext == "js") {
		return "content-type: text/javascript; charset=utf-8\r\n";
	} else if (ext == "css") {
		return "content-type: text/css; charset=utf-8\r\n";
	}

	return "content-type: text/plain; charset=utf-8\r\n";
}

std::vector<std::string> WebServer::info_request(char *buffer, size_t size) {
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

std::string WebServer::get_content(char *buffer, size_t size) {
	
	for (size_t i = 0; i < size-4; i ++) {
		if (buffer[i] == '\r' && buffer[i+1] == '\n' && buffer[i+2] == '\r' && buffer[i+3] == '\n') {
			std::string content { buffer };
			content = content.substr(i+4);
			return content;
		}
	}

	return "";
}


std::unordered_map<std::string, std::string> WebServer::get_params(std::string &url) {
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

void WebServer::stop(int /*signal*/) {
	webSocket.stop();

	shutdown(server_fd, SHUT_RD);
	is_alive = false;

	std::cout << "end server" << std::endl;
	thread_ptr->join();
}

WebServer::~WebServer() {}
