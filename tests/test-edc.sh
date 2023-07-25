#!/bin/bash

cdrdao="/home/alex/bin/cdrdao-0afa49e-pled-1.0.2-x86_64/cdrdao"

set -e

cd "$(dirname "$0")"/../
clear
rm -rf tmp
unzip "$1" -d tmp
files=(tmp/*.bin)
cd tmp
"$cdrdao" write --speed 1 --eject --swap --driver generic-mmc --eject -n *.cue
cd ..
rm -rf tmp
