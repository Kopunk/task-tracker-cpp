#!/bin/bash

cmake --version >/dev/null || echo "cmake not found" && exit 1

STATE_PATH="${HOME}/.task_tracker"

mkdir -p ${STATE_PATH}/bin
mkdir -p ${STATE_PATH}/history

echo '{"project_names":["personal"]}' >${STATE_PATH}/config

cmake .
cmake --build .

cp -r ./bin ${STATE_PATH}/bin

PATH="$PATH:${STATE_PATH}/bin"

echo
echo 'INFO: add this line to your preferred profile file to add `track` command to PATH:'
echo 'PATH="$PATH:${STATE_PATH}/bin" to your '
echo
