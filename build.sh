#!/bin/bash

DIR="$( cd "$(dirname "$0")" && pwd )"
SRC_DIR="$DIR/src"
INC_DIR="$DIR/include"
BIN_DIR="$DIR/bin"
LIB_DIR="$DIR/lib"
BUILD_DIR_REL="$DIR/.build"
BUILD_DIR_DEBUG="$DIR/.build-debug"
MAIN_BIN="main"
CMAKELISTS="CMakeLists.txt"

# Flags
DEBUG=false
VERBOSE=false

function create() {
  echo "Creating directories...";
  mkdir -v "$SRC_DIR" "$LIB_DIR" "$INC_DIR" "$BIN_DIR";
  echo "int main ()
{
  return 0;
}" > "$SRC_DIR/Main.cpp";
  echo "What is the name of the project you wish to start?";
  read PROJ_NAME;
  echo "Creating $CMAKELISTS file as project $PROJ_NAME...";
  echo 'cmake_minimum_required(VERSION 2.8)
project('"$PROJ_NAME"')

set(CMAKE_CXX_FLAGS_DEBUG "-std=c++0x -g -Werror -Wall -Wfatal-errors -DDEBUG")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")
set(INSTALL_DIR "${PROJECT_SOURCE_DIR}/bin")
set(LIB_DIR "${PROJECT_SOURCE_DIR}/lib")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set('"$PROJ_NAME"'_SRCS
  ${SRC_DIR}/Main.cpp
)

include_directories(${INCLUDE_DIR})

add_executable('"$MAIN_BIN"' ${'"$PROJ_NAME"'_SRCS})
install(PROGRAMS ${CMAKE_BINARY_DIR}/'"$MAIN_BIN"' DESTINATION ${INSTALL_DIR})' > "$CMAKELISTS"
}

function build() {
  if [ ! -e "$CMAKELISTS" ]; then
    echo "$CMAKELISTS does not exist, do you want to start a new project? [y/n]";
    read input;
    if [ "$input" == "yes" ] || [ "$input" == "y" ] || [ "$input" == "" ]; then
      create;
    else
      echo "Nothing to do, exiting...";
      exit 0;
    fi
  fi
  BUILD_DIR="$BUILD_DIR_REL"
  if $DEBUG; then
    BUILD_DIR="$BUILD_DIR_DEBUG"
  fi
  mkdir -v "$BUILD_DIR" &>/dev/null;
  cd "$BUILD_DIR";
  CMAKE_OPTION="-DCMAKE_BUILD_TYPE="
  if $DEBUG; then
    CMAKE_OPTION=${CMAKE_OPTION}"Debug"
  else
    CMAKE_OPTION=${CMAKE_OPTION}"Release"
  fi
  cmake ${CMAKE_OPTION} ..;
  if $VERBOSE; then
    make VERBOSE=1;
  else
    make;
  fi
  make install;
}

function clean_builds() {
  echo "Cleaning...";
  rm -rf "$BUILD_DIR_REL"* &>/dev/null;
  rm "$BIN_DIR"/* &>/dev/null;
}

function print_help() {
  echo "Help here";
}

while getopts ":cdsv" flag; do
  case $flag in
    c)
      clean_builds;
      exit 0;
      ;;
    d)
      DEBUG=true
      ;;
    s)
      create;
      exit 0;
      ;;
    v)
      VERBOSE=true
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2;
      print_help;
      exit 1;
      ;;
    \:)
      echo "Option -$OPTARG requires an argument" >&2;
      print_help;
      exit 1;
      ;;
  esac
done

cd "$DIR";
build;
