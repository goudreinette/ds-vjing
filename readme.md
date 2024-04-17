# magic 

```sh

cd assets && sh convert.sh && cd .. && make clean && make && open tilechange.nds

# watch
chokidar "source/**.cpp" -c "cd assets && sh convert.sh && cd .. && make clean && make && open tilechange.nds"

chokidar "source/**.cpp" -c "make && open tilechange.nds"


chokidar "source/**.cpp" "assets/**.png" "assets/convert.sh" -c "cd assets && sh convert.sh && cd .. && make clean && make && open particlefun.nds" --initial true

```


32 x 32 tiles in a 256px map

first rows, then columns

25 = separate squares


animations: in a column

cmd-shift-x to export spritesheet