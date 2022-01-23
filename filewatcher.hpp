#pragma once

#include <string>

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
#include <chrono>

using json = nlohmann::json;

class FileWatcher {
	public:
		FileWatcher();
		~FileWatcher();
		std::vector<std::string> load(json &data);
		bool json_change();
		std::string new_modif();
	private:
		std::vector<std::string> m_files;
		std::vector<fs::file_time_type> m_times;
};
