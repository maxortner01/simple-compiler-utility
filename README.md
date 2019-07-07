# Simple Compiler Utility
### *Super Simple, Compiler-Abstract, C/C++ Compiling Utility*

## Why Use SCU

I have found myself over the years growing increasingly resentful towards the tools out there used for compiling. Writing make files or cmake configurations for each project can be a total drag. Therefore, I wanted a utility that I can simply run in my project's directory and do all the heavy lifting. Another *very* important thing for me, is platform portability. Therefore, I strive to make this utility completely platform and compiler independent. 

## How to Use

In the bin folder are two executables named for the platform on which they were compiled. The usage of them is exactly the same. At the moment, the only configured compiler is `g++`, but due to the architecture of this program, writing in different compilers is super easy.

To make a project, go to your projects root directory with the executable and run it with the argument `makeproject [name]` (where name is the name of your project). SCU will generate a `.proj` file which you need to fill out. **Includes and libraries are not necessary, the other information is.** Now you can run the executable with no arguments, or with a `compile` argument, and it will compile with the information you gave it.

The other acceptable arguments are `clean` and `recompile`. The former will remove all files in the `obj` directory (gathered non-recursively), and the latter will `clean` and then `compile` each project.

## Compiling

If you wish to compile the project on your platform, navigate to the make folder and type the command `make`. It should take care of all the compiling. The makefile has only been written for a MinGW-W64 compiler. The W64 is unfortunately crucial, since spdlog depends on some multithreading support not supplied by other versions.

## External Dependancies

SCU uses no external dependancies that are located outside of the `src/headers` folder. It uses the header-only spdlog library, as well as the tinyxml2 library for parsing the xml-based project files.