# magic 

```sh

cd assets && sh convert.sh && cd .. && make clean && make && open tilechange.nds

# watch
chokidar "source/**.cpp" -c "cd assets && sh convert.sh && cd .. && make clean && make && open tilechange.nds"

chokidar "source/**.cpp" -c "make && open tilechange.nds"

# chokidar "source/**.cpp" "assets/**.png" "assets/convert.sh" -c "cd assets && sh convert.sh && cd .. && make clean && make && open microphone-sprite-size.nds" --initial true


# Newest one
chokidar "source/**.*" "assets/**.png" "graphics/**.png" "assets.sh" -c "sh assets.sh && make clean && make && open data-lek.nds" --initial true

# Transfer to DS and eject disc
cp microphone-sprite-size.nds /Volumes/DS/
diskutil unmount DS

# In one go
cp microphone-sprite-size.nds /Volumes/DS/ && diskutil unmount DS


chokidar "source/**.c" -c "make" --initial true
```


32 x 32 tiles in a 256px map

first rows, then columns

25 = separate squares


animations: in a column

cmd-shift-x to export spritesheet