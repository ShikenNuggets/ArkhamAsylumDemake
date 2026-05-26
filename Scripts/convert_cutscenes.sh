#!/usr/bin/env bash

# Check if a file was provided
if [ -z $1 ]; then
    echo Usage $0 video.bik
    exit 1
fi

INPUT=$1

# Extract the filename without the extension
BASENAME=${INPUT%.}

echo Converting $INPUT to ${BASENAME}.m1v...

ffmpeg -i $INPUT -c:v mpeg1video -b:v 1500k -r 29.97 -s 640x360 -an ${BASENAME}.m1v

echo Done!
