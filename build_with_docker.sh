#!/bin/bash
echo "Building project using pebbledev/pebble-sdk..."
docker run --rm -v "$PWD":/code -w /code rebble/pebble-sdk pebble build
