#include "export.hpp"

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


//#include <nlohmann/json.hpp>
#include "dependencies/json/json.hpp"
#include <string>
#include "dependencies/inja/inja.hpp"

using json = nlohmann::json;

void Export() {
	json data;
	std::ifstream json_config { "data.json" };

	if (json_config) {
		json_config >> data;
	} else {
		std::cout << "Please, put you on a project jinja folder" << std::endl;
		return;
	}

	json_config.close();

	inja::Environment env;

	std::cout << "Open includes files\n" << std::endl;
	for (const auto& file: fs::directory_iterator("includes/")) {
		if ( std::string(file.path().filename())[0] != '.') {
			std::cout << "---> " << file.path() << " ...\n";
			inja::Template temp = env.parse_template(file.path());
			env.include_template(file.path().filename(), temp);
		}
	}

	std::vector<std::string> files_name;
	std::vector<std::size_t> files_pos;

	for (const std::string& file : data["files"]) {


		std::string really { "" };
	
		auto pos = file.find(".jinja2.");

		if (pos == std::string::npos) {
			std::cout << "Warning: " << file << " is not a valid file\n";
			std::cout << "Please add \".jinja2.\" inside it like index.jinja2.html or style_main_page.jinja2.css\n";
			std::cout << "    Else, you will have problems during the export of the project." << std::endl;

			return;
		}

		files_pos.push_back(pos);

		really = file.substr(0, pos);
		really += file.substr(pos + 7);

		files_name.push_back(really);
	}

	std::cout << "\nRendering files\n";

	for (std::size_t i {0}; i < data["files"].size(); i++) {
		std::cout << "[" << i << "/" << data["files"].size() << "] ---> " << data["files"][i] << "\n";

		std::string result { env.render_file(data["files"][i].get<std::string>(), data["variables"]) };

		for (std::size_t j {0}; j < data["files"].size(); j++) {
			std::size_t pos = result.find(data["files"][j]);
			while (pos  != std::string::npos) {
				result.erase(pos + files_pos[j] + 1,  7);
				pos = result.find(data["files"][j]);
			}
		}

		std::ofstream file_r { files_name[i] };
		file_r.write(result.c_str(), result.size());
		file_r.close();
	}

	std::cout << "\nEnd of the export\n";
}
