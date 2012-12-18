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
  ${SRC_DIR}/Drawable.cpp
  ${SRC_DIR}/Main.cpp
  ${SRC_DIR}/Screen.cpp
  ${SRC_DIR}/Shader.cpp
  ${SRC_DIR}/World.cpp
)

find_package(CEGUI REQUIRED COMPONENTS OPENGL)
find_package(SDL REQUIRED)
find_package(Boost COMPONENTS filesystem system REQUIRED)

include_directories(${INCLUDE_DIR})
include_directories(${LIB_DIR}/glm)
include_directories(${Boost_INCLUDE_DIR})

link_libraries(${CEGUI_LIBRARY} ${SDL_LIBRARY} ${Boost_LIBRARIES})
if(CMAKE_BUILD_TYPE MATCHES Debug)
  find_package(OpenGL)
  find_package(GLEW)
  link_libraries(${OPENGL_LIBRARY} ${GLEW_LIBRARIES})
endif(CMAKE_BUILD_TYPE MATCHES Debug)

include(ExternalProject)
ExternalProject_Add(triangle
  SOURCE_DIR ${LIB_DIR}/triangle
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_COMMAND $(MAKE)
)
ExternalProject_Add(gompile
  SOURCE_DIR ${LIB_DIR}/gompile
  BUILD_IN_SOURCE 1
  CONFIGURE_COMMAND ""
  INSTALL_COMMAND ""
  BUILD_COMMAND $(MAKE)
)

add_executable(main ${revolution_SRCS})
install(PROGRAMS ${CMAKE_BINARY_DIR}/main DESTINATION ${INSTALL_DIR})
install(PROGRAMS ${LIB_DIR}/triangle/triangle DESTINATION ${INSTALL_DIR})
install(PROGRAMS ${LIB_DIR}/triangle/showme DESTINATION ${INSTALL_DIR})
install(PROGRAMS ${LIB_DIR}/gompile/gompile DESTINATION ${INSTALL_DIR})
