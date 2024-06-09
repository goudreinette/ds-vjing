#!/bin/sh

NITRO_ENGINE=$DEVKITPRO/nitro-engine
ASSETS=$NITRO_ENGINE/examples/assets
TOOLS=$NITRO_ENGINE/tools
OBJ2DL=$TOOLS/obj2dl/obj2dl.py


rm -rf data
mkdir -p data

python3 $OBJ2DL \
    --input ./models/biertje.obj \
    --output data/biertje.bin \
    --texture 256 256
