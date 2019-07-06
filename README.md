# Simple Compiler Utility
### *Super Simple, Compiler-Abstract, C/C++ Compiling Utility*

## How to Use

The only binary I have avaliable at the moment is the `main` which was built on ubuntu with gcc. At the moment, the only configured compiler is `g++`, but due to the architecture of this program, writing in different compilers is super easy.

To make a project, go to your projects root directory with the executable and run it with the argument `makeproject [name]` (where name is the name of your project). SCU will generate a `.proj` file which you need to fill out. **Includes and libraries are not necessary, the other information is.** Now you can run the executable with no arguments, or with a `compile` argument, and it will compile with the information you gave it.

The other acceptable arguments are `clean` and `recompile`. The former will remove all files in the `obj` directory (gathered non-recursivly), and the latter with `clean` and then `compile` each project.

## Why Use SCU

I have found myself over the years growing increasingly resentful towards the tools out there used for compiling. Writing make files or cmake configurations for each project can be a total drag. Therefore, I wanted a utility that I can simply run in my project's directory and do all the heavy lifting. Another *very* important thing for me, is platform portability. Therefore, I strive to make this utility completely platform and compiler independent. 

## External Dependancies

SCU uses no external dependancies that are located outside of the `src/headers` folder. It uses the header-only spdlog library, as well as the tinyxml2 library for parsing the xml-based project files.