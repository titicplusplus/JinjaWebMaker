#include "create_f.hpp"
#include <iostream>

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

#include <fstream>
#include "dependencies/json/json.hpp"

using json = nlohmann::json;


Create::Create(std::string m_path) {
	const fs::path pjt { m_path };

	if (!fs::exists(pjt)) {
		fs::create_directory(pjt);
		fs::create_directory(pjt/"styles/");
		fs::create_directory(pjt/"scripts/");
		fs::create_directory(pjt/"files/");
		fs::create_directory(pjt/"includes/");

		std::ofstream index_html { pjt/"index.jinja2.html" };

		index_html << "<!DOCTYPE HTML>\n"
			   << "<html lang=\"en\">\n"
			   << "\t<head>\n"
			   << "\t\t<title>WebMakerPy</title>\n"
			   << "\t\t<link rel=\"stylesheet\" type=\"text/css\" href=\"/styles/style.jinja2.css\">\n"
			   << "\t</head>\n"
			   << "\t<body>\n"
			   << "\t\t{% include \"title.html\" %}\n"
			   << "\t\t<script src=\"/scripts/script.jinja2.js\"></script>\n"
			   << "\t</body>\n"
			   << "</html>" << std::endl;

		index_html.close();

		std::ofstream title_html { pjt/"includes/title.html" };

		title_html << "<h1>Welcome to WebMakerPy</h1>\n<p>Version 0.6</p>" << std::endl;
		title_html.close();

		std::ofstream styles_css { pjt/"styles/style.jinja2.css" };
		styles_css << "h1 {\n"
			   << "\tcolor: {{ primaryColor }}\n"
			   << "}\n\n"
			   << "h2 {\n"
			   << "\tcolor: {{ secondaryColor }}\n"
			   << "}\n"
			   << std::endl;

		styles_css.close();

		std::ofstream scripts_js { pjt/"scripts/script.jinja2.js" };
		scripts_js << std::endl;
		scripts_js.close();

		json data_json = {
			{"variables", {
				{"primaryColor", "red"},
				{"secondaryColor", "green"},
			}},

			{"files", {
				"index.jinja2.html",
				"styles/style.jinja2.css",
				"scripts/script.jinja2.js"
			}},
		};

		std::ofstream file_data_json { pjt/"data.json" };
		file_data_json << data_json.dump(4) << std::endl;
		file_data_json.close();

		std::cout << "The project is starting...\n";
		std::cout << "    cd " << m_path << "\n";
		std::cout << "    WebMakerCpp server" << std::endl;


	} else {
		std::cout << "The folder already exists." << std::endl;
	}
	
}

Create::~Create() {}
