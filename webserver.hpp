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
#include "dependencies/json/json.hpp"
#include "filewatcher.hpp"

//#include "dependencies/WebSocket/WebSocket.h"

#include "websocket.hpp"

using json = nlohmann::json;

/// @brief Class to manage the Http connection with TCP socket
/// @author Thibaud VINCENT
class WebServer {
	public:

		/// @brief Construct WebServer Object (init polling_js and error 404)
		WebServer();
		
		/// Class Destructor
		~WebServer();

		/// @brief Setup TCP connection
		/// 
		/// @pre port need to be a integer
		/// @param port The port for the socket
		/// @return void
		void config_server(int port);

		/// @brief The TCP Main loop, accept connection
		/// @return void
		void start();

		/// @brief To stop the TCP Main loop when user wants
		/// @return void
		void stop(int signal);

		/// @brief Method that anwsers the web browser or http protocol when a new TCP connection happens
		/// @return void
		void new_http_request(int port);

		/// @brief Method to get the main information with the request sush as GET/POST/PUT, pathfile ( "/" ), and HTML version
		/// @param buffer The text request
		/// @param size The max-size of the buffer
		/// @return std::vector<std::string>
		std::vector<std::string> info_request(char *buffer, size_t size);

		/// @brief Get the content of the POST/PUT request
		/// @param buffer The text request
		/// @param size The max-size of the buffer
		/// 
		/// @return std::string
		std::string get_content(char *buffer, size_t size);
		

		/// @brief Get the paramaters of the URL
		/// 
		/// @param &url The url
		/// @return std::unordered_map<std::string>
		std::unordered_map<std::string, std::string> get_params(std::string &url);

		/// @brief To open or render with jinja a file
		/// 
		/// @param filename  the path of the file
		/// @return std::string
		std::string open_file(std::string filename);
		std::string content_type(std::string &filenames);

		/// @brief Setup files for the project such as data.json, the html/css/js files, ...
		/// @return void
		void setup_file();

		/// @biref Reload a file
		/// @return void
		/// @param file_name
		void reload_file(std::string file_name);

	private:
		struct sockaddr_in addr;
		int server_fd;
		bool is_alive;

		inja::Template temp;
		inja::Environment env;

		std::vector<inja::Template> includes_temp;
		std::vector<std::string> includes_files;

		json data;

		FileWatcher file;

		std::string polling_js;

		WebSocket webSocket;
		std::unique_ptr<std::thread> thread_ptr;

		std::string error404;

		//std::vector<std::thread> threads;
};

#endif // WEBSERVER_HPP
