# Life

Over engineered implementation of Conway's Game of Life.

![example](example.gif)

## Usage

Build using `make`. You'll get an executable `life.exe`.

You can pass different arguments to customise the simulation:

 - `--rows <r>` specify the number of rows
 - `--cols <c>` specify the number of cols
 - `--fps <fps>` specify the FPS at which the simulation should run. (1 FPS == 1 Iteration)
 - `--ggg` generate a gosper glider gun
 - `--lwss` generate a lightweight spaceship
 - `--pacman` enable pacman effect on borders

 When the simulation is running:

 - `<q>` to quit the simulation
 - `<->` to decrease the simulation speed
 - `<+>` to increase the simulation speed

 ## Notes

 - Input handling does not work on Windows
