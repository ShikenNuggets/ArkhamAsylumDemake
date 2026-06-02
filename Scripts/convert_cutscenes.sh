#!/usr/bin/env bash

# Check if a file was provided
if [ -z $1 ]; then
    echo Usage $0 video.bik
    exit 1
fi

INPUT=$1

# Extract the filename without the extension
BASENAME=${INPUT%.*}

echo "Converting $INPUT to ${BASENAME}.m1v..."

# Video Track
ffmpeg -i "$INPUT" -c:v mpeg1video -b:v 1500k -r 29.97 -s 640x360 -an "${BASENAME}.m1v"

# Audio Track(s)
AUDIO_STREAM_COUNT=$(ffprobe -v error -select_streams a -show_entries stream=index -of csv=p=0 "$INPUT" | wc -l)

echo "Found $AUDIO_STREAM_COUNT audio stream(s)"

if [ "$AUDIO_STREAM_COUNT" -gt 0 ]; then
    # Track indices in ffmpeg are 0-based, so if count is 2, we loop 0 and 1
    for ((i=0; i<AUDIO_STREAM_COUNT; i++)); do
        echo "Extracting audio stream #$i to ${BASENAME}_${i}.ogg..."
        
        # -map 0:a:$i dynamically selects the current audio stream in the loop
        ffmpeg -y -i "$INPUT" -map 0:a:"$i" -c:a libvorbis -q:a 4 -ar 48000 -ac 2 "${BASENAME}_${i}.ogg"
    done
else
    echo "No audio streams found in $INPUT."
fi

echo "Done!"
