#!/bin/bash

cdrdao="/home/alex/bin/cdrdao-0afa49e-pled-1.0.2-x86_64/cdrdao"
ps1demoswap="/home/alex/bin/ps1_demoswap_patcher_1.0.9_linux_x86_64_static/ps1demoswap"
duckstation="/home/alex/bin/DuckStation-x64.AppImage"
set -e

cd "$(dirname "$0")"/../
clear
make clean
make
rm -rf tmp
unzip "$1" -d tmp
files=(tmp/*.bin)
"$ps1demoswap" -t "${files[0]}"
./edcre "${files[0]}"
cd tmp
"$duckstation" *.cue

