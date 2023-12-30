# fileorg
Organize your files automatically with the speed of C! 

The way it works is simple. The program will parse the source directories you set in your configuration file, checking for any file with the prefix you also set in the configuration file. Once it finds one, it will move that file to the associated directory. 

## Installation
Run these three commands:
```shell
git clone https://github.com/little-umbrellas/fileorg &&
cd fileorg &&
make install
```

## Configuration
Once ran, the program will prompt you for a source directory, filename prefix, and destination directory. After that, if you wish to add more, simply edit the configuration file found in $HOME/.config/fileorg.conf. The syntax is very straight-forward:
```shell
# You can add as many source directories as you want
[Source]
dir = $HOME
dir = ~/Downloads

[Destination]
ext = f-  # The prefix can be anything, though dashes help me differentiate it
dir = ~/Pictures/frogs

ext = wrk-
dir = ~/Documents/work
```
