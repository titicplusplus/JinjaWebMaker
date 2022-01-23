cmake_minimum_required(VERSION 3.10)

# set the project name
project(WebMakerJinja VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)

# add the executable
add_executable(
	WebMakerJinja
	main.cpp
	webserver.cpp
	create_f.cpp
	filewatcher.cpp
	dependencies/WebSocket/base64/base64.cpp
	dependencies/WebSocket/sha1/sha1.cpp
	export.cpp
	websocket.cpp
)

set(CMAKE_BUILD_TYPE Debug)

target_link_libraries(
	WebMakerJinja 
	stdc++fs
	pthread
)
