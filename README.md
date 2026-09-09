# City Simulation
Currently just some demos for the core functionality.

## Build Instructions
Project is built with raylib and uses CMake to compile. From the root folder, the following commands build and run the project: 
```
cmake -B build
cmake --build build
cd build
./Simulation
```
This will download all necessary raylib dependancies, and create a `compile_commands.json` recognised by clangd.
