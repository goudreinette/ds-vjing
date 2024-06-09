#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
ASSETS=$NITRO_ENGINE/examples/assets
TOOLS=$NITRO_ENGINE/tools
OBJ2DL=$TOOLS/obj2dl/obj2dl.py


rm -rf data
mkdir -p data

python3 $OBJ2DL \
    --input ./models/clippy.obj \
    --output data/clippy.bin \
    --texture 256 256

python3 $OBJ2DL \
    --input ./models/cloud.obj \
    --output data/cloud.bin \
    --texture 256 256


python3 $OBJ2DL \
    --input ./models/paper.obj \
    --output data/paper.bin \
    --texture 256 256
