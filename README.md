# Cereal Box Snow
This is a snow sim. It can simulate pixel art style snow and fire.

## Screenshots
<img src="https://github.com/tallHouse64/CerealBoxSnow/blob/main/screenshot1.png?raw=true" width="428" alt="A screenshot of Cereal Box Snow simulating snow."/>
<img src="https://github.com/tallHouse64/CerealBoxSnow/blob/main/screenshot2.png?raw=true" width="428" alt="Another screenshot of Cereal Box Snow simulating snow with the UI shown."/>
<img src="https://github.com/tallHouse64/CerealBoxSnow/blob/main/screenshot3.png?raw=true" width="428" alt="A screenshot of Cereal Box Snow's fire simulation."/>

## Features
This program doesn't have many features.
There is a slider to control the number of particles simulated.

It can simulate two things:
- Snow
- Fire

## Supported platforms

Cereal Box Snow supports:
- Windows
- Linux
- Nintendo DS

## How to install

First download Cereal Box Snow from the releases page (https://github.com/tallHouse64/CerealBoxSnow/releases/latest). Then extract the zip file to anywhere and double click the executable (for Windows that is "cbsnow.exe").

## How to compile

To compile this you do need to be comfortable using a terminal, extracting zip files and know how to use the "cd" command.

Windows:
You need to install:
- msys2 (https://www.msys2.org/)
- gcc (inside msys2)
- make (inside msys2)
- SDL2 (inside msys2)

Then you can download a copy of the source code (https://github.com/tallHouse64/CerealBoxSnow/releases/latest download the zip file).

Extract the zip file anywhere and open msys2 ucrt64 terminal. Then navigate to the folder with the "cd" command and run "make".

## Dependencies
This program uses Drws-lib for graphics, input and output. Drws-lib uses SDL2 as an optional dependency.

## System requirements
It runs great on a small laptop 4 Core 1.1Ghz CPU with 4GB RAM.
It would probably run fine on a potato.
