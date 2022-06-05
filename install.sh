#!/bin/bash

STATE_PATH="${HOME}/.task_tracker"

mkdir -p ${STATE_PATH}/bin
mkdir -p ${STATE_PATH}/history

echo '{"project_names":["personal"]}' >${STATE_PATH}/config.json

cmake .
cmake --build .

cp -r ./bin ${STATE_PATH}/

echo
echo 'INFO: add this line to your preferred profile file to add `track` command to PATH:'
echo 'PATH="$PATH:${HOME}/.task_tracker/bin"'
echo
