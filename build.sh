#!/bin/bash

export PKG_CONFIG_PATH=~/rpi-rgb-led-matrix/lib/pkgconfig:$PKG_CONFIG_PATH
pkg-config --modversion rgbmatrix


cmake .
make
