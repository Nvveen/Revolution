#!/bin/bash

if [[ "$1" == "clean" ]]; then
  echo "Cleaning..."
  rm -rf .build/
  rm main
fi

if [[ ! -d ".build/" ]]; then
  mkdir -v .build
fi

cd .build/
cmake ..
make
cd ..
