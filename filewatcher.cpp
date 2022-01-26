#include "filewatcher.hpp"
#include <iostream>

FileWatcher::FileWatcher() {}


std::vector<std::string> FileWatcher::load(json &data) {
	std::vector<std::string> includes_file;

	for (const std::string &key : data["files"]) {
		if (key.find(".jinja2.") == std::string::npos) {
			std::cout << "Warning: " << key << " is not a valid file\n";
			std::cout << "Please add \".jinja2.\" inside it like index.jinja2.html or style_main_page.jinja2.css\n";
			std::cout << "    Else, you will have problems during the export of the project." << std::endl;
		}

		m_files.push_back(key);
		m_times.push_back(fs::last_write_time(key));
	}


	for (const auto & entry : fs::directory_iterator("includes/")) {
		if ( std::string(entry.path().filename())[0] != '.') {
			includes_file.push_back(entry.path().filename());
			m_files.push_back(entry.path());
			m_times.push_back( fs::last_write_time(entry) );
		}
	}

	m_files.push_back("data.json");
	m_times.push_back( fs::last_write_time("data.json") );

	return includes_file;
}

FileWatcher::~FileWatcher() {}

std::string FileWatcher::new_modif() {
	for (std::size_t i {0}; i < m_files.size() - 1; i++) {
		const auto tfile { fs::last_write_time(m_files[i]) };

		if (tfile != m_times[i]) {
			m_times[i] = tfile;

			std::cout << "File watcher detected: " << m_files[i] << std::endl;

			return "true";
		}
	}


	return "false";
}


bool FileWatcher::json_change() {
	/**std::size_t end = m_files.size()-1;

	const auto tfile { fs::last_write_time(m_files[end]) };

	if (tfile != m_times[end]) {
		m_times[end] = tfile;

		return true;
	}**/

	return false;
}
