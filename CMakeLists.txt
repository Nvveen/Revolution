cmake_minimum_required(VERSION 2.8)
project(revolution)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules/)

find_package(OpenGL REQUIRED)
find_package(SDL REQUIRED)

include(FindPkgConfig)

pkg_check_modules(CEGUI REQUIRED CEGUI-OPENGL)
add_definitions(${CEGUI_CFLAGS})
link_libraries(${CEGUI_LDFLAGS})
 
set(CMAKE_CXX_FLAGS "-std=c++0x -g")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR})

include_directories(${PROJECT_SOURCE_DIR}/include/)
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
