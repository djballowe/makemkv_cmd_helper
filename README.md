# makemkv_cmd_helper
#### All descriptions assume discs are DRM free

A wrapper around makemkvcon that makes it easier to identify titles and rip .mkv files using only the command line. Particularly usefull for headless setups where using the makemkv GUI is not possible.

Unfortunatly can only rip one title at a time. Consider checking out docker makemkv if that will not work for you.

### Usage
```
mkdir build
cd build
cmake ..
make
```
```
makemkv_helper destination/folder
```
