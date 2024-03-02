#!/bin/sh

grit blueball.png -ftb -fh! -gTFF00FF -gb -gB8 -m!
grit redcar.png -ftb -fh! -gTFF00FF -gb -gB8 -m!
grit pp.png -ftb -fh! -gTFF00FF -gb -gB8 -m!

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

mv *.pal *.img ../nitrofiles/sprite

grit bg3.png -ftb -fh! -gTFF00FF -gt -gB8 -mR8 -mLs

for file in *.bin; do
    mv -- "$file" "${file%.bin}"
done

mv *.pal *.img *.map ../nitrofiles/bg
