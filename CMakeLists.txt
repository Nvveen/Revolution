cmake_minimum_required(VERSION 2.8)
project(revolution)

set(CMAKE_CXX_FLAGS "-std=c++0x -W")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -DDEBUG -DNOCEGUI")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")
set(INSTALL_DIR "${PROJECT_SOURCE_DIR}/bin")
set(LIB_DIR "${PROJECT_SOURCE_DIR}/lib")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(revolution_SRCS
  ${SRC_DIR}/Camera.cpp
  ${SRC_DIR}/DataManager.cpp
  ${SRC_DIR}/Drawable.cpp
  ${SRC_DIR}/Main.cpp
  ${SRC_DIR}/Screen.cpp
  ${SRC_DIR}/Shader.cpp
  ${SRC_DIR}/World.cpp
)

find_package(CEGUI REQUIRED COMPONENTS OPENGL)
find_package(SDL REQUIRED)
find_package(Boost COMPONENTS filesystem system REQUIRED)
find_package(OpenGL)
find_package(GLEW)

include_directories(${INCLUDE_DIR})
include_directories(${LIB_DIR}/glm)
include_directories(${Boost_INCLUDE_DIR})

link_libraries(${CEGUI_LIBRARY} ${SDL_LIBRARY} ${Boost_LIBRARIES})
link_libraries(${OPENGL_LIBRARY} ${GLEW_LIBRARIES})

add_executable(main ${revolution_SRCS})
install(PROGRAMS ${CMAKE_BINARY_DIR}/main DESTINATION ${INSTALL_DIR})
