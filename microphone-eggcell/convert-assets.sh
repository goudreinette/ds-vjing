#!/bin/sh

grit assets/eggcell.png -ftb -fh! -gT000000 -gb -gB8 -m!
grit assets/tiles.png -ftb -fh! -gTFFF1E8 -gt -gB8 -mR8 -mLs

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

mv *.pal *.img *.map ./nitrofiles/
