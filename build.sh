#!/bin/bash

HOME_DIR=$(pwd)
BUILD_DIR="$HOME_DIR/.build"
BIN_DIR="$HOME_DIR/bin"
SRC_DIR="$HOME_DIR/src"
INCLUDE_DIR="$HOME_DIR/include"
LIB_DIR="$HOME_DIR/lib"
MAIN_BIN="main"
CMAKELISTS="CMakeLists.txt"
NO_BUILD="no"

function buildCMake() {
  echo "Creating the CMakeLists.txt file...";
  echo "Give a project name: "; read PROJ_NAME;
  echo 'cmake_minimum_required(VERSION 2.8)
project('"$PROJ_NAME"')

set(CMAKE_CXX_FLAGS "-std=c++0x -g -Werror -Wall -Wfatal-errors")
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")
set(INSTALL_DIR "${PROJECT_SOURCE_DIR}/bin")
set(LIB_DIR "${PROJECT_SOURCE_DIR}/lib")
set(INCLUDE_DIR "${PROJECT_SOURCE_DIR}/include")
set(SRC_DIR "${PROJECT_SOURCE_DIR}/src")
set(BUILD_DIR "${PROJECT_SOURCE_DIR}/.build")
set('"$PROJ_NAME"'_SRCS
  ${SRC_DIR}/
)

include_directories(${INCLUDE_DIR})

add_executable('"$MAIN_BIN"' ${'"$PROJ_NAME"'_SRCS})
install(PROGRAMS '"$MAIN_BIN"' DESTINATION ${INSTALL_DIR})' > "$CMAKELISTS"
}

function clean() {
  echo "Cleaning..."
  rm -rf "$BUILD_DIR" 2>/dev/null
  rm -r "$BIN_DIR/*" &>/dev/null && mkdir "$BIN_DIR" &>/dev/null
}

function purge() {
  echo "Are you sure you want to purge? Type \"yes\" if you are certain:";
  read PURGE;
  if [ "$PURGE" == "yes" ]; then
    echo "Purging...";
    rm -r "$SRC_DIR" "$BIN_DIR" "$INCLUDE_DIR" "$CMAKELISTS" &>/dev/null;
    rm -r "$BUILD_DIR" "$LIB_DIR" &>/dev/null
    exit 0;
  fi
}

if [ ! -d "src" ] || [ ! -d "include" ] || [ ! -d "bin" ]; then
  echo "You need at least the src/, include/ and bin/ directories to build a \
valid project" >&2;
  echo "Do you want to create these directories to start a project? [y/n]";
  CREATE="yes"; read CREATE;
  if [ "$CREATE" == "yes" ] || [ "$CREATE" == "y" ]; then
    mkdir -v "$BIN_DIR" "$SRC_DIR" "$INCLUDE_DIR" >/dev/null;
  fi
  if [ ! -d "lib" ]; then
    echo "Do you want to create the lib/ directory? [y/n]";
    CREATE="yes"; read CREATE;
    if [ "$CREATE" == "yes" ] || [ "$CREATE" == "y" ]; then
      mkdir -v "$LIB_DIR" >/dev/null;
    fi
  fi
  if [ "$CREATE" != "yes" ] && [ "$CREATE" != "y" ]; then
    echo "Created directories.";
  fi
fi

while getopts ":bcd:np" flag; do
  case $flag in
    b)
      buildCMake;
      exit 0;
      ;;
    c)
      clean;
      exit 0;
      ;;
    d)
      MAIN_BIN="$OPTARG"
      ;;
    n)
      NO_BUILD="yes"
      ;;
    p)
      purge
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2;
      ;;
    \:)
      echo "Option -$OPTARG requires an argument" >&2;
      ;;
  esac
done

if [ "$NO_BUILD" == "no" ]; then
  mkdir .build &>/dev/null
  cd .build
  cmake ..
  make && make install
else
  echo "Running quietly.";
fi
