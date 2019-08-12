cmake_minimum_required(VERSION 3.0.0)

PROJECT(ImageAOI)


set(CMAKE_INCLUDE_CURRENT_DIR ON)


find_package(Qt5Core CONFIG REQUIRED)
find_package(Qt5Widgets REQUIRED) 

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)


add_executable(${PROJECT_NAME}
	main.cpp
	src/xlabel.cpp
	)

target_link_libraries(${PROJECT_NAME} 
	Qt5::Core	
	Qt5::Widgets
	Qt5::Gui
)