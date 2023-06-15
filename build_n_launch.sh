#!/bin/bash

cd build
make;

./create_raster $1 $2

mv raster.ppm ../assets
cd ../assets
xdg-open ./raster.ppm
cd ..
