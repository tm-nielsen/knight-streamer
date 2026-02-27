#!/bin/sh

if [[ $1 = "-b" ]]; then
    echo rebuilding...
    cmake --build build --target knight_streamer || exit 1
    echo ---
fi
bin/Debug/knight_streamer