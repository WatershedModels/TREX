# TREX: <u>T</u>wo-Dimensional <u>R</u>unoff <u>E</u>rosion and E<u>x</u>port
Revised TREX watershed model, source code from [CSU](http://www.engr.colostate.edu/~pierre/ce_old/Projects/TREX%20Web%20Pages/TREX-Home.html).

# 1. Prerequisite 

+ CMake2.8+
+ Windows:
  + Microsoft Visual Studio 2010+
  + or CLion and mingw64 4.8+
+ Linux/macOS:
  + GCC 4.8+
  + clang
  + icc

# 2. Compile procedure

+ common commands

  ```shell
  cd <path to TREX>
  mkdir build
  cmake ..
  make && make install
  ```

## 2.1. Using Visual Studio

 Take VS2013 as an example.

+ Open "Intel compiler 17.0 Update 4 Intel(R) 64 Visual Studio 2013" from start menu.
+ cd to the compile destination folder. e.g., `cd D:/compile/trex`
+ Run `cmake <path to TREX src path>`. e.g., `cmake C:\z_code\Hydro\TREX`
  + Compile 64-bit version: `cmake -G "Visual Studio 12 2013 Win64" C:\z_code\Hydro\TREX`
+ Open the project `TREX20110210.sln`, and do what you want!

## 2.2. Using GCC under Windows (mingw64 recommended), Linux, and macOS

If you prefer the command line, the common commands above should be OK. 

However, if you want a user-friendly IDE for to learn and improve the TREX model, I recommend the cross-platform IDE [CLion](https://www.jetbrains.com/clion/).

CLion use CMake to manage projects. Under Windows, you may also install mingw64 (or mingw) for the GCC compiler.

Using CLion is quite easy and intuitive. Just open the TREX path from `File -> Open...`. Then CLion will automatically load the project by CMakeLists.txt existed in TREX directory.  Now, you can build TREX model by typing `Ctrl+F9` or clicking the build button.

# 3. Update with the latest TREX source (For administrator only)

In order to make it easier to track code changes and keep up with newer revisions, I use subtree to manage the [TREX source code from CSU](https://github.com/crazyzlj/trex_src_csu).

+ Set subtree as `trex_src`(only do once at the very beginning of this repository)

  ```bash
   git remote add -f trex_src git@github.com:crazyzlj/trex_src_csu.git
   git subtree add --prefix=trex_src trex_src master --squash
  ```

+ Do any modification to the `trex_src` to meet our specific needs.

+ If the TREX source is updated we should pull the latest code from [trex_src_csu](https://github.com/crazyzlj/trex_src_csu) and merge it.

  ```shell
  git fetch trex_src master
  git subtree pull --prefix=trex_src trex_src master --squash
  ```

# 4. Changelog

+ 10/12/2017: initial by the source code updated in 2/10/2011.